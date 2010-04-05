/*
 * beep - just what it sounds like, makes the console beep - but with
 * precision control.  See the man page for details.
 *
 * Try beep -h for command line args
 *
 * This code is copyright (C) Johnathan Nightingale, 2000.
 *
 * This code may distributed only under the terms of the GNU Public License
 * which can be found at http://www.gnu.org/copyleft or in the file COPYING
 * supplied with this code.
 *
 * This code is not distributed with warranties of any kind, including implied
 * warranties of merchantability or fitness for a particular use or ability to
 * breed pandas in captivity, it just can't be done.
 *
 * Bug me, I like it:  http://johnath.com/  or johnath@johnath.com
 */

#define _XOPEN_SOURCE 500 /* Or: #define _BSD_SOURCE (for usleep) */

#include "beep.h"

#include <fcntl.h>          /* O_WRONLY */
#include <stdio.h>          /* printf() */
#include <unistd.h>         /* usleep() close() */
#include <sys/ioctl.h>      /* ioctl() */
#include <linux/kd.h>       /* KIOCSOUND */

#ifndef CLOCK_TICK_RATE
#define CLOCK_TICK_RATE 1193180
#endif

/* Meaningful Defaults */
//#define DEFAULT_FREQ        440.0   /* Middle A */
//#define DEFAULT_LENGTH      200     /* milliseconds */
//#define DEFAULT_REPS        1
//#define DEFAULT_DELAY       100     /* milliseconds */

typedef struct beep_parms_t {
    float freq;                     /* tone frequency (Hz)                  */
    unsigned int length;            /* tone length    (ms)                  */
    unsigned int reps;              /* number of repetitions                */
    unsigned int delay;             /* delay between reps  (ms)             */
    int inc;                        /* increment of freq for each rep (Hz)  */
} beep_parms_t;


void play_beep(beep_parms_t beep) {
    int console_fd = open("/dev/console", O_WRONLY);
    if(console_fd < 0) {
        fprintf(stderr, "Could not open /dev/console for writing.\n");
        printf("\a");  /* Output the only beep we can, in an effort to fall back on usefulness */
        return;
    }

    for (register unsigned int i = 0; i < beep.reps; i++) {
        /* start beep */
        if(ioctl(console_fd, KIOCSOUND, (int)(CLOCK_TICK_RATE/beep.freq)) < 0) {
            fprintf(stderr, "Could not write to /dev/console.\n");
            printf("\a");  /* Output the only beep we can, in an effort to fall back on usefulness */
        }
        usleep(1000 * beep.length);
        ioctl(console_fd, KIOCSOUND, 0); /* stop beep  */
        if(i+1 < beep.reps) {
            usleep(1000 * beep.delay);
            beep.freq += beep.inc;
        }
    }

    close(console_fd);
}

void beep(float frequency, unsigned int length, unsigned int repetitions,
        unsigned int delay, int increment) {
    beep_parms_t beep;
    beep.freq = frequency;
    beep.length = length;
    beep.reps = repetitions;
    beep.delay = delay;
    beep.inc = increment;
    play_beep(beep);
}