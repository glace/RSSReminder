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

#ifndef XMLITEM_H
#define XMLITEM_H

#include <stdio.h>
#include <stdlib.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string.h>

#include "global.h"
#include "md5.h"

enum{
    XML_DESC_FORMAT_STANDARD,
    XML_DESC_FORMAT_ID
};

enum{
    XML_FETCH_ITEM_NOG,
    XML_FETCH_ITEM_START,
    XML_FETCH_ITEM_CHILD
};

enum{
    FRSS_ITEM_TITLE,
    FRSS_ITEM_TIME,
    FRSS_ITEM_CONTENT,
    FRSS_ITEM_GUID
};

struct xmlDesc_t{
    int type;  // Text  ||  Item
    char * title;
    char * attrTitle[100];
    char * attrValue[100];
    int attrTotal;
};

struct xmlItem_t{
    char * text[10];
    char * richText[10];
};

struct xmlDescID_t{
    int data[10][100];
};

struct xmlItem_t xmlItemList[MAX_ITEMLIST_LENGTH];  // used start with subscript 1
int xmlItemListTotal;
int xmlItemListAlarmTotal;

int importRuleFile( const char * filename, struct itemnode_t * itemList, int * itemListTotal, struct xmlDesc_t * xmlDesc );

int exportRuleFile( const char * filename, struct xmlDesc_t * xmlDesc );

int fetchRule( struct itemnode_t * itemList, int * itemListTotal, struct xmlDesc_t xmlDesc, int * flag );

int fetchRuleSingle( struct itemnode_t item, struct xmlDesc_t xmlDesc );

int generateXmlItem( struct itemnode_t * itemList, int * itemListTotal, struct xmlDesc_t * xmlDesc , struct xmlItem_t * xmlItemList, int * xmlItemListTotal, int * xmlItemListAlarmTotal );

int xmlDescPrint( struct xmlDescID_t * xmlDesc);

int printDescNode(struct xmlDesc_t * xmlDesc, int nodeSubscript);

int getItemContent( struct itemnode_t * itemList, int * itemListTotal, int * flag, int offset, char * outStr );

int getItemRichContent( struct itemnode_t * itemList, int * itemListTotal, int * flag, int offset, char * outStr );

int xmlItemListSearch( struct xmlItem_t * xmlItemList, int * xmlItemListTotal, char * guid );

int printXmlItemListNode( struct xmlItem_t * xmlItemList, int * xmlItemListTotal, int targetSubscript );

int importXmlItemFile( const char * filename, struct xmlItem_t * xmlItemList, int * xmlItemListTotal );

int exportXmlItemFile( const char * filename, struct xmlItem_t * xmlItemList, int * xmlItemListTotal );

int exportXmlItemFileForAlarm( const char * filename, struct xmlItem_t * xmlItemList, int * xmlItemListTotal, int * xmlItemListAlarmTotal );

#endif 
