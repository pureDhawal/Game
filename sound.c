#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#else
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#endif



// ====== SOUND FUNCTIONS ======
void playInputSound()
{
#ifdef _WIN32
    Beep(750, 120);
#else
    printf("\a");
    fflush(stdout);
#endif
}

void playWinSound()
{
#ifdef _WIN32
    Beep(1000, 150);
    Beep(1200, 150);
    Beep(1500, 200);
#elif _APPLE_
    system("afplay /System/Library/Sounds/Tink.aiff 2>/dev/null &");
#else
    system("play -q -n synth 0.3 tri 800 fade 0 0.1 0.2 2>/dev/null &");
#endif
}

void playDrawSound()
{
#ifdef _WIN32
    Beep(700, 200);
    Beep(700, 200);
#elif _APPLE_
    system("afplay /System/Library/Sounds/Pop.aiff 2>/dev/null &");
#else
    printf("\a");
    fflush(stdout);
#endif
}
