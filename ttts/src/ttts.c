#include <stdio.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <pthread.h>

#include "ttt_command.h"
#include "ttt_misc.h"
#include "sbuf.h"

#define PORT "3490"
#define MAXBUF 1024
#define BACKLOG 512
#define DELIM "|"
#define UNNAMED "unnamed"

#define SA struct sockaddr

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void *get_in_port(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_port);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_port);
}

void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}

/* pthread condition for when we can construct a game thread (2 or more players on queue) */
pthread_cond_t enoughPlayers = PTHREAD_COND_INITIALIZER;
pthread_mutex_t queueMutex = PTHREAD_MUTEX_INITIALIZER;

/* Asynchronous Queue for players waiting for a game */
sbuf_t playerQueue;


// Threads
void *listener_thread(void *vargp);
void *game_thread(void *vargp);
void *handleConnection(void *vargp);

// Forward Declarations
GameSession *makeGameSession();
void checkBoard(GameSession *session);
void processCommand(Player *player, Command *cmd);
void processPlayer(GameSession *session, Player *player);
void move(GameSession *session, Player *player, Command *cmd);
void resign(GameSession *session, Player *player);
void sendOver(GameSession *session, Player *player);
Player *getWinner(GameSession *session, PlayerSymbol s);
Player *acceptPlayer(int listenfd);
Command* receiveMessage(Player *player);
int openlistenfd();

int main() {
    int *listenfd = malloc(sizeof(int));
    struct sigaction sa;
    pthread_t listener;
    
    *listenfd = openlistenfd();

    sbuf_init(&playerQueue, BACKLOG);

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("server: successfully started! waiting for connections...\n");
    pthread_create(&listener, NULL, listener_thread, (listenfd));

    // TODO: add thread pools
    for (;;) {
        pthread_mutex_lock(&queueMutex);
        while (sbuf_size(&playerQueue) < 2) {
            pthread_cond_wait(&enoughPlayers, &queueMutex);
        };
        pthread_t gameThread;
        GameSession* gsession = makeGameSession();

        // we can unlock mutex after game session is created.
        pthread_mutex_unlock(&queueMutex);

        pthread_create(&gameThread, NULL, game_thread, gsession);
    }

}

void *listener_thread(void *vargp) {
    int listenfd = *(int *)vargp;
    for (;;) {
        pthread_t tid;
        Player *player = acceptPlayer(listenfd);
        pthread_create(&tid, NULL, handleConnection, player);
        pthread_detach(tid);
        
        pthread_mutex_lock(&queueMutex);
        if (sbuf_size(&playerQueue) >= 2) {
            pthread_cond_signal(&enoughPlayers);
        }
        pthread_mutex_unlock(&queueMutex);

    }
}

void *handleConnection(void *vargp) {
    Player* player = (Player *)vargp;
    bool isPlaying = false;

    for (;;) {
        Command *cmd = receiveMessage(player);
        // should only expect PLAY or RSGN.
        if (cmd->command == PLAY) {
            processCommand(player, cmd);
            sbuf_insert(&playerQueue, player);
            pthread_exit(NULL);
        }
        if (cmd->command == RSGN) {
            processCommand(player, cmd);
            pthread_exit(NULL);
        }

        processCommand(player, cmd);
    }
}

// Some thread should take responsibility of continiously watching acceptPlayer and accept()
Player *acceptPlayer(int listenfd) {
    socklen_t sin_size = sizeof(struct sockaddr_storage);
    Player *player = malloc(sizeof(Player));
    player->name = UNNAMED;

    player->playerfd = accept(listenfd, ((SA *) &player->addr), &sin_size);
    if (player->playerfd == -1) {
        fprintf(stderr, "server: failed accept() call");
        perror("accept()");
        return NULL;
    }
    player->ipAddress = malloc(INET6_ADDRSTRLEN);
    inet_ntop(player->addr.ss_family,
        get_in_addr((SA *)&player->addr), player->ipAddress, INET6_ADDRSTRLEN);
    printf("server: got connection from %s\n", player->ipAddress);

    return player;
}

// Called only when 2 players are on the queue
// BUG: Does not handle player disconnects when they are on the queue.
// Consider putting a nonblocking recv call to read any disconnects before starting the thread.
GameSession *makeGameSession() {
    GameSession *session = malloc(sizeof(GameSession));
    session->gameState = malloc(sizeof(GameState));
    memset(session->gameState, 0, sizeof(session->gameState));
    session->gameState->turn = Cross; // Cross always moves first

    printf("server: making new game session...\n");

    session->player1 = (Player *) sbuf_remove(&playerQueue);
    session->player2 = (Player *) sbuf_remove(&playerQueue);

    // set symbol
    if (rand()%2==0) {
        session->player1->symbol = Nought;
        session->player2->symbol = Cross;
    } else {
        session->player1->symbol = Cross;
        session->player2->symbol = Nought;
    }

    return session;
}

void *game_thread(void *vargp) {
    GameSession* session = (GameSession *)vargp;
    char p1msg[MAXBUF], p2msg[MAXBUF];
    int p1fd = session->player1->playerfd;
    int p2fd = session->player2->playerfd;
    fd_set fdread, fdready;
    FD_ZERO(&fdread);
    FD_SET(p1fd, &fdread);
    FD_SET(p2fd, &fdread);

    printf("thread %ld: starting new game with players %s (%s) and %s (%s)\n", 
        pthread_self(), 
        session->player1->ipAddress, session->player1->name, 
        session->player2->ipAddress, session->player2->name
    );

    snprintf(p1msg, MAXBUF, "BEGN|%s|%s|", PlayerSymbolToString(session->player1->symbol), session->player2->name);
    snprintf(p2msg, MAXBUF, "BEGN|%s|%s|", PlayerSymbolToString(session->player2->symbol), session->player1->name);
    printf("thread %ld: sending %s to %s\n", pthread_self(), p1msg, session->player1->ipAddress);
    if (send(p1fd, p1msg, strlen(p1msg), 0) == -1) {
        perror("send()");
        exit(0);
    }
    printf("thread %ld: sending %s to %s\n", pthread_self(), p2msg, session->player2->ipAddress);
    if (send(p2fd, p2msg, strlen(p2msg), 0) == -1) {
        perror("send()");
        exit(0);
    }

    for (;;) {
        fdready = fdread;
        select(p1fd+p2fd+1, &fdready, NULL, NULL, NULL);
        if (FD_ISSET(p1fd, &fdready)) {
            processPlayer(session, session->player1);
        }
        if (FD_ISSET(p2fd, &fdready)) {
            processPlayer(session, session->player2);
        }
        // check board for win.
        checkBoard(session);

        // check game status, and end if not active
        if (session->status == Over) {
            sbuf_insert(&playerQueue, session->player1);
            sbuf_insert(&playerQueue, session->player2);
            free(session);

            pthread_mutex_lock(&queueMutex);
            if (sbuf_size(&playerQueue) >= 2) {
                pthread_cond_signal(&enoughPlayers);
            }
            pthread_mutex_unlock(&queueMutex);
            
            pthread_exit(NULL);
        }
    }
}

Player *getWinner(GameSession *session, PlayerSymbol s) {
    if (session->player1->symbol==s) {
        return session->player1;
    } else if (session->player2->symbol==s) {
        return session->player2;
    }
    return NULL;
}

void checkBoard(GameSession *session) {
    GameState *s = session->gameState;
    // Check rows
    for (int row = 0; row < 3; row++) {
        if (s->board[row][0] != Unspecified && s->board[row][0] == s->board[row][1] && s->board[row][1] == s->board[row][2]) {
            Player *winner = getWinner(session, s->board[row][0]);
            sendOver(session, winner);
            return;
        }
    }
    // Check columns
    for (int col = 0; col < 3; col++) {
        if (s->board[0][col] != Unspecified && s->board[0][col] == s->board[1][col] && s->board[1][col] == s->board[2][col]) {
            Player *winner = getWinner(session, s->board[0][col]);
            sendOver(session, winner);
            return;
        }
    }
    // Check diagnol
    if (s->board[0][0] != Unspecified && s->board[0][0] == s->board[1][1] && s->board[1][1] == s->board[2][2]) {
        Player *winner = getWinner(session, s->board[0][0]);
        sendOver(session, winner);
        return;
    }
    if (s->board[0][2] != Unspecified && s->board[0][2] == s->board[1][1] && s->board[1][1] == s->board[2][0]) {
        Player *winner = getWinner(session, s->board[0][2]);
        sendOver(session, winner);
        return;
    }
    // Check for draw
    int gameOngoing = 0;
    for (int i=0; i<3; i++) {
        for (int j=0; j<3; j++) {
            if (s->board[i][j]==Unspecified) {
                gameOngoing = 1;
            }
        }
    }
    if (!gameOngoing) {
        sendOver(session, NULL);
        session->status = Over;
    }
    return;
}

void processCommand(Player *player, Command *cmd) {
    char pmsg[MAXBUF];
    switch (cmd->command) {
    case PLAY:
        if (cmd->numArgs < 1) break;
        player->name = cmd->args[0];
        snprintf(pmsg, MAXBUF, "WAIT|");
        printf("thread %ld: sending %s to %s\n", pthread_self(), pmsg, player->ipAddress);
        send(player->playerfd, pmsg, strlen(pmsg), 0);
        break;
    case DISC:
        printf("thread %ld: %s disconnected\n", pthread_self(), player->ipAddress);
        freePlayer(player);
        break;
    default:
        // cmd not recognized
        snprintf(pmsg, MAXBUF, "INVL|CMD not recognized or declined.|");
        printf("thread %ld: sending %s to %s\n", pthread_self(), pmsg, player->ipAddress);
        send(player->playerfd, pmsg, strlen(pmsg), 0);
    }
    free(cmd);
}

void processPlayer(GameSession *session, Player *player) {
    Command* cmd = receiveMessage(player);
    switch (cmd->command) {
    case MOVE:
        move(session, player, cmd);
        break;
    case RSGN:
        resign(session, player);
        break;
    case DISC:
        resign(session, player);
        freePlayer(player);
        break;
    default:
        processCommand(player, cmd);
    }
    free(cmd);
}

void move(GameSession *session, Player *player, Command *cmd) {
    GameState *state = session->gameState;
    char pmsg[MAXBUF];
    
    if (session->gameState->turn != player->symbol) {
        snprintf(pmsg, MAXBUF, "INVL|Not your turn|");
        printf("thread %ld: sending %s to %s\n", pthread_self(), pmsg, player->ipAddress);
        send(player->playerfd, pmsg, strlen(pmsg), 0);
        return;
    }

    if (cmd->numArgs < 2) {
        snprintf(pmsg, MAXBUF, "INVL|No move specified.|");
        printf("thread %ld: sending %s to %s\n", pthread_self(), pmsg, player->ipAddress);
        send(player->playerfd, pmsg, strlen(pmsg), 0);
        return;
    }

    int x = atoi(cmd->args[0]); int y = atoi(cmd->args[1]);
    if (state->board[x][y] != Unspecified) {
        snprintf(pmsg, MAXBUF, "INVL|Move invalid: spot is already taken.|");
        printf("thread %ld: sending %s to %s\n", pthread_self(), pmsg, player->ipAddress);
        send(player->playerfd, pmsg, strlen(pmsg), 0);
        return;
    }
    
    session->gameState->board[x][y] = player->symbol;

    snprintf(pmsg, MAXBUF, "MOVD|%d|%d|%d|", x, y, session->gameState->turn);
    printf("thread %ld: sending %s to %s\n", pthread_self(), pmsg, session->player1->ipAddress);
    send(session->player1->playerfd, pmsg, strlen(pmsg), 0);
    printf("thread %ld: sending %s to %s\n", pthread_self(), pmsg, session->player2->ipAddress);
    send(session->player2->playerfd, pmsg, strlen(pmsg), 0);

    if (state->turn == Cross) {
        session->gameState->turn = Nought;
    } else {
        session->gameState->turn = Cross;
    }
}

void resign(GameSession *session, Player *player) {
    if (player==session->player1) {
        sendOver(session, session->player2);
    } else {
        sendOver(session, session->player1);
    }
}

// Sends Over with player being the winner. 
// If NULL is specified as player, then a draw is inferred.
void sendOver(GameSession *session, Player *player) {
    char pmsg[MAXBUF];
    char p1res, p2res;

    if (player == session->player1) {
        p1res = 'L'; p2res = 'W';
    } else if (player == session->player2) {
        p1res = 'W'; p2res = 'L';
    } else {
        p1res = p2res = 'D';
    }
    session->status = Over;
    
    snprintf(pmsg, MAXBUF, "OVER|%c|", p1res);
    printf("thread %ld: sending %s to %s\n", pthread_self(), pmsg, session->player1->ipAddress);
    send(session->player1->playerfd, pmsg, strlen(pmsg), 0);
    
    snprintf(pmsg, MAXBUF, "OVER|%c|", p2res);
    printf("thread %ld: sending %s to %s\n", pthread_self(), pmsg, session->player2->ipAddress);
    send(session->player2->playerfd, pmsg, strlen(pmsg), 0);
}

Command* receiveMessage(Player *player) {
    char buf[MAXBUF];
    int numBytes;
    Command *cmd = malloc(sizeof(Command));
    cmd->numArgs = 0;

    if ((numBytes = recv(player->playerfd, buf, MAXBUF, 0)) <= 0) {
        if (numBytes == -1) perror("recv");
        printf("thread %ld: %s disconnected, sending resign message...\n", pthread_self(), player->ipAddress);
        cmd->command = DISC;
        return cmd;
    }
    buf[numBytes] = '\0';

    char *cmdPtr;
    char *cmdBuf;
    cmdBuf = strtok_r(buf, DELIM, &cmdPtr);
    cmd->command = stringToCommand(cmdBuf);
    while ((cmdBuf = strtok_r(NULL, DELIM, &cmdPtr)) != NULL) {
        cmd->args[cmd->numArgs++] = cmdBuf;
        if (cmd->numArgs >= MAXARGS) {
            printf("thread %ld: server parsed too many arguments.\n", pthread_self());
            break;
        }
    }

    printf("thread %ld: received %s ", pthread_self(), commandToString(cmd->command));
    for (int i=0; i<cmd->numArgs; i++) {
        printf("%s ", cmd->args[i]);
    }
    printf("from %s.\n", player->ipAddress);

    return cmd;
}

int openlistenfd() {
    char ipstr[INET6_ADDRSTRLEN];
    int listenfd;
    struct addrinfo *p, *listp, hints;
    int rc;
    int yes = 1;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rc = getaddrinfo(NULL, PORT, &hints, &listp)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s", gai_strerror(rc));
        exit(1);
    }
    
    // attempt to bind new listening socket to the first valid addrinfo
    for (p = listp; p; p=p->ai_next) {
        if ((listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(listenfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(listenfd);
            perror("server: bind");
            continue;
        }
        freeaddrinfo(listp);
        break;
    }

    if (p == NULL) {
        fprintf(stderr, "server: failed to bind");
        exit(1);
    }

    inet_ntop(p->ai_family, get_in_addr((SA *) &p->ai_addr), ipstr, sizeof(ipstr));
    uint16_t port = ntohs(*(uint16_t *)get_in_port((SA *) &p->ai_addr));

    printf("server: attempting to start server on %s, port %d\n", ipstr, port);

    if (listen(listenfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    return listenfd;
}