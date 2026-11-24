#include <switch.h>
#include <SDL/SDL.h>
#include <stdint.h>

static uint64_t timer_start = 0;

unsigned int SDL_GetTicks(void)
{
    uint64_t now = armGetSystemTick();

    if (timer_start == 0)
        timer_start = now;

    uint64_t elapsed = now - timer_start;
    uint64_t nanoseconds = armTicksToNs(elapsed);

    return (unsigned int)(nanoseconds / 1000000ULL);
}

void SDL_Delay(unsigned int milliseconds)
{
    if (milliseconds == 0)
    {
        svcSleepThread(0);
        return;
    }

    svcSleepThread((int64_t)milliseconds * 1000000LL);
}
