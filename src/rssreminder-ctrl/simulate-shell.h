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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef SIMULATE_SHELL_H
#define SIMULATE_SHELL_H

#define MAX_CMD_LENGTH 100000

struct cmdnode_t{
    char cmd[MAX_CMD_LENGTH];
    char cmdmain[MAX_CMD_LENGTH];
    char * cmdargv[30];
    int argc;
    int pipe_fd[2];
    int hasInputPipe,hasOutputPipe;
    int hasInputFile,hasOutputFile;
    char inputFilename[30];
    char outputFilename[30];
};

char orgin_cmd[MAX_CMD_LENGTH];
struct cmdnode_t cmdlist[20];
int cmdtotal;

int getCmd(char * outCmd, int maxLength);

int isodd(int num);

int specialSymbol(char ch);

int errReport(const char * errinfo);

int fetchWord(char * wordlist, char * word, int * offset);

int printCmdnode(struct cmdnode_t target);

int splitByPipe(char * InCmd, struct cmdnode_t * OutCmdList, int * cmdtotal);

int execute_inner(struct cmdnode_t * InCmdList, int current, int cmdtotal);

int execute(struct cmdnode_t * InCmdList, int cmdtotal);
        
#endif
