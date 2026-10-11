#include <stdint.h>

#include "scheduler.h"
#include "process.h"


static process_t *scheduler_processes[MAX_PROCESSES];

static uint32_t scheduler_count = 0;

static uint32_t current_index = 0;


/*
 * Initialize scheduler.
 */
void scheduler_init(void)
{
    for (uint32_t i = 0;
         i < MAX_PROCESSES;
         i++)
    {
        scheduler_processes[i] = 0;
    }

    scheduler_count = 0;

    current_index = 0;
}


/*
 * Add a process to the scheduler.
 */
void scheduler_add_process(void *process)
{
    if (process == 0)
    {
        return;
    }

    if (scheduler_count >= MAX_PROCESSES)
    {
        return;
    }

    scheduler_processes[scheduler_count] =
        (process_t *)process;

    scheduler_count++;
}


/*
 * Return the currently selected process.
 */
void *scheduler_get_current(void)
{
    if (scheduler_count == 0)
    {
        return 0;
    }

    return scheduler_processes[current_index];
}


/*
 * Select and return the next process.
 */
void *scheduler_next(void)
{
    if (scheduler_count == 0)
    {
        return 0;
    }

    current_index++;

    if (current_index >= scheduler_count)
    {
        current_index = 0;
    }

    return scheduler_processes[current_index];
}


/*
 * Return the current process.
 */
process_t *scheduler_get_current_process(void)
{
    if (scheduler_count == 0)
    {
        return 0;
    }

    return scheduler_processes[current_index];
}


/*
 * Select the next process.
 */
process_t *scheduler_select_next(void)
{
    if (scheduler_count == 0)
    {
        return 0;
    }

    current_index++;

    if (current_index >= scheduler_count)
    {
        current_index = 0;
    }

    return scheduler_processes[current_index];
}


/*
 * Called by the timer interrupt.
 *
 * current_context is the ESP value pointing at
 * the currently saved interrupt context.
 *
 * The function returns the ESP value of the
 * process that should run next.
 */
uint32_t scheduler_tick(uint32_t current_context)
{
    /*
     * If no processes have been registered,
     * keep using the current interrupt context.
     */
    if (scheduler_count == 0)
    {
        return current_context;
    }


    /*
     * Get the currently running process.
     */
    process_t *current =
        scheduler_processes[current_index];


    /*
     * Save the current process's context.
     */
    if (current != 0)
    {
        current->context =
            (interrupt_context_t *)current_context;

        current->state =
            PROCESS_READY;
    }


    /*
     * Select the next process.
     */
    process_t *next =
        scheduler_select_next();


    /*
     * Safety check.
     */
    if (next == 0)
    {
        return current_context;
    }


    /*
     * Mark the selected process as running.
     */
    next->state =
        PROCESS_RUNNING;


    /*
     * Return the stack address containing
     * the next process's saved CPU context.
     */
    return (uint32_t)next->context;
}
