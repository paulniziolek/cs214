#include "mysh.h"
#include "builtins.h"
#include <glob.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>

//GLOBALS
int last_status = 0;
int bash_mode = 0;
char input_buffer[MAXLINE];
int input_buffer_idx = MAXLINE;
int input_buffer_size = MAXLINE;
int input_fd = 0;
int invalid_cmd = 0;

//HELPER FUNCTIONS
void panic(char *msg) {
    write(1, msg, strlen(msg));
    exit(1);
}
void debug(char *msg) {
    write(1, msg, strlen(msg));
}

void shell_print(char *msg) {
    write(1,"mysh> ", strlen("mysh> "));
    write(1, msg, strlen(msg));
}
void expandArgs(char* argv[], char* eargv[]);

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

    ecmd->eargv[0] = malloc(strlen(name));
    strcpy(ecmd->eargv[0], name);
    
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
    bcmd->argv[0] = (char *)malloc(strlen("builtincmd"));
    bcmd->eargv[0] = (char *)malloc(strlen("builtincmd"));
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
    if(cmd == NULL) return;
    struct conditioncmd *ccmd = NULL;
    struct execcmd *ecmd = NULL;
    struct redircmd *rcmd = NULL;
    struct pipecmd *pcmd = NULL;
    struct builtincmd *bcmd = NULL;

    switch(cmd->type){
        case conditioncmd:
            ccmd = (struct conditioncmd *) cmd;
            free_cmd(ccmd->cmd);
            free(ccmd);
            break;
        case execcmd:
            ecmd = (struct execcmd *) cmd;
            for(int i = 0; i < MAXARGS && ecmd->argv[i]!=NULL; i++) free(ecmd->argv[i]);
            for(int i = 0; i < MAXARGS && ecmd->eargv[i]!=NULL; i++) free(ecmd->eargv[i]);
            free(ecmd);
            break;
        case redircmd:
            rcmd = (struct redircmd *) cmd;
            free_cmd(rcmd->cmd);
            free(rcmd->file);
            free(rcmd);
            break;
        case pipecmd:
            pcmd = (struct pipecmd *) cmd;
            free_cmd(pcmd->left);
            free_cmd(pcmd->right);
            free(pcmd);
            break;
        case builtincmd:
            bcmd = (struct builtincmd *) cmd;
            for(int i = 0; i < MAXARGS && bcmd->argv[i]!=NULL; i++) free(bcmd->argv[i]);
            for(int i = 0; i < MAXARGS && bcmd->eargv[i]!=NULL; i++) free(bcmd->eargv[i]);
            free(bcmd);
            break;
        default:
            panic("unknown\n");
    }
}


//EXECUTE COMMANDS


void runcmd(struct cmd *cmd){
    if(cmd == NULL) panic("invalid cmd\n");
    struct conditioncmd *ccmd = NULL;
    struct execcmd *ecmd = NULL;
    struct redircmd *rcmd = NULL;
    struct pipecmd *pcmd = NULL;
    struct builtincmd *bcmd = NULL;
    int fd, ogstdin, ogstdout; 
    int pipefd[2];
    pid_t pid;


    switch (cmd->type){
        case execcmd:
            ecmd = (struct execcmd *) cmd;
            pid = fork();
            if(pid < 0) {
                debug("fork failed\n");
                last_status = 1;
                break;
            }
            if(pid == 0) {
                execv(_getExecPath(ecmd->argv[0]), ecmd->eargv);
                panic("exec failed\n"); //this is fine because terminates child process with status 1
            }
            else waitpid(pid, &last_status, 0);
            
            break;
        case redircmd:
            rcmd = (struct redircmd *) cmd;
            switch(rcmd->mode){
                case REDIR_IN:
                    fd = open(rcmd->file, O_RDONLY), ogstdin = dup(0);
                    if(fd < 0) {
                        debug("could not open file\n"); 
                        last_status = 1;
                        break;
                    }
                    dup2(fd, 0);
                    runcmd(rcmd->cmd);
                    dup2(ogstdin, 0);
                    break;
                case REDIR_OUT:
                    fd = open(rcmd->file, O_WRONLY | O_CREAT | O_TRUNC, 0640), ogstdout = dup(1);
                    if(fd < 0){
                        debug("could not open file\n"); 
                        last_status = 1;
                        break;
                    }
                    dup2(fd, 1);
                    runcmd(rcmd->cmd);
                    dup2(ogstdout, 1);
                    break;
                default:
                    panic("unknown redirection mode\n");
            }
            break;
        case pipecmd:
            pcmd = (struct pipecmd *) cmd;
            if(pipe(pipefd) == -1) {
                debug("pipe failed\n");
                last_status = 1;
                break;
            }
            ogstdin = dup(0), ogstdout = dup(1);

            dup2(pipefd[1], 1);
            runcmd(pcmd->left);
            dup2(ogstdout, 1);

            dup2(pipefd[0], 0);
            runcmd(pcmd->right);
            dup2(ogstdin, 0);

            break;
        case builtincmd:
            bcmd = (struct builtincmd *) cmd;
            switch (bcmd->mode){
                case cd:
                    last_status=execcd(bcmd->eargv[1]);
                    break;
                case which:
                    last_status=execwhich(1, bcmd->eargv[1]);
                    break;
                case pwd:
                    last_status=execpwd(1);
                    break;
                default:
                    panic("unknown built in command\n");
            }
            break;
        case conditioncmd:
            ccmd = (struct conditioncmd *) cmd;
            if(ccmd->mode == _then && last_status == 0) runcmd(ccmd->cmd);
            else if(ccmd->mode == _else && last_status != 0) runcmd(ccmd->cmd);
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
    return i;
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
    if(tok == NULL) {
        invalid_cmd = 1;
        return cmd; //something went wrong
    }
    if(first && !strcmp(tok,"exit")) {
        if(!bash_mode) shell_print("goodbye :)\n");
        exit(0);
    }
    //conditions
    if(!strcmp(tok,"then") || !strcmp(tok,"else")){   
        if(!first) invalid_cmd = 1; //set invalid_cmd flag
        ccmd = build_ccmd((!strcmp(tok,"then")) ? _then : _else, parsecmd(NULL, false));
        cmd = (struct cmd *) ccmd;
    } //built in commands
    else if(!strcmp(tok,"cd") || !strcmp(tok,"which") || !strcmp(tok,"pwd")){
        int mode = (!strcmp(tok,"cd")) ? cd : (!strcmp(tok,"which")) ? which : pwd;
        bcmd = build_bcmd(mode);
        cmd = (struct cmd *) bcmd;

        int i = 1;
        bcmd->argv[1] = NULL;
        while((tok = strtok(NULL, " \n")) != NULL){
            if(i >= MAXARGS) panic("too many arguments");
            if(!strcmp(tok,"<") || !strcmp(tok,">")){
                char *ntok = strtok(NULL, " \n");
                if(ntok == NULL) panic("expected file name after redirection\n");
                
                rcmd = build_rcmd(cmd, ntok, (!strcmp(tok,"<")) ? REDIR_IN : REDIR_OUT);
                cmd = (struct cmd *) rcmd;

                continue;
            }
            else if(!strcmp(tok,"|")){
                pcmd = build_pcmd(cmd, parsecmd(NULL, false));
                cmd = (struct cmd *) pcmd;

                return cmd;
            }

            bcmd->argv[i] = (char *)malloc(strlen(tok));
            strcpy(bcmd->argv[i], tok);
            i++;
            bcmd->argv[i]=NULL;
        }
        expandArgs(bcmd->argv, bcmd->eargv);
    }
    else{ //exec commands
        ecmd = build_ecmd(tok);
        cmd = (struct cmd *) ecmd;

        int i = 1;
        ecmd->argv[1] = NULL;
        while((tok = strtok(NULL, " \n")) != NULL){
            if(i >= MAXARGS) panic("too many arguments");
            if(!strcmp(tok,"<") || !strcmp(tok,">")){
                char *ntok = strtok(NULL, " \n");
                if(ntok == NULL) panic("expected file name after redirection\n");
                
                rcmd = build_rcmd(cmd, ntok, (!strcmp(tok,"<")) ? REDIR_IN : REDIR_OUT);
                cmd = (struct cmd *) rcmd;

                continue;
            }
            else if(!strcmp(tok,"|")){
                pcmd = build_pcmd(cmd, parsecmd(NULL, false));
                cmd = (struct cmd *) pcmd;
                return cmd;
            }

            ecmd->argv[i] = (char *)malloc(strlen(tok));
            strcpy(ecmd->argv[i], tok);

            i++;
            ecmd->argv[i]=NULL;
        }
        expandArgs(ecmd->argv, ecmd->eargv);
    }
    return cmd;
}

void expandArgs(char* argv[], char* eargv[]) {
    int numEargs = 1;
    for (int i = 1; i < MAXARGS; i++) {
        if (argv[i] == NULL) break;
        if (strstr(argv[i], "*") == NULL) {
            // not a wildcard
            eargv[numEargs] = argv[i];
            numEargs++;
            continue;
        };

        // argv[i] has wildcard token(s)
        glob_t glob_result;
        struct stat statbuf;
        glob(argv[i], 0, NULL, &glob_result);

        if (glob_result.gl_pathc == 0) {
            // no matches
            eargv[numEargs++] = argv[i];
            if (numEargs >= MAXARGS) panic("too many arguments");
            continue;
        }
        for (size_t j = 0; j < glob_result.gl_pathc; ++j) {
            // only add to eargs if is path to file, and not path to dir
            if (stat(glob_result.gl_pathv[j], &statbuf) == 0) {
                if (S_ISREG(statbuf.st_mode)) {
                    eargv[numEargs++] = strdup(glob_result.gl_pathv[j]);
                    if (numEargs >= MAXARGS) panic("too many arguments");
                }
            }
        }
        globfree(&glob_result);
    }
    eargv[numEargs] = NULL;
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
        if(cmd == NULL || invalid_cmd) {
            invalid_cmd = 0;
            debug("invalid command\n");
        }
        else runcmd(cmd);
        free_cmd(cmd);
    }
    return 0;    
}