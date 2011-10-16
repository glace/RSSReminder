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

#define MAX_FILE_LENGTH 3000000
#define MAX_ITEMLIST_LENGTH 100000 

enum{
    IT_NOG,     // Nothing
    IT_TXT,     // Text
    IT_ITM,     // Item
    IT_TAL,     // Tail
    IT_SCT      // Script
};

enum{
    WT_ITM_SRT, // wordType Item Start
    WT_ITM_END, // wordType Item End
    WT_ITM_TLE, // wordType Item Title
    WT_ITM_ATT, // wordType Item Attribution
    WT_ITM_VAL, // wordType Item Attribution Value
    WT_SYM_EQU, // wordType Symbol Equal
    WT_SYM_SLH, // wordType Symbol Slash
    WT_NOG      // wordType Nothing
};

struct itemnode_t {
    int type;  // Text  ||  Item
    char * title;
    char * attrTitle[100];
    char * attrValue[100];
    int attrTotal;
    int parent;
    int tail;
};

char fileContent[MAX_FILE_LENGTH];
int fileContentLen;
struct itemnode_t itemList[MAX_ITEMLIST_LENGTH];
int itemListTotal;

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

int isgraphPlus(char ch){
    if (ch == '\n') return 0;
    if (ch == '\t') return 0;
    if (ch == ' ') return 0;
    return 1;
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

int fetchWordOld(char * wordlist, char * word, char * unformattedWord, int * offset){
    int total=0;
    char tword[1000000];
    char ufTword[1000000];
    char foresee[100];
    int double_quote = 0;
    int single_quote = 0;
    int backslash = 0;
    int len, i;
    char ch1,ch2,ch3;
    
    while (*offset<strlen(wordlist) && !isgraphPlus(wordlist[*offset])) (*offset)++;
    if (*offset>=strlen(wordlist)) return 0;
    while (*offset<strlen(wordlist) 
        && ( (isgraphPlus(wordlist[*offset]) && !specialSymbol(wordlist[*offset])) 
            || isodd(single_quote) || isodd(double_quote))){
                
        tword[total] = tolower(wordlist[*offset]);
        ufTword[total] = wordlist[*offset];

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
            ufTword[total] = wordlist[*offset];

            total++;
            (*offset)++;

            if (tword[total-1] == '<'){
                staticFetch(wordlist, foresee, *offset, 3);
                if (strcmp(foresee, "!--")==0){
                    tword[total]='\0';
                    ufTword[total]='\0';
                    strcat(tword, "!--");
                    strcat(ufTword,"!--");
                    total+=3;
                    (*offset)+=3;
                    ch1=ch2=ch3='\0';
                    while (!(ch1=='-' && ch2=='-' && ch3=='>') && *offset<strlen(wordlist)){
                        ch1=ch2;ch2=ch3;
                        ufTword[total]=tword[total]=ch3=wordlist[*offset];
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
    ufTword[total]='\0';
    
    // len = strlen(tword);
    // if (len >= 2){
    //     if (tword[0] == tword[len-1] && (tword[0] == '\'' || tword[0] == '"')){
    //         for (i=0;i<len-2;i++) tword[i] = tword[i+1];
    //         tword[len-2] = '\0';
    //     }
    // }
        
    strcpy(word, tword);
    strcpy(unformattedWord, ufTword);
    return 1;
}

int fetchWord(char * wordlist, char * word, char * unformattedWord, int * offset){
    int total=0;
    char tword[1000000];
    char ufTword[1000000];
    char foresee[100];
    int double_quote = 0;
    int single_quote = 0;
    int backslash = 0;
    int len, i;
    char ch1,ch2,ch3;
    
    while (*offset<strlen(wordlist) && !isgraphPlus(wordlist[*offset])) (*offset)++;
    if (*offset>=strlen(wordlist)) return 0;
    while (*offset<strlen(wordlist) 
        && ( (isgraphPlus(wordlist[*offset]) && !specialSymbol(wordlist[*offset])) 
            || isodd(single_quote) || isodd(double_quote))){
                
        tword[total] = tolower(wordlist[*offset]);
        ufTword[total] = wordlist[*offset];

        
        if (tword[total] == '\'' && !backslash){
            if (single_quote > 0 || double_quote == 0){
                single_quote++;
                if (total > 1 && single_quote == 1) break;
            }
        }
        if (tword[total] == '"' && !backslash){
            if (double_quote > 0 || single_quote == 0){
                double_quote++;
                if (total > 1 && double_quote == 1) break;
            }
        }
        if (tword[total] == '\\') backslash = !backslash;
        else backslash = 0;

        total++;
        (*offset)++;
                if (single_quote == 2) break;
                if (double_quote == 2) break;
    }
    if (total == 0)
        if (*offset<strlen(wordlist) && specialSymbol(wordlist[*offset])){
            tword[total] = tolower(wordlist[*offset]);
            ufTword[total] = wordlist[*offset];

            total++;
            (*offset)++;

            if (tword[total-1] == '<'){
                staticFetch(wordlist, foresee, *offset, 3);
                if (strcmp(foresee, "!--")==0){
                    tword[total]='\0';
                    ufTword[total]='\0';
                    strcat(tword, "!--");
                    strcat(ufTword,"!--");
                    total+=3;
                    (*offset)+=3;
                    ch1=ch2=ch3='\0';
                    while (!(ch1=='-' && ch2=='-' && ch3=='>') && *offset<strlen(wordlist)){
                        ch1=ch2;ch2=ch3;
                        ufTword[total]=tword[total]=ch3=wordlist[*offset];
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
    ufTword[total]='\0';
    
    // len = strlen(tword);
    // if (len >= 2){
    //     if (tword[0] == tword[len-1] && (tword[0] == '\'' || tword[0] == '"')){
    //         for (i=0;i<len-2;i++) tword[i] = tword[i+1];
    //         tword[len-2] = '\0';
    //     }
    // }
        
    strcpy(word, tword);
    strcpy(unformattedWord, ufTword);
    return 1;
}

int checkWordType(char * word){
    if (strcmp(word, "<") == 0) return WT_ITM_SRT;
    if (strcmp(word, ">") == 0) return WT_ITM_END;
    if (strcmp(word, "=") == 0) return WT_SYM_EQU;
    if (strcmp(word, "/") == 0) return WT_SYM_SLH;
    return WT_NOG;
}

int checkWordTypePlus(int type, int wordTypePrePre, int wordTypePre, int wordType){
    if (wordType == WT_ITM_SRT) return wordType;
    if (type != IT_ITM) return WT_NOG;
    if (wordType == WT_SYM_EQU || wordType == WT_SYM_SLH) return wordType;
    if (wordType == WT_ITM_END) return wordType;
    if (wordTypePre == WT_ITM_SRT || wordTypePrePre == WT_ITM_SRT && wordTypePre == WT_SYM_SLH) return WT_ITM_TLE;
    if (wordTypePre == WT_SYM_EQU) return WT_ITM_VAL;
    return WT_ITM_ATT;
}

int printWT(int wordType){
    if (wordType == WT_ITM_SRT) return printf("WT_ITM_SRT");
    if (wordType == WT_ITM_END) return printf("WT_ITM_END");
    if (wordType == WT_ITM_TLE) return printf("WT_ITM_TLE");
    if (wordType == WT_ITM_ATT) return printf("WT_ITM_ATT");
    if (wordType == WT_ITM_VAL) return printf("WT_ITM_VAL");
    if (wordType == WT_SYM_SLH) return printf("WT_SYM_SLH");
    if (wordType == WT_SYM_EQU) return printf("WT_SYM_EQU");
    if (wordType == WT_NOG) return printf("WT_NOG");
    return printf("Nothing");
}

int printIT(int itemnodeType){
    if (itemnodeType == IT_NOG ) return printf("IT_NOG");
    if (itemnodeType == IT_TXT ) return printf("IT_TXT");
    if (itemnodeType == IT_ITM ) return printf("IT_ITM");
    if (itemnodeType == IT_TAL ) return printf("IT_TAL");
    return printf("Nothing");
}

int printSpace(int len){
    int i;
    for (i=0; i<len; i++)
        printf("\t");
    return 0;
}

int printItemNode(struct itemnode_t * itemList, int nodeSubscript, int deepLen){
    int type;
    int parent;
    int i;
    struct itemnode_t * target;
    if (nodeSubscript < 0 || nodeSubscript > itemListTotal) return 0;
    if (deepLen < 0) return 0;
    target = &itemList[nodeSubscript];
    type = target->type;
    parent = target->parent;
    printSpace(deepLen);
    printIT(type);
    printf("\t\t nodeSubscript: %d", nodeSubscript);

    // printSpace(deepLen);
    printf("\t parent: %d\n", parent);

    printSpace(deepLen);
    if (type == IT_ITM || type == IT_TAL){
        printf("<");
        if (type == IT_TAL) printf("/");
        printf("%s", target->title);
        for (i=0; i<target->attrTotal ; i++){
            printf(" %s", target->attrTitle[i]);
            if (target->attrValue[i] != NULL)
                printf("=%s", target->attrValue[i]);
        }
        printf(">\n");
    }else if (type == IT_TXT || type == IT_NOG){
        printf("%s\n", target->title);
    }
    return 1;
}

int checkWordTypeForScript(int type, int scriptDeep, int wordType, char * fileContent, int offset){
    char word[1000000];
    char unformattedWord[1000000];
    int loffset = offset;
    if (scriptDeep <= 0) return wordType;
    if (type == IT_ITM) return wordType;
    if (!fetchWord(fileContent, word, unformattedWord, &loffset)) return wordType;
    // return wordType;
    if (strcmp(word, "/") != 0) return WT_NOG;
    if (!fetchWord(fileContent, word, unformattedWord, &loffset)) return WT_NOG;
    if (strcmp(word, "script") != 0) return WT_NOG;
    return wordType;
}

int generateItemList(struct itemnode_t * itemList, int * itemListTotal){
    char word[1000000];
    char unformattedWord[1000000];
    char tword[1000000];
    int offset = 0;
    int parent = 0;
    int type = IT_NOG;
    int wordType = WT_NOG;
    int wordTypePre = WT_NOG;
    int wordTypePrePre = WT_NOG;
    int headnodeSubscript;
    int subscript;
    int parentConfirm[MAX_ITEMLIST_LENGTH];
    int tailConfirm[MAX_ITEMLIST_LENGTH];
    int deepLen[MAX_ITEMLIST_LENGTH];
    int flag;
    int scriptDeep;
    int scriptDeepToAdd;
    char * title;
    char * str;
    int tot;
    // if (argc >= 2 )
    //     importFile(argv[1]);
    // printf("%s\n%d\n", fileContent, fileContentLen);

    strcpy(tword, "");
    *itemListTotal = 0;
    itemList[0].type = IT_NOG;
    itemList[0].attrTotal = 0;
    parent = 0;
    // memset(parentConfirm, 0, sizeof(parentConfirm));
    parentConfirm[0] = 1;
    scriptDeep = 0;
    scriptDeepToAdd = 0;
    
    // return 0;
    
    while ( fetchWord(fileContent, word, unformattedWord, &offset)){
        wordTypePrePre = wordTypePre;
        wordTypePre = wordType;
        wordType = checkWordType(word);
        printWT(wordType);
        printf("\t");
        wordType = checkWordTypePlus(type, wordTypePrePre, wordTypePre, wordType);
        printWT(wordType);
        printf("\t");
        wordType = checkWordTypeForScript(type, scriptDeep, wordType, fileContent, offset);
        printWT(wordType);
        printf("\t\t \"%s\"\n",word);
        if (wordType == WT_ITM_SRT){
            if (strlen(tword) > 0){
                (*itemListTotal)++;
                printf("itemListTotal: %d\n",*itemListTotal);
                itemList[*itemListTotal].type = IT_TXT;
                str = (char *) malloc(strlen(tword)+1);
                strcpy(str, tword);
                itemList[*itemListTotal].title = str;
                itemList[*itemListTotal].attrTotal = 0;
                itemList[*itemListTotal].parent = parent;
                itemList[*itemListTotal].tail = 0;
                parentConfirm[*itemListTotal] = 0;
                tailConfirm[*itemListTotal] = 0;
                printf("%s\n", tword);
                strcpy(tword, "");
            }

            (*itemListTotal)++;
            printf("itemListTotal: %d\n",*itemListTotal);
            itemList[*itemListTotal].type = IT_ITM;
            itemList[*itemListTotal].title = NULL;
            itemList[*itemListTotal].attrTotal = 0;
            itemList[*itemListTotal].parent = parent;
            // parent = *itemListTotal;
            type = IT_ITM;
        }else if (wordType == WT_ITM_END){
            if (itemList[*itemListTotal].type == IT_TAL){
                if (strcmp(itemList[*itemListTotal].title, "script") == 0) scriptDeep--;
                for (subscript = *itemListTotal - 1; subscript > 0; subscript--){
                    if (itemList[subscript].type != IT_ITM) continue;
                    if (tailConfirm[subscript] == 1) continue;
                    if (strcmp(itemList[subscript].title, itemList[*itemListTotal].title) != 0) continue;
                    break;
                }
                if (subscript > 0){
                    headnodeSubscript = subscript;
                    itemList[headnodeSubscript].tail = *itemListTotal;
                    tailConfirm[headnodeSubscript] = 1;
                    for (subscript = headnodeSubscript + 1; subscript<=*itemListTotal; subscript++)
                        if (parentConfirm[subscript] != 1){
                            parentConfirm[subscript] = 1;
                            itemList[subscript].parent = headnodeSubscript;
                        }
                }
            }else if (itemList[*itemListTotal].type == IT_ITM){
                if (itemList[*itemListTotal].title != NULL)
                if (strcmp(itemList[*itemListTotal].title, "script") == 0) scriptDeep++;
            }

            type = IT_NOG;
        }else if (wordType == WT_ITM_TLE){
            title = (char *) malloc(strlen(word)+1);
            strcpy(title, word);
                printf("%s\n", word);
            itemList[*itemListTotal].title = title;
        }else if (wordType == WT_ITM_ATT){
            tot = itemList[*itemListTotal].attrTotal++;
            str = (char *) malloc(strlen(word)+1);
            strcpy(str, word);
                printf("%s\n", word);
                printf(" tot: %d\n", tot);
            itemList[*itemListTotal].attrTitle[ tot ] = str;
            itemList[*itemListTotal].attrValue[ tot ] = NULL;
        }else if (wordType == WT_ITM_VAL){
            tot = itemList[*itemListTotal].attrTotal -1;
            str = (char *) malloc(strlen(unformattedWord)+1);
            strcpy(str, unformattedWord);
                printf("%s\n", unformattedWord);
            itemList[*itemListTotal].attrValue[ tot ] = str;
        }else if (wordType == WT_SYM_SLH){
            itemList[*itemListTotal].type = IT_TAL;
        }else if (wordType == WT_NOG){
            strcat(tword, unformattedWord);
            printf("%s\n", unformattedWord);
        }
        // printf("%s\n", word);
    }
    if (strlen(tword) > 0){
        (*itemListTotal)++;
        itemList[*itemListTotal].type = IT_TXT;
        str = (char *) malloc(strlen(tword)+1);
        strcpy(str, tword);
                printf("%s\n", word);
        itemList[*itemListTotal].title = str;
        itemList[*itemListTotal].attrTotal = 0;
        itemList[*itemListTotal].parent = parent;
        strcpy(tword, "");
    }
    deepLen[0] = -1;
    for (subscript = 1; subscript <= *itemListTotal; subscript++)
        deepLen[subscript] = deepLen[ itemList[subscript].parent ] + 1;
    printf("\n\n========================\n\n");
    int i;
    for (i=1; i<=*itemListTotal;i++){
        printItemNode(itemList, i, deepLen[i]);
        printf("\n\n");
    }
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
    // while ( fetchWord(fileContent, word, &offset))
    //     printf("%s\n", word);
    generateItemList(itemList, &itemListTotal);
    return 0;
}

