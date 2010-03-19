#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "property.h"

#define NL '\n'
#define SEPARATOR ':'
#define BUFFER_SIZE 1024

void load_file(hashmap *map, char *filename) {
    FILE *file = 0;
    file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Could open file: %s\n", filename);
        exit(EXIT_FAILURE);
    }
    load_properties(map, file);
    fclose(file);
}

void load_properties(hashmap *map, FILE *file) {
    int temp = 0;
    int key_done = 0, done = 0;
    char key[BUFFER_SIZE];
    char value[BUFFER_SIZE];
    
    memset(key, '\0', BUFFER_SIZE);
    memset(value, '\0', BUFFER_SIZE);
    
    register unsigned int i = 0;
    while (!done) {
        temp = fgetc(file);
        switch (temp) {
            case EOF:
                done = 1;
                // break;
            case NL:
                value[i] = '\0';
                char *data = malloc(sizeof(value) + 1);
                strcpy(data, value);
                if (strlen(key) > 0) {
                    hm_set(map, key, data);
                }
                memset(key, '\0', BUFFER_SIZE);
                memset(value, '\0', BUFFER_SIZE);
                key_done = 0;
                i = 0;
                break;
            case SEPARATOR:
                key[i] = '\0';
                i = 0;
                key_done = 1;
                break;
            case ' ':
            case '\t':
                if (i == 0) break;
                // break;
            default:
                if (key_done)
                    value[i++] = temp;
                else
                    key[i++] = temp;
                break;
        }
    }
}