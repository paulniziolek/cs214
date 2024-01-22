#ifndef TTT_MISC_H
#define TTT_MISC_H

#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef enum PlayerSymbol {
    Unspecified,
    Nought, 
    Cross,
} PlayerSymbol;

typedef enum Status {
    UnspecifiedStatus,
    Active,
    Over,
} Status;

char *PlayerSymbolToString(PlayerSymbol s) {
    switch (s) {
    case Nought:
        return "X";
    case Cross:
        return "O";
    default:
        return "";
    }
}

typedef struct Player {
    int playerfd;
    struct sockaddr_storage addr;
    char *ipAddress;
    const char *name;
    PlayerSymbol symbol;
} Player;

typedef struct GameState {
    int board[3][3];
    PlayerSymbol turn;
} GameState;

typedef struct GameSession {
    Player *player1;
    Player *player2;
    GameState *gameState;
    Status status;
} GameSession;


void freePlayer(Player *p) {
    close(p->playerfd);
    free(p);
}

void freeSession(GameSession *s) {
    free(s->gameState);
    free(s);
}

#endif