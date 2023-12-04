#include "mysh.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>


//GLOBALS
int last_status = 0;
int bash_mode = 0;
char input_buffer[MAXLINE];
int input_buffer_idx = MAXLINE;
int input_buffer_size = MAXLINE;
int input_fd = 0;

//HELPER FUNCTIONS
void panic(char *msg) {
    write(1, msg, strlen(msg));
    exit(1);
}

void shell_print(char *msg) {
    write(1,"mysh> ", strlen("mysh> "));
    write(1, msg, strlen(msg));
}

//CONSTRUCTORS
struct conditioncmd *build_ccmd(int mode, struct cmd *cmd){
    struct conditioncmd *ccmd = malloc(sizeof(struct conditioncmd));
    ccmd->type = conditioncmd;
    ccmd->mode = mode;
    ccmd->cmd = cmd;
    return ccmd;
}
struct execcmd *build_ecmd(char *name){
    struct execcmd *ecmd = malloc(sizeof(struct execcmd));
    ecmd->type = execcmd;
    ecmd->argv[0] = malloc(strlen(name));
    strcpy(ecmd->argv[0], name);
    return ecmd;
}
struct pipecmd *build_pcmd(struct cmd *left, struct cmd *right){
    struct pipecmd *pcmd = malloc(sizeof(struct pipecmd));
    pcmd->type = pipecmd;
    pcmd->left = left;
    pcmd->right = right;
    return pcmd;
}
struct builtincmd *build_bcmd(int mode){
    struct builtincmd *bcmd = malloc(sizeof(struct builtincmd));
    bcmd->type = builtincmd;
    bcmd->mode = mode;
    return bcmd;
}
struct redircmd *build_rcmd(struct cmd *cmd, char *file, int mode){
    struct redircmd *rcmd = malloc(sizeof(struct redircmd));
    rcmd->type = redircmd;
    rcmd->cmd = cmd;
    rcmd->file = malloc(strlen(file)); 
    strcpy(rcmd->file, file);
    rcmd->mode = mode;
    rcmd->fd = (mode == REDIR_IN) ? 0 : 1;
    return rcmd;
}
//DESTRUCTOR
void free_cmd(struct cmd *cmd){
    //todo
}


//EXECUTE COMMANDS
void runcmd(struct cmd *cmd){
    struct conditioncmd *ccmd = NULL;
    struct execcmd *ecmd = NULL;
    struct redircmd *rcmd = NULL;
    struct pipecmd *pcmd = NULL;
    struct builtincmd *bcmd = NULL;

    switch (cmd->type){
        case execcmd:
            ecmd = (struct execcmd *) cmd;
            write(1, "execcmd\n", strlen("execcmd\n"));
            break;
        case redircmd:
            rcmd = (struct redircmd *) cmd;
            write(1, "redircmd\n", strlen("redircmd\n"));
            runcmd(rcmd->cmd);
            break;
        case pipecmd:
            pcmd = (struct pipecmd *) cmd;
            write(1, "pipecmd\n", strlen("pipecmd\n"));
            runcmd(pcmd->left);
            runcmd(pcmd->right);
            break;
        case builtincmd:
            bcmd = (struct builtincmd *) cmd;
            write(1, "builtincmd\n", strlen("builtincmd\n"));
            break;
        case conditioncmd:
            ccmd = (struct conditioncmd *) cmd;
            write(1, "conditioncmd\n", strlen("conditioncmd\n"));
            runcmd(ccmd->cmd);
            break;
        default:
            panic("unknown\n");
    }
}

//STRING PARSING
int readcmd(char *buff, size_t size) {
    if(!bash_mode) shell_print("");
    int i = 0;
    while(i < size){
        if(input_buffer_idx == input_buffer_size){
            input_buffer_size = read(input_fd, input_buffer, MAXLINE);
            input_buffer_idx = 0;
        }
        if(input_buffer_idx == input_buffer_size) break;
        if(input_buffer[input_buffer_idx] == '\n') {input_buffer_idx++;break;}
        buff[i] = input_buffer[input_buffer_idx];
        input_buffer_idx++;
        i++;
    }
    buff[i] = '\0';
}

//expect buff to be null terminated
struct cmd *parsecmd(char *buff, bool first){
    struct cmd *cmd = NULL;
    struct conditioncmd *ccmd = NULL;
    struct execcmd *ecmd = NULL;
    struct redircmd *rcmd = NULL;
    struct pipecmd *pcmd = NULL;
    struct builtincmd *bcmd = NULL;

    char *tok = strtok(buff, " \n");
    if(tok == NULL) panic("expected command\n");

    //TODO: print goodbye message when in console mode
    if(first && !strcmp(tok,"exit")) {
        if(!bash_mode) shell_print("goodbye :)\n");
        exit(0);
    }
    //conditions
    if(!strcmp(tok,"then") || !strcmp(tok,"else")){   
        if(!first) panic("conditions can only be used at the beginning of a line\n");
        ccmd = build_ccmd((!strcmp(tok,"then")) ? _then : _else, parsecmd(NULL, false));
        cmd = (struct cmd *) ccmd;
    } //built in commands
    else if(!strcmp(tok,"cd") || !strcmp(tok,"which") || !strcmp(tok,"pwd")){
        int mode = (!strcmp(tok,"cd")) ? cd : (!strcmp(tok,"which")) ? which : pwd;
        bcmd = build_bcmd(mode);
        cmd = (struct cmd *) bcmd;

        int i = 1;
        while((tok = strtok(NULL, " \n")) != NULL){
            if(i >= MAXARGS) panic("too many arguments");
            if(!strcmp(tok,"<") || !strcmp(tok,">")){
                char *ntok = strtok(NULL, " \n");
                if(ntok == NULL) panic("expected file name after redirection\n");
                
                rcmd = build_rcmd(cmd, ntok, (!strcmp(tok,"<")) ? REDIR_IN : REDIR_OUT);
                cmd = (struct cmd *) rcmd;

                i+=2;
                continue;
            }
            else if(!strcmp(tok,"|")){
                pcmd = build_pcmd(cmd, parsecmd(NULL, false));
                cmd = (struct cmd *) pcmd;
                return cmd;
            }

            bcmd->argv[i] = malloc(strlen(tok));
            strcpy(bcmd->argv[i], tok);

            i++;
        }
    }
    else{ //exec commands
        ecmd = build_ecmd(tok);
        cmd = (struct cmd *) ecmd;

        int i = 1;
        while((tok = strtok(NULL, " \n")) != NULL){
            if(i >= MAXARGS) panic("too many arguments");
            if(!strcmp(tok,"<") || !strcmp(tok,">")){
                char *ntok = strtok(NULL, " \n");
                if(ntok == NULL) panic("expected file name after redirection\n");
                
                rcmd = build_rcmd(cmd, ntok, (!strcmp(tok,"<")) ? REDIR_IN : REDIR_OUT);
                cmd = (struct cmd *) rcmd;

                i+=2;
                continue;
            }
            else if(!strcmp(tok,"|")){
                pcmd = build_pcmd(cmd, parsecmd(NULL, false));
                cmd = (struct cmd *) pcmd;
                return cmd;
            }

            ecmd->argv[i] = malloc(strlen(tok));
            strcpy(ecmd->argv[i], tok);

            i++;
        }
    }
    return cmd;
}

void print_cmd_type(int type){
    switch (type){
        case execcmd:
            printf("execcmd\n");
            break;
        case redircmd:
            printf("redircmd\n");
            break;
        case pipecmd:
            printf("pipecmd\n");
            break;
        case builtincmd:
            printf("builtincmd\n");
            break;
        case conditioncmd:
            printf("conditioncmd\n");
            break;
        default:
            printf("unknown\n");
            break;
    }
}

int main(int argc, char *argv[]) {
    char buff[MAXLINE+1]; //+1 for null terminator
    bash_mode = (argc>1);
    
    if(!bash_mode) input_fd = 0; //stdinput
    else input_fd = open(argv[1], O_RDONLY);

    if(input_fd < 0) panic("could not open file\n");

    while(readcmd(buff, MAXLINE)){
        struct cmd *cmd = parsecmd(buff, true);
        if(cmd == NULL) panic("invalid cmd\n");
        runcmd(cmd);
    }
    return 0;    
}