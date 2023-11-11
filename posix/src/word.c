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

    mapSet(wordCount, "not-good", 1);
    mapSet(wordCount,"paul",2);
    mapSet(wordCount,"asdasd",5);

    key_value *kv = mapKeyValuePairs(wordCount);
    qsort(kv, wordCount->size, sizeof(key_value), key_value_cmp);

    //TODO, turn printf into write()
    for (int i = 0; i < wordCount->size; i++) {
        printf("%s: %lld\n", kv[i].key, kv[i].value);
    }

    free(kv);
    mapDestroy(wordCount);
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

char* append(char c, char *word, size_t wordSize, size_t wordIndex) {
    if (wordIndex == wordSize) {
        wordSize *= 2;
        word = (char *)realloc(word, wordSize);
        if (!word) {
            writeErr("Memory reallocation for word failed");
            exit(EXIT_FAILURE);
        }
    }
    word[wordIndex] = c;
    return word;
}

void processFile(HashMap* map, const char* arg) {
    int fd = open(arg, O_RDONLY);
    if (fd == -1) {
        writeErr("File %s failed to open", arg);
        return;
    }

    size_t wordSize = INIT_WORD_SIZE;
    size_t bytesRead = 0;
    char buffer[BUFFER_SIZE];
    size_t wordIndex = 0;
    char c; // current character
    char* word = (char *)malloc(INIT_WORD_SIZE);
    if (!word) {
        writeErr("Memory allocation for word failed");
        exit(EXIT_FAILURE);
    }

    while((bytesRead = read(fd,buffer,BUFFER_SIZE))){
        for(int i = 0;i<bytesRead;i++){
            c = buffer[i];
            if(isalpha(c) || c=='\''){
                word = append(c, word, wordSize, wordIndex);
                wordIndex++;
            }else if(wordIndex && c == '-' && word[wordIndex-1] != '-'){
                word = append(c, word, wordSize, wordIndex);
                wordIndex++;
            }else{
                if(wordIndex!=0){ //add cur word
                    if(word[wordIndex-1] == '-') wordIndex--; //remove last '-'
                    word = append('\0', word, wordSize, wordIndex);
                    mapInc(map, word);
                    wordIndex = 0;
                }
            }
        }
    }
    //add last word
    if(wordIndex!=0){
        if(word[wordIndex-1] == '-') wordIndex--; //remove last '-'
        word = append('\0', word, wordSize, wordIndex);
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
