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
#include "simulate-shell.h"
#include "database.h"

int display_usage(){
    printf("rssreminder-core\n");
    return 1;
}

int parseCmdOpt( int argc, char *argv[] ){
    int opt = 0;
    int longIndex = 0;
    char * mstr;
    
    /* Initialize globalArgs before we get to work. */
    globalArgs.noIndex = 0;		/* false */
    globalArgs.ruleFileName = NULL;
    globalArgs.xmlItemListFileName = NULL;
    globalArgs.alarmFileName = NULL;
    globalArgs.outFileName = NULL;
    globalArgs.outFile = NULL;
    globalArgs.mode = MODE_GENERAL_RUN;
    globalArgs.inputFiles = NULL;
    globalArgs.numInputFiles = 0;
    globalArgs.debug_mode = 0;

    mstr = malloc( strlen("database.xml")+1 );
    strcpy( mstr, "database.xml" );
    globalArgs.databaseFileName = mstr;

    globalArgs.username = NULL;
    globalArgs.url = NULL;
    globalArgs.prefix = NULL;
    
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

            case 'u':
                globalArgs.username = optarg;
                break;

            case 'x':
                globalArgs.xmlItemListFileName = optarg;
                break;
                
            case 'o':
                globalArgs.outFileName = optarg;
                break;
                
            case 'm':
                if (strcmp( optarg, "general") == 0 ){
                    globalArgs.mode = MODE_GENERAL_RUN;
                }else if (strcmp( optarg, "useradd" ) == 0){
                    globalArgs.mode = MODE_USER_ADD;
                }else if (strcmp( optarg, "userdel" ) == 0){
                    globalArgs.mode = MODE_USER_DEL;
                }else if (strcmp( optarg, "setrulefile" ) == 0){
                    globalArgs.mode = MODE_SET_RULEFILE;
                }else if (strcmp( optarg, "testrulefile" ) == 0 || strcmp( optarg, "test" ) == 0){
                    globalArgs.mode = MODE_TEST_RULEFILE;
                }else if (strcmp( optarg, "urladd" ) == 0){
                    globalArgs.mode = MODE_URL_ADD;
                }else if (strcmp( optarg, "urldel" ) == 0){
                    globalArgs.mode = MODE_URL_DEL;
                }else if (strcmp( optarg, "urlenable" ) == 0){
                    globalArgs.mode = MODE_URL_ENABLE;
                }else if (strcmp( optarg, "urldisable" ) == 0){
                    globalArgs.mode = MODE_URL_DISABLE;
                }else if (strcmp( optarg, "refresh" ) == 0){
                    globalArgs.mode = MODE_REFRESH;
                }else if (strcmp( optarg, "userinfo" ) == 0 || strcmp( optarg, "info" ) == 0){
                    globalArgs.mode = MODE_SHOW_USERINFO;
                }else if (strcmp( optarg, "setuserenable" ) == 0 || strcmp( optarg, "userenable" ) == 0){
                    globalArgs.mode = MODE_SET_USERSTATUS_ENABLE;
                }else if (strcmp( optarg, "setuserdisable" ) == 0 || strcmp( optarg, "userdisable" ) == 0){
                    globalArgs.mode = MODE_SET_USERSTATUS_DISABLE;
                }else{
                    printf( "Error: unknown option value of --mode, %s.\n", optarg);
                    return 0;
                }
                break;
                
            case 'h':   /* fall-through is intentional */
            case '?':
                display_usage();
                return 0;
                break;

            case 0:     /* long option without a short arg */
                if( strcmp( "url", longOpts[longIndex].name ) == 0 ) {
                    globalArgs.url= optarg;
                    printf( "url: %s\n", globalArgs.url);
                }
                if( strcmp( "prefix", longOpts[longIndex].name ) == 0 ) {
                    globalArgs.prefix = optarg;
                    printf( "prefix: %s\n", globalArgs.prefix);
                }
                if( strcmp( "alarm", longOpts[longIndex].name ) == 0 ) {
                    globalArgs.alarmFileName = optarg;
                }
                if( strcmp( "show", longOpts[longIndex].name ) == 0 ) {
                    globalArgs.xmlDomShwoFileName = optarg;
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

int quickRun( char * cmd ){
    strcpy( orgin_cmd, cmd );
    // printf( "%s\n", cmd );
    splitByPipe(orgin_cmd, cmdlist, &cmdtotal);
    if (execute(cmdlist, cmdtotal) == -1) return 0;
    return 1;
}


int main( int argc, char* argv[] ){
    char cmd[10000];
    char md5str[100];
    char * longStr;
    int subscript;
    int offset=0;

    if ( !parseCmdOpt(argc,argv) ) return 0;
    // int ans;
    // while (1){
    //     printf("$ ");
    //     getCmd(orgin_cmd, MAX_CMD_LENGTH);
    //     splitByPipe(orgin_cmd, cmdlist, &cmdtotal);
    //     if (ans=execute(cmdlist, cmdtotal) == -1) return 0;
    //     // printf( "%d\n", ans );
    // }

    if ( globalArgs.username == NULL ){
        printf( "Error: username not specified. use -u / --username option.\n" );
        return 0;
    }
    if ( globalArgs.mode == MODE_GENERAL_RUN ){
        sprintf( cmd, "mkdir %s", globalArgs.username );
        quickRun( cmd );
        sprintf( cmd, "cp script/load.js %s", globalArgs.username );
        quickRun( cmd );
        // sprintf( cmd, "cp rssreminder-core %s", globalArgs.username );
        // quickRun( cmd );
        // sprintf( cmd, "cp cliofetion-plus %s", globalArgs.username );
        // quickRun( cmd );
        sprintf( cmd, "cd %s", globalArgs.username );
        quickRun( cmd );

        if (!importDatabase( globalArgs.databaseFileName, databaseList, &databaseListTotal, &userAttr ))
            exportDatabase( globalArgs.databaseFileName, databaseList, &databaseListTotal, &userAttr );

        if ( userAttr.userStatus == NULL || strcmp( userAttr.userStatus, "disable" ) == 0 ){
            printf( "MODE_GENERAL_RUN success. userStatus disable \n" );
            return 0;
        }

        for (subscript=1; subscript<=databaseListTotal; subscript++){
            if ( strcmp( databaseList[subscript].urlstatus, "enable" ) != 0 ) continue;
            sprintf( cmd, "phantomjs load.js '%s' html", databaseList[subscript].url );
            // printf( "%s\n", cmd );
            quickRun( cmd );

            // printf( "%s\n", cmd );

            quickRun( "../rssreminder-core html -m html -o dom" );

            // printf( "rssreminder-core html -m html -o dom\n" );

            sprintf( cmd, "../rssreminder-core dom -m xml -r %srulefile -x %sxmlitemlist --alarm alarmfile"
                     ,databaseList[subscript].filenamePrefix 
                     ,databaseList[subscript].filenamePrefix );
            // printf( "%s\n", cmd );
            quickRun( cmd );
            // printf( "ba%s\n", cmd );
            
            // quickRun( "./cliofetion-plus -f send -u '15195840377' -p 'bdcccom08' -t '15195840377' -T 'mobile' -i 'alarmfile'");
            system( "../cliofetion-plus -f send -u '15195840377' -p 'bdcccom08' -t '15195840377' -T 'mobile' -i 'alarmfile'");

            // something here !!!

            // quickRun( "phantomjs load.js \"http://blog.sina.com.cn/glacebai\" html" );
        }
        printf( "MODE_GENERAL_RUN success.\n" );
        
    }else if ( globalArgs.mode == MODE_USER_ADD ){
        sprintf( cmd, "mkdir %s", globalArgs.username );
        quickRun( cmd );

        databaseListTotal = 0;
        exportDatabase( globalArgs.databaseFileName, databaseList, &databaseListTotal, &userAttr );
        printf( "MODE_USER_ADD success.\n" );

    }else if ( globalArgs.mode == MODE_USER_DEL ){
        sprintf( cmd, "rm %s -r -f", globalArgs.username );
        quickRun( cmd );
        printf( "MODE_USER_DEL success.\n" );

    }else if ( globalArgs.mode == MODE_SET_RULEFILE ){   // need --rule ruleFileName --prefix filenamePrefix 
        sprintf( cmd, "mkdir %s", globalArgs.username );
        quickRun( cmd );
        sprintf( cmd, "cd %s", globalArgs.username );
        quickRun( cmd );

        if ( globalArgs.ruleFileName == NULL ){
            printf( "Error: ruleFileName not specified. use --rule ruleFileName\n" );
            return 0;
        }
        if ( globalArgs.prefix == NULL ){
            printf( "Error: filenamePrefix not specified. use --prefix filenamePrefix\n" );
            return 0;
        }

        sprintf( cmd, "cp ../%s %srulefile", globalArgs.ruleFileName, globalArgs.prefix );
        quickRun( cmd );

        if (!importDatabase( globalArgs.databaseFileName, databaseList, &databaseListTotal, &userAttr ))
            exportDatabase( globalArgs.databaseFileName, databaseList, &databaseListTotal, &userAttr );

        for ( subscript=1; subscript<=databaseListTotal; subscript++ )
            if ( strcmp(databaseList[subscript].filenamePrefix, globalArgs.prefix) == 0 ){
                databaseList[subscript].filenamePrefix = malloc( strlen(globalArgs.prefix)+1 );
                strcpy( databaseList[subscript].filenamePrefix, globalArgs.prefix );
                break;
            }
        exportDatabase( globalArgs.databaseFileName, databaseList, &databaseListTotal, &userAttr );
        printf( "MODE_SET_RULEFILE success.\n" );

    }else if ( globalArgs.mode == MODE_TEST_RULEFILE ){   // need --prefix filenamePrefix
        sprintf( cmd, "mkdir %s", globalArgs.username );
        quickRun( cmd );
        sprintf( cmd, "cd %s", globalArgs.username );
        quickRun( cmd );

        if ( globalArgs.prefix == NULL ){
            printf( "Error: filenamePrefix not specified. use --prefix filenamePrefix\n" );
            return 0;
        }

        if (!importDatabase( globalArgs.databaseFileName, databaseList, &databaseListTotal, &userAttr ))
            exportDatabase( globalArgs.databaseFileName, databaseList, &databaseListTotal, &userAttr );

        // printf( "234  %d\n",databaseListTotal );
        for ( subscript=1; subscript<=databaseListTotal; subscript++ )
            if ( strcmp(databaseList[subscript].filenamePrefix, globalArgs.prefix) == 0 ){
                sprintf( cmd, "../rssreminder-core html -m html -o dom"
                         ,databaseList[subscript].filenamePrefix 
                         ,databaseList[subscript].filenamePrefix );
                // printf( "%s\n", cmd );
                sprintf( cmd, "../rssreminder-core dom -m xml -r %srulefile -x %sxmlitemlist --alarm alarmfile --show showfile"
                         ,databaseList[subscript].filenamePrefix 
                         ,databaseList[subscript].filenamePrefix );
                // printf( "%s\n", cmd );
                quickRun( cmd );
                quickRun( "mv showfile ../" );
                break;
            }

        sprintf( cmd, "mv %sxmlitemlist ../" ,databaseList[subscript].filenamePrefix );
        // printf( "%s\n", cmd );
        quickRun( cmd );
        printf( "MODE_TEST_RULEFILE success.\n" );

    }else if ( globalArgs.mode == MODE_URL_ADD ){   // need --url url
        sprintf( cmd, "mkdir %s", globalArgs.username );
        quickRun( cmd );
        sprintf( cmd, "cd %s", globalArgs.username );
        quickRun( cmd );

        if ( globalArgs.url == NULL ){
            printf( "Error: url not specified. use --url url\n" );
            return 0;
        }

        if (!importDatabase( globalArgs.databaseFileName, databaseList, &databaseListTotal, &userAttr ))
            exportDatabase( globalArgs.databaseFileName, databaseList, &databaseListTotal, &userAttr );

        databaseListTotal++;
        longStr = malloc( strlen(globalArgs.url)+1 );
        strcpy( longStr, globalArgs.url );
        databaseList[databaseListTotal].url = longStr;

        longStr = malloc( strlen("disable")+1 );
        strcpy( longStr, "disable" );
        databaseList[databaseListTotal].urlstatus = longStr;

        strcpy( cmd, globalArgs.url );
        Rmd5( cmd, md5str );
        longStr = malloc( strlen(md5str)+1 );
        strcpy( longStr, md5str );
        databaseList[databaseListTotal].filenamePrefix = longStr;

        exportDatabase( globalArgs.databaseFileName, databaseList, &databaseListTotal, &userAttr );
        printf( "MODE_URL_ADD success.\n" );
        
    }else if ( globalArgs.mode == MODE_URL_DEL ){   // need --prefix filenamePrefix
        sprintf( cmd, "mkdir %s", globalArgs.username );
        quickRun( cmd );
        sprintf( cmd, "cd %s", globalArgs.username );
        quickRun( cmd );

        importDatabase( globalArgs.databaseFileName, databaseList, &databaseListTotal, &userAttr );

        if ( globalArgs.prefix == NULL ){
            printf( "Error: filenamePrefix not specified. use --prefix filenamePrefix\n" );
            return 0;
        }

        offset = 0;
        for ( subscript=1; subscript<=databaseListTotal; subscript++ )
            if ( strcmp(databaseList[subscript].filenamePrefix, globalArgs.prefix) == 0 ){
                free( databaseList[subscript].url);
                free( databaseList[subscript].urlstatus );
                free( databaseList[subscript].filenamePrefix);

                databaseList[subscript].url = databaseList[databaseListTotal].url;
                databaseList[subscript].urlstatus = databaseList[databaseListTotal].urlstatus;
                databaseList[subscript].filenamePrefix = databaseList[databaseListTotal].filenamePrefix;
                offset = 1;
                break;
            }

        exportDatabase( globalArgs.databaseFileName, databaseList, &databaseListTotal, &userAttr );
        if ( offset ) printf( "MODE_URL_DEL success.\n" );
        else printf( "MODE_USER_DEL target not Found.\n" );

    }else if ( globalArgs.mode == MODE_URL_ENABLE ){   // need --prefix filenamePrefix
        sprintf( cmd, "mkdir %s", globalArgs.username );
        quickRun( cmd );
        sprintf( cmd, "cd %s", globalArgs.username );
        quickRun( cmd );

        importDatabase( globalArgs.databaseFileName, databaseList, &databaseListTotal, &userAttr );

        if ( globalArgs.prefix == NULL ){
            printf( "Error: filenamePrefix not specified. use --prefix filenamePrefix\n" );
            return 0;
        }

        for ( subscript=1; subscript<=databaseListTotal; subscript++ )
            if ( strcmp(databaseList[subscript].filenamePrefix, globalArgs.prefix) == 0 ){
                free( databaseList[subscript].urlstatus );
                longStr = malloc( strlen("enable")+1 );
                strcpy( longStr, "enable" );
                databaseList[subscript].urlstatus = longStr;
                break;
            }

        exportDatabase( globalArgs.databaseFileName, databaseList, &databaseListTotal, &userAttr );
        printf( "MODE_URL_ENABLE success.\n" );

    }else if ( globalArgs.mode == MODE_URL_DISABLE ){   // need --prefix filenamePrefix
        sprintf( cmd, "mkdir %s", globalArgs.username );
        quickRun( cmd );
        sprintf( cmd, "cd %s", globalArgs.username );
        quickRun( cmd );

        importDatabase( globalArgs.databaseFileName, databaseList, &databaseListTotal, &userAttr );

        if ( globalArgs.prefix == NULL ){
            printf( "Error: filenamePrefix not specified. use --prefix filenamePrefix\n" );
            return 0;
        }

        for ( subscript=1; subscript<=databaseListTotal; subscript++ )
            if ( strcmp(databaseList[subscript].filenamePrefix, globalArgs.prefix) == 0 ){
                free( databaseList[subscript].urlstatus );
                longStr = malloc( strlen("disable")+1 );
                strcpy( longStr, "disable" );
                databaseList[subscript].urlstatus = longStr;
                break;
            }

        exportDatabase( globalArgs.databaseFileName, databaseList, &databaseListTotal, &userAttr );
        printf( "MODE_URL_DISABLE success.\n" );

    }else if ( globalArgs.mode == MODE_REFRESH ){   // need --prefix filenamePrefix
        sprintf( cmd, "mkdir %s", globalArgs.username );
        quickRun( cmd );
        sprintf( cmd, "cd %s", globalArgs.username );
        quickRun( cmd );

        if ( globalArgs.prefix == NULL ){
            printf( "Error: filenamePrefix not specified. use --prefix filenamePrefix\n" );
            return 0;
        }
        
        sprintf( cmd, "rm %sxmlitemlist", globalArgs.prefix );
        // printf( "%s\n", cmd );
        quickRun( cmd );
        printf( "MODE_REFRESH success.\n" );

    }else if ( globalArgs.mode == MODE_SHOW_USERINFO ){
        sprintf( cmd, "mkdir %s", globalArgs.username );
        quickRun( cmd );
        sprintf( cmd, "cd %s", globalArgs.username );
        quickRun( cmd );

        importDatabase( globalArgs.databaseFileName, databaseList, &databaseListTotal, &userAttr );

        if ( userAttr.userStatus == NULL || strcmp( userAttr.userStatus, "disable" ) == 0 )
            printf( "userStatus disable \n" );
        else printf( "userStatus %s\n", userAttr.userStatus );

        for ( subscript=1; subscript<=databaseListTotal; subscript++ ){
            printf( "  subscript: %d\n", subscript );
            printf( "url       : %s\n", databaseList[subscript].url );
            printf( "urlstatus : %s\n", databaseList[subscript].urlstatus );
            printf( "prefix    : %s\n", databaseList[subscript].filenamePrefix);
            printf( "\n" );
        }
        printf( "MODE_SHOW_USERINFO success.\n" );

    }else if ( globalArgs.mode == MODE_SET_USERSTATUS_ENABLE ){
        sprintf( cmd, "mkdir %s", globalArgs.username );
        quickRun( cmd );
        sprintf( cmd, "cd %s", globalArgs.username );
        quickRun( cmd );

        importDatabase( globalArgs.databaseFileName, databaseList, &databaseListTotal, &userAttr );

        userAttr.userStatus = malloc( strlen("enable")+1 );
        strcpy( userAttr.userStatus, "enable" );

        exportDatabase( globalArgs.databaseFileName, databaseList, &databaseListTotal, &userAttr );
        printf( "MODE_SET_USERSTATUS_ENABLE success.\n" );

    }else if ( globalArgs.mode == MODE_SET_USERSTATUS_DISABLE ){
        sprintf( cmd, "mkdir %s", globalArgs.username );
        quickRun( cmd );
        sprintf( cmd, "cd %s", globalArgs.username );
        quickRun( cmd );

        importDatabase( globalArgs.databaseFileName, databaseList, &databaseListTotal, &userAttr );

        userAttr.userStatus = malloc( strlen("disable")+1 );
        strcpy( userAttr.userStatus, "disable" );

        exportDatabase( globalArgs.databaseFileName, databaseList, &databaseListTotal, &userAttr );
        printf( "MODE_SET_USERSTATUS_DISABLE success.\n" );

    }else{
        printf( "Sorry, this mode not coded yet.\n" );
    }
        
    return 0;
}

