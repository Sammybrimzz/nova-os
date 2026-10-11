#ifndef NOVA_SCHEDULER_H
#define NOVA_SCHEDULER_H

#include <stdint.h>

void scheduler_init(void);

void scheduler_add_process(void *process);

void *scheduler_get_current(void);

void *scheduler_next(void);

#endif
