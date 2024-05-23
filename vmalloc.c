#include "vm.h"
#include "vmlib.h"

void *vmalloc(size_t size)
{
    if (size <= 0) {
        return NULL; // no allocation made
    }

    // Find total size needed for allocation
    size_t total_size = size + sizeof(struct block_header);
    total_size = ROUND_UP(total_size, BLKALIGN); // Round up to nearest 16

    struct block_header *current = heapstart;
    struct block_header *best_fit = NULL;

    // While not at the end of the block, traverse through the block
    while (current->size_status != VM_ENDMARK) {
        size_t curr_block_sz = BLKSZ(current);

        // Use bit operators to check if space is available at current block.
        if (!(current->size_status & VM_BUSY) && curr_block_sz >= total_size) {
            if (best_fit == NULL || curr_block_sz < BLKSZ(best_fit)) {
                best_fit = current; // Set the pointer to the best found block
            }
        }

        // Traverse to next block
        current = (struct block_header *)((char *)current + curr_block_sz);
    }

    // If there is no space, return null
    if (best_fit == NULL) {
        return NULL;
    }

    // Allocate the block
    size_t best_fit_sz = BLKSZ(best_fit);
    if (best_fit_sz > total_size + sizeof(struct block_header)) {
        struct block_header *new_block =
            (struct block_header *)((char *)best_fit + total_size);
        new_block->size_status =
            (best_fit_sz - total_size) | (best_fit->size_status & VM_PREVBUSY);

        // Create footer for new block
        struct block_footer *new_footer =
            (struct block_footer *)((char *)new_block + BLKSZ(new_block) -
                                    sizeof(struct block_footer));
        new_footer->size = BLKSZ(new_block);

        // Update best fit block to busy
        best_fit->size_status =
            total_size | VM_BUSY | (best_fit->size_status & VM_PREVBUSY);
        struct block_footer *best_fit_footer =
            (struct block_footer *)((char *)best_fit + total_size -
                                    sizeof(struct block_footer));
        best_fit_footer->size = total_size;
    } else {
        // If best_fit_sz is the same as total_size, just allocate the whole
        // block
        best_fit->size_status =
            best_fit_sz | VM_BUSY | (best_fit->size_status & VM_PREVBUSY);
    }

    struct block_header *next_block =
        (struct block_header *)((char *)best_fit + BLKSZ(best_fit));
    if (next_block->size_status != VM_ENDMARK) {
        next_block->size_status |= VM_PREVBUSY;
    }

    // Return the pointer to the payload
    return (void *)((char *)best_fit + sizeof(struct block_header));
}
