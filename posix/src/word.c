#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include "hashmap.h"

// PROTO
void processEntity(HashMap* map, const char* arg);
void processFile(HashMap* map, const char* arg);
void processDir(HashMap* map, const char* arg);
char* constructNewPath(const char* arg, const char* name);

int main(int argc, char *argv[]){
    if (argc < 2) {
        fprintf(stderr, "Usage for files: build/posix [file/directory]");
        exit(EXIT_FAILURE);
    }

    HashMap* wordCount = initMap();

    for (int i = 1; i < argc; i++) {
        processEntity(wordCount, argv[i]);
    }

    // TODO: Sort word counts & return

}

void processEntity(HashMap* map, const char* arg) {
    struct stat entity;
    
    if (stat(arg, &entity) != 0) {
        fprintf(stderr, "stat() error: %s", arg);
        exit(EXIT_FAILURE);
    }

    if (S_ISDIR(entity.st_mode)) {
        // arg is dir
        processDir(map, arg);
        return;
    }
    if (S_ISREG(entity.st_mode)) {
        // arg is file
        processFile(map, arg);
        return;
    }

    fprintf(stderr, "Argument %s is not a file or directory", arg);
}

void processDir(HashMap* map, const char* arg) {
    DIR* dir = opendir(arg);
    if (dir == NULL) {
        fprintf(stderr, "Directory %s failed to open", arg);
    }
    
    struct dirent* content;
    while ((content = readdir(dir)) != NULL) {
        // skipping parent & current directory
        if (strcmp(content->d_name, ".") == 0 || strcmp(content->d_name, "..") == 0) {
            continue;
        }

        char* path = constructNewPath(arg, content->d_name);
        processEntity(map, path);
        free(path);
    }

    closedir(dir);
}

void processFile(HashMap* map, const char* arg) {
    int fd = open(arg, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "File %s failed to open", arg);
        return;
    }

    // TODO: process files
    printf("Reached processFile() for file %s\n", arg); // debugging only

}

char* constructNewPath(const char* arg, const char* name) {
    // +2 for '/' and '\0' characters
    int newPathLen = strlen(arg) + strlen(name) + 2;
    char* newPath = (char *)malloc(newPathLen * sizeof(char));
    if (newPath == NULL) {
        fprintf(stderr, "Memory allocation for path failed");
        exit(EXIT_FAILURE);
    }

    snprintf(newPath, newPathLen, "%s/%s", arg, name);
    return newPath;
}
