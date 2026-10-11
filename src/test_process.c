#include <stdint.h>

#define SYS_WRITE  1
#define SYS_GETPID 2
#define SYS_YIELD  3

static uint32_t syscall_write(const char *text)
{
    uint32_t result;

    __asm__ volatile (
        "int $0x80"
        : "=a"(result)
        : "a"(SYS_WRITE),
          "b"(text)
        : "memory"
    );

    return result;
}

static uint32_t syscall_getpid(void)
{
    uint32_t result;

    __asm__ volatile (
        "int $0x80"
        : "=a"(result)
        : "a"(SYS_GETPID)
        : "memory"
    );

    return result;
}

static uint32_t syscall_yield(void)
{
    uint32_t result;

    __asm__ volatile (
        "int $0x80"
        : "=a"(result)
        : "a"(SYS_YIELD)
        : "memory"
    );

    return result;
}

static void print_number(uint32_t number)
{
    char buffer[12];
    int i = 0;

    if (number == 0)
    {
        syscall_write("0");
        return;
    }

    while (number > 0)
    {
        buffer[i] =
            '0' + (number % 10);

        number /= 10;
        i++;
    }

    while (i > 0)
    {
        i--;

        char text[2];

        text[0] = buffer[i];
        text[1] = '\0';

        syscall_write(text);
    }
}

static void delay(void)
{
    for (volatile uint32_t i = 0;
         i < 30000000;
         i++)
    {
    }
}

void nova_process_test(void)
{
    while (1)
    {
        uint32_t pid =
            syscall_getpid();

        syscall_write("PROCESS PID: ");

        print_number(pid);

        syscall_write(" -> YIELD\n");

        syscall_yield();

        delay();
    }
}

void nova_process_test_2(void)
{
    while (1)
    {
        uint32_t pid =
            syscall_getpid();

        syscall_write("PROCESS PID: ");

        print_number(pid);

        syscall_write(" -> YIELD\n");

        syscall_yield();

        delay();
    }
}
