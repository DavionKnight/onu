
/******************************************************
*
* Example:
*
*
*
*    typedef struct{
*        gw_node node;
*        int a;
*        int b;
*        int c[100];
*    }structA;
*
*   gw_list myList;
*
*   lstInit(&myList);
*   for(i = 0 ; i < 50 ; i++)
*   {
*        structA *p;
*        p = (structA *)iros_malloc(pool, sizeof(structA));
*        p->a = i;
*        p->b = i;
*        lstAdd(&myList , (gw_node *)p);
*   }
*
*   for(p = (structA *)lstFirst(&myList) ; p ; p = (structA *)lstNext((gw_node *)p))
*   {
*        <scan the list>
*   }

Description:
    init list with the compare function, the compare function is necessary for
    remove the specific node and find the specific node
    compare function has two arguments,Eg.
    compare(pNodeA , pNodeB)
    if(pNodeA < pNodeB) return -1;
    if(pNodeA = pNodeB) return 0;
    if(pNodeA > pNodeB) return 1;
    typedef struct {
        gw_node node;
        int key1;
        int key2;
        int key3;
        char data[1000];
    }tStruct;

    yourself_compre((tStruct *)TA , (tStruct *)TB)
    {
        if(TA->key1 > TB->key1) return 1;
        else if(TA->key1 < TB->key1) return -1;
        else ;

        if(TA->key2 > TB->key2) return 1;
        else if(TA->key2 < TB->key2) return -1;
        else ;

        if(TA->key3 > TB->key3) return 1;
        else if(TA->key3 < TB->key3) return -1;
        else return 0;
    }
**********************************************************/

#include "../include/gw_types.h"
#include "../include/gw_os_common.h"
#include "../include/util_list.h"


void gw_lst_init(gw_list * pLst , gw_int32 (*compare)(void * , gw_uint32))
{
    pLst->gw_head = NULL;
    pLst->gw_tail = NULL;
    pLst->count = 0;
    pLst->compare = compare;
}

/**********************************************************
Description:
   insert a new node 'pNode' after  the specific node  'pPrevious'
**********************************************************/
void gw_lst_insert(gw_list * pLst, gw_node * pPrevious, gw_node * pNode)
{
    gw_node * pNext = NULL;

    if (NULL == pPrevious) {
        pNext = pLst->gw_head;
        pLst->gw_head = pNode;
    } else {
        pNext = pPrevious->next;
        pPrevious->next = pNode;
    }

    if (NULL == pNext) {
        pLst->gw_tail = pNode;
    } else {
        pNext->previous = pNode;
    }

    pNode->previous = pPrevious;
    pNode->next = pNext;

    pLst->count++;
}

/**********************************************************
Description:
   add a new node to the tail of 'pLst'
**********************************************************/
void gw_lst_add(gw_list * pLst, gw_node * pNode)
{
    gw_lst_insert(pLst, pLst->gw_tail, pNode);
}

/**********************************************************
Description:
   Find the actual node according to the 'key' and remove it from 'pLst',
   The operatoin doesn't mean to free the node , it is only cut off from 'pLst'
**********************************************************/
gw_node *gw_lst_remove(gw_list * pLst , gw_uint32 key)
{
    gw_node *remove_node = (gw_node *)NULL;

    if (pLst->compare == NULL) {
        return (gw_node *)remove_node;
    }

    gw_lst_scan(pLst , remove_node , gw_node *) {
        if (pLst->compare((void *)remove_node , key) == 0) {
            gw_lst_delete(pLst , remove_node);
            return (gw_node *)remove_node;
        }
    }

    return (gw_node *)NULL;
}

/**********************************************************
Description:
   The operatoin doesn't mean to free the node , it is only cut off from 'pLst'
**********************************************************/
void gw_lst_delete(gw_list * pLst, gw_node * pNode)
{
    if (pNode->previous == NULL) {
        pLst->gw_head = pNode->next;
    } else {
        pNode->previous->next = pNode->next;
    }

    if (pNode->next == NULL) {
        pLst->gw_tail = pNode->previous;
    } else {
        pNode->next->previous = pNode->previous;
    }

    pLst->count--;
}

gw_node * gw_lst_first(gw_list * pLst)
{
    return pLst->gw_head;
}

gw_node * gw_lst_last(gw_list * pLst)
{
    return pLst->gw_tail;
}

/**********************************************************
Description:
   get the pointer of the first node in the list , and the node will be cutted off
   from the header of the list
**********************************************************/
gw_node * gw_lst_get(gw_list * pLst)
{
    gw_node * pNode = NULL;

    pNode = pLst->gw_head;

    if (pNode != NULL) {
        pLst->gw_head = pNode->next;

        if (pNode->next == NULL) {
            pLst->gw_tail = NULL;
        } else {
            pNode->next->previous = NULL;
        }

        pLst->count--;
    }

    return pNode;
}


gw_node * gw_lst_nth(gw_list * pLst, int nodeNum)
{
    gw_node * pNode = NULL;

    if (nodeNum < 1 || nodeNum > pLst->count) {
        return NULL;
    }

    if (nodeNum < ((pLst->count + 1) >> 1)) {
        pNode = pLst->gw_head;

        while (--nodeNum > 0) {
            pNode = pNode->next;
        }
    } else {
        nodeNum = pLst->count - nodeNum;
        pNode = pLst->gw_tail;

        while (++nodeNum > 0) {
            pNode = pNode->previous;
        }
    }

    return pNode;
}

/**********************************************************
Description:
   Get the pointer of the actual node according to the 'key'
**********************************************************/
gw_node *gw_lst_find(gw_list *pLst , gw_uint32 key)
{
    gw_node *find_node = NULL;

    if (pLst->compare == NULL) {
        return (gw_node *)find_node;
    }

    gw_lst_scan(pLst , find_node , gw_node *) {
        if (pLst->compare((void *)find_node , key) == 0)
            return (gw_node *)find_node;
    }

    return (gw_node *)NULL;
}

gw_node * gw_lst_prev(gw_node * pNode)
{
    return pNode->previous;
}

gw_node * gw_lst_next(gw_node * pNode)
{
    return pNode->next;
}

gw_uint32 gw_lst_count(gw_list * pLst)
{
    return pLst->count;
}

gw_list *gw_lst_concat(gw_list *pDst , gw_list *pSrc)
{
    if(pDst == NULL || pSrc == NULL){
        return NULL;
    }

    if(pDst->compare != pSrc->compare) {
        return NULL;
    }

    if(pDst->count == 0) {
        memcpy(pDst , pSrc , sizeof(gw_list));
        return (gw_list *)pDst;
    }

    if(pSrc->count == 0) {
        return (gw_list *)pDst;
    }

    pDst->gw_tail->next = pSrc->gw_head;
    pSrc->gw_head->previous = pDst->gw_tail;
    pDst->gw_tail = pSrc->gw_tail;

    pDst->count += pSrc->count;

    gw_lst_init(pSrc , pSrc->compare);

    return (gw_list *)pDst;
}


