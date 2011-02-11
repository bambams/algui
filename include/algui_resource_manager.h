#ifndef ALGUI_RESOURCE_MANAGER_H
#define ALGUI_RESOURCE_MANAGER_H


#include <allegro5/allegro.h>


/** installs a new resource to the resource manager.
    The new resource's reference count is 1.
    All the resources will be automatically destroyed at program exit.
    @param res resource.
    @param name the resource's name (UTF-8 string); the string is copied internally.
    @param dtor destructor; invoked when the resource is removed.
    @return non-zero if the operation suceeded, zero if the resource already exists.
 */
int algui_install_resource(void *res, const char *name, void (*dtor)(void *));


/** uninstalls a resource from the resource manager.
    The destructor of the resource is not invoked.
    @param res pointer to the resource to uninstall.
    @return non-zero if the operation suceeded, zero if the resource does not exist.
 */
int algui_uninstall_resource(void *res); 


/** retrieves a resource from the resource manager by name.
    If the resource is found, then its reference count is incremented.
    @param name the resource's name (UTF-8 string).
    @return pointer to the resource or NULL if the resource is not found.
 */
void *algui_acquire_resource(const char *name); 


/** releases a resource.
    If the resource is found, then its reference count is decremented.
    If the resource's reference count drops to 0, the resource is deleted and uninstalled.
    @param res resource.
    @return non-zero if the operation suceeded, zero if the resource does not exist.
 */
int algui_release_resource(void *res); 


/** destroys and uninstalls all resources.
    This is invoked automatically at exit.
 */
void algui_destroy_resources(); 


/** destructor for a bitmap resource.
    It destroys the allegro bitmap.
    @param res pointer to resource to destroy.
 */
void algui_bitmap_resource_destructor(void *res); 


/** destructor for a font resource.
    It destroys the allegro font.
    @param res pointer to resource to destroy.
 */
void algui_font_resource_destructor(void *res); 


/** destructor for an Allegro string resource.
    It frees the string.
    @param res pointer to resource to destroy.
 */
void algui_ustr_resource_destructor(void *res); 


#endif //ALGUI_RESOURCE_MANAGER_H
