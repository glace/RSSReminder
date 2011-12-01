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

#include "md5.h"

#define DEBUG_MODE 1 

#define MAX_FILE_LENGTH 3000000
#define MAX_ITEMLIST_LENGTH 100000 
#define MAX_ITEMLIST_SAVE 100

struct globalArgs_t {
	int noIndex;				/* -I option */
	const char *ruleFileName;           /* -r --rule option */
	const char *xmlItemListFileName;           /* -x --xml option */
	const char *alarmFileName;           /* --alarm option */
	const char *xmlDomShwoFileName;           /* --alarm option */
	const char *outFileName;	/* -o option */
	const char *username;	/* -u --username option */
	const char *url;	/* --url option */
	const char *prefix;	/* --url option */

	const char *databaseFileName;
	FILE *outFile;
	int mode;				/* -v option */
	char **inputFiles;			/* input files */
	int numInputFiles;			/* # of input files */
    int debug_mode;             /* --debug option */
} globalArgs;

static const char *optString = "Ir:u:x:o:m:h?";

static const struct option longOpts[] = {
	{ "no-index", no_argument, NULL, 'I' },
	{ "rule", required_argument, NULL, 'r' },
	{ "username", required_argument, NULL, 'u' },
	{ "url", required_argument, NULL, 0 },
	{ "prefix", required_argument, NULL, 0 },
	{ "xml", required_argument, NULL, 'x' },
	{ "alarm", required_argument, NULL, 0 },
	{ "show", required_argument, NULL, 0 },
	{ "output", required_argument, NULL, 'o' },
	{ "mode", required_argument, NULL, 'm' },
	{ "debug", no_argument, NULL, 0 },
	{ "help", no_argument, NULL, 'h' },
	{ NULL, no_argument, NULL, 0 }
};

enum{
    MODE_GENERAL_RUN,
    MODE_USER_ADD,
    MODE_USER_DEL,
    MODE_SET_RULEFILE,
    MODE_SET_USERSTATUS_ENABLE,
    MODE_SET_USERSTATUS_DISABLE,
    MODE_TEST_RULEFILE,
    MODE_URL_ADD,
    MODE_URL_DEL,
    MODE_URL_ENABLE,
    MODE_URL_DISABLE,
    MODE_REFRESH,
    MODE_SHOW_USERINFO
};

// enum{
//     XML_ITEM_NOG,
//     XML_ITEM_REGION,
//     XML_ITEM_TITLE,
//     XML_ITEM_TIME,
//     XML_ITEM_CONTENT,
//     XML_ITEM_LAST
// };

// char fileContent[MAX_FILE_LENGTH];
// int fileContentLen;
// int itemListTotal;
// int isodd(int num);

// int errReport(const char * errinfo);

// int init();

// int importFile( char * filename );

int swap( int * a, int * b );

#endif
