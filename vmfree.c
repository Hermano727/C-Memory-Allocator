#include "vm.h"
#include "vmlib.h"

/**
 * The vmfree() function frees the memory space pointed to by ptr,
 * which must have been returned by a previous call to vmmalloc().
 * Otherwise, or if free(ptr) has already been called before,
 * undefined behavior occurs.
 * If ptr is NULL, no operation is performed.
 */
void vmfree(void *ptr)
{
    if (ptr == NULL) {
        return;
    }

    // Find starting address of the block header
    struct block_header *curr =
        (struct block_header *)((char *)ptr - sizeof(struct block_header));

    // If block header indicates the block is already free, do nothing
    if (!(curr->size_status & VM_BUSY)) {
        return;
    }

    // Unset status bit to 0. Mark the block as free.
    curr->size_status &= ~VM_BUSY;

    // Update footer for current block. Will help for coalescing prev blocks
    struct block_footer *current_footer =
        (struct block_footer *)((char *)curr + BLKSZ(curr) -
                                sizeof(struct block_footer));
    current_footer->size = BLKSZ(curr);

    // Locate next block
    struct block_header *next_block =
        (struct block_header *)((char *)curr + BLKSZ(curr));

    // If the next block is not the end marker, set prev block
    // status to free 
    if (next_block->size_status != VM_ENDMARK) {
        next_block->size_status &= ~VM_PREVBUSY;

        // Coalesce curr block with next block if it is also free
        if (!(next_block->size_status & VM_BUSY)) {
            curr->size_status += BLKSZ(next_block);
            struct block_footer *next_footer =
                (struct block_footer *)((char *)next_block + BLKSZ(next_block) -
                                        sizeof(struct block_footer));
            current_footer = next_footer;
            current_footer->size = BLKSZ(curr);
        }
    }

    // If prev block is free, curr block is coalesced with prev block
    if (!(curr->size_status & VM_PREVBUSY)) {
        struct block_footer *prev_footer =
            (struct block_footer *)((char *)curr - sizeof(struct block_footer));
        struct block_header *prev_block =
            (struct block_header *)((char *)curr - prev_footer->size);
        prev_block->size_status += BLKSZ(curr);
        current_footer->size = BLKSZ(prev_block);
    }
}
