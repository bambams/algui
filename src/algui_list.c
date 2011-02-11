#include "algui_list.h"
#include <assert.h>
#include <stddef.h>


/******************************************************************************
    PUBLIC
 ******************************************************************************/


/** returns the first node of a list.
    @param list list to get the node from.
    @return a pointer to the first node of the list or null if there is none.
 */
ALGUI_LIST_NODE *algui_get_first_list_node(ALGUI_LIST *list) {
    assert(list);
    return list->first;
}


/** returns the last node of a list.
    @param list list to get the node from.
    @return a pointer to the last node of the list or null if there is none.
 */
ALGUI_LIST_NODE *algui_get_last_list_node(ALGUI_LIST *list) {
    assert(list);
    return list->last;
}


/** returns the length of the list.
    @param list list to get the length of.
    @return the length of the list of zero of the list is empty.
 */
unsigned long algui_get_list_length(ALGUI_LIST *list) {
    assert(list);
    return list->length;
}


/** returns the next node.
    @param node node to get the next node.
    @return a pointer to the next node of the list or null if there is none.
 */
ALGUI_LIST_NODE *algui_get_next_list_node(ALGUI_LIST_NODE *node) {
    assert(node);
    return node->next;
}


/** returns the previous node.
    @param node node to get the next node.
    @return a pointer to the previous node of the list or null if there is none.
 */
ALGUI_LIST_NODE *algui_get_prev_list_node(ALGUI_LIST_NODE *node) {
    assert(node);
    return node->prev;
}


/** returns the data of a list node.
    @param node node to get the data of.
    @return a pointer to the node's data.
 */
void *algui_get_list_node_data(ALGUI_LIST_NODE *node) {
    assert(node);
    return node->data;
}


/** initializes a list node.
    @param node node to initialize.
    @param data the initial node data.
 */
void algui_init_list_node(ALGUI_LIST_NODE *node, void *data) {
    assert(node);
    node->prev = NULL;
    node->next = NULL;
    node->data = data;
}


/** initializes a list.
    @param list list to initialize.
 */
void algui_init_list(ALGUI_LIST *list) {
    assert(list);
    list->first = NULL;
    list->last = NULL;
    list->length = 0;
}


/** inserts a node in a list.
    @param list list to insert the node to.
    @param node node to insert.
    @param next next node; can be null.
 */
void algui_insert_list_node(ALGUI_LIST *list, ALGUI_LIST_NODE *node, ALGUI_LIST_NODE *next) {
    ALGUI_LIST_NODE *prev;

    assert(list);
    assert(node);
    assert(node->next == NULL);
    assert(node->prev == NULL);
    
    //get previous
    prev = next ? next->prev : list->last;
    
    //set up the node
    node->prev = prev;
    node->next = next;
    
    //link the node
    if (prev) prev->next = node; else list->first = node;
    if (next) next->prev = node; else list->last = node;
    
    //add one more node
    ++list->length;
}


/** adds a node to the beginning of a list.
    @param list list to prepend the node to.
    @param node node to prepen.
 */
void algui_prepend_list_node(ALGUI_LIST *list, ALGUI_LIST_NODE *node) {
    algui_insert_list_node(list, node, algui_get_first_list_node(list));
}


/** adds a node to the end of a list.
    @param list list to append the node to.
    @param node node to append.
 */
void algui_append_list_node(ALGUI_LIST *list, ALGUI_LIST_NODE *node) {
    algui_insert_list_node(list, node, NULL);
}


/** removes a node from a list.
    @param list list to remove the node from.
    @param node node to remove.
 */
void algui_remove_list_node(ALGUI_LIST *list, ALGUI_LIST_NODE *node) {
    assert(list);
    assert(node);
    
    //unlink node
    if (node->prev) node->prev->next = node->next; else list->first = node->next;
    if (node->next) node->next->prev = node->prev; else list->last = node->prev;
    
    //one less node
    --list->length;
    
    //reset the node
    node->prev = NULL;
    node->next = NULL;
}


/** removes all nodes.
    @param list list to clear.
 */
void algui_clear_list(ALGUI_LIST *list) {
    ALGUI_LIST_NODE *node, *next;

    assert(list);
    
    //unlink all nodes
    for(node = list->first; node; ) {    
        //get next
        next = node->next;
        
        //unlink node
        node->prev = NULL;
        node->next = NULL;
        
        //proceed to next
        node = next;
    }
    
    //reset length
    list->length = 0;
}


/** sets the data of a node.
    @param node node to set.
    @param data pointer to node's data.
 */
void algui_set_list_node_data(ALGUI_LIST_NODE *node, void *data) {
    assert(node);
    node->data = data;
}

