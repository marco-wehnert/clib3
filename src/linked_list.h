#ifndef __linked_list_h__
#define __linked_list_h__

//  implemented similar to the std::list from C++

typedef struct list_element_s
{
    void* object;
    struct list_element_s* prev;
    struct list_element_s* next;
} list_element_t;

typedef struct linked_list_s
{
    list_element_t* front;
    list_element_t* back;
    int count;

} linked_list_t;

void  ll_init(linked_list_t* list);
void  ll_push_front(linked_list_t* list, void* object);
void  ll_push_back(linked_list_t* list, void* object);
void* ll_pop_front(linked_list_t* list);
void* ll_pop_back(linked_list_t* list);
void  ll_clear(linked_list_t* list);
void  ll_remove(linked_list_t* list, void* object);

#endif
