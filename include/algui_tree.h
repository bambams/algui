#ifndef ALGUI_TREE_H
#define ALGUI_TREE_H


#include "algui_list.h"


/** An N-ary tree node.
 */
typedef struct ALGUI_TREE {
    struct ALGUI_TREE *parent;
    ALGUI_LIST_NODE node;
    ALGUI_LIST children;
    void *data;
} ALGUI_TREE;


/** returns the parent tree node.
    @param tree tree node to get the parent of.
    @return the parent tree node.
 */
ALGUI_TREE *algui_get_parent_tree(ALGUI_TREE *tree);


/** returns the previous sibling tree node.
    @param tree tree node to get the sibling of.
    @return the previous sibling tree node.
 */
ALGUI_TREE *algui_get_prev_sibling_tree(ALGUI_TREE *tree); 


/** returns the next sibling tree node.
    @param tree tree node to get the sibling of.
    @return the next sibling tree node.
 */
ALGUI_TREE *algui_get_next_sibling_tree(ALGUI_TREE *tree); 


/** returns the first child tree node.
    @param tree tree node to get the first child of.
    @return the first child tree node.
 */
ALGUI_TREE *algui_get_first_child_tree(ALGUI_TREE *tree); 


/** returns the last child tree node.
    @param tree tree node to get the last child of.
    @return the last child tree node.
 */
ALGUI_TREE *algui_get_last_child_tree(ALGUI_TREE *tree); 


/** returns the number of children tree nodes.
    @param tree tree node to get the number of children.
    @return the number of children.
 */
size_t algui_get_tree_child_count(ALGUI_TREE *tree);


/** returns the data stored in a tree node.
    @param tree tree node to get the data of.
    @return the node's data.
 */
void *algui_get_tree_data(ALGUI_TREE *tree);


/** returns the root node of a tree node.
    @param tree tree node to get the root of.
    @return the root of the tree.
 */
ALGUI_TREE *algui_get_root_tree(ALGUI_TREE *tree); 


/** checks if a tree node is ancestor of another tree node.
    @param ancestor ancestor node.
    @param descentant descentant node.
    @return non-zero if the relationship is true, zero otherwise.
 */
int algui_is_ancestor_tree(ALGUI_TREE *ancestor, ALGUI_TREE *descentant);


/** inits a tree.
    @param tree tree to initialize.
    @param data tree data.
 */
void algui_init_tree(ALGUI_TREE *tree, void *data); 


/** inserts a tree node as a child into another tree node.
    @param parent parent tree node.
    @param child child node.
    @param next next sibling tree node; it may be null.
    @return non-zero if the operation was successful, zero otherwise.
 */
int algui_insert_tree(ALGUI_TREE *parent, ALGUI_TREE *child, ALGUI_TREE *next); 


/** inserts a child node into the beginning of a tree's children list.
    @param parent parent tree node.
    @param child child node.
    @return non-zero if the operation was successful, zero otherwise.
 */
int algui_prepend_tree(ALGUI_TREE *parent, ALGUI_TREE *child); 


/** inserts a child node at the end of a tree's children list.
    @param parent parent tree node.
    @param child child node.
    @return non-zero if the operation was successful, zero otherwise.
 */
int algui_append_tree(ALGUI_TREE *parent, ALGUI_TREE *child); 


/** removes a child node from its parent.
    @param parent parent tree node.
    @param child child tree node.
    @return non-zero if the operation succeeded, zero otherwise.
 */
int algui_remove_tree(ALGUI_TREE *parent, ALGUI_TREE *child); 


/** detaches a child tree node from its parent, if it exists.
    @param child child tree node to detach from its parent.
 */
void algui_detach_tree(ALGUI_TREE *tree); 


/** clears a tree by removing all children nodes.
    @param tree tree node to clear.
 */
void algui_clear_tree(ALGUI_TREE *tree); 


/** sets the data of a tree node.
    @param tree tree node to set the data of.
    @param data data of the tree node.
 */
void algui_set_tree_data(ALGUI_TREE *tree, void *data); 


#endif //ALGUI_TREE_H
