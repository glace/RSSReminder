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

#ifndef XMLDOM_H
#define XMLDOM_H

#include <stdio.h>
#include <stdlib.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string.h>

#include "global.h"


char * mallocItoa( int num ){
    char temp[100];
    char * result;
    sprintf( temp, "%d", num);
    result = malloc( strlen(temp)+1 );
    strcpy(result, temp);
    return result;
}

int xitoa( char * outStr, int Innum){
    return sprintf(outStr, "%d", Innum);
}

int xatoi( char * inStr ){
    int result;
    sscanf( inStr, "%d", &result );
    return result;
}

int pushCDATA( char * inStr ){
    char * str;
    str = malloc( strlen(inStr) + strlen("<![CDATA[") + strlen("]]>") +1 );
    strcpy( str, "<![CDATA[" );
    strcat( str, inStr );
    strcat( str, "]]>");
    strcpy( inStr, str);
    free(str);
    return 1;
}

int exportXmlDom( const char * filename, struct itemnode_t * itemList, int * itemListTotal){
    int subscript;
    int attrSubscript;
    char longStr[100000];
    xmlNodePtr node;
    xmlNodePtr content;
    xmlNodePtr sonNode;
    xmlNodePtr grandsonNode;

    int strType;
    char title;
    char attrTitle[100];
    char attrValue[100];
    int attrTotal;
    int parent;
    int tail;

    // create the doc and root node
    xmlDocPtr doc = xmlNewDoc( BAD_CAST "1.0" );
    xmlNodePtr root_node = xmlNewNode( NULL, BAD_CAST "xmlDomList" );

    xitoa( longStr, *itemListTotal);
    xmlNewProp( root_node, BAD_CAST "itemListTotal", BAD_CAST longStr );

    // set the root node
    xmlDocSetRootElement( doc, root_node );

    

    for (subscript=1; subscript <= *itemListTotal; subscript++){
        node = xmlNewNode( NULL, BAD_CAST "item" );
        xmlAddChild( root_node, node );

        xitoa( longStr, subscript );
        xmlNewProp( node, BAD_CAST "subscript", BAD_CAST longStr );

        xitoa( longStr, itemList[subscript].type );
        sonNode = xmlNewNode( NULL, BAD_CAST "type" );
        content = xmlNewText( BAD_CAST longStr );
        xmlAddChild( sonNode, content );
        xmlAddChild( node, sonNode );

        sonNode = xmlNewNode( NULL, BAD_CAST "title" );
        // strcpy( longStr, itemList[subscript].title );
        // pushCDATA( longStr );
        content = xmlNewCDataBlock( NULL, BAD_CAST itemList[subscript].title, strlen(itemList[subscript].title) );
        xmlAddChild( sonNode, content );
        xmlAddChild( node, sonNode );

        sonNode = xmlNewNode( NULL, BAD_CAST "attr" );
        xitoa( longStr, itemList[subscript].attrTotal );
        xmlNewProp( sonNode, BAD_CAST "attrTotal", BAD_CAST longStr);
        xmlAddChild( node, sonNode );

        for ( attrSubscript=0; attrSubscript < itemList[subscript].attrTotal; attrSubscript++){
            xitoa( longStr, attrSubscript );

            grandsonNode = xmlNewNode( NULL, BAD_CAST "attrTitle");
            xmlNewProp( grandsonNode, BAD_CAST "attrSubscript", BAD_CAST longStr );
            content = xmlNewText( BAD_CAST itemList[subscript].attrTitle[attrSubscript]);
            xmlAddChild( grandsonNode, content );
            xmlAddChild( sonNode, grandsonNode );

            grandsonNode = xmlNewNode( NULL, BAD_CAST "attrValue");
            xmlNewProp( grandsonNode, BAD_CAST "attrSubscript", BAD_CAST longStr );
            content = xmlNewText( BAD_CAST itemList[subscript].attrValue[attrSubscript]);
            xmlAddChild( grandsonNode, content );
            xmlAddChild( sonNode, grandsonNode );
        }

        xitoa( longStr, itemList[subscript].parent );
        sonNode = xmlNewNode( NULL, BAD_CAST "parent" );
        content = xmlNewText( BAD_CAST longStr );
        xmlAddChild( sonNode, content );
        xmlAddChild( node, sonNode );

        xitoa( longStr, itemList[subscript].tail );
        sonNode = xmlNewNode( NULL, BAD_CAST "tail" );
        content = xmlNewText( BAD_CAST longStr );
        xmlAddChild( sonNode, content );
        xmlAddChild( node, sonNode );
    }

    int nRel = xmlSaveFile( filename, doc);
    /*if (nRel != -1)
        printf(" created, %d bytes.\n", nRel);*/

    xmlFreeDoc(doc);

    if (nRel != -1) return 1;
    else return 0;
}

#endif
