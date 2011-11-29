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

#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

#define DEBUG_MODE 1 

#define MAX_FILE_LENGTH 3000000
#define MAX_ITEMLIST_LENGTH 100000 

struct globalArgs_t {
	int noIndex;				/* -I option */
	const char *ruleFileName;           /* -r --rule option */
	const char *outFileName;	/* -o option */
	FILE *outFile;
	int mode;				/* -v option */
	char **inputFiles;			/* input files */
	int numInputFiles;			/* # of input files */
    int debug_mode;             /* --debug option */
} globalArgs;

static const char *optString = "Ir:o:m:h?";

static const struct option longOpts[] = {
	{ "no-index", no_argument, NULL, 'I' },
	{ "rule", required_argument, NULL, 'r' },
	{ "output", required_argument, NULL, 'o' },
	{ "mode", required_argument, NULL, 'm' },
	{ "debug", no_argument, NULL, 0 },
	{ "help", no_argument, NULL, 'h' },
	{ NULL, no_argument, NULL, 0 }
};

enum{
    MODE_PARSE_HTML,
    MODE_GENERATE_XML
};

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

enum{
    XML_ITEM_NOG,
    XML_ITEM_REGION,
    XML_ITEM_TITLE,
    XML_ITEM_TIME,
    XML_ITEM_CONTENT,
    XML_ITEM_LAST
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
int isodd(int num);

int specialSymbol(char ch);

int errReport(const char * errinfo);

int isgraphPlus(char ch);

int staticFetch(char * wordlist, char * word, int offset, int len);

int fetchWord(char * wordlist, char * word, char * unformattedWord, int * offset);

int checkWordType(char * word);

int checkWordTypePlus(int type, int wordTypePrePre, int wordTypePre, int wordType);

int printWT(int wordType);

int printIT(int itemnodeType);

int printSpace(int len);

int printItemNode(struct itemnode_t * itemList, int nodeSubscript, int deepLen);

int checkWordTypeForScript(int type, int scriptDeep, int wordType, char * fileContent, int offset);

int generateItemList(struct itemnode_t * itemList, int * itemListTotal);

int init();

int importFile( char * filename );

#endif
