
#include <stdint.h>

/*
 * One memory frame is 4 KiB.
 */
#define FRAME_SIZE 4096

/*
 * Manage the first 16 MiB of physical memory.
 */
#define MEMORY_SIZE (16 * 1024 * 1024)
#define FRAME_COUNT (MEMORY_SIZE / FRAME_SIZE)
#define BITMAP_SIZE (FRAME_COUNT / 8)

/*
 * The heap occupies physical memory from 2 MiB to 3 MiB.
 */
#define HEAP_START 0x00200000
#define HEAP_END   0x00300000

/*
 * Defined by linker.ld.
 * This symbol marks the end of the kernel's linked sections.
 */
extern uint8_t _kernel_end;

static uint8_t frame_bitmap[BITMAP_SIZE];

/*
 * Mark a frame as used.
 */
static void frame_set(uint32_t frame)
{
    frame_bitmap[frame / 8] |= (1 << (frame % 8));
}

/*
 * Mark a frame as free.
 */
static void frame_clear(uint32_t frame)
{
    frame_bitmap[frame / 8] &= ~(1 << (frame % 8));
}

/*
 * Check whether a frame is used.
 */
static int frame_is_used(uint32_t frame)
{
    return (frame_bitmap[frame / 8] &
            (1 << (frame % 8))) != 0;
}

/*
 * Reserve every frame touched by [start, end).
 *
 * Round the beginning down and the end up so that
 * partially occupied frames are reserved too.
 */
static void reserve_region(uint32_t start, uint32_t end)
{
    uint32_t first_frame = start / FRAME_SIZE;
    uint32_t end_frame =
        (end / FRAME_SIZE) + ((end % FRAME_SIZE) != 0);

    if (first_frame > FRAME_COUNT)
        first_frame = FRAME_COUNT;

    if (end_frame > FRAME_COUNT)
        end_frame = FRAME_COUNT;

    for (uint32_t frame = first_frame;
         frame < end_frame;
         frame++)
    {
        frame_set(frame);
    }
}

/*
 * Initialize the physical frame allocator.
 */
void memory_init(void)
{
    for (uint32_t i = 0; i < BITMAP_SIZE; i++)
    {
        frame_bitmap[i] = 0;
    }

    /*
     * Reserve low memory and the complete kernel,
     * including its bitmap and other BSS variables.
     */
    reserve_region(0, (uint32_t)&_kernel_end);

    /*
     * Reserve the fixed heap independently.
     */
    reserve_region(HEAP_START, HEAP_END);
}

/*
 * Allocate one free physical frame.
 *
 * Returns its physical address, or 0 if exhausted.
 */
uint32_t memory_alloc_frame(void)
{
    for (uint32_t frame = 0;
         frame < FRAME_COUNT;
         frame++)
    {
        if (!frame_is_used(frame))
        {
            frame_set(frame);
            return frame * FRAME_SIZE;
        }
    }

    return 0;
}

/*
 * Release a previously allocated frame.
 */
void memory_free_frame(uint32_t address)
{
    /*
     * Reject unaligned addresses and address zero.
     */
    if (address == 0 || address % FRAME_SIZE != 0)
        return;

    uint32_t frame = address / FRAME_SIZE;

    if (frame < FRAME_COUNT)
    {
        /*
         * Never release memory reserved for the kernel
         * or the fixed heap.
         */
        uint32_t kernel_end =
            (uint32_t)&_kernel_end;

        uint32_t aligned_kernel_end =
            (kernel_end / FRAME_SIZE) * FRAME_SIZE;

        if (address < 0x00100000)
            return;

        if (address < aligned_kernel_end)
            return;

        if (address >= HEAP_START && address < HEAP_END)
            return;

        frame_clear(frame);
    }
}

/*
 * Return the number of managed physical frames.
 */
uint32_t memory_total_frames(void)
{
    return FRAME_COUNT;
}
