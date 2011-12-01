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

#include "database.h"

int xatoi( char * inStr ){
    int result;
    sscanf( inStr, "%d", &result );
    return result;
}

int xitoa( char * outStr, int Innum){
    return sprintf(outStr, "%d", Innum);
}

int importDatabase( const char * filename, struct database_t * databaseList, int * databaseListTotal, struct userAttr_t * userAttr ){
    int subscript;
    int subscriptCount;
    int attrSubscript;
    xmlChar * longStr;
    xmlNodePtr node;
    xmlNodePtr content;
    xmlNodePtr sonNode;
    xmlNodePtr grandsonNode;

    *databaseListTotal = 0;
    userAttr->userStatus = NULL;
    userAttr->mobileNumber = NULL;

    xmlKeepBlanksDefault(0);

    // create the doc and root node
    xmlDocPtr doc = xmlReadFile( filename, "UTF-8", XML_PARSE_RECOVER );

    if ( doc == NULL ){
        // printf("Error: xml Data file \"%s\" import failed.\n", filename);
        return 0;
    }

    xmlNodePtr root_node = xmlDocGetRootElement( doc );

    if ( root_node == NULL ){
        // printf("Error: failed to get the root of xml doc.\n");
        return 0;
    }

    if ( xmlStrcmp( root_node->name, BAD_CAST "databaseList") != 0 ){
        // printf("Error: name of root node of xml doc not matched.\n");
        return 0;
    }

    if ( xmlHasProp( root_node, BAD_CAST "databaseListTotal") && xmlHasProp( root_node, BAD_CAST "userStatus") ){
        longStr = xmlGetProp( root_node, BAD_CAST "databaseListTotal" );
        *databaseListTotal = xatoi( (char *) longStr );
        if ( *databaseListTotal > MAX_DATABASE_SAVE ) *databaseListTotal = MAX_DATABASE_SAVE;

        longStr = xmlGetProp( root_node, BAD_CAST "userStatus" );
        userAttr->userStatus = malloc( strlen(longStr)+1 );
        strcpy( userAttr->userStatus, longStr );
        // printf("itemListTotal: %d\n", *itemListTotal );
    }else{
        // printf("Error: failed to get the itemListTotal.\n");
        return 0;
    }

    if ( xmlHasProp( root_node, BAD_CAST "mobileNumber") ){
        longStr = xmlGetProp( root_node, BAD_CAST "mobileNumber" );
        userAttr->mobileNumber = malloc( strlen(longStr)+1 );
        strcpy( userAttr->mobileNumber, longStr );
    }

    node = root_node->xmlChildrenNode;

    for (subscriptCount=1; subscriptCount <= *databaseListTotal; subscriptCount++){
        // printf("subscriptCount: %d\n", subscriptCount);
        if ( node == NULL ){
            // printf("Error: not enough item node.\n");
            *databaseListTotal = 0;
            return 0;
        }

        subscript = subscriptCount;

        databaseList[subscript].url = NULL;
        databaseList[subscript].urlstatus = NULL;
        databaseList[subscript].filenamePrefix = NULL;

        sonNode = node->xmlChildrenNode;
        while( sonNode != NULL ){

            if ( xmlStrcmp( sonNode->name, BAD_CAST "url" ) == 0){
                longStr = xmlNodeGetContent( sonNode );
                databaseList[subscript].url = malloc( strlen(longStr) +1 );
                strcpy( databaseList[subscript].url, (char *) longStr );
            }else if ( xmlStrcmp( sonNode->name, BAD_CAST "urlstatus" ) == 0){
                longStr = xmlNodeGetContent( sonNode );
                databaseList[subscript].urlstatus = malloc( strlen(longStr) +1 );
                strcpy( databaseList[subscript].urlstatus, (char *) longStr );
            }else if ( xmlStrcmp( sonNode->name, BAD_CAST "filenamePrefix" ) == 0){
                longStr = xmlNodeGetContent( sonNode );
                databaseList[subscript].filenamePrefix = malloc( strlen(longStr) +1 );
                strcpy( databaseList[subscript].filenamePrefix, (char *) longStr );
            }else{
                // printf("Error: unknown sonNode of xml doc item, %s", (char *) sonNode->name );
                *databaseListTotal = 0;
                return 0;
            }

            sonNode = sonNode->next;
        }

        node = node->next;
    }
    printf("databaseList import success.\n");
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return 1;
}

int exportDatabase( const char * filename, struct database_t * databaseList, int * databaseListTotal, struct userAttr_t * userAttr ){
    int subscript;
    char longStr[100000];
    char message[500];
    xmlNodePtr node;
    xmlNodePtr content;
    xmlNodePtr sonNode;
    xmlNodePtr grandsonNode;

    // create the doc and root node
    xmlDocPtr doc = xmlNewDoc( BAD_CAST "1.0" );
    xmlNodePtr root_node = xmlNewNode( NULL, BAD_CAST "databaseList" );

    xitoa( longStr, *databaseListTotal);
    xmlNewProp( root_node, BAD_CAST "databaseListTotal", BAD_CAST longStr );

    if ( userAttr->userStatus == NULL )
        xmlNewProp( root_node, BAD_CAST "userStatus", BAD_CAST "disable" );
    else xmlNewProp( root_node, BAD_CAST "userStatus", BAD_CAST userAttr->userStatus );

    if ( userAttr->mobileNumber != NULL )
        xmlNewProp( root_node, BAD_CAST "userStatus", BAD_CAST userAttr->mobileNumber );

    // set the root node
    xmlDocSetRootElement( doc, root_node );

    // if ( *xmlItemListAlarmTotal > *xmlItemListTotal ) *xmlItemListAlarmTotal = *xmlItemListTotal;

    for (subscript=1; subscript <= *databaseListTotal; subscript++){
        node = xmlNewNode( NULL, BAD_CAST "item" );
        xmlAddChild( root_node, node );

        // xitoa( longStr, subscript );
        // xmlNewProp( node, BAD_CAST "subscript", BAD_CAST longStr );

        sonNode = xmlNewNode( NULL, BAD_CAST "url" );
        content = xmlNewCDataBlock( NULL, BAD_CAST databaseList[subscript].url, strlen(databaseList[subscript].url) );
        xmlAddChild( sonNode, content );
        xmlAddChild( node, sonNode );

        sonNode = xmlNewNode( NULL, BAD_CAST "urlstatus" );
        content = xmlNewCDataBlock( NULL, BAD_CAST databaseList[subscript].urlstatus, strlen(databaseList[subscript].urlstatus) );
        xmlAddChild( sonNode, content );
        xmlAddChild( node, sonNode );

        sonNode = xmlNewNode( NULL, BAD_CAST "filenamePrefix" );
        content = xmlNewCDataBlock( NULL, BAD_CAST databaseList[subscript].filenamePrefix, strlen(databaseList[subscript].filenamePrefix) );
        xmlAddChild( sonNode, content );
        xmlAddChild( node, sonNode );
    }

    int nRel = xmlSaveFormatFileEnc( filename, doc, "UTF-8", 1);
    /*if (nRel != -1)
        printf(" created, %d bytes.\n", nRel);*/

    xmlFreeDoc(doc);

    if (nRel != -1) return 1;
    else return 0;
}
