#ifndef MYSH_H
#define MYSH_H

#define MAXARGS 10
#define MAXTOKEN 100
#define MAXLINE 1000

//STRUCTS & ENUMS
enum mode {
    BASH,
    CONSOLE
};
enum CMDTYPE {
    execcmd,
    redircmd,
    pipecmd,
    builtincmd,
    conditioncmd
};

struct cmd {
    int type;
};
struct execcmd {
    int type; 
    char *argv[MAXARGS];
    char *eargv[MAXARGS]; //may change this to a linked list later
};

enum REDIRTYPE {
    REDIR_IN,
    REDIR_OUT,
};
struct redircmd {
    int type;
    struct cmd *cmd;
    char *file;
    int mode;
    int fd;
};

struct pipecmd {
    int type;
    struct cmd *left;
    struct cmd *right;
};

enum BUILTIN {
    cd,
    which,
    pwd,
};
struct builtincmd {
    int type;
    int mode;
    char *argv[MAXARGS];
    char *eargv[MAXARGS]; //may change this to a linked list later
};

enum CONDITIONMODE {
    _then,
    _else
};
struct conditioncmd {
    int type;
    int mode;
    struct cmd *cmd;
};

#endif