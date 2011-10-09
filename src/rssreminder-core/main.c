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

int isodd(int num){
    return num % 2;
}

int specialSymbol(char ch){
    return ch == '=' || ch == '<' || ch == '>';
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
