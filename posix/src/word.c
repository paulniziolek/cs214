#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include "hashmap.h"

// PROTO
void processArgument(HashMap* map, const char* arg);
void processFile(HashMap* map, const char* arg);
void processDir(HashMap* map, const char* arg);

int main(int argc, char *argv[]){
    if (argc < 2) {
        fprintf(stderr, "Usage for files: build/posix [file/directory]");
        exit(EXIT_FAILURE);
    }

    HashMap* wordCount = initMap();

    for (int i = 1; i < argc; i++) {
        processArgument(wordCount, argv[i]);
    }

    // TODO: Sort word counts & return

}

void processArgument(HashMap* map, const char* arg) {
    struct stat entity;
    
    if (stat(arg, &entity) != 0) {
        fprintf(stderr, "stat() error: %s", arg);
        exit(EXIT_FAILURE);
    }

    if (S_ISREG(entity.st_mode)) {
        // arg is file
        processFile(map, arg);
        return;
    }
    if (S_ISDIR(entity.st_mode)) {
        // arg is dir
        processDir(map, arg);
        return;
    }
    fprintf(stderr, "Argument %s is not a file or directory", arg);
}

void processFile(HashMap* map, const char* arg) {
    
}

void processDir(HashMap* map, const char* arg) {

}
