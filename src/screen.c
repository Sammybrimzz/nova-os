#include <stdint.h>

#define VIDEO_MEMORY 0xB8000
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

static volatile char *video = (volatile char *)VIDEO_MEMORY;

static int cursor = 0;

void screen_clear(void)
{
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
    {
        video[i * 2] = ' ';
        video[i * 2 + 1] = 0x07;
    }

    cursor = 0;
}

void screen_put_char(char c)
{
    if (c == '\n')
    {
        cursor = ((cursor / SCREEN_WIDTH) + 1) * SCREEN_WIDTH;
        return;
    }

    if (c == '\b')
    {
        if (cursor > 0)
        {
            cursor--;

            video[cursor * 2] = ' ';
            video[cursor * 2 + 1] = 0x07;
        }

        return;
    }

    video[cursor * 2] = c;
    video[cursor * 2 + 1] = 0x07;

    cursor++;

    if (cursor >= SCREEN_WIDTH * SCREEN_HEIGHT)
    {
        cursor = 0;
    }
}

void screen_print(const char *text)
{
    for (int i = 0; text[i] != '\0'; i++)
    {
        screen_put_char(text[i]);
    }
}
