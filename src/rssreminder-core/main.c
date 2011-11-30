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
#include <unistd.h>
#include <getopt.h>
#include <string.h>

#include "global.h"
#include "xmldom.h"
#include "xmlitem.h"

int display_usage(){
    printf("rssreminder-core\n");
    return 1;
}

int parseCmdOpt( int argc, char *argv[] ){
    int opt = 0;
    int longIndex = 0;
    
    /* Initialize globalArgs before we get to work. */
    globalArgs.noIndex = 0;		/* false */
    globalArgs.ruleFileName = NULL;
    globalArgs.xmlItemListFileName = NULL;
    globalArgs.alarmFileName = NULL;
    globalArgs.outFileName = NULL;
    globalArgs.outFile = NULL;
    globalArgs.mode = MODE_PARSE_HTML;
    globalArgs.inputFiles = NULL;
    globalArgs.numInputFiles = 0;
    globalArgs.debug_mode = 0;
    
    /* Process the arguments with getopt_long(), then 
     * populate globalArgs. 
     */
    opt = getopt_long( argc, argv, optString, longOpts, &longIndex );
    while( opt != -1 ) {
        switch( opt ) {
            case 'I':
                globalArgs.noIndex = 1;	/* true */
                break;
                
            case 'r':
                globalArgs.ruleFileName = optarg;
                break;

            case 'x':
                globalArgs.xmlItemListFileName = optarg;
                break;
                
            case 'o':
                globalArgs.outFileName = optarg;
                break;
                
            case 'm':
                if (strcmp( optarg, "html") == 0 || strcmp( optarg, "parse") == 0){
                    globalArgs.mode = MODE_PARSE_HTML;
                }else if (strcmp( optarg, "xml" ) == 0 || strcmp( optarg, "generate" ) == 0){
                    globalArgs.mode = MODE_GENERATE_XML;
                }else{
                    printf( "Error: unknown option value of --mode, %s.\n", optarg);
                }
                break;
                
            case 'h':   /* fall-through is intentional */
            case '?':
                display_usage();
                break;

            case 0:     /* long option without a short arg */
                if( strcmp( "alarm", longOpts[longIndex].name ) == 0 ) {
                    globalArgs.alarmFileName = optarg;
                }
                if( strcmp( "debug", longOpts[longIndex].name ) == 0 ) {
                    globalArgs.debug_mode = 1;
                }
                break;
                
            default:
                /* You won't actually get here. */
                break;
        }
        
        opt = getopt_long( argc, argv, optString, longOpts, &longIndex );
    }
    
    globalArgs.inputFiles = argv + optind;
    globalArgs.numInputFiles = argc - optind;

    return 1;
}

int main( int argc, char* argv[] ){
    char word[10000];
    struct xmlDesc_t xmlDesc[10];
    int offset=0;
    parseCmdOpt(argc,argv);
    if ( globalArgs.mode == MODE_PARSE_HTML ){
        if (globalArgs.numInputFiles > 0){
            importFile(globalArgs.inputFiles[0]);
            generateItemList(itemList, &itemListTotal);
        }
        if (globalArgs.outFileName != NULL)
            exportXmlDom( globalArgs.outFileName, itemList, &itemListTotal);
        else printf( "Error: export filename not specified.\n");
    }else if ( globalArgs.mode == MODE_GENERATE_XML ){
        if (globalArgs.numInputFiles > 0)
            importXmlDom( globalArgs.inputFiles[0], itemList, &itemListTotal);
            if (globalArgs.ruleFileName != NULL){
                printf("ruleFileName: %s\n", globalArgs.ruleFileName);
                importRuleFile( globalArgs.ruleFileName, itemList, &itemListTotal, xmlDesc );
                // xmlDescPrint( &xmlDesc );
            }
        else printf( "Error: import filename not specified.\n");
        if (globalArgs.xmlItemListFileName != NULL){
            // exportXmlDom( globalArgs.outFileName, itemList, &itemListTotal);
            importXmlItemFile( globalArgs.xmlItemListFileName, xmlItemList, &xmlItemListTotal );
            generateXmlItem( itemList, &itemListTotal, xmlDesc, xmlItemList, &xmlItemListTotal, &xmlItemListAlarmTotal );
            exportXmlItemFile( globalArgs.xmlItemListFileName, xmlItemList, &xmlItemListTotal );
        }else printf( "Error: xmlItemListFileName not specified. use -x / --xml option.\n");
        if (globalArgs.alarmFileName != NULL){
            exportXmlItemFileForAlarm( globalArgs.alarmFileName, xmlItemList, &xmlItemListTotal, &xmlItemListAlarmTotal );
        }else printf( "Error: alarmFileName not specified. use --alarm option.\n");
    }
    return 0;
}

