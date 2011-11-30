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

#include "xmlitem.h"
#include "md5.h"

int exportRuleFile( const char * filename, struct xmlDesc_t * xmlDesc ){
    int subscript;
    int subscriptCount;
    int attrSubscript;
    char longStr[100000];
    char * str;
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
    xmlNodePtr root_node = xmlNewNode( NULL, BAD_CAST "xmlItemDesc" );

    xmlNewProp( root_node, BAD_CAST "flag", BAD_CAST "standard");

    // set the root node
    xmlDocSetRootElement( doc, root_node );

    

    for (subscriptCount=1; subscriptCount <= 4; subscriptCount++){
        if (subscriptCount == 1) subscript = XML_ITEM_REGION;
        else if (subscriptCount == 2 ) subscript = XML_ITEM_TITLE;
        else if (subscriptCount == 3 ) subscript = XML_ITEM_TIME;
        else if (subscriptCount == 4 ) subscript = XML_ITEM_CONTENT;

        if ( xmlDesc[subscript].title == NULL && xmlDesc[subscript].attrTotal <= 0 ) continue;

        if ( subscript == XML_ITEM_REGION ) node = xmlNewNode( NULL, BAD_CAST "region" );
        else if ( subscript == XML_ITEM_TITLE) node = xmlNewNode( NULL, BAD_CAST "title" );
        else if ( subscript == XML_ITEM_TIME ) node = xmlNewNode( NULL, BAD_CAST "time" );
        else if ( subscript == XML_ITEM_CONTENT ) node = xmlNewNode( NULL, BAD_CAST "content" );
        xmlAddChild( root_node, node );

        // xitoa( longStr, subscript );
        // xmlNewProp( node, BAD_CAST "subscript", BAD_CAST longStr );

        // xitoa( longStr, itemList[subscript].type );
        // sonNode = xmlNewNode( NULL, BAD_CAST "type" );
        // content = xmlNewText( BAD_CAST longStr );
        // xmlAddChild( sonNode, content );
        // xmlAddChild( node, sonNode );

        if ( xmlDesc[subscript].title != NULL ){
            sonNode = xmlNewNode( NULL, BAD_CAST "title" );
            // strcpy( longStr, itemList[subscript].title );
            // pushCDATA( longStr );
            content = xmlNewCDataBlock( NULL, BAD_CAST xmlDesc[subscript].title, strlen(xmlDesc[subscript].title) );
            // content = xmlNewText( BAD_CAST itemList[subscript].attrValue[attrSubscript]);

            //         str= (char *)xmlNodeGetContent( content);
            // printf("%d:%d[%s]%s\n",subscript, strlen(xmlDesc[subscript].title), xmlDesc[subscript].title,str);
            xmlAddChild( sonNode, content );
            xmlAddChild( node, sonNode );
        }

        if ( xmlDesc[subscript].attrTotal > 0){
            sonNode = xmlNewNode( NULL, BAD_CAST "attr" );
            xitoa( longStr, xmlDesc[subscript].attrTotal );
            xmlNewProp( sonNode, BAD_CAST "attrTotal", BAD_CAST longStr);
            xmlAddChild( node, sonNode );

            for ( attrSubscript=0; attrSubscript < xmlDesc[subscript].attrTotal; attrSubscript++){
                xitoa( longStr, attrSubscript );
    
                grandsonNode = xmlNewNode( NULL, BAD_CAST "attrTitle");
                xmlNewProp( grandsonNode, BAD_CAST "attrSubscript", BAD_CAST longStr );
                content = xmlNewText( BAD_CAST xmlDesc[subscript].attrTitle[attrSubscript]);
                xmlAddChild( grandsonNode, content );
                xmlAddChild( sonNode, grandsonNode );

                grandsonNode = xmlNewNode( NULL, BAD_CAST "attrValue");
                xmlNewProp( grandsonNode, BAD_CAST "attrSubscript", BAD_CAST longStr );
                content = xmlNewText( BAD_CAST xmlDesc[subscript].attrValue[attrSubscript]);
                xmlAddChild( grandsonNode, content );
                xmlAddChild( sonNode, grandsonNode );
            }
        }
    }

    int nRel = xmlSaveFormatFileEnc( filename, doc, "UTF-8", 1);
    /*if (nRel != -1)
        printf(" created, %d bytes.\n", nRel);*/

    xmlFreeDoc(doc);

    if (nRel != -1) return 1;
    else return 0;
}

int importRuleFile( const char * filename, struct itemnode_t * itemList, int * itemListTotal, struct xmlDesc_t * xmlDesc ){
    int subscript;
    int subscriptCount;
    int attrSubscript;
    int targetSubscript;
    int targetAttrSubscript;
    int targetCount;
    xmlChar * longStr;
    xmlNodePtr node;
    xmlNodePtr content;
    xmlNodePtr sonNode;
    xmlNodePtr grandsonNode;

    int format=1;
    int type;
    int total;
    struct xmlDescID_t xmlDescID;

    int found;

    xmlDescID.data[XML_ITEM_NOG][0] = 0;
    xmlDescID.data[XML_ITEM_REGION][0] = 0;
    xmlDescID.data[XML_ITEM_TITLE][0] = 0;
    xmlDescID.data[XML_ITEM_TIME][0] = 0;
    xmlDescID.data[XML_ITEM_CONTENT][0] = 0;

    xmlDesc[XML_ITEM_REGION].title = NULL;
    xmlDesc[XML_ITEM_REGION].attrTotal = 0;
    xmlDesc[XML_ITEM_TITLE].title = NULL;
    xmlDesc[XML_ITEM_TITLE].attrTotal = 0;
    xmlDesc[XML_ITEM_TIME].title = NULL;
    xmlDesc[XML_ITEM_TIME].attrTotal = 0;
    xmlDesc[XML_ITEM_CONTENT].title = NULL;
    xmlDesc[XML_ITEM_CONTENT].attrTotal = 0;


    xmlKeepBlanksDefault(0);

    // create the doc and root node
    xmlDocPtr doc = xmlReadFile( filename, "UTF-8", XML_PARSE_RECOVER );

    if ( doc == NULL ){
        printf("Error: xml Data file \"%s\" import failed.\n", filename);
        return 0;
    }

    xmlNodePtr root_node = xmlDocGetRootElement( doc );

    if ( root_node == NULL ){
        printf("Error: failed to get the root of xml doc.\n");
        return 0;
    }

    if ( xmlStrcmp( root_node->name, BAD_CAST "xmlItemDesc") != 0 ){
        printf("Error: name of root node of xml rulefile not matched.\n");
        return 0;
    }

    if ( xmlHasProp( root_node, BAD_CAST "flag") ){
        longStr = xmlGetProp( root_node, BAD_CAST "flag" );
        if ( xmlStrcmp( longStr, BAD_CAST "id" ) == 0 ) format= XML_DESC_FORMAT_ID;
        else if ( xmlStrcmp( longStr, BAD_CAST "standard" ) == 0 ) format= XML_DESC_FORMAT_STANDARD;
        else{
            printf("Error: unknown flag value.\n");
            return 0;
        }
    }

    node = root_node->xmlChildrenNode;

    while( node != NULL ){
        // printf("subscriptCount: %d\n", subscriptCount);
        if ( node == NULL ){
            printf("Error: not enough item node.\n");
            // *itemListTotal = 0;
            return 0;
        }

        type = XML_ITEM_NOG;
        if ( xmlStrcmp( node->name, BAD_CAST "region") == 0) type = XML_ITEM_REGION;
        else if ( xmlStrcmp( node->name, BAD_CAST "title") == 0) type = XML_ITEM_TITLE;
        else if ( xmlStrcmp( node->name, BAD_CAST "time") == 0) type = XML_ITEM_TIME;
        else if ( xmlStrcmp( node->name, BAD_CAST "content") == 0) type = XML_ITEM_CONTENT;

        if ( type == XML_ITEM_NOG ){
            node = node->next;
            continue;
        }
        total = 0;
        sonNode = node->xmlChildrenNode;
        while( sonNode != NULL ){
            if ( format == XML_DESC_FORMAT_ID){
                if ( xmlStrcmp( sonNode->name, BAD_CAST "subscript" ) == 0){
                    longStr = xmlNodeGetContent( sonNode );
                    total++;
                    xmlDescID.data[type][total] = xatoi( (char *) longStr );
                }else{
                    printf("Error: unknown sonNode of xml item desc, %s", (char *) sonNode->name );
                    return 0;
                }
            }else if ( format == XML_DESC_FORMAT_STANDARD ){
                if ( xmlStrcmp( sonNode->name, BAD_CAST "title" ) == 0){
                    longStr = xmlNodeGetContent( sonNode );
                    xmlDesc[type].title = longStr; 
                }else if ( xmlStrcmp( sonNode->name, BAD_CAST "attr" ) == 0){
                    
                    if ( xmlHasProp( sonNode, BAD_CAST "attrTotal") ){
                        longStr = xmlGetProp( sonNode, BAD_CAST "attrTotal" );
                        xmlDesc[type].attrTotal = xatoi( (char *) longStr );
                        for (subscript=1; subscript<= xmlDesc[type].attrTotal; subscript++){
                            xmlDesc[type].attrTitle[subscript] = NULL;
                            xmlDesc[type].attrValue[subscript] = NULL;
                        }
                        grandsonNode = sonNode->xmlChildrenNode;
                        while ( grandsonNode != NULL ){
                            if ( !xmlHasProp( grandsonNode, BAD_CAST "attrSubscript" )){
                                printf("Error: failed to get the attrSubscript.\n");
                                return 0;
                            }
                            longStr = xmlGetProp( grandsonNode, BAD_CAST "attrSubscript" );
                            attrSubscript = xatoi( (char *) longStr );

                            if ( xmlStrcmp( grandsonNode->name, BAD_CAST "attrTitle" ) == 0 ){
                                longStr = xmlNodeGetContent( grandsonNode );
                                xmlDesc[type].attrTitle[attrSubscript] = malloc( strlen(longStr) +1 );
                                strcpy( xmlDesc[type].attrTitle[attrSubscript], (char *) longStr );
                            }if ( xmlStrcmp( grandsonNode->name, BAD_CAST "attrValue" ) == 0 ){
                                longStr = xmlNodeGetContent( grandsonNode );
                                xmlDesc[type].attrValue[attrSubscript] = malloc( strlen(longStr) +1 );
                                strcpy( xmlDesc[type].attrValue[attrSubscript], (char *) longStr );
                            }
                            grandsonNode = grandsonNode->next;
                        }
                    }else{
                        printf("Error: failed to get attrTotal.\n");
                        return 0;
                    }

                }else{
                    printf("Error: unknown sonNode of xml item desc, %s", (char *) sonNode->name );
                    return 0;
                }
            }

            sonNode = sonNode->next;
        }
        if ( format == XML_DESC_FORMAT_ID ) xmlDescID.data[type][0] = total;

        node = node->next;
    }
    if ( format == XML_DESC_FORMAT_ID ){
        xmlDescPrint( &xmlDescID );

        for (subscriptCount=1; subscriptCount<=4; subscriptCount++){
            if (subscriptCount == 1) subscript = XML_ITEM_REGION;
            else if (subscriptCount == 2 ) subscript = XML_ITEM_TITLE;
            else if (subscriptCount == 3 ) subscript = XML_ITEM_TIME;
            else if (subscriptCount == 4 ) subscript = XML_ITEM_CONTENT;

                printf("-");
            if ( xmlDescID.data[subscript][0] <= 0 ) continue;
                printf("+\n");

            for ( targetCount=1; targetCount<=xmlDescID.data[subscript][0]; targetCount++ ){
                targetSubscript = xmlDescID.data[subscript][targetCount];
                // printf("234\n");

                if (targetCount == 1){
                    longStr = itemList[targetSubscript].title;
                    // printf("%s\n", (char *) longStr);
                    xmlDesc[subscript].title = malloc( strlen(longStr) +1 );
                    strcpy( xmlDesc[subscript].title, (char *) longStr );
                    // printf("%s\n", xmlDesc[subscript].title);

                    for ( attrSubscript=0; attrSubscript<itemList[targetSubscript].attrTotal; attrSubscript++ ){
                        longStr = itemList[targetSubscript].attrTitle[attrSubscript];
                        xmlDesc[subscript].attrTitle[attrSubscript] = malloc( strlen(longStr) +1 );
                        strcpy( xmlDesc[subscript].attrTitle[attrSubscript], (char *) longStr );
                        printf("Title:  %s\n", (char *) longStr);
    
                        longStr = itemList[targetSubscript].attrValue[attrSubscript];
                        xmlDesc[subscript].attrValue[attrSubscript] = malloc( strlen(longStr) +1 );
                        strcpy( xmlDesc[subscript].attrValue[attrSubscript], (char *) longStr );
                        printf("Value:  %s\n", (char *) longStr);
                    }
                    xmlDesc[subscript].attrTotal = itemList[targetSubscript].attrTotal;
                    printf("attrTotal: %d\n", xmlDesc[subscript].attrTotal);
                    for ( attrSubscript=0; attrSubscript<itemList[targetSubscript].attrTotal; attrSubscript++ ){
                        printf("%s\n%s\n", xmlDesc[subscript].attrTitle[attrSubscript],xmlDesc[subscript].attrValue[attrSubscript]);
                    }
                    printf("=====\n");
                }else{
                    longStr = itemList[targetSubscript].title;
                    if ( xmlDesc[subscript].title != NULL && xmlStrcmp( longStr, BAD_CAST xmlDesc[subscript].title ) != 0){
                        free( xmlDesc[subscript].title );
                        xmlDesc[subscript].title = NULL;
                    }
                    for ( attrSubscript=0; attrSubscript<xmlDesc[subscript].attrTotal; attrSubscript++ ){
                        printf("%s\n%s\n", xmlDesc[subscript].attrTitle[attrSubscript],xmlDesc[subscript].attrValue[attrSubscript]);
                    }
                    printf("=====\n");
                    attrSubscript = 0;
                    while( attrSubscript<xmlDesc[subscript].attrTotal ){
                        found = -1;
                            printf("  %s\n%s\n",xmlDesc[subscript].attrTitle[attrSubscript],xmlDesc[subscript].attrValue[attrSubscript]);
                        for ( targetAttrSubscript=0; targetAttrSubscript<itemList[targetSubscript].attrTotal; targetAttrSubscript++)
                            if ( strcmp( xmlDesc[subscript].attrTitle[attrSubscript], itemList[targetSubscript].attrTitle[targetAttrSubscript]) == 0 ){
                                if ( xmlDesc[subscript].attrValue[attrSubscript] == NULL && itemList[targetSubscript].attrValue[targetAttrSubscript] == NULL){
                                    found = targetAttrSubscript;
                                    break;
                                }else if ( xmlDesc[subscript].attrValue[attrSubscript] != NULL && itemList[targetSubscript].attrValue[targetAttrSubscript] != NULL 
                                           && strcmp( xmlDesc[subscript].attrValue[attrSubscript], itemList[targetSubscript].attrValue[targetAttrSubscript]) == 0 ){
                                    found = targetAttrSubscript;
                                    break;
                                }
                            }
                        if ( found == -1 ){
                            free( xmlDesc[subscript].attrTitle[attrSubscript] );
                            xmlDesc[subscript].attrTitle[attrSubscript] = xmlDesc[subscript].attrTitle[ xmlDesc[subscript].attrTotal-1 ];
                            if ( xmlDesc[subscript].attrValue[attrSubscript] != NULL ){
                                free( xmlDesc[subscript].attrValue[attrSubscript] );
                                xmlDesc[subscript].attrValue[attrSubscript] = xmlDesc[subscript].attrValue[ xmlDesc[subscript].attrTotal-1 ];
                            }
                            xmlDesc[subscript].attrTotal--;
                        }else attrSubscript++;
                    }
                    printf("-===-\n");
                    for ( attrSubscript=0; attrSubscript<xmlDesc[subscript].attrTotal; attrSubscript++ ){
                        printf("%s\n%s\n", xmlDesc[subscript].attrTitle[attrSubscript],xmlDesc[subscript].attrValue[attrSubscript]);
                    }
                    printf("=====\n");
                }
            }
            printf("subscript: %d\n", subscript);
            printDescNode( xmlDesc, subscript );
            // printf("%s\n", xmlDesc[XML_ITEM_TITLE].title);
        }
    }
    printf("import success.\n");
    xmlFreeDoc(doc);
    xmlCleanupParser();
    exportRuleFile( filename, xmlDesc );
    return 1;
}

int xmlDescPrint( struct xmlDescID_t * xmlDesc ){
    int i;
    printf("Region:\n");
    for (i=1;i<=xmlDesc->data[XML_ITEM_REGION][0];i++) printf(" %d", xmlDesc->data[XML_ITEM_REGION][i]);
    printf("\n");

    printf("Title:\n");
    for (i=1;i<=xmlDesc->data[XML_ITEM_TITLE][0];i++) printf(" %d", xmlDesc->data[XML_ITEM_TITLE][i]);
    printf("\n");

    printf("Time:\n");
    for (i=1;i<=xmlDesc->data[XML_ITEM_TIME][0];i++) printf(" %d", xmlDesc->data[XML_ITEM_TIME][i]);
    printf("\n");

    printf("Content:\n");
    for (i=1;i<=xmlDesc->data[XML_ITEM_CONTENT][0];i++) printf(" %d", xmlDesc->data[XML_ITEM_CONTENT][i]);
    printf("\n###");
    return 1;

}

int printDescNode(struct xmlDesc_t * xmlDesc, int nodeSubscript){
    int type;
    int parent;
    int tail;
    int i;
    struct xmlDesc_t * target;
    // if (nodeSubscript < 0 || nodeSubscript > itemListTotal) return 0;
    target = &xmlDesc[nodeSubscript];
    type = target->type;
    // printSpace(deepLen);
    // printIT(type);
    printf("\t\t nodeSubscript: %d\n", nodeSubscript);

    // printSpace(deepLen);
    // printf("\t parent: %d\t tail: %d\n", parent, tail);

    // printSpace(deepLen);
    // if (type == IT_ITM || type == IT_TAL){
        printf("<");
        if (type == IT_TAL) printf("/");
        if (target->title) printf("%s", target->title);
        for (i=0; i<target->attrTotal ; i++){
            printf(" %s", target->attrTitle[i]);
            if (target->attrValue[i] != NULL)
                printf("=%s", target->attrValue[i]);
        }
        printf(">\n");
    // }else if (type == IT_TXT || type == IT_NOG){
    //     printf("%s\n", target->title);
    // }
    return 1;
}

int fetchRuleSingle( struct itemnode_t item, struct xmlDesc_t xmlDesc ){
    int matched = 0;
    int attrSubscript;
    int targetAttrSubscript;
    int found;
    if ( xmlDesc.title != NULL ){
        if ( item.title == NULL ) return 0;
        if ( strcmp( item.title, xmlDesc.title ) != 0 ) return 0;
    }
    for ( attrSubscript=0; attrSubscript<xmlDesc.attrTotal; attrSubscript++ ){
        found = -1;
        for ( targetAttrSubscript=0; targetAttrSubscript<item.attrTotal; targetAttrSubscript++)
            if ( strcmp( xmlDesc.attrTitle[attrSubscript], item.attrTitle[targetAttrSubscript]) == 0 ){
                if ( xmlDesc.attrValue[attrSubscript] == NULL && item.attrValue[targetAttrSubscript] == NULL){
                    found = targetAttrSubscript;
                    break;
                }else if ( xmlDesc.attrValue[attrSubscript] != NULL && item.attrValue[targetAttrSubscript] != NULL 
                           && strcmp( xmlDesc.attrValue[attrSubscript], item.attrValue[targetAttrSubscript]) == 0 ){
                    found = targetAttrSubscript;
                    break;
                }
            }
        if ( found == -1 ) return 0;
    }
    return 1;
}

int fetchRule( struct itemnode_t * itemList, int * itemListTotal, struct xmlDesc_t xmlDesc, int * flag ){
    int subscript;
    flag[0] = XML_FETCH_ITEM_NOG;
    for (subscript=1; subscript<=*itemListTotal; subscript++){
        flag[subscript] = XML_FETCH_ITEM_NOG;
        if ( flag[ itemList[subscript].parent ] != XML_FETCH_ITEM_NOG ){
            flag[subscript] = XML_FETCH_ITEM_CHILD;
            continue;
        }else if ( fetchRuleSingle( itemList[subscript], xmlDesc ) ){
            flag[subscript] = XML_FETCH_ITEM_START;
            continue;
        }
    }
    return 1;
}

int generateXmlItem( struct itemnode_t * itemList, int * itemListTotal, struct xmlDesc_t * xmlDesc , struct xmlItem_t * xmlItemList, int * xmlItemListTotal, int * xmlItemListAlarmTotal ){
    char longStr[100000];
    char md5BaseStr[100000];
    char guid[50];
    int flagRegion[MAX_ITEMLIST_LENGTH];
    int flagTitle[MAX_ITEMLIST_LENGTH];
    int flagTime[MAX_ITEMLIST_LENGTH];
    int flagContent[MAX_ITEMLIST_LENGTH];

    int flagXmlItem[MAX_ITEMLIST_LENGTH][7];
    int xmlItemTotal = 0;

    int subscript;
    int targetSubscript;
    int xmlItemListFreshPoint = 0;
    int offset;

    fetchRule( itemList, itemListTotal, xmlDesc[XML_ITEM_REGION], flagRegion );
    fetchRule( itemList, itemListTotal, xmlDesc[XML_ITEM_TITLE], flagTitle );
    fetchRule( itemList, itemListTotal, xmlDesc[XML_ITEM_TIME], flagTime );
    fetchRule( itemList, itemListTotal, xmlDesc[XML_ITEM_CONTENT], flagContent );

    memset(flagXmlItem, 0, sizeof(flagXmlItem));

    for ( subscript = 1; subscript<=*itemListTotal; subscript++ ){
        if ( flagRegion[subscript] == XML_FETCH_ITEM_NOG ) continue;
        if ( flagTitle[subscript] == XML_FETCH_ITEM_START ){
            if ( flagXmlItem[xmlItemTotal][XML_ITEM_TITLE] != 0 || xmlItemTotal ==0 ) xmlItemTotal++;
            flagXmlItem[xmlItemTotal][XML_ITEM_TITLE] = subscript;
        }
        if ( flagTime[subscript] == XML_FETCH_ITEM_START ){
            if ( flagXmlItem[xmlItemTotal][XML_ITEM_TIME] != 0 || xmlItemTotal ==0 ) xmlItemTotal++;
            flagXmlItem[xmlItemTotal][XML_ITEM_TIME] = subscript;
        }
        if ( flagContent[subscript] == XML_FETCH_ITEM_START ){
            if ( flagXmlItem[xmlItemTotal][XML_ITEM_CONTENT] != 0 || xmlItemTotal ==0 ) xmlItemTotal++;
            flagXmlItem[xmlItemTotal][XML_ITEM_CONTENT] = subscript;
        }
    }

    // *xmlItemListTotal = 0;  // warning !!!  this line just for debuging

    // printf("{================\n");
    // for ( subscript=1; subscript<=*xmlItemListTotal; subscript++)
    //     printXmlItemListNode( xmlItemList, xmlItemListTotal, subscript );
    // printf("}================\n");

    for (subscript=1; subscript<=xmlItemTotal; subscript++){
        printf("fetch\n\ttitle: %d\n\ttime: %d\n\tcontent: %d\n", flagXmlItem[subscript][XML_ITEM_TITLE],flagXmlItem[subscript][XML_ITEM_TIME],flagXmlItem[subscript][XML_ITEM_CONTENT]);
        // getItemContent( itemList, itemListTotal, flagTitle, flagXmlItem[subscript][XML_ITEM_TITLE], longStr );
        // printf("%s\n", longStr);
        // Rmd5(longStr,longStr);
        // printf("%s\n",longStr);
        // getItemRichContent( itemList, itemListTotal, flagTitle, flagXmlItem[subscript][XML_ITEM_TITLE], longStr );
        // // printf("%s\n", longStr);
        // getItemContent( itemList, itemListTotal, flagContent, flagXmlItem[subscript][XML_ITEM_CONTENT], longStr );
        // printf("%s\n", longStr);
        // Rmd5(longStr,longStr);
        // printf("%s\n",longStr);
        // getItemRichContent( itemList, itemListTotal, flagContent, flagXmlItem[subscript][XML_ITEM_CONTENT], longStr );
        // // printf("%s\n", longStr);

        strcpy( md5BaseStr, "" );
        getItemContent( itemList, itemListTotal, flagTitle, flagXmlItem[subscript][XML_ITEM_TITLE], longStr );
        strcat( md5BaseStr, longStr );
        getItemContent( itemList, itemListTotal, flagContent, flagXmlItem[subscript][XML_ITEM_CONTENT], longStr );
        strcat( md5BaseStr, longStr );
        Rmd5( md5BaseStr, guid );
        // printf("md5 guid: %s\n", guid);
        
        targetSubscript = xmlItemListSearch( xmlItemList, xmlItemListTotal, guid );
        // printf("%d\n",targetSubscript);
        if ( targetSubscript == 0 ){
            printf("not found, to create it. %d\n", targetSubscript);
            (*xmlItemListTotal)++;
            targetSubscript = *xmlItemListTotal;
            // printf("%d\n",targetSubscript);

            xmlItemList[targetSubscript].text[FRSS_ITEM_TITLE] = NULL;
            // xmlItemList[targetSubscript].richText[FRSS_ITEM_TITLE] = NULL;
            xmlItemList[targetSubscript].text[FRSS_ITEM_TIME] = NULL;
            // xmlItemList[targetSubscript].richText[FRSS_ITEM_TIME] = NULL;
            xmlItemList[targetSubscript].text[FRSS_ITEM_CONTENT] = NULL;
            xmlItemList[targetSubscript].richText[FRSS_ITEM_CONTENT] = NULL;
            xmlItemList[targetSubscript].text[FRSS_ITEM_GUID] = NULL;
            // xmlItemList[targetSubscript].richText[FRSS_ITEM_GUID] = NULL;
            if ( xmlItemListFreshPoint == 0 ) xmlItemListFreshPoint = targetSubscript;

            if ( getItemContent( itemList, itemListTotal, flagTitle, flagXmlItem[subscript][XML_ITEM_TITLE], longStr ) ){
                xmlItemList[targetSubscript].text[FRSS_ITEM_TITLE] = malloc( strlen(longStr)+1 );
                strcpy( xmlItemList[targetSubscript].text[FRSS_ITEM_TITLE], longStr );
            }
            if ( getItemContent( itemList, itemListTotal, flagTime, flagXmlItem[subscript][XML_ITEM_TIME], longStr ) ){
                xmlItemList[targetSubscript].text[FRSS_ITEM_TIME] = malloc( strlen(longStr)+1 );
                strcpy( xmlItemList[targetSubscript].text[FRSS_ITEM_TIME], longStr );
            }
            if ( getItemContent( itemList, itemListTotal, flagContent, flagXmlItem[subscript][XML_ITEM_CONTENT], longStr ) ){
                xmlItemList[targetSubscript].text[FRSS_ITEM_CONTENT] = malloc( strlen(longStr)+1 );
                strcpy( xmlItemList[targetSubscript].text[FRSS_ITEM_CONTENT], longStr );
            }

            xmlItemList[targetSubscript].text[FRSS_ITEM_GUID] = malloc( strlen(guid)+1 );
            strcpy( xmlItemList[targetSubscript].text[FRSS_ITEM_GUID], guid );
        }
    }
    // for ( subscript=1; subscript<=*xmlItemListTotal; subscript++)
    //     printXmlItemListNode( xmlItemList, xmlItemListTotal, subscript );

    if ( xmlItemListFreshPoint != 0 ){
        offset = *xmlItemListTotal - (xmlItemListFreshPoint-1);
        *xmlItemListAlarmTotal = offset;
        for ( subscript=1; subscript<=offset; subscript++ )
            swapXmlItemListNode( xmlItemList, xmlItemListTotal, *xmlItemListTotal+subscript, xmlItemListFreshPoint-1+subscript );
        for ( subscript=xmlItemListFreshPoint-1; subscript>=1; subscript--)
            swapXmlItemListNode( xmlItemList, xmlItemListTotal, subscript, subscript+offset );
        for ( subscript=1; subscript<=offset; subscript++ )
            swapXmlItemListNode( xmlItemList, xmlItemListTotal, *xmlItemListTotal+subscript, subscript );
    }

    // for ( subscript=1; subscript<=*xmlItemListTotal; subscript++)
    //     printXmlItemListNode( xmlItemList, xmlItemListTotal, subscript );

    // int a[10]={0,4,5,6,7,1,2,3};
    // int i;
    // for (i=1;i<=7;i++) printf(" %d",a[i]);
    // printf("\n");
    // xmlItemListFreshPoint = 5;
    // *xmlItemListTotal=7;
    // if ( xmlItemListFreshPoint != 0 ){
    //     offset = 7- (xmlItemListFreshPoint-1);
    //     for ( subscript=1; subscript<=offset; subscript++ )
    //         swap(  &(a[*xmlItemListTotal+subscript]), &(a[xmlItemListFreshPoint-1+subscript]) );
    //     for ( subscript=xmlItemListFreshPoint-1; subscript>=1; subscript--)
    //         swap( &(a[subscript]), &(a[subscript+offset]) );
    //     for ( subscript=1; subscript<=offset; subscript++ )
    //         swap( &(a[*xmlItemListTotal+subscript]), &(a[subscript]) );
    // }
    // for (i=1;i<=7;i++) printf(" %d",a[i]);
    // printf("\n");

    return 1;
}

int getItemContent( struct itemnode_t * itemList, int * itemListTotal, int * flag, int offset, char * outStr ){
    char longStr[100000];
    int subscript;

    strcpy( longStr, "" );
    strcpy( outStr, "" );
    if ( offset <= 0 ) return 0;
    if ( flag[offset] != XML_FETCH_ITEM_START ) return 0;
    if ( itemList[offset].type == IT_TXT || itemList[offset].type == IT_NOG )
        strcpy( longStr, itemList[offset].title );
    for (subscript=offset+1; subscript<=*itemListTotal; subscript++){
        if ( flag[subscript] != XML_FETCH_ITEM_CHILD ) break;
        if ( itemList[subscript].type == IT_TXT || itemList[subscript].type == IT_NOG )
            strcat( longStr, itemList[subscript].title );
    }
    strcpy( outStr, longStr );
    return 1;
}

int getItemRichContent( struct itemnode_t * itemList, int * itemListTotal, int * flag, int offset, char * outStr ){
    char longStr[100000];
    int subscript;

    int type;
    int i;
    struct itemnode_t * target;

    int passed=0;

    strcpy( longStr, "" );
    strcpy( outStr, "" );
    if ( offset <= 0 ) return 0;
    if ( flag[offset] != XML_FETCH_ITEM_START ) return 0;
    for (subscript=offset; subscript<=*itemListTotal; subscript++){
        if ( flag[subscript] != XML_FETCH_ITEM_CHILD && flag[subscript] != XML_FETCH_ITEM_START ) break;
        if ( flag[subscript] == XML_FETCH_ITEM_START ) passed++;
        if ( passed > 1 ) break;
        target = &itemList[subscript];
        type = target->type;

        if (type == IT_ITM || type == IT_TAL){
            strcat( longStr, "<" );
            if (type == IT_TAL) strcat( longStr, "/" );
            strcat( longStr, target->title);
            for (i=0; i<target->attrTotal ; i++){
                strcat( longStr , " " );
                strcat( longStr , target->attrTitle[i] );
                if (target->attrValue[i] != NULL){
                    strcat( longStr, "=" );
                    strcat( longStr, target->attrValue[i] );
                }
            }
            strcat( longStr, ">" );
        }else if (type == IT_TXT || type == IT_NOG){
            strcat( longStr, target->title );
        }

    }
    strcpy( outStr, longStr );
    return 1;
}

int xmlItemListSearch( struct xmlItem_t * xmlItemList, int * xmlItemListTotal, char * guid ){
    int subscript;
    for (subscript=1; subscript<=*xmlItemListTotal; subscript++)
        if ( xmlItemList[subscript].text[FRSS_ITEM_GUID] != NULL &&
             strcmp( xmlItemList[subscript].text[FRSS_ITEM_GUID], guid ) == 0 )
            return subscript;
    return 0;
}

int printXmlItemListNode( struct xmlItem_t * xmlItemList, int * xmlItemListTotal, int targetSubscript ){
    if ( targetSubscript <= 0 || targetSubscript > *xmlItemListTotal ) return 0;

    printf( "xmlItemList subscript: %d\n", targetSubscript );
    if ( xmlItemList[targetSubscript].text[FRSS_ITEM_TITLE] != NULL )
        printf( "\ttitle:%s \n", xmlItemList[targetSubscript].text[FRSS_ITEM_TITLE] );
    if ( xmlItemList[targetSubscript].text[FRSS_ITEM_TIME] != NULL )
        printf( "\ttime:%s \n", xmlItemList[targetSubscript].text[FRSS_ITEM_TIME] );
    if ( xmlItemList[targetSubscript].text[FRSS_ITEM_CONTENT] != NULL )
        printf( "\tcontent:%s \n", xmlItemList[targetSubscript].text[FRSS_ITEM_CONTENT] );
    if ( xmlItemList[targetSubscript].richText[FRSS_ITEM_CONTENT] != NULL )
        printf( "\tRich content:%s \n", xmlItemList[targetSubscript].richText[FRSS_ITEM_CONTENT] );
    if ( xmlItemList[targetSubscript].text[FRSS_ITEM_GUID] != NULL )
        printf( "\tguid:%s \n", xmlItemList[targetSubscript].text[FRSS_ITEM_GUID] );

    return 1;
}

int swapXmlItemListNode( struct xmlItem_t * xmlItemList, int * xmlItemListTotal, int aSub, int bSub ){
    char * strPtr;
    if ( aSub <= 0 || aSub > *xmlItemListTotal || bSub <= 0 || bSub > *xmlItemListTotal ) return 0;
    strPtr = xmlItemList[ aSub ].text[FRSS_ITEM_TITLE];
    xmlItemList[aSub].text[FRSS_ITEM_TITLE] = xmlItemList[bSub].text[FRSS_ITEM_TITLE];
    xmlItemList[bSub].text[FRSS_ITEM_TITLE] = strPtr;

    strPtr = xmlItemList[ aSub ].text[FRSS_ITEM_TIME];
    xmlItemList[aSub].text[FRSS_ITEM_TIME] = xmlItemList[bSub].text[FRSS_ITEM_TIME];
    xmlItemList[bSub].text[FRSS_ITEM_TIME] = strPtr;

    strPtr = xmlItemList[ aSub ].text[FRSS_ITEM_CONTENT];
    xmlItemList[aSub].text[FRSS_ITEM_CONTENT] = xmlItemList[bSub].text[FRSS_ITEM_CONTENT];
    xmlItemList[bSub].text[FRSS_ITEM_CONTENT] = strPtr;

    strPtr = xmlItemList[ aSub ].richText[FRSS_ITEM_CONTENT];
    xmlItemList[aSub].richText[FRSS_ITEM_CONTENT] = xmlItemList[bSub].richText[FRSS_ITEM_CONTENT];
    xmlItemList[bSub].richText[FRSS_ITEM_CONTENT] = strPtr;

    strPtr = xmlItemList[ aSub ].text[FRSS_ITEM_GUID];
    xmlItemList[aSub].text[FRSS_ITEM_GUID] = xmlItemList[bSub].text[FRSS_ITEM_GUID];
    xmlItemList[bSub].text[FRSS_ITEM_GUID] = strPtr;

    return 1;
}

int importXmlItemFile( const char * filename, struct xmlItem_t * xmlItemList, int * xmlItemListTotal ){
    int subscript;
    int subscriptCount;
    int attrSubscript;
    xmlChar * longStr;
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

    *xmlItemListTotal = 0;

    xmlKeepBlanksDefault(0);

    // create the doc and root node
    xmlDocPtr doc = xmlReadFile( filename, "UTF-8", XML_PARSE_RECOVER );

    if ( doc == NULL ){
        printf("Error: xml Data file \"%s\" import failed.\n", filename);
        exportXmlItemFile( filename, xmlItemList, xmlItemListTotal );
        return 0;
    }

    xmlNodePtr root_node = xmlDocGetRootElement( doc );

    if ( root_node == NULL ){
        printf("Error: failed to get the root of xml doc.\n");
        return 0;
    }

    if ( xmlStrcmp( root_node->name, BAD_CAST "xmlItemList") != 0 ){
        printf("Error: name of root node of xml doc not matched.\n");
        return 0;
    }

    if ( xmlHasProp( root_node, BAD_CAST "xmlItemListTotal") ){
        longStr = xmlGetProp( root_node, BAD_CAST "xmlItemListTotal" );
        *xmlItemListTotal = xatoi( (char *) longStr );
        if ( *xmlItemListTotal > MAX_ITEMLIST_SAVE ) *xmlItemListTotal = MAX_ITEMLIST_SAVE;
        // printf("itemListTotal: %d\n", *itemListTotal );
    }else{
        printf("Error: failed to get the itemListTotal.\n");
        return 0;
    }

    node = root_node->xmlChildrenNode;

    for (subscriptCount=1; subscriptCount <= *xmlItemListTotal; subscriptCount++){
        // printf("subscriptCount: %d\n", subscriptCount);
        if ( node == NULL ){
            printf("Error: not enough item node.\n");
            *xmlItemListTotal = 0;
            return 0;
        }

        subscript = subscriptCount;
        // if ( xmlHasProp( node, BAD_CAST "subscript") ){
        //     longStr = xmlGetProp( node, BAD_CAST "subscript" );
        //     subscript = xatoi( (char *) longStr );
        // }else{
        //     printf("Error: failed to get the subscript of item.\n");
        //     return 0;
        // }
        // printf("subscript: %d\n", subscript);
        xmlItemList[subscript].text[FRSS_ITEM_TITLE] = NULL;
        xmlItemList[subscript].text[FRSS_ITEM_TIME] = NULL;
        xmlItemList[subscript].text[FRSS_ITEM_CONTENT] = NULL;
        xmlItemList[subscript].richText[FRSS_ITEM_CONTENT] = NULL;
        xmlItemList[subscript].text[FRSS_ITEM_GUID] = NULL;

        sonNode = node->xmlChildrenNode;
        while( sonNode != NULL ){
            //     longStr = xmlNodeGetContent( sonNode );
            // printf("name: %s\ncontent: %s\n", (char *) sonNode->name, (char *) longStr);

            if ( xmlStrcmp( sonNode->name, BAD_CAST "title" ) == 0){
                longStr = xmlNodeGetContent( sonNode );
                xmlItemList[subscript].text[FRSS_ITEM_TITLE] = malloc( strlen(longStr) +1 );
                strcpy( xmlItemList[subscript].text[FRSS_ITEM_TITLE], (char *) longStr );
            }else if ( xmlStrcmp( sonNode->name, BAD_CAST "time" ) == 0){
                longStr = xmlNodeGetContent( sonNode );
                xmlItemList[subscript].text[FRSS_ITEM_TIME] = malloc( strlen(longStr) +1 );
                strcpy( xmlItemList[subscript].text[FRSS_ITEM_TIME], (char *) longStr );
            }else if ( xmlStrcmp( sonNode->name, BAD_CAST "content" ) == 0){
                longStr = xmlNodeGetContent( sonNode );
                xmlItemList[subscript].text[FRSS_ITEM_CONTENT] = malloc( strlen(longStr) +1 );
                strcpy( xmlItemList[subscript].text[FRSS_ITEM_CONTENT], (char *) longStr );
            }else if ( xmlStrcmp( sonNode->name, BAD_CAST "content:encoded" ) == 0){
                longStr = xmlNodeGetContent( sonNode );
                xmlItemList[subscript].richText[FRSS_ITEM_CONTENT] = malloc( strlen(longStr) +1 );
                strcpy( xmlItemList[subscript].richText[FRSS_ITEM_CONTENT], (char *) longStr );
            }else if ( xmlStrcmp( sonNode->name, BAD_CAST "guid" ) == 0){
                longStr = xmlNodeGetContent( sonNode );
                xmlItemList[subscript].text[FRSS_ITEM_GUID] = malloc( strlen(longStr) +1 );
                strcpy( xmlItemList[subscript].text[FRSS_ITEM_GUID], (char *) longStr );
            }else{
                printf("Error: unknown sonNode of xml doc item, %s", (char *) sonNode->name );
                *xmlItemListTotal = 0;
                return 0;
            }

            sonNode = sonNode->next;
        }

        node = node->next;
    }
    printf("import success.\n");
    printf("xmlItemListTotal: %d\n", *xmlItemListTotal);
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return 1;
}

int exportXmlItemFile( const char * filename, struct xmlItem_t * xmlItemList, int * xmlItemListTotal ){
    int subscript;
    int attrSubscript;
    char longStr[100000];
    xmlNodePtr node;
    xmlNodePtr content;
    xmlNodePtr sonNode;
    xmlNodePtr grandsonNode;

    // create the doc and root node
    xmlDocPtr doc = xmlNewDoc( BAD_CAST "1.0" );
    xmlNodePtr root_node = xmlNewNode( NULL, BAD_CAST "xmlItemList" );

    xitoa( longStr, *xmlItemListTotal );
    xmlNewProp( root_node, BAD_CAST "xmlItemListTotal", BAD_CAST longStr );

    // set the root node
    xmlDocSetRootElement( doc, root_node );

    if ( *xmlItemListTotal > MAX_ITEMLIST_SAVE ) *xmlItemListTotal = MAX_ITEMLIST_SAVE;
    

    for (subscript=1; subscript <= *xmlItemListTotal; subscript++){
        node = xmlNewNode( NULL, BAD_CAST "item" );
        xmlAddChild( root_node, node );

        // xitoa( longStr, subscript );
        // xmlNewProp( node, BAD_CAST "subscript", BAD_CAST longStr );

        if ( xmlItemList[subscript].text[FRSS_ITEM_TITLE] != NULL ){
            sonNode = xmlNewNode( NULL, BAD_CAST "title" );
            content = xmlNewCDataBlock( NULL, BAD_CAST xmlItemList[subscript].text[FRSS_ITEM_TITLE], strlen(xmlItemList[subscript].text[FRSS_ITEM_TITLE]) );
            xmlAddChild( sonNode, content );
            xmlAddChild( node, sonNode );
        }
        if ( xmlItemList[subscript].text[FRSS_ITEM_TIME] != NULL ){
            sonNode = xmlNewNode( NULL, BAD_CAST "time" );
            content = xmlNewCDataBlock( NULL, BAD_CAST xmlItemList[subscript].text[FRSS_ITEM_TIME], strlen(xmlItemList[subscript].text[FRSS_ITEM_TIME]) );
            xmlAddChild( sonNode, content );
            xmlAddChild( node, sonNode );
        }
        if ( xmlItemList[subscript].text[FRSS_ITEM_CONTENT] != NULL ){
            sonNode = xmlNewNode( NULL, BAD_CAST "content" );
            content = xmlNewCDataBlock( NULL, BAD_CAST xmlItemList[subscript].text[FRSS_ITEM_CONTENT], strlen(xmlItemList[subscript].text[FRSS_ITEM_CONTENT]) );
            xmlAddChild( sonNode, content );
            xmlAddChild( node, sonNode );
        }
        if ( xmlItemList[subscript].richText[FRSS_ITEM_CONTENT] != NULL ){
            sonNode = xmlNewNode( NULL, BAD_CAST "content:encoded" );
            content = xmlNewCDataBlock( NULL, BAD_CAST xmlItemList[subscript].richText[FRSS_ITEM_CONTENT], strlen(xmlItemList[subscript].richText[FRSS_ITEM_CONTENT]) );
            xmlAddChild( sonNode, content );
            xmlAddChild( node, sonNode );
        }
        if ( xmlItemList[subscript].text[FRSS_ITEM_GUID] != NULL ){
            sonNode = xmlNewNode( NULL, BAD_CAST "guid" );
            content = xmlNewCDataBlock( NULL, BAD_CAST xmlItemList[subscript].text[FRSS_ITEM_GUID], strlen(xmlItemList[subscript].text[FRSS_ITEM_GUID]) );
            xmlAddChild( sonNode, content );
            xmlAddChild( node, sonNode );
        }
    }

    int nRel = xmlSaveFormatFileEnc( filename, doc, "UTF-8", 1);
    /*if (nRel != -1)
        printf(" created, %d bytes.\n", nRel);*/

    xmlFreeDoc(doc);

    if (nRel != -1) return 1;
    else return 0;
}

int exportXmlItemFileForAlarm( const char * filename, struct xmlItem_t * xmlItemList, int * xmlItemListTotal, int * xmlItemListAlarmTotal ){
    int subscript;
    char longStr[100000];
    char message[500];
    xmlNodePtr node;
    xmlNodePtr content;
    xmlNodePtr sonNode;
    xmlNodePtr grandsonNode;

    // create the doc and root node
    xmlDocPtr doc = xmlNewDoc( BAD_CAST "1.0" );
    xmlNodePtr root_node = xmlNewNode( NULL, BAD_CAST "xmlItemListAlarm" );

    xitoa( longStr, *xmlItemListAlarmTotal);
    xmlNewProp( root_node, BAD_CAST "xmlItemListAlarmTotal", BAD_CAST longStr );

    // set the root node
    xmlDocSetRootElement( doc, root_node );

    if ( *xmlItemListAlarmTotal > *xmlItemListTotal ) *xmlItemListAlarmTotal = *xmlItemListTotal;
    

    for (subscript=1; subscript <= *xmlItemListAlarmTotal; subscript++){
        node = xmlNewNode( NULL, BAD_CAST "item" );
        xmlAddChild( root_node, node );

        // xitoa( longStr, subscript );
        // xmlNewProp( node, BAD_CAST "subscript", BAD_CAST longStr );

        strcpy( longStr, "RSSR Alarm. \n" );
        if ( xmlItemList[subscript].text[FRSS_ITEM_TITLE] != NULL ){
            // sonNode = xmlNewNode( NULL, BAD_CAST "title" );
            // content = xmlNewCDataBlock( NULL, BAD_CAST xmlItemList[subscript].text[FRSS_ITEM_TITLE], strlen(xmlItemList[subscript].text[FRSS_ITEM_TITLE]) );
            // xmlAddChild( sonNode, content );
            // xmlAddChild( node, sonNode );
            strcat( longStr, " Title: " );
            strcat( longStr, xmlItemList[subscript].text[FRSS_ITEM_TITLE] );
            strcat( longStr, " \n" );
        }
        if ( xmlItemList[subscript].text[FRSS_ITEM_TIME] != NULL ){
            strcat( longStr, " Time: " );
            strcat( longStr, xmlItemList[subscript].text[FRSS_ITEM_TIME] );
            strcat( longStr, " \n" );
        }
        if ( xmlItemList[subscript].text[FRSS_ITEM_CONTENT] != NULL ){
            strcat( longStr, " Content: " );
            // strncpy( message, xmlItemList[subscript].text[FRSS_ITEM_CONTENT], 300 );
            strcat( longStr, xmlItemList[subscript].text[FRSS_ITEM_CONTENT] );
            // strcat( longStr, message );
            strcat( longStr, " \n" );

            // sonNode = xmlNewNode( NULL, BAD_CAST "title" );
            // content = xmlNewCDataBlock( NULL, BAD_CAST xmlItemList[subscript].text[FRSS_ITEM_CONTENT], strlen(xmlItemList[subscript].text[FRSS_ITEM_CONTENT]) );
            // printf( "%s\n", xmlItemList[subscript].text[FRSS_ITEM_CONTENT]);
            // xmlAddChild( sonNode, content );
            // xmlAddChild( node, sonNode );
        }
        sonNode = xmlNewNode( NULL, BAD_CAST "message" );
        content = xmlNewCDataBlock( NULL, BAD_CAST longStr, strlen(longStr) );
        // printf( "%s",longStr );
        // content = xmlNewCDataBlock( NULL, BAD_CAST xmlItemList[subscript].text[FRSS_ITEM_CONTENT], strlen(xmlItemList[subscript].text[FRSS_ITEM_CONTENT]) );
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
