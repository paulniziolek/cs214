#ifndef TTT_COMMAND_H
#define TTT_COMMAND_H

#include <string.h>

#define MAXARGS 10

typedef enum Command_t{
    UNSPECIFIED,
    PLAY, 
    WAIT,
    MOVE, 
    RSGN, 
    DRAW,  
    BEGN, 
    MOVD, 
    INVL, 
    OVER,
    DISC,
} Command_t;

typedef struct Command {
    Command_t command;
    int numArgs;
    char* args[MAXARGS];
} Command;

char *commandToString(Command_t command) {
    switch (command) {
    case PLAY:
        return "PLAY";
    case WAIT:
        return "WAIT";
    case MOVE:
        return "MOVE";
    case RSGN:
        return "RSGN";
    case DRAW:
        return "DRAW";
    case BEGN:
        return "BEGN";
    case MOVD:
        return "MOVD";
    case INVL:
        return "INVL";
    case OVER:
        return "OVER";
    case DISC:
        return "DISC";
    case UNSPECIFIED:
    default:
        return "";
    }
}
Command_t stringToCommand(char *str) {
    if (strcmp(str, "PLAY")==0) {
        return PLAY;
    }
    if (strcmp(str, "WAIT")==0) {
        return WAIT;
    }
    if (strcmp(str, "MOVE")==0) {
        return MOVE;
    }
    if (strcmp(str, "RSGN")==0) {
        return RSGN;
    }
    if (strcmp(str, "DRAW")==0) {
        return DRAW;
    }
    if (strcmp(str, "MOVD")==0) {
        return MOVD;
    }
    if (strcmp(str, "INVL")==0) {
        return INVL;
    }
    if (strcmp(str, "OVER")==0) {
        return OVER;
    }
    if (strcmp(str, "DISC")==0) {
        return DISC;
    }
    return UNSPECIFIED;
}

#endif
