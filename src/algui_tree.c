#include "algui_tree.h"
#include <assert.h>
#include <stddef.h>


/******************************************************************************
    PUBLIC
 ******************************************************************************/


/** returns the parent tree node.
    @param tree tree node to get the parent of.
    @return the parent tree node.
 */
ALGUI_TREE *algui_get_parent_tree(ALGUI_TREE *tree) {
    assert(tree);
    return tree->parent;
}


/** returns the previous sibling tree node.
    @param tree tree node to get the sibling of.
    @return the previous sibling tree node.
 */
ALGUI_TREE *algui_get_prev_sibling_tree(ALGUI_TREE *tree) {    
    ALGUI_LIST_NODE *prev;        
    assert(tree);
    prev = algui_get_prev_list_node(&tree->node);    
    return prev ? (ALGUI_TREE *)algui_get_list_node_data(prev) : NULL;
}


/** returns the next sibling tree node.
    @param tree tree node to get the sibling of.
    @return the next sibling tree node.
 */
ALGUI_TREE *algui_get_next_sibling_tree(ALGUI_TREE *tree) {
    ALGUI_LIST_NODE *node;        
    assert(tree);
    node = algui_get_next_list_node(&tree->node);    
    return node ? (ALGUI_TREE *)algui_get_list_node_data(node) : NULL;
}


/** returns the first child tree node.
    @param tree tree node to get the first child of.
    @return the first child tree node.
 */
ALGUI_TREE *algui_get_first_child_tree(ALGUI_TREE *tree) {
    ALGUI_LIST_NODE *node;        
    assert(tree);
    node = algui_get_first_list_node(&tree->children);    
    return node ? (ALGUI_TREE *)algui_get_list_node_data(node) : NULL;
}


/** returns the last child tree node.
    @param tree tree node to get the last child of.
    @return the last child tree node.
 */
ALGUI_TREE *algui_get_last_child_tree(ALGUI_TREE *tree) {
    ALGUI_LIST_NODE *node;        
    assert(tree);
    node = algui_get_last_list_node(&tree->children);    
    return node ? (ALGUI_TREE *)algui_get_list_node_data(node) : NULL;
}


/** returns the number of children tree nodes.
    @param tree tree node to get the number of children.
    @return the number of children.
 */
unsigned long algui_get_tree_child_count(ALGUI_TREE *tree) {
    assert(tree);
    return algui_get_list_length(&tree->children);
}


/** returns the data stored in a tree node.
    @param tree tree node to get the data of.
    @return the node's data.
 */
void *algui_get_tree_data(ALGUI_TREE *tree) {
    assert(tree);
    return tree->data;
}


/** returns the root node of a tree node.
    @param tree tree node to get the root of.
    @return the root of the tree.
 */
ALGUI_TREE *algui_get_root_tree(ALGUI_TREE *tree) {
    assert(tree);
    for(; tree->parent; tree = tree->parent);
    return tree;
}


/** checks if a tree node is ancestor of another tree node.
    @param ancestor ancestor node.
    @param descentant descentant node.
    @return non-zero if the relationship is true, zero otherwise.
 */
int algui_is_ancestor_tree(ALGUI_TREE *ancestor, ALGUI_TREE *descentant) {
    assert(ancestor);
    assert(descentant);
    for(descentant = descentant->parent; descentant; descentant = descentant->parent) {
        if (descentant == ancestor) return 1;
    }
    return 0;
}


/** inits a tree.
    @param tree tree to initialize.
    @param data tree data.
 */
void algui_init_tree(ALGUI_TREE *tree, void *data) {
    assert(tree);
    tree->parent = NULL;
    algui_init_list_node(&tree->node, tree);
    algui_init_list(&tree->children);
    tree->data = data;
}


/** inserts a tree node as a child into another tree node.
    @param parent parent tree node.
    @param child child node.
    @param next next sibling tree node; it may be null.
    @return non-zero if the operation was successful, zero otherwise.
 */
int algui_insert_tree(ALGUI_TREE *parent, ALGUI_TREE *child, ALGUI_TREE *next) {
    assert(parent);
    assert(child);
    if (child == parent || child->parent || algui_is_ancestor_tree(child, parent)) return 0;
    if (next && next->parent != parent) return 0;
    child->parent = parent;
    algui_insert_list_node(&parent->children, &child->node, next ? &next->node : NULL);
    return 1;
}


/** inserts a child node into the beginning of a tree's children list.
    @param parent parent tree node.
    @param child child node.
    @return non-zero if the operation was successful, zero otherwise.
 */
int algui_prepend_tree(ALGUI_TREE *parent, ALGUI_TREE *child) {
    return algui_insert_tree(parent, child, algui_get_first_child_tree(parent));
}


/** inserts a child node at the end of a tree's children list.
    @param parent parent tree node.
    @param child child node.
    @return non-zero if the operation was successful, zero otherwise.
 */
int algui_append_tree(ALGUI_TREE *parent, ALGUI_TREE *child) {
    return algui_insert_tree(parent, child, NULL);
}


/** removes a child node from its parent.
    @param parent parent tree node.
    @param child child tree node.
    @return non-zero if the operation succeeded, zero otherwise.
 */
int algui_remove_tree(ALGUI_TREE *parent, ALGUI_TREE *child) {
    assert(parent);
    assert(child);
    if (child->parent != parent) return 0;
    child->parent = 0;
    algui_remove_list_node(&parent->children, &child->node);
    return 1;
}


/** detaches a child tree node from its parent, if it exists.
    @param child child tree node to detach from its parent.
 */
void algui_detach_tree(ALGUI_TREE *tree) {
    if (tree->parent) algui_remove_tree(tree->parent, tree);
}


/** clears a tree by removing all children nodes.
    @param tree tree node to clear.
 */
void algui_clear_tree(ALGUI_TREE *tree) {
    ALGUI_TREE *child, *next;
    assert(tree);
    for(child = algui_get_first_child_tree(tree); child; ) {
        next = algui_get_next_sibling_tree(child);
        algui_remove_tree(tree, child);
        child = next;
    }
}


/** sets the data of a tree node.
    @param tree tree node to set the data of.
    @param data data of the tree node.
 */
void algui_set_tree_data(ALGUI_TREE *tree, void *data) {
    assert(tree);
    tree->data = data;
}
