/*
  This file is part of the RSSReminder project.

  Copyright (C) 2011 glace <glacebai@gmail.com>
  Copyright (C) 2011 DoDo
  Copyright (C) 2011 kqdmqx <kqdmqx@gmail.com>
  Copyright (C) 2011 lucifer

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "simulate-shell.h"

int getCmd(char * outCmd, int maxLength){
    fgets(outCmd,maxLength,stdin);
    return 1;
}

int isodd(int num){
    return num % 2;
}

int specialSymbol(char ch){
    return ch == '|' || ch == '<' || ch == '>';
}

int errReport(const char * errinfo){
    printf("%s\n",errinfo);
    return 0;
}

int fetchWord(char * wordlist, char * word, int * offset){
    int total=0;
    char tword[100];
    int double_quote = 0;
    int single_quote = 0;
    int backslash = 0;
    int len, i;
    
    while (*offset<strlen(wordlist) && !isgraph(wordlist[*offset])) (*offset)++;
    if (*offset>=strlen(wordlist)) return 0;
    while (*offset<strlen(wordlist) 
        && ( (isgraph(wordlist[*offset]) && !specialSymbol(wordlist[*offset])) 
            || isodd(single_quote) || isodd(double_quote))){
                
        tword[total] = tolower(wordlist[*offset]);
        
        if (tword[total] == '\''){
            if (single_quote > 0 || double_quote == 0){
                if (single_quote > 0 && backslash) single_quote--;
                single_quote++;
            }
        }
        if (tword[total] == '"'){
            if (double_quote > 0 || single_quote == 0){
                if (double_quote > 0 && backslash) double_quote--;
                double_quote++;
            }
        }
        if (tword[total] == '\\') backslash = !backslash;
        else backslash = 0;
            
        total++;
        (*offset)++;
    }
    if (total == 0)
        while (*offset<strlen(wordlist) && specialSymbol(wordlist[*offset])){
            tword[total] = tolower(wordlist[*offset]);
            total++;
            (*offset)++;
        }
    tword[total]='\0';
    
    len = strlen(tword);
    if (len >= 2){
        if (tword[0] == tword[len-1] && (tword[0] == '\'' || tword[0] == '"')){
            for (i=0;i<len-2;i++) tword[i] = tword[i+1];
            tword[len-2] = '\0';
        }
    }
        
    strcpy(word, tword);
    return 1;
}

int printCmdnode(struct cmdnode_t target){
    printf("cmd: %s\n",target.cmd);
    printf("hasInputPipe : %9d    hasOutputPipe : %9d\n",target.hasInputPipe,target.hasOutputPipe);
    printf("hasInputFile : %9d    hasOutputFile : %9d\n",target.hasInputFile,target.hasOutputFile);
    printf("inputFilename: %9s    outputFilename: %9s\n\n",target.inputFilename,target.outputFilename);
    return 1;
}

int splitByPipe(char * InCmd, struct cmdnode_t * OutCmdList, int * cmdtotal){
    int offset = 0;
    int argc = 0;
    char word[MAX_CMD_LENGTH];
    char cmd[MAX_CMD_LENGTH];
    int hasInputFile,hasOutputFile;
    char inputFilename[30];
    char outputFilename[30];
    
    *cmdtotal = 0;
    hasInputFile = 0;
    hasOutputFile = 0;
    strcpy(inputFilename, "");
    strcpy(outputFilename, "");
    strcpy(cmd, "");
    strcat(InCmd, "|");
    
    while (fetchWord(InCmd, word, &offset)){
        if (strcmp(word, "<") == 0)
            hasInputFile = 1;
        else if (strcmp(word, ">") == 0)
            hasOutputFile = 1;
        else if (strcmp(word, "|") == 0){
            OutCmdList[*cmdtotal].hasInputFile = hasInputFile;
            OutCmdList[*cmdtotal].hasOutputFile = hasOutputFile;
            if (hasInputFile && strcmp(inputFilename, "") == 0) return errReport("inputname not detected.");
            if (hasOutputFile && strcmp(outputFilename, "") == 0) return errReport("outputname not detected.");
            
            if (*cmdtotal > 0) OutCmdList[*cmdtotal].hasInputPipe = 1;
            else OutCmdList[*cmdtotal].hasInputPipe = 0;
            if ((*cmdtotal) > 0){
                OutCmdList[*cmdtotal-1].hasOutputPipe = 1;
                //printf("23432   %d\n",OutCmdList[*cmdtotal-1].hasOutputPipe);
            }
            OutCmdList[*cmdtotal].hasOutputPipe = 1;
            
            strcpy(OutCmdList[*cmdtotal].inputFilename, inputFilename);
            strcpy(OutCmdList[*cmdtotal].outputFilename, outputFilename);
            
            strcpy(OutCmdList[*cmdtotal].cmd, cmd);
            
            OutCmdList[*cmdtotal].argc = argc;
            OutCmdList[*cmdtotal].cmdargv[argc] = NULL;
            
            //if (pipe(OutCmdList[*cmdtotal].pipe_fd) < 0) return errReport("pipe create failed.");
            
            //printf("!!! %s\n", cmd);
            
            (*cmdtotal)++;
            
            hasInputFile = 0;
            hasOutputFile = 0;
            strcpy(inputFilename, "");
            strcpy(outputFilename, "");
            strcpy(cmd, "");
            argc = 0;
        }else{
            if (hasInputFile && strcmp(inputFilename, "") == 0){
                strcpy(inputFilename, word);
            }else if (hasOutputFile && strcmp(outputFilename, "") == 0){
                strcpy(outputFilename, word);
            }else{
                if (strcmp(cmd, "") == 0) strcpy(OutCmdList[*cmdtotal].cmdmain, word);
                OutCmdList[*cmdtotal].cmdargv[argc] = (char *)malloc(sizeof(word));
                strcpy(OutCmdList[*cmdtotal].cmdargv[argc++], word);
                
                if (strcmp(cmd, "") != 0) strcat(cmd, " ");
                strcat(cmd, word);
            }
        }
        //printf("%d: %s\n", id++, word);
    }
    InCmd[strlen(InCmd)-1] = '\0';
    if (*cmdtotal > 0) OutCmdList[(*cmdtotal) - 1].hasOutputPipe = 0;
    
    //int i;
    //for (i=0;i<*cmdtotal;i++)
    //    printCmdnode(OutCmdList[i]);
    return 1;
}

int execute_inner(struct cmdnode_t * InCmdList, int current, int cmdtotal){
    int fin,fout;
    int pid;
    int pid2;
    int pid_status;
    int pid2_status;
    if (current >= cmdtotal) return 1;
    if (current < 0) return errReport("execute function call failed.");
    if (pipe(InCmdList[current].pipe_fd) < 0) return errReport("pipe create failed.");
    
    pid = fork();
    if (pid==0){
        if (current+1 >= cmdtotal) exit(0);
        close(InCmdList[current].pipe_fd[1]);
        execute_inner(InCmdList, current + 1, cmdtotal);
    }else{
        //printf("! %s\n   argc: %d\n",InCmdList[current].cmdmain, InCmdList[current].argc);
        //int i;
        //for (i=0;i<InCmdList[current].argc;i++)
        //    printf(" [%s] ",InCmdList[current].cmdargv[i]);
        //printf("\n");
        
        close(InCmdList[current].pipe_fd[0]);
        pid2=fork();
        if (pid2==0){
            if (InCmdList[current].hasInputFile){
                fin = open(InCmdList[current].inputFilename, O_RDONLY);
                dup2(fin, 0);
            }
            if (InCmdList[current].hasOutputFile){
                fout = open(InCmdList[current].outputFilename, O_CREAT|O_RDWR);
                dup2(fout, 1);
            }
            if (InCmdList[current].hasInputPipe){
                dup2(InCmdList[current-1].pipe_fd[0], 0);
            }
            if (InCmdList[current].hasOutputPipe){
                dup2(InCmdList[current].pipe_fd[1], 1);
            }
        
            execvp(InCmdList[current].cmdmain, InCmdList[current].cmdargv);
            
        }else{
            close(InCmdList[current].pipe_fd[1]);
            waitpid(pid2, &pid2_status, 0);
            waitpid(pid, &pid_status, 0);
            // if ( WIFEXITED(pid_status) ){
            //     printf(" WIFEXITED pid1 %X \n", WEXITSTATUS(pid_status));
            // }
            // if ( WIFEXITED(pid2_status) ){
            //     printf(" WIFEXITED pid2 %X \n", WEXITSTATUS(pid2_status));
            // }
            // printf("pid_status: %d   pid2_status: %d\n", WEXITSTATUS(pid_status), WEXITSTATUS(pid2_status));
            if ( current+1>=cmdtotal ) return WEXITSTATUS(pid_status);
            else return WEXITSTATUS(pid2_status);
        }
            
    }
    
    return 1;
}

int execute(struct cmdnode_t * InCmdList, int cmdtotal){
    int pid;
    char path[512];
    if (cmdtotal <= 0) return 0;
    
    if (strcmp(InCmdList[0].cmdmain, "cd") == 0){
        if (chdir(InCmdList[0].cmdargv[1]) < 0) return 0;
        else return 1;
    }else if (strcmp(InCmdList[0].cmdmain, "pwd") == 0){
        getcwd(path, sizeof(path));
        printf("%s\n", path);
        return 1;
    }else if (strcmp(InCmdList[0].cmdmain, "exit") == 0){
        return -1;
    }
    
    pid = fork();
    if (pid == 0){
        execute_inner(InCmdList, 0, cmdtotal);
        exit(0);
    }else{
        waitpid(pid, NULL ,0);
    }
    
    return 1;
}

// int main(){
//     while (1){
//         printf("$ ");
//         getCmd(orgin_cmd, MAX_CMD_LENGTH);
//         splitByPipe(orgin_cmd, cmdlist, &cmdtotal);
//         if (execute(cmdlist, cmdtotal) == -1) return 0;
//     }
//     return 0;
// }
