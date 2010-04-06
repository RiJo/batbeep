#ifndef _BATBEEP_H_
#define _BATBEEP_H_ 1

#define PROGRAM_NAME        "batbeep"
#define PROGRAM_VERSION     "1.3.1"
#define PROGRAM_DATE        "2010-04-06"
#define PROGRAM_AUTHORS     "Rikard Johansson, 2010"

#define PID_FILE            "/var/run/" PROGRAM_NAME ".pid"
#define SETTINGS_FILE       "/etc/" PROGRAM_NAME ".conf"

#define BUFFER_SIZE         255
#define SLEEP(ms)           usleep(ms*1000)

#endif