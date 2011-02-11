#include "algui_resource_manager.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include "algui_list.h"


/******************************************************************************
    PRIVATE TYPES
 ******************************************************************************/
 
 
//resource node
typedef struct _RESOURCE {
    ALGUI_LIST_NODE node;
    void *data;
    void *name;
    void (*destructor)(void *);
    int ref_count;
} _RESOURCE; 
 
 
/******************************************************************************
    PRIVATE VARIABLES
 ******************************************************************************/
 
 
//mutex
static ALLEGRO_MUTEX *_mutex = NULL; 
 
 
//resources
static ALGUI_LIST _resources = ALGUI_LIST_INITIALIZER; 
 
 
/******************************************************************************
    PRIVATE FUNCTIONS
 ******************************************************************************/
 
 
//initializes the resource manager; invoked from algui_init
int _algui_init_resource_manager() {
    _mutex = al_create_mutex();
    if (!_mutex) return 0;
    return 1;
} 
 
 
//cleans up the resource manager; invoked from algui_cleanup
void _algui_cleanup_resource_manager() {
    algui_destroy_resources();
    al_destroy_mutex(_mutex);
} 
 
 
//locates a resource by name
static _RESOURCE *_find_resource_by_name(const char *name) {
    ALGUI_LIST_NODE *node;
    _RESOURCE *res;    
    for(node = algui_get_first_list_node(&_resources); node; node = algui_get_next_list_node(node)) {
        res = (_RESOURCE *)algui_get_list_node_data(node);
        if (strcmp(res->name, name) == 0) return res;
    }
    return NULL;
}


//locates a resource by data
static _RESOURCE *_find_resource_by_data(void *data) {
    ALGUI_LIST_NODE *node;
    _RESOURCE *res;    
    for(node = algui_get_first_list_node(&_resources); node; node = algui_get_next_list_node(node)) {
        res = (_RESOURCE *)algui_get_list_node_data(node);
        if (res->data == data) return res;
    }
    return NULL;
}


//creates a resource 
static _RESOURCE *_create_resource(void *data, const char *name, void (*dtor)(void *), int ref_count) {
    _RESOURCE *res = al_malloc(sizeof(_RESOURCE));
    assert(res);
    algui_init_list_node(&res->node, res);
    res->data = data;
    res->name = strdup(name);
    res->destructor = dtor;
    res->ref_count = ref_count;
    return res;
}


//destroys a resource
static void _destroy_resource(_RESOURCE *res, int invoke_dtor) {
    if (invoke_dtor) res->destructor(res->data);
    free(res->name);
    al_free(res);
}
 
 
/******************************************************************************
    PUBLIC FUNCTIONS
 ******************************************************************************/
  
  
/** installs a new resource to the resource manager.
    The new resource's reference count is 1.
    @param res resource.
    @param name the resource's ASCII name; the string is copied internally.
    @param dtor destructor; invoked when the resource is removed.
    @return non-zero if the operation suceeded, zero if the resource already exists.
 */
int algui_install_resource(void *res, const char *name, void (*dtor)(void *)) {
    _RESOURCE *node;
    
    assert(res);
    assert(name);
    assert(dtor);
    
    al_lock_mutex(_mutex);
    
    //find the resource
    node = _find_resource_by_name(name);
    
    //if the resource is already installed, return error
    if (node) {
        al_unlock_mutex(_mutex);
        return 0;
    }        
    
    //create a new resource node
    node = _create_resource(res, name, dtor, 1);
    
    //add it to the resource list
    algui_append_list_node(&_resources, &node->node);
    
    al_unlock_mutex(_mutex);
        
    //success
    return 1;
}


/** uninstalls a resource from the resource manager.
    The destructor of the resource is not invoked.
    @param res pointer to the resource to uninstall; can be null.
    @return non-zero if the operation suceeded, zero if the resource does not exist.
 */
int algui_uninstall_resource(void *res) {
    _RESOURCE *node;

    //no resource
    if (!res) return 0;
    
    al_lock_mutex(_mutex);
    
    //find the resource
    node = _find_resource_by_data(res);
    
    //if the resource is not found, return error
    if (!node) {
        al_unlock_mutex(_mutex);
        return 0;
    }        
    
    //remove the node from the resources
    algui_remove_list_node(&_resources, &node->node);
    
    //destroy the node without invoking the destructor
    _destroy_resource(node, 0);
    
    al_unlock_mutex(_mutex);
    
    //success
    return 1;
}


/** retrieves a resource from the resource manager by name.
    If the resource is found, then its reference count is incremented.
    @param name the resource's ASCII name.
    @return pointer to the resource or NULL if the resource is not found.
 */
void *algui_acquire_resource(const char *name) {
    _RESOURCE *node;

    assert(name);
    
    al_lock_mutex(_mutex);
        
    //find the resource
    node = _find_resource_by_name(name);
    
    //if not found, return error
    if (!node) {
        al_unlock_mutex(_mutex);
        return NULL;
    }        
    
    //increment the resource's ref count
    ++node->ref_count;
    assert(node->ref_count != 0);
    
    al_unlock_mutex(_mutex);
    
    //return the resource data
    return node->data;
}


/** releases a resource.
    If the resource is found, then its reference count is decremented.
    If the resource's reference count drops to 0, the resource is deleted and uninstalled.
    @param res resource; can be null.
    @return non-zero if the operation suceeded, zero if the resource does not exist.
 */
int algui_release_resource(void *res) {
    _RESOURCE *node;

    //no resource
    if (!res) return 0;    
    
    al_lock_mutex(_mutex);
        
    //find the resource
    node = _find_resource_by_name(res);
    
    //if not found, return error
    if (!node) {
        al_unlock_mutex(_mutex);
        return 0;
    }        
    
    //decrement the resource's ref count
    assert(node->ref_count > 0);
    --node->ref_count;
    
    al_lock_mutex(_mutex);
        
    //success
    return 1;
}


/** destroys and uninstalls all resources.
 */
void algui_destroy_resources() {
    ALGUI_LIST_NODE *node, *next;    
    
    al_lock_mutex(_mutex);
    
    for(node = algui_get_first_list_node(&_resources); node; ) {
        next = algui_get_next_list_node(node);
        algui_remove_list_node(&_resources, node);
        _destroy_resource((_RESOURCE *)algui_get_list_node_data(node), 1);
        node = next;
    }
    
    al_unlock_mutex(_mutex);
}


/** destructor for a bitmap resource.
    It destroys the allegro bitmap.
    @param res pointer to resource to destroy.
 */
void algui_bitmap_resource_destructor(void *res) {
    assert(res);
    al_destroy_bitmap((ALLEGRO_BITMAP *)res);
}


/** destructor for a font resource.
    It destroys the allegro font.
    @param res pointer to resource to destroy.
 */
void algui_font_resource_destructor(void *res) {
    assert(res);
    al_destroy_font((ALLEGRO_FONT *)res);
}
