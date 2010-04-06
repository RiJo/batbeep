#include "daemon.h"

#define _XOPEN_SOURCE 500

#include <sys/stat.h>       /* umask() */
#include <unistd.h>         /* chdir() chroot() */
#include <stdio.h>
#include <stdlib.h>

int save_pid_to_file(char *pid_file, pid_t pid) {
    if (pid_file == NULL) {
        return -1;
    }
    FILE *file = fopen(pid_file, "w");
    if (file == NULL) {
        return -2;
    }
    fprintf(file, "%u\n", pid);
    fflush(file);
    fclose(file);
    return 0;
}

/*
 * Reference:
 * http://www.netzmafia.de/skripten/unix/linux-daemon-howto.html
 */
int daemonize(char *pid_file, char *run_dir, int change_root) {
    /* Fork new process */
    pid_t pid, sid;
    pid = fork();
    if (pid < 0) {
        fprintf(stderr, "Error: could not create child process\n");
        return 0;
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS); /* kill parent */
    }
    /* Change the file mode mask */
    umask(0);
    /* Create a new SID for the child process */
    sid = setsid();
    if (sid < 0) {
        fprintf(stderr, "Error: could not create new SID\n");
        return 0;
    }
    /* Save pid file */
    if (save_pid_to_file(pid_file, sid) < 0) {
        fprintf(stderr, "Error: could open pid file for writing: %s\n", pid_file);
        return 0;
    }
    /* Change the current working directory */
    if (chdir(run_dir) < 0) {
        fprintf(stderr, "Error: could not change working directory\n");
        return 0;
    }
    /* Change root to current directory */
    if (change_root && (chroot(run_dir) < 0)) {
        fprintf(stderr, "Error: could not change root directory\n");
        return 0;
    }
    /* Close out the standard file descriptors */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    return 1;
}
