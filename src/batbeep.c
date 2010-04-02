/*
    Copyright (C) 2010 Rikard Johansson

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

    This program includes a partly rewritten version of Johnathan Nightingale's
    program beep-1.2.2, http://johnath.com/
*/


/*
    Todo
        * Several different warnings in .settings(?)
*/

#define _XOPEN_SOURCE 500 /* Or: #define _BSD_SOURCE (for usleep) */

#include "batbeep.h"

#include "daemon.h"
#include "property.h"
#include "hashmap.h"
#include "beep.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h> /* usleep() */
#include <getopt.h> /* getopt_long() */

hashmap *settings = NULL;
hashmap *batt_info = NULL;

unsigned int poll_timeout;
unsigned int warning_level;
unsigned int warning_timeout;
unsigned int beep_frequency;
unsigned int beep_duration;
unsigned int beep_repetitions;
unsigned int beep_pause;

unsigned int sleep_timeout;
char *bat_state;
char *bat_capacity;
char *bat_remaining;
float capacity_factor;

char acpi[BUFFER_SIZE], acpi_alarm[BUFFER_SIZE], acpi_info[BUFFER_SIZE], acpi_state[BUFFER_SIZE];

void cleanup() {
    hm_delete(settings, NULL);
    hm_delete(batt_info, NULL);
}

void sigint_received(int sig) {
    fprintf(stderr, "\n\nInterrupted...\n\n");
    cleanup();
    exit(sig);
}

void assure_key(hashmap *map, char *key) {
    if(!hm_isset(map, key)) {
        fprintf(stderr, "Error: Could not find the key \"%s\"\n", key);
        hm_dump(map, stderr, NULL);
        exit(EXIT_FAILURE);
    }
}

void to_string(FILE *output, char *key, void *value) {
    fprintf(output, "%-30s%s\n", key, (char *)value);
}

unsigned int gcd(unsigned int high, unsigned int low) {
    if (high < low) {
        gcd(low, high);
    }
    unsigned int rest = high % low;
    if (rest == 0) {
        return low;
    }
    else {
        return gcd(low, rest);
    }
}

unsigned int max(unsigned int x, unsigned int y) {
    return (x > y) ? x : y;
}

void debug_print() {
    printf("%s, version %s\n", PROGRAM_NAME, PROGRAM_VERSION);
    printf("   compiled %s, %s\n\n", __DATE__, __TIME__);
    printf("Entering debug mode (non daemon)...\n\n");
    printf("%s:\n", SETTINGS_FILE);
    hm_print(settings, stdout, to_string);
    printf("\nSleep timeout = %d ms\n", sleep_timeout);
    printf("\nStarting poll...\n");
}

void print_help() {
    printf("Usage: %s [-b] [-d] [-h] [-v]\n", PROGRAM_NAME);
    printf("Continously beeps system speaker when battery is below certain level\n\n");
    printf("  -b, --beep                 plays a beep according to the config\n");
    printf("  -d, --debug                print debugging messages (non daemon)\n");
    printf("  -h, --help                 display this help and exit\n");
    printf("  -v, --version              output version information and exit\n");
}

void print_version() {
    printf("%s, version %s, released %s\n", PROGRAM_NAME, PROGRAM_VERSION, PROGRAM_DATE);
    printf("   compiled %s, %s\n\n", __DATE__, __TIME__);
    printf("%s\n", PROGRAM_AUTHORS);
}

void print_usage() {
    printf("Usage: %s [-b] [-d] [-h] [-v]\n", PROGRAM_NAME);
    printf("Try '%s --help' for more information.\n", PROGRAM_NAME);
}

void read_settings() {
    load_file(settings, SETTINGS_FILE);

    assure_key(settings, "acpi_location");
    assure_key(settings, "poll_timeout");
    assure_key(settings, "warning_level");
    assure_key(settings, "warning_timeout");
    assure_key(settings, "beep_frequency");
    assure_key(settings, "beep_duration");
    assure_key(settings, "beep_repetitions");
    assure_key(settings, "beep_pause");
    assure_key(settings, "bat_state");
    assure_key(settings, "bat_capacity");
    assure_key(settings, "bat_remaining");

    poll_timeout = atoi(hm_get(settings, "poll_timeout"));
    warning_level = atoi(hm_get(settings, "warning_level"));
    warning_timeout = atoi(hm_get(settings, "warning_timeout"));
    beep_frequency = atoi(hm_get(settings, "beep_frequency"));
    beep_duration = atoi(hm_get(settings, "beep_duration"));
    beep_repetitions = atoi(hm_get(settings, "beep_repetitions"));
    beep_pause = atoi(hm_get(settings, "beep_pause"));
    sleep_timeout = gcd(poll_timeout, warning_timeout);
}

void read_acpi() {
    load_file(batt_info, acpi_alarm);
    load_file(batt_info, acpi_info);
    load_file(batt_info, acpi_state);

    assure_key(batt_info, hm_get(settings, "bat_state"));
    assure_key(batt_info, hm_get(settings, "bat_capacity"));
    assure_key(batt_info, hm_get(settings, "bat_remaining"));

    bat_state = hm_get(batt_info, hm_get(settings, "bat_state"));
    bat_capacity = hm_get(batt_info, hm_get(settings, "bat_capacity"));
    bat_remaining = hm_get(batt_info, hm_get(settings, "bat_remaining"));
    capacity_factor = (float)atoi(bat_remaining) / (float)atoi(bat_capacity);
}


int main(int argc, char **argv) {
    (void) signal(SIGINT, sigint_received);

    settings = hm_create(10, 0.5, 7);
    batt_info = hm_create(10, 0.5, 7);

    read_settings();

    strcpy(acpi, hm_get(settings, "acpi_location"));
    strcpy(acpi_alarm, acpi);
    strcat(acpi_alarm, "/alarm\0");
    strcpy(acpi_info, acpi);
    strcat(acpi_info, "/info\0");
    strcpy(acpi_state, acpi);
    strcat(acpi_state, "/state\0");

    read_acpi();

    /* Handle main arguments */
    struct option opt_list[] = {
        {"beep",    0, NULL, 'b'},
        {"help",    0, NULL, 'h'},
        {"version", 0, NULL, 'v'},
        {"debug",   0, NULL, 'd'},
        {0,0,0,0}
    };
    int debugging = 0;
    int arg = EOF;
    while((arg = getopt_long(argc, argv, "bhvd", opt_list, NULL)) != EOF) {
        switch (arg) {
            case 'b':
                beep(beep_frequency, beep_duration, beep_repetitions, beep_pause);
                exit(EXIT_SUCCESS);
            case 'h':
                print_help();
                exit(EXIT_SUCCESS);
            case 'v':
                print_version();
                exit(EXIT_SUCCESS);
            case 'd':
                debugging = 1;
                break;
            default:
                print_usage();
                exit(EXIT_FAILURE);
        }
    }

    if (debugging) {
        debug_print();
    }
    else if (!daemonize(PID_FILE)) {
        exit(EXIT_FAILURE);
    }

    unsigned int poll_count = 0;
    unsigned int warning_count = 0;
    while (1) {
        if ((sleep_timeout * (poll_count + 1)) % poll_timeout == 0) {
            read_acpi();
            if (debugging) {
                printf("> Reading acpi\n");
            }
        }
        if ((sleep_timeout * (warning_count + 1)) % warning_timeout == 0) {
            if (capacity_factor <= (float)warning_level / 100.0 && strcmp(bat_state, "discharging") == 0) {
                beep(beep_frequency, beep_duration, beep_repetitions, beep_pause);
            }
            if (debugging) {
                printf("> Capacity: %.2f%%\tState: %s\n", capacity_factor * 100, bat_state);
            }
        }

        poll_count++;
        poll_count = poll_count % (poll_timeout / sleep_timeout);
        warning_count++;
        warning_count = warning_count % (warning_timeout / sleep_timeout);

        SLEEP(sleep_timeout);
    }

    cleanup();

    return EXIT_SUCCESS;
}