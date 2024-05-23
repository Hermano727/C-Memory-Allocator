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
   
   struct block_header *current = (struct block_header *)((char *)ptr - sizeof(struct block_header));

   // If block header indicates the block is already free, do nothing
   if (!(current->size_status & VM_BUSY)) {
           return;
   }

   // Unset status bit to 0
   current->size_status &= ~VM_BUSY;
   
}
