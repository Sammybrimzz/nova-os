
extern void gdt_flush(void);

void gdt_init(void)
{
    gdt_flush();
}
