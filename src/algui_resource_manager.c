#include "algui_resource_manager.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include "algui_list.h"


/******************************************************************************
    PRIVATE TYPES
 ******************************************************************************/
 
 
//resource node
typedef struct _RESOURCE {
    //list node
    ALGUI_LIST_NODE node;
    
    //pointer to data (bitmap, font, string etc)
    void *data;
    
    //the resource's name
    ALLEGRO_USTR *name;
    
    //used to destroy the data
    void (*destructor)(void *);
    
    //the reference count
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
    ALLEGRO_USTR_INFO info;
    ALLEGRO_USTR *name_ustr;
    
    //create a temporary name ustr to use in comparison
    name_ustr = al_ref_cstr(&info, name);
    
    //iterate the resources
    for(node = algui_get_first_list_node(&_resources); node; node = algui_get_next_list_node(node)) {
    
        //get the resource
        res = (_RESOURCE *)algui_get_list_node_data(node);
        
        //compare the strings
        if (al_ustr_equal(res->name, name_ustr)) return res;
    }
    
    //not found
    return NULL;
}


//locates a resource by data
static _RESOURCE *_find_resource_by_data(void *data) {
    ALGUI_LIST_NODE *node;
    _RESOURCE *res;    
    
    //iterate the resources
    for(node = algui_get_first_list_node(&_resources); node; node = algui_get_next_list_node(node)) {
        //get the resource
        res = (_RESOURCE *)algui_get_list_node_data(node);
        
        //compare the resource
        if (res->data == data) return res;
    }
    
    //not found
    return NULL;
}


//creates a resource 
static _RESOURCE *_create_resource(void *data, const char *name, void (*dtor)(void *), int ref_count) {
    _RESOURCE *res;
    
    //allocate the resource
    res = al_malloc(sizeof(_RESOURCE));
    assert(res);
    
    //init the list node
    algui_init_list_node(&res->node, res);
    
    //init the data
    res->data = data;
    
    //init the name; copy the given name
    res->name = al_ustr_new(name);
    
    //init the destructor
    res->destructor = dtor;
    
    //init the ref count
    res->ref_count = ref_count;
    
    return res;
}


//destroys a resource
static void _destroy_resource(_RESOURCE *res, int invoke_dtor) {
    //invoke the destructor
    if (invoke_dtor) res->destructor(res->data);
    
    //destroy the name string
    al_ustr_free(res->name);
    
    //free the memory occupied by the resource
    al_free(res);
}


//removes a resource node from the list of resources and destroys the resource
static void _uninstall_resource(_RESOURCE *res, int invoke_dtor) {
    //remove the node from the list
    algui_remove_list_node(&_resources, &res->node);
    
    //destroy the resoruce
    _destroy_resource(res, invoke_dtor);
}
 
 
/******************************************************************************
    PUBLIC FUNCTIONS
 ******************************************************************************/
  
  
/** installs a new resource to the resource manager.
    The new resource's reference count is 1.
    @param res resource.
    @param name the resource's name (UTF-8 string); the string is copied internally.
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
    
    //uninstall the resource without invoking the destructor
    _uninstall_resource(node, 0);
    
    al_unlock_mutex(_mutex);
    
    //success
    return 1;
}


/** retrieves a resource from the resource manager by name.
    If the resource is found, then its reference count is incremented.
    @param name the resource's name (UTF-8 string).
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
    node = _find_resource_by_data(res);
    
    //if not found, return error
    if (!node) {
        al_unlock_mutex(_mutex);
        return 0;
    }        
    
    //decrement the resource's ref count
    assert(node->ref_count > 0);
    --node->ref_count;
    
    //if the ref count reaches 0, uninstall the resource
    if (!node->ref_count) _uninstall_resource(node, 1);
    
    al_lock_mutex(_mutex);
        
    //success
    return 1;
}


/** destroys and uninstalls all resources.
 */
void algui_destroy_resources() {
    ALGUI_LIST_NODE *node, *next;    
    
    //lock the resources
    al_lock_mutex(_mutex);
    
    //iterate the resources
    for(node = algui_get_first_list_node(&_resources); node; ) {    
        //get next
        next = algui_get_next_list_node(node);
        
        //uninstall the resource
        _uninstall_resource((_RESOURCE *)algui_get_list_node_data(node), 1);
        
        //continue with the next
        node = next;
    }
    
    //unlock the resources
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


/** destructor for an Allegro string resource.
    It frees the string.
    @param res pointer to resource to destroy.
 */
void algui_ustr_resource_destructor(void *res) {
    assert(res);
    al_ustr_free((ALLEGRO_USTR *)res);
}
