#include "linked_list.h"
#include <stdlib.h>
#include <string.h> // memset
//#include <stdio.h> // printf

void ll_init(linked_list_t* list)
{
    memset(list, 0, sizeof(linked_list_t));
}

void ll_clear(linked_list_t* list)
{
    list_element_t* element;
    list_element_t* next;
    element = list->front;
    while (element != NULL)
    {
        next = element->next;
        free(element);
        element = next;
    }
}

void ll_push_front(linked_list_t* list, void* object)
{
    list_element_t* new_list_element;

    new_list_element = calloc(1, sizeof(list_element_t));
    new_list_element->object = object;
    if (list->count == 0)
    {
        list->front = new_list_element;
        list->back  = new_list_element;
        list->count = 1;
    }
    else
    {
        list_element_t* old_front = list->front;
        list->front = new_list_element;
        new_list_element->next = old_front;
        old_front->prev = new_list_element;
        list->count++;
    }
}

void ll_push_back(linked_list_t* list, void* object)
{
    list_element_t* new_list_element;

    new_list_element = calloc(1, sizeof(list_element_t));
    new_list_element->object = object;
    if (list->count == 0)
    {
        list->front = new_list_element;
        list->back  = new_list_element;
        list->count = 1;
    }
    else
    {
        list_element_t* old_back = list->back;
        list->back = new_list_element;
        new_list_element->prev = old_back;
        old_back->next = new_list_element;
        list->count++;
    }
}

void* ll_pop_back(linked_list_t* list)
{
    void* object;

    if (list->count == 0)
    {
        return NULL;
    }

    object = list->back->object;
    if (list->count == 1)
    {
        free(list->back);
        list->back = NULL;
        list->front = NULL;
        list->count = 0;
    }
    else
    {
        list_element_t* old_back = list->back;
        list->back = old_back->prev;
        old_back->prev->next = NULL;
        free(old_back);
        list->count--;
    }

    return object;
}

void* ll_pop_front(linked_list_t* list)
{
    void* object;

    if (list->count == 0)
    {
        return NULL;
    }

    object = list->front->object;
    if (list->count == 1)
    {
        free(list->front);
        list->front = NULL;
        list->back = NULL;
        list->count = 0;
    }
    else
    {
        list_element_t* old_front = list->front;
        list->front = old_front->next;
        old_front->next->prev = NULL;
        free(old_front);
        list->count--;
    }

    return object;
}

void ll_remove(linked_list_t* list, void* object)
{
    list_element_t* element;

    if (list->count == 0)
    {
        return;
    }

    if (list->count == 1 && list->front->object == object)
    {
        free(list->front);
        list->front = NULL;
        list->back = NULL;
        list->count = 0;
        return;
    }
    else // count > 1
    {
        if (list->front->object == object)
        {
            // Remove first element
            element = list->front;
            list->front = element->next;
            element->next->prev = NULL;
            free(element);
            list->count--;
        }
        else if (list->back->object == object)
        {
            element = list->back;
            list->back = element->prev;
            element->prev->next = NULL;
            free(element);
            list->count--;
        }
        else
        {
            // Element might be somewhere in the middle
            element = list->front->next;
            while (element != NULL)
            {
                if (element->object == object)
                {
                    element->prev->next = element->next;
                    element->next->prev = element->prev;
                    free(element);
                    list->count--;
                    return;
                }
                element = element->next;
            }
        }

    }
}
