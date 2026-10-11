#include <stdint.h>
#include "tss.h"

static tss_entry_t tss;

extern void tss_flush(void);
extern void gdt_install_tss(uint32_t base, uint32_t limit);

uint32_t tss_get_address(void)
{
    return (uint32_t)&tss;
}

uint32_t tss_get_size(void)
{
    return sizeof(tss);
}

void tss_init(uint32_t kernel_stack)
{
    uint8_t *bytes = (uint8_t *)&tss;

    for (uint32_t i = 0; i < sizeof(tss); i++)
    {
        bytes[i] = 0;
    }

    tss.ss0 = 0x10;
    tss.esp0 = kernel_stack;
    tss.iomap_base = sizeof(tss);

    gdt_install_tss(tss_get_address(), tss_get_size() - 1);
    tss_flush();
}

void tss_set_kernel_stack(uint32_t kernel_stack)
{
    tss.esp0 = kernel_stack;
}
