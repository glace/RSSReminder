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
#include <string.h>

#define MAX_FILE_LENGTH 30000

char fileContent[MAX_FILE_LENGTH];
int fileContentLen;

int isodd(int num){
    return num % 2;
}

int specialSymbol(char ch){
    return ch == '=' || ch == '<' || ch == '>' || ch == '/';
}

int errReport(const char * errinfo){
    printf("%s\n",errinfo);
    return 0;
}

int staticFetch(char * wordlist, char * word, int offset, int len){
    int total=0;
    char tword[10000];
    int i;

    if (len < 0 ){
        strcpy(word, "");
        return 0;
    }
    if (offset + len - 1 >= strlen(wordlist))
        len = strlen(wordlist) - 1 - offset + 1; 
    for (i=0;i<len;i++)
        tword[total++] = tolower(wordlist[offset + i]);
    tword[total] = '\0';
    strcpy(word, tword);
    return 1;
}

int fetchWord(char * wordlist, char * word, int * offset){
    int total=0;
    char tword[10000];
    char foresee[100];
    int double_quote = 0;
    int single_quote = 0;
    int backslash = 0;
    int len, i;
    char ch1,ch2,ch3;
    
    while (*offset<strlen(wordlist) && !isgraph(wordlist[*offset])) (*offset)++;
    if (*offset>=strlen(wordlist)) return 0;
    while (*offset<strlen(wordlist) 
        && ( (isgraph(wordlist[*offset]) && !specialSymbol(wordlist[*offset])) 
            || isodd(single_quote) || isodd(double_quote))){
                
        tword[total] = tolower(wordlist[*offset]);

        total++;
        (*offset)++;
        
        if (tword[total-1] == '\''){
            if (single_quote > 0 || double_quote == 0){
                single_quote++;
                if (total > 1 && single_quote == 1){
                    total--;
                    (*offset)--;
                    break;
                }
                if (single_quote == 2) break;
            }
        }
        if (tword[total-1] == '"'){
            if (double_quote > 0 || single_quote == 0){
                double_quote++;
                if (total > 1 && double_quote == 1){
                    total--;
                    (*offset)--;
                    break;
                }
                if (double_quote == 2) break;
            }
        }
            
    }
    if (total == 0)
        if (*offset<strlen(wordlist) && specialSymbol(wordlist[*offset])){
            tword[total] = tolower(wordlist[*offset]);
            total++;
            (*offset)++;

            if (tword[total-1] == '<'){
                staticFetch(wordlist, foresee, *offset, 3);
                if (strcmp(foresee, "!--")==0){
                    tword[total]='\0';
                    strcat(tword, "!--");
                    total+=3;
                    (*offset)+=3;
                    ch1=ch2=ch3='\0';
                    while (!(ch1=='-' && ch2=='-' && ch3=='>') && *offset<strlen(wordlist)){
                        ch1=ch2;ch2=ch3;
                        tword[total]=ch3=wordlist[*offset];
                        total++;
                        (*offset)++;
                    }
                }
            }
        }
        // while (*offset<strlen(wordlist) && specialSymbol(wordlist[*offset])){
        //     tword[total] = tolower(wordlist[*offset]);
        //     total++;
        //     (*offset)++;
        //     break;
        // }
    tword[total]='\0';
    
    // len = strlen(tword);
    // if (len >= 2){
    //     if (tword[0] == tword[len-1] && (tword[0] == '\'' || tword[0] == '"')){
    //         for (i=0;i<len-2;i++) tword[i] = tword[i+1];
    //         tword[len-2] = '\0';
    //     }
    // }
        
    strcpy(word, tword);
    return 1;
}

int init(){
    fileContentLen=0;
    strcpy(fileContent,"0");
    return 1;
}

int importFile( char * filename ){
    FILE * fin;
    char ch;
    fileContentLen = 0;
    fin = fopen(filename, "r");
    if (!fin){
        printf("error\n");
        return 0;
    }
    while ( EOF != ( ch = fgetc( fin ) ) ){
        fileContent[fileContentLen++] = ch;
        if (fileContentLen + 1 > MAX_FILE_LENGTH) break;
    }
    fileContent[fileContentLen] = '\0';
    fclose(fin);
    return 1;
}

int main( int argc, char* argv[] ){
    char word[10000];
    int offset=0;
    if (argc >= 2 )
        importFile(argv[1]);
    // printf("%s\n%d\n", fileContent, fileContentLen);
    while ( fetchWord(fileContent, word, &offset))
        printf("%s\n", word);
    return 0;
}

