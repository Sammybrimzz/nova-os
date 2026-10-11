#include <stdint.h>

/*
 * Simple kernel heap
 *
 * Heap:
 * 0x00200000 → 0x00300000
 *
 * Total size: 1 MiB
 */

#define HEAP_START 0x00200000
#define HEAP_SIZE  (1024 * 1024)
#define HEAP_END   (HEAP_START + HEAP_SIZE)

#define HEAP_MAGIC 0x4E4F5641

/*
 * Every allocated/free block has a header.
 *
 * Layout:
 *
 * [ header ][ user memory ]
 */
struct heap_block
{
    uint32_t magic;
    uint32_t size;
    uint32_t free;

    struct heap_block *next;
};

/*
 * First block in the heap.
 */
static struct heap_block *heap_first = 0;


/*
 * Align size to 8 bytes.
 */
static uint32_t align8(uint32_t size)
{
    return (size + 7) & ~7;
}


/*
 * Initialize the heap.
 */
void heap_init(void)
{
    heap_first = (struct heap_block *)HEAP_START;

    heap_first->magic = HEAP_MAGIC;
    heap_first->size = HEAP_SIZE - sizeof(struct heap_block);
    heap_first->free = 1;
    heap_first->next = 0;
}


/*
 * Split a free block if it is large enough.
 */
static void split_block(struct heap_block *block, uint32_t size)
{
    uint32_t remaining =
        block->size - size;

    if (remaining <= sizeof(struct heap_block) + 8)
    {
        return;
    }

    struct heap_block *new_block =
        (struct heap_block *)(
            (uint8_t *)block +
            sizeof(struct heap_block) +
            size
        );

    new_block->magic = HEAP_MAGIC;
    new_block->size =
        remaining - sizeof(struct heap_block);
    new_block->free = 1;
    new_block->next = block->next;

    block->size = size;
    block->next = new_block;
}


/*
 * Allocate memory from the kernel heap.
 */
void *kmalloc(uint32_t size)
{
    if (size == 0)
    {
        return 0;
    }

    size = align8(size);

    struct heap_block *current = heap_first;

    while (current != 0)
    {
        if (current->magic != HEAP_MAGIC)
        {
            return 0;
        }

        if (current->free && current->size >= size)
        {
            split_block(current, size);

            current->free = 0;

            return (void *)(
                (uint8_t *)current +
                sizeof(struct heap_block)
            );
        }

        current = current->next;
    }

    return 0;
}


/*
 * Merge adjacent free blocks.
 */
static void merge_free_blocks(void)
{
    struct heap_block *current = heap_first;

    while (current != 0 && current->next != 0)
    {
        if (current->free &&
            current->next->free)
        {
            current->size +=
                sizeof(struct heap_block) +
                current->next->size;

            current->next =
                current->next->next;

            continue;
        }

        current = current->next;
    }
}


/*
 * Free previously allocated memory.
 */
void kfree(void *address)
{
    if (address == 0)
    {
        return;
    }

    struct heap_block *block =
        (struct heap_block *)(
            (uint8_t *)address -
            sizeof(struct heap_block)
        );

    if (block->magic != HEAP_MAGIC)
    {
        return;
    }

    block->free = 1;

    merge_free_blocks();
}


/*
 * Return the first heap block address.
 *
 * Used for debugging.
 */
uint32_t heap_get_position(void)
{
    return HEAP_START;
}
