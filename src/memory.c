#include <stdint.h>

/*
 * One memory frame is 4 KiB.
 */
#define FRAME_SIZE 4096

/*
 * For this first allocator we'll manage
 * the first 16 MiB of physical memory.
 */
#define MEMORY_SIZE (16 * 1024 * 1024)

/*
 * Number of 4 KiB frames in 16 MiB.
 */
#define FRAME_COUNT (MEMORY_SIZE / FRAME_SIZE)

/*
 * One bit represents one frame.
 *
 * 0 = free
 * 1 = used
 */
#define BITMAP_SIZE (FRAME_COUNT / 8)

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
 * Initialize the frame allocator.
 *
 * Initially every frame is free.
 */
void memory_init(void)
{
    for (uint32_t i = 0; i < BITMAP_SIZE; i++)
    {
        frame_bitmap[i] = 0;
    }

    /*
     * Reserve the first 1 MiB.
     *
     * Our kernel is loaded around 1 MiB,
     * and we don't want the allocator
     * handing this memory out.
     */
    uint32_t reserved_frames = (1024 * 1024) / FRAME_SIZE;

    for (uint32_t frame = 0;
         frame < reserved_frames;
         frame++)
    {
        frame_set(frame);
    }
}

/*
 * Find and allocate one free frame.
 *
 * Returns the physical address of the frame.
 *
 * Returns 0 if no frame is available.
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
    uint32_t frame = address / FRAME_SIZE;

    if (frame < FRAME_COUNT)
    {
        frame_clear(frame);
    }
}

/*
 * Return the number of frames managed
 * by this allocator.
 */
uint32_t memory_total_frames(void)
{
    return FRAME_COUNT;
}

