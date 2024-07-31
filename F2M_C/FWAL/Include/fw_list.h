#ifndef _FW_LIST_H_
#define _FW_LIST_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_type.h"


/*
 * @双向链表
 */
typedef struct FW_List_Node
{
    struct FW_List_Node *Next;
    struct FW_List_Node *Prev;
}FW_List_Type;


__INLINE_STATIC_ void FW_List_Init(FW_List_Type *list)
{
    list->Next = list;
    list->Prev = list;
}

__INLINE_STATIC_ void FW_List_InsertAfter(FW_List_Type *list, FW_List_Type *node)
{
    list->Next->Prev = node;
    node->Next = list->Next;
    
    list->Next = node;
    node->Prev = list;
}

__INLINE_STATIC_ void FW_List_InsertBefore(FW_List_Type *list, FW_List_Type *node)
{
    list->Prev->Next = node;
    node->Prev = list->Prev;
    
    list->Prev = node;
    node->Next = list;
}

__INLINE_STATIC_ void FW_List_Remove(FW_List_Type *node)
{
    node->Next->Prev = node->Prev;
    node->Prev->Next = node->Next;
    
    node->Next = node;
    node->Prev = node;
}

__INLINE_STATIC_ Bool FW_List_IsEmpty(const FW_List_Type *list)
{
    return (Bool)(list->Next == list);
}

__INLINE_STATIC_ u32  FW_List_GetLength(const FW_List_Type *list)
{
    u32 length = 0;
    const FW_List_Type *p = list;
    
    while(p->Next != list)
    {
        p = p->Next;
        length++;
    }
    
    return length;
}

//__INLINE_STATIC_ FW_List_Type *FW_List_Foreach(FW_List_Type *node, const FW_List_Type *head)
//{
//    
//}

//#define FW_LIST_FOREACH(node, head)\
//for(node = (head)->Next; node != (head); node = node->Next)

//#define FW_List_Foreach_Safe(node, tmp, head)\
//for(node = (head)->Next, tmp = node->Next;\
//    node != (head);\
//    node = tmp, tmp = node->Next)

#define FW_LIST_FOREACH(node, tmp, head)\
for(node = (head)->Next, tmp = node->Next;\
    node != (head);\
    node = tmp, tmp = node->Next)


__INLINE_STATIC_ FW_List_Type *FW_List_GetPrev(const FW_List_Type *list)
{
    return list->Prev;
}

__INLINE_STATIC_ FW_List_Type *FW_List_GetNext(const FW_List_Type *list)
{
    return list->Next;
}

//__INLINE_STATIC_ FW_List_Type *FW_List_GetHead(const FW_List_Type *list)
//{
//    FW_List_Type *tmp = (FW_List_Type *)list;
//    while(tmp->Prev != tmp)
//    {
//        tmp = tmp->Prev;
//    }
//    return tmp;
//}

//__INLINE_STATIC_ FW_List_Type *FW_List_GetTail(const FW_List_Type *list)
//{
//    FW_List_Type *tmp = (FW_List_Type *)list;
//    while(tmp->Next != tmp)
//    {
//        tmp = tmp->Next;
//    }
//    return tmp;
//}

//__INLINE_STATIC_ Bool FW_List_IsHead(const FW_List_Type *list)
//{
//    return (Bool)(list->Prev == list);
//}

//__INLINE_STATIC_ Bool FW_List_IsTail(const FW_List_Type *list)
//{
//    return (Bool)(list->Next == list);
//}


/*
 * @单向链表
 */
typedef struct FW_SList_Node
{
    struct FW_SList_Node *Next;
}FW_SList_Type;


__INLINE_STATIC_ void FW_SList_Init(FW_SList_Type *list)
{
    list->Next = NULL;
}

__INLINE_STATIC_ void FW_SList_Append(FW_SList_Type *list, FW_SList_Type *node)
{
    FW_SList_Type *p = list;
    
    while(p->Next)  p = p->Next;
    
    p->Next = node;
    node->Next = NULL;
}

__INLINE_STATIC_ void FW_SList_Insert(FW_SList_Type *list, FW_SList_Type *node)
{
    node->Next = list->Next;
    list->Next = node;
}

__INLINE_STATIC_ u32 FW_SList_GetLength(FW_SList_Type *list)
{
    u32 length = 0;
    const FW_SList_Type *p = list->Next;
    
    while(p)
    {
        p = p->Next;
        length++;
    }
    
    return length;
}

__INLINE_STATIC_ FW_SList_Type *FW_SList_Remove(FW_SList_Type *list, FW_SList_Type *node)
{
    FW_SList_Type *p = list;
    
    while(p->Next && p->Next != node)  p = p->Next;
    
    if(p->Next != NULL)  node->Next = node->Next->Next;
    
    return list;
}

__INLINE_STATIC_ FW_SList_Type *FW_SList_GetNext(const FW_SList_Type *list)
{
    return list->Next;
}

__INLINE_STATIC_ FW_SList_Type *FW_SList_GetHead(const FW_SList_Type *list)
{
    return list->Next;
}

__INLINE_STATIC_ FW_SList_Type *FW_SList_GetTail(FW_SList_Type *list)
{
    while(list->Next)  list = list->Next;
    return list;
}

__INLINE_STATIC_ Bool FW_SList_IsEmpty(const FW_SList_Type *list)
{
    return (Bool)(list->Next == NULL);
}


#define FW_SList_GetEntry(node, type, member)\
    Container_Of(node, type, member)


#ifdef __cplusplus
}
#endif

#endif
