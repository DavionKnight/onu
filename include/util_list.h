/***********************************************************************/
/* This file contains unpublished documentation and software                                            */
/* proprietary to Cortina Systems Incorporated. Any use or disclosure,                              */
/* in whole or in part, of the information in this file without a                                              */
/* written consent of an officer of Cortina Systems Incorporated is                                     */
/* strictly prohibited.                                                                                                             */
/* Copyright (c) 2002-2010 by Cortina Systems Incorporated.                                            */
/***********************************************************************/

#ifndef _UTIL_LIST_H_
#define _UTIL_LIST_H_

#include "gw_types.h"

typedef struct node_t
{
    struct node_t * previous;
    struct node_t * next;
}gw_node;

typedef struct list
{
    gw_node node;
    gw_uint32 count;
    gw_int32 (*compare)(void * , gw_uint32);
}gw_list;

#define gw_tail node.previous
#define gw_head node.next

extern void gw_lst_init(gw_list * pLst , gw_int32 (*compare)(void * , gw_uint32));
extern void gw_lst_insert(gw_list * pLst, gw_node * pPrevious, gw_node * pNode);
extern void gw_lst_add(gw_list * pLst, gw_node * pNode);
gw_node *gw_lst_remove(gw_list * pLst , gw_uint32 key);
extern void gw_lst_delete(gw_list * pLst, gw_node * pNode);
extern gw_node * gw_lst_first(gw_list * pLst);
extern gw_node * gw_lst_last(gw_list * pLst);
extern gw_node * gw_lst_get(gw_list * pLst);
extern gw_node * gw_lst_nth(gw_list * pLst, int nodeNum);
gw_node *gw_lst_find(gw_list *pLst , gw_uint32 key);
extern gw_node * gw_lst_prev(gw_node * pNode);
extern gw_node * gw_lst_next(gw_node * pNode);
extern gw_uint32 gw_lst_count(gw_list * pLst);
extern gw_list *gw_lst_concat(gw_list *pDst , gw_list *pSrc);

#define gw_lst_scan(pList , pNode , type) for(pNode=(type)gw_lst_first(pList);pNode;pNode=(type)gw_lst_next((gw_node *)pNode))


#endif

