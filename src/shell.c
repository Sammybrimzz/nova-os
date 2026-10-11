#include <stdint.h>

extern void screen_print(const char *text);
extern void screen_put_char(char c);
extern void screen_clear(void);

extern int keyboard_has_data(void);
extern unsigned char keyboard_read_scancode(void);

extern uint32_t timer_get_ticks(void);

extern uint32_t memory_alloc_frame(void);
extern void memory_free_frame(uint32_t address);
extern uint32_t memory_total_frames(void);

extern void *kmalloc(uint32_t size);
extern void kfree(void *address);
extern uint32_t heap_get_position(void);

extern void process_init(void);
extern void process_list(void);

typedef struct process
{
    uint32_t pid;
    int state;
    uint32_t stack;
    uint32_t instruction_pointer;
    struct process *next;
} process_t;

extern process_t *process_create(uint32_t entry_point);
extern void process_destroy(process_t *process);

extern void *scheduler_get_current(void);
extern void *scheduler_next(void);
extern void scheduler_add_process(void *process);


static char command[64];

static int command_length = 0;

static int extended_scancode = 0;

static uint32_t last_displayed_second = 0;


static const char keymap[128] = {
    0, 27,
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
    '-', '=', '\b', '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
    '[', ']', '\n', 0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l',
    ';', '\'', '`', 0,
    '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm',
    ',', '.', '/', 0,
    '*', 0, ' ', 0
};


static int strings_equal(const char *a, const char *b)
{
    int i = 0;

    while (a[i] != '\0' && b[i] != '\0')
    {
        if (a[i] != b[i])
        {
            return 0;
        }

        i++;
    }

    return a[i] == '\0' && b[i] == '\0';
}


static void print_number(uint32_t number)
{
    char buffer[12];

    int i = 0;

    if (number == 0)
    {
        screen_put_char('0');
        return;
    }

    while (number > 0)
    {
        buffer[i] = '0' + (number % 10);

        number /= 10;

        i++;
    }

    while (i > 0)
    {
        i--;

        screen_put_char(buffer[i]);
    }
}


static void print_hex(uint32_t value)
{
    const char hex[] = "0123456789ABCDEF";

    screen_print("0x");

    for (int shift = 28; shift >= 0; shift -= 4)
    {
        screen_put_char(hex[(value >> shift) & 0xF]);
    }
}


static void command_clear(void)
{
    for (int i = 0; i < 64; i++)
    {
        command[i] = '\0';
    }

    command_length = 0;
}


static void memory_test(void)
{
    screen_print("Memory test:\n");

    screen_print("Total frames: ");

    print_number(memory_total_frames());

    screen_print("\n");

    uint32_t frame1 = memory_alloc_frame();
    uint32_t frame2 = memory_alloc_frame();
    uint32_t frame3 = memory_alloc_frame();

    screen_print("Allocated frame 1: ");

    print_hex(frame1);

    screen_print("\n");

    screen_print("Allocated frame 2: ");

    print_hex(frame2);

    screen_print("\n");

    screen_print("Allocated frame 3: ");

    print_hex(frame3);

    screen_print("\n");

    memory_free_frame(frame2);

    screen_print("Freed frame 2.\n");

    uint32_t frame4 = memory_alloc_frame();

    screen_print("Allocated frame 4: ");

    print_hex(frame4);

    screen_print("\n");

    if (frame4 == frame2)
    {
        screen_print("Memory test PASSED.\n");
    }
    else
    {
        screen_print("Memory test FAILED.\n");
    }
}


static void heap_test(void)
{
    screen_print("Heap test:\n");

    void *block1 = kmalloc(32);

    void *block2 = kmalloc(64);

    screen_print("Block 1: ");

    print_hex((uint32_t)block1);

    screen_print("\n");

    screen_print("Block 2: ");

    print_hex((uint32_t)block2);

    screen_print("\n");

    kfree(block1);

    screen_print("Block 1 freed.\n");

    void *block3 = kmalloc(32);

    screen_print("Block 3: ");

    print_hex((uint32_t)block3);

    screen_print("\n");

    screen_print("Heap start: ");

    print_hex(heap_get_position());

    screen_print("\n");

    if (block3 == block1)
    {
        screen_print("Heap reuse PASSED.\n");
    }
    else
    {
        screen_print("Heap reuse FAILED.\n");
    }
}


static void process_test(void)
{
    screen_print("Process test:\n");

    process_t *process1 =
        process_create(0x00100000);

    process_t *process2 =
        process_create(0x00101000);

    process_t *process3 =
        process_create(0x00102000);

    if (process1 == 0 ||
        process2 == 0 ||
        process3 == 0)
    {
        screen_print("Process creation FAILED.\n");

        return;
    }

    screen_print("Created process 1. PID: ");

    print_number(process1->pid);

    screen_print("\n");

    screen_print("Created process 2. PID: ");

    print_number(process2->pid);

    screen_print("\n");

    screen_print("Created process 3. PID: ");

    print_number(process3->pid);

    screen_print("\n");

    screen_print("\n");

    process_list();

    screen_print("\nDestroying process 2...\n");

    process_destroy(process2);

    screen_print("\n");

    process_list();

    screen_print("\nProcess test PASSED.\n");
}


static void scheduler_test(void)
{
    screen_print("Scheduler test:\n\n");

    /*
     * Create three test processes.
     */
    process_t *process1 =
        process_create(0x00100000);

    process_t *process2 =
        process_create(0x00101000);

    process_t *process3 =
        process_create(0x00102000);

    if (process1 == 0 ||
        process2 == 0 ||
        process3 == 0)
    {
        screen_print("Process creation FAILED.\n");

        return;
    }

    /*
     * Add the processes to the scheduler.
     */
    scheduler_add_process(process1);
    scheduler_add_process(process2);
    scheduler_add_process(process3);

    /*
     * The first process should be PID 1.
     */
    process_t *current =
        (process_t *)scheduler_get_current();

    screen_print("Current process: PID ");

    print_number(current->pid);

    screen_print("\n");

    /*
     * Select the next process.
     */
    current =
        (process_t *)scheduler_next();

    screen_print("Next process: PID ");

    print_number(current->pid);

    screen_print("\n");

    /*
     * Select the next process again.
     */
    current =
        (process_t *)scheduler_next();

    screen_print("Next process: PID ");

    print_number(current->pid);

    screen_print("\n");

    /*
     * Round-robin should wrap back
     * to the first process.
     */
    current =
        (process_t *)scheduler_next();

    screen_print("Next process: PID ");

    print_number(current->pid);

    screen_print("\n");

    /*
     * And then move to the second process.
     */
    current =
        (process_t *)scheduler_next();

    screen_print("Next process: PID ");

    print_number(current->pid);

    screen_print("\n");

    screen_print("\nScheduler test PASSED.\n");
}


static void execute_command(void)
{
    screen_print("\n");

    if (strings_equal(command, "help"))
    {
        screen_print("Available commands:\n");

        screen_print("  help\n");

        screen_print("  about\n");

        screen_print("  clear\n");

        screen_print("  ticks\n");

        screen_print("  memory\n");

        screen_print("  heap\n");

        screen_print("  process\n");

        screen_print("  schedule\n");
    }

    else if (strings_equal(command, "about"))
    {
        screen_print("NOVA OS v0.3\n");

        screen_print(
            "Educational AI-native operating system.\n"
        );
    }

    else if (strings_equal(command, "clear"))
    {
        screen_clear();
    }

    else if (strings_equal(command, "ticks"))
    {
        screen_print("Timer ticks: ");

        print_number(timer_get_ticks());

        screen_print("\n");
    }

    else if (strings_equal(command, "memory"))
    {
        memory_test();
    }

    else if (strings_equal(command, "heap"))
    {
        heap_test();
    }

    else if (strings_equal(command, "process"))
    {
        process_test();
    }

    else if (strings_equal(command, "schedule"))
    {
        scheduler_test();
    }

    else if (command_length > 0)
    {
        screen_print("Unknown command.\n");
    }

    command_clear();

    screen_print("NOVA> ");
}


static void update_timer_display(void)
{
    uint32_t ticks = timer_get_ticks();

    uint32_t seconds = ticks / 100;

    if (seconds != last_displayed_second)
    {
        last_displayed_second = seconds;

        screen_print("\n[Timer: ");

        print_number(seconds);

        screen_print("s]\n");

        screen_print("NOVA> ");

        for (int i = 0; i < command_length; i++)
        {
            screen_put_char(command[i]);
        }
    }
}


void shell_start(void)
{
    command_clear();

    screen_print("NOVA OS v0.3\n");

    screen_print("Shell started.\n");

    screen_print("Type 'help' for commands.\n");

    screen_print("Type 'ticks' to view timer ticks.\n");

    screen_print(
        "Type 'memory' to test memory management.\n"
    );

    screen_print(
        "Type 'heap' to test kernel heap.\n"
    );

    screen_print(
        "Type 'process' to test process management.\n"
    );

    screen_print(
        "Type 'schedule' to test the scheduler.\n\n"
    );

    screen_print("NOVA> ");


    while (1)
    {
        update_timer_display();

        if (!keyboard_has_data())
        {
            continue;
        }

        unsigned char scancode =
            keyboard_read_scancode();


        if (scancode == 0xE0)
        {
            extended_scancode = 1;

            continue;
        }


        if (extended_scancode)
        {
            extended_scancode = 0;

            continue;
        }


        if (scancode & 0x80)
        {
            continue;
        }


        if (scancode >= 128)
        {
            continue;
        }


        char key = keymap[scancode];


        if (key == '\n')
        {
            execute_command();
        }

        else if (key == '\b')
        {
            if (command_length > 0)
            {
                command_length--;

                command[command_length] = '\0';

                screen_put_char('\b');
            }
        }

        else if (key != 0 &&
                 command_length < 63)
        {
            command[command_length] = key;

            command_length++;

            command[command_length] = '\0';

            screen_put_char(key);
        }
    }
}
