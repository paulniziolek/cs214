#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include "hashmap.h"
#include "error.h"

#define BUFFER_SIZE 1024
#define INIT_WORD_SIZE 256

// PROTO
void processEntity(HashMap* map, const char* arg);
void processFile(HashMap* map, const char* arg);
void processDir(HashMap* map, const char* arg);
char* constructNewPath(const char* arg, const char* name);

int main(int argc, char *argv[]){
    if (argc < 2) {
        writeErr("Usage for posix: build/posix [file/directory]");
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
        writeErr("stat() error: %s", arg);
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

    writeErr("Argument %s is not a file or directory", arg);
}

void processDir(HashMap* map, const char* arg) {
    DIR* dir = opendir(arg);
    if (dir == NULL) {
        writeErr("Directory %s failed to open", arg);
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
        writeErr("File %s failed to open", arg);
        return;
    }
    // debugging only
    //printf("Reached processFile() for file %s\n", arg);

    size_t wordSize = INIT_WORD_SIZE;
    size_t wordIndex = 0;
    ssize_t bytesRead = 0;
    char buffer[BUFFER_SIZE];
    char* word = (char *)malloc(INIT_WORD_SIZE);
    if (!word) {
        writeErr("Memory allocation for word failed");
        exit(EXIT_FAILURE);
    }

    // 1. Keep reading file while bytesRead > 0;
    // 2. While word is not followed by any separator, we exponentially increase word size with realloc() and read characters in
    // 3. If word reaches a separator condition, we clean word (if needed, in cases such as double hyphen) and then add to hashmap
    // 4. Once loop from (1) is finished, we add in the word (if remaining) to hashmap since no separator follows it.

    while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0) {
        for (ssize_t i = 0; i < bytesRead; i++) {
            // check if - ' and other separators are included in isalnum()
            if (isalnum(buffer[i]) || buffer[i] == '-') {
                if (wordIndex >= wordSize - 1) {
                    // reallocate word
                    wordSize *= 2;
                    char *temp = (char *)realloc(word, wordSize);
                    if (!temp) {
                        writeErr("Memory reallocation for word failed");
                        exit(EXIT_FAILURE);
                    }
                    word = temp;
                }
                word[wordIndex] = buffer[i];
                wordIndex += 1;
                continue;
            }
            if  (wordIndex > 0) {
                word[wordIndex] = '\0';
                mapInc(map, word);
                wordIndex = 0;
            }
        }
    }

    if (wordIndex > 0) {
        word[wordIndex] = '\0';
        mapInc(map, word);
    }
    free(word);

    if (close(fd) == -1) {
        writeErr("File %s was interrupted during close()", arg);
    }
}

char* constructNewPath(const char* arg, const char* name) {
    // +2 for '/' and '\0' characters
    int newPathLen = strlen(arg) + strlen(name) + 2;
    char* newPath = (char *)malloc(newPathLen * sizeof(char));
    if (newPath == NULL) {
        writeErr("Memory allocation for path %s/%s failed", arg, name);
        exit(EXIT_FAILURE);
    }

    snprintf(newPath, newPathLen, "%s/%s", arg, name);
    return newPath;
}
