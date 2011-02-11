#ifndef ALGUI_SKIN_H
#define ALGUI_SKIN_H


#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include "algui_version.h"


/** an algui skin is nothing more an an Allegro config file with resource strings and a path.
    The folder of the skin must contain the config file and the resources.
    Widgets can then use the skin to load the resources from the disk,
    with the help of the skin functions.
 */
typedef struct ALGUI_SKIN {
    char *filename;
    ALLEGRO_CONFIG *config;
} ALGUI_SKIN;


/** returns the current filename of a skin.
    @param skin skin to get the filename of.
    @return the filename of a skin; a pointer to the internal character buffer.
 */
const char *algui_get_skin_filename(ALGUI_SKIN *skin); 


/** returns the allegro config of a skin structure.
    @param skin skin to get the config of.
    @return the config of the skin.
 */
ALLEGRO_CONFIG *algui_get_skin_config(ALGUI_SKIN *skin); 


/** returns an integer value from a skin.
    @param skin skin to get the value from.
    @param wgt widget name.
    @param res resource name.
    @param def default value.
    @return the value from the config or the default if not found.
 */
int algui_get_skin_int(ALGUI_SKIN *skin, const char *wgt, const char *res, int def); 


/** returns an unsigned integer value from a skin.
    @param skin skin to get the value from.
    @param wgt widget name.
    @param res resource name.
    @param def default value.
    @return the value from the config or the default if not found.
 */
unsigned int algui_get_skin_uint(ALGUI_SKIN *skin, const char *wgt, const char *res, unsigned int def); 


/** returns a double value from a skin.
    @param skin skin to get the value from.
    @param wgt widget name.
    @param res resource name.
    @param def default value.
    @return the value from the config or the default if not found.
 */
double algui_get_skin_double(ALGUI_SKIN *skin, const char *wgt, const char *res, double def); 


/** returns a string from a skin.
    @param skin skin to get the value from.
    @param wgt widget name.
    @param res resource name.
    @param def default value.
    @return the value from the config or the default if not found. The pointer returned must not be freed.
 */
const char *algui_get_skin_str(ALGUI_SKIN *skin, const char *wgt, const char *res, const char *def); 


/** returns a color from a skin.
    @param skin skin to get the color from.
    @param wgt widget name.
    @param res resource name.
    @param def default value.
    @return the value from the config or the default if not found.
 */
ALLEGRO_COLOR algui_get_skin_color(ALGUI_SKIN *skin, const char *wgt, const char *res, ALLEGRO_COLOR def); 


/** loads a bitmap from a skin.
    The bitmap is managed via the resource manager.
    @param skin skin.
    @param wgt widget name.
    @param res resource name.
    @param def default resource.
    @return the loaded resource or the default resource if the resource is not found.
 */
ALLEGRO_BITMAP *algui_get_skin_bitmap(ALGUI_SKIN *skin, const char *wgt, const char *res, ALLEGRO_BITMAP *def); 


/** loads a font from a skin.
    The font is managed via the resource manager.
    @param skin skin.
    @param wgt widget name.
    @param res resource name.
    @param def default resource.
    @param def_size def size of font, in case the size of the font is not found.
    @param def_flags def size of font, in case the flags of the font are not found.
    @return the loaded resource or the default resource if the resource is not found.
 */
ALLEGRO_FONT *algui_get_skin_font(ALGUI_SKIN *skin, const char *wgt, const char *res, ALLEGRO_FONT *def, unsigned int def_size, unsigned int def_flags); 


/** initializes a skin structure.
    Creates a new empty allegro config for the skin structure.
    @param skin skin structure to initialize.
 */
void algui_init_skin(ALGUI_SKIN *skin); 


/** cleans up a skin structure.
    Destroys the allegro config.
    @param skin skin structure to cleanup.
 */
void algui_cleanup_skin(ALGUI_SKIN *skin); 


/** creates a skin structure with an empty allegro config.
 */
ALGUI_SKIN *algui_create_skin(); 


/** destroys a skin structure and its allegro config.
 */
void algui_destroy_skin(ALGUI_SKIN *skin); 


/** loads a skin from disk.
    @param skin skin to load.
    @param filename filename of the skin's config file.
    @return the skin or NULL if the skin could not be loaded.
 */
ALGUI_SKIN *algui_load_skin(const char *filename); 


/** saves a skin to disk.
    @param skin skin to save.
    @param filename filename.
    @return non-zero on success, zero on failure.
 */
int algui_save_skin(ALGUI_SKIN *skin, const char *filename);


/** adds or sets an integer value of a skin.
    @param skin skin to set the value of.
    @param wgt widget name.
    @param res resource name.
    @param def default value.
    @return non-zero on success, zero on failure.
 */
int algui_set_skin_int(ALGUI_SKIN *skin, const char *wgt, const char *res, int val); 


/** adds or sets an unsigned integer value of a skin.
    @param skin skin to set the value of.
    @param wgt widget name.
    @param res resource name.
    @param val resource value.
    @return non-zero on success, zero on failure.
 */
int algui_set_skin_uint(ALGUI_SKIN *skin, const char *wgt, const char *res, unsigned int val); 


/** adds or sets a double value of a skin.
    @param skin skin to set the value of.
    @param wgt widget name.
    @param res resource name.
    @param val resource value.
    @return non-zero on success, zero on failure.
 */
int algui_set_skin_double(ALGUI_SKIN *skin, const char *wgt, const char *res, double val); 


/** adds or sets a string of a skin.
    @param skin skin to set the value of.
    @param wgt widget name.
    @param res resource name.
    @param val resource value.
    @return non-zero on success, zero on failure.
 */
int algui_set_skin_str(ALGUI_SKIN *skin, const char *wgt, const char *res, const char *val); 


/** adds or sets a color of a skin.
    @param skin skin to set the color of.
    @param wgt widget name.
    @param res resource name.
    @param val resource value.
    @return non-zero on success, zero on failure.
 */
int algui_set_skin_color(ALGUI_SKIN *skin, const char *wgt, const char *res, ALLEGRO_COLOR color); 


/** adds or sets a bitmap of a skin.
    The bitmap must be placed manually in the skin's folder.
    @param skin skin to set the bitmap of.
    @param wgt widget name.
    @param res resource name.
    @param filename the filename of the bitmap.
    @return non-zero on success, zero on failure.
 */
int algui_set_skin_bitmap(ALGUI_SKIN *skin, const char *wgt, const char *res, const char *filename); 


/** adds or sets a font of a skin.
    The font must be placed manually in the skin's folder.
    The font is not saved in 
    @param skin skin to set the bitmap of.
    @param wgt widget name.
    @param res resource name.
    @param filename the filename of the font.
    @param size the size of the font.
    @param flags font flags, as in al_load_font.
    @return non-zero on success, zero on failure.
 */
int algui_set_skin_font(ALGUI_SKIN *skin, const char *wgt, const char *res, const char *filename, unsigned int size, unsigned int flags);


#endif //ALGUI_SKIN_H
