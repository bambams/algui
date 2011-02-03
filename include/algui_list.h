#ifndef ALGUI_LIST_H
#define ALGUI_LIST_H


#include <stddef.h>


/** double-linked list node.
 */
typedef struct ALGUI_LIST_NODE {
    struct ALGUI_LIST_NODE *prev;
    struct ALGUI_LIST_NODE *next;
    void *data;
} ALGUI_LIST_NODE;


/** double-linked list.    
 */
typedef struct ALGUI_LIST {
    ALGUI_LIST_NODE *first;
    ALGUI_LIST_NODE *last;
    size_t length;
} ALGUI_LIST;


/** returns the first node of a list.
    @param list list to get the node from.
    @return a pointer to the first node of the list or null if there is none.
 */
ALGUI_LIST_NODE *algui_get_first_list_node(ALGUI_LIST *list);


/** returns the last node of a list.
    @param list list to get the node from.
    @return a pointer to the last node of the list or null if there is none.
 */
ALGUI_LIST_NODE *algui_get_last_list_node(ALGUI_LIST *list);


/** returns the length of the list.
    @param list list to get the length of.
    @return the length of the list of zero of the list is empty.
 */
size_t algui_get_list_length(ALGUI_LIST *list); 


/** returns the next node.
    @param node node to get the next node.
    @return a pointer to the next node of the list or null if there is none.
 */
ALGUI_LIST_NODE *algui_get_next_list_node(ALGUI_LIST_NODE *node);


/** returns the previous node.
    @param node node to get the next node.
    @return a pointer to the previous node of the list or null if there is none.
 */
ALGUI_LIST_NODE *algui_get_prev_list_node(ALGUI_LIST_NODE *node);


/** returns the data of a list node.
    @param node node to get the data of.
    @return a pointer to the node's data.
 */
void *algui_get_list_node_data(ALGUI_LIST_NODE *node);


/** initializes a list node.
    @param node node to initialize.
    @param data the initial node data.
 */
void algui_init_list_node(ALGUI_LIST_NODE *node, void *data);


/** initializes a list.
    @param list list to initialize.
 */
void algui_init_list(ALGUI_LIST *list);


/** inserts a node in a list.
    @param list list to insert the node to.
    @param node node to insert.
    @param next next node; can be null.
 */
void algui_insert_list_node(ALGUI_LIST *list, ALGUI_LIST_NODE *node, ALGUI_LIST_NODE *next);


/** adds a node to the beginning of a list.
    @param list list to prepend the node to.
    @param node node to prepen.
 */
void algui_prepend_list_node(ALGUI_LIST *list, ALGUI_LIST_NODE *node);


/** adds a node to the end of a list.
    @param list list to append the node to.
    @param node node to append.
 */
void algui_append_list_node(ALGUI_LIST *list, ALGUI_LIST_NODE *node);


/** removes a node from a list.
    @param list list to remove the node from.
    @param node node to remove.
 */
void algui_remove_list_node(ALGUI_LIST *list, ALGUI_LIST_NODE *node);


/** removes all nodes.
    @param list list to clear.
 */
void algui_clear_list(ALGUI_LIST *list); 


/** sets the data of a node.
    @param node node to set.
    @param data pointer to node's data.
 */
void algui_set_list_node_data(ALGUI_LIST_NODE *node, void *data);


#endif //ALGUI_LIST_H
