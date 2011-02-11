#include "algui_skin.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "algui_resource_manager.h"


/******************************************************************************
    INTERNAL FUNCTIONS
 ******************************************************************************/
 
 
//adds a substring
static int _add_substring(ALLEGRO_USTR *str, int begin_pos, int end_pos, ALLEGRO_USTR *result[], int result_len, int *count) {
    //if the count exceeds the string length, do nothing
    if (*count >= result_len) return 0;
    
    //create a substring; remove whitespace
    result[*count] = al_ustr_dup_substr(str, begin_pos, end_pos);
    al_ustr_trim_ws(result[*count]);
    
    //increment the count
    ++(*count);
    
    //success
    return 1;
}


//splits a string by a character
static int _split_string_by_char(const char *str, int ch, ALLEGRO_USTR *result[], int result_len) {
    //start from the string beginning
    int pos = 0, next_pos;
    
    //count of sub-strings found
    int count = 0;
    
    //temp allegro string 
    ALLEGRO_USTR_INFO temp_info;
    ALLEGRO_USTR *temp = al_ref_cstr(&temp_info, str);

    //iterate until the end of string
    for(;;) {
        //find the character
        next_pos = al_ustr_find_chr(temp, pos, ch);
        
        //if not found, end
        if (next_pos == -1) break;
        
        //add a string; if no more strings can be added, return
        if (!_add_substring(temp, pos, next_pos, result, result_len, &count)) goto END;
        
        //continue after the character
        pos = next_pos + al_utf8_width(ch);
    }
    
    //set the remainder of the string to be the last string in the array
    _add_substring(temp, pos, al_ustr_size(temp), result, result_len, &count);
    
    END:
    
    return count;
} 


//frees a string array
static void _free_string_array(ALLEGRO_USTR *str[], int count) {
    int i;
    for(i = 0; i < count; ++i) {
        al_ustr_free(str[i]);
    }
}
 
 
//converts a string to integer
static int _string_to_int(const char *str, int *result) {
    char *endptr;
    long temp;

    //if the string is null, the result is invalid
    if (!str) return 0;
    
    //convert the string to integer
    temp = strtol(str, &endptr, 0);
    
    //check for overflow/underflow
    if (errno == ERANGE) return 0;
    
    //check for conversion error
    if (temp == 0 && endptr == str) return 0;
    
    //success
    *result = temp;
    return 1;
} 
 
 
//converts a string to unsigned integer
static int _string_to_uint(const char *str, unsigned int *result) {
    char *endptr;
    unsigned long temp;

    //if the string is null, the result is invalid
    if (!str) return 0;
    
    //convert the string to integer
    temp = strtoul(str, &endptr, 0);
    
    //check for overflow/underflow
    if (errno == ERANGE) return 0;
    
    //check for conversion error
    if (temp == 0 && endptr == str) return 0;
        
    //success
    *result = temp;
    return 1;
} 
 
 
//converts a string to a double
static int _string_to_double(const char *str, double *result) {
    char *endptr;
    double temp;

    //if the string is null, the result is invalid
    if (!str) return 0;
    
    //convert the string to double
    temp = strtod(str, &endptr);
    
    //check for overflow/underflow
    if (errno == ERANGE) return 0;
    
    //check for conversion error
    if (temp == 0.0 && endptr == str) return 0;
        
    //success
    *result = temp;
    return 1;
} 


//converts a string to color
static int _string_to_color(const char *str, ALLEGRO_COLOR *color) {
    ALLEGRO_USTR *result[4];
    unsigned int red, green, blue, alpha;
    int count, ok = 0;
    
    //if the string is null, the result is invalid
    if (!str) return 0;
    
    //split the string by ','
    count = _split_string_by_char(str, ',', result, 4);
    
    switch (count) {
        //read red, green, blue
        case 3:
            _string_to_uint(al_cstr(result[0]), &red);
            _string_to_uint(al_cstr(result[1]), &green);
            _string_to_uint(al_cstr(result[2]), &blue);
            *color = al_map_rgb(red, green, blue);
            ok = 1;
            break;
            
        //read red, green, blue, alpha
        case 4:
            _string_to_uint(al_cstr(result[0]), &red);
            _string_to_uint(al_cstr(result[1]), &green);
            _string_to_uint(al_cstr(result[2]), &blue);
            _string_to_uint(al_cstr(result[3]), &alpha);
            *color = al_map_rgba(red, green, blue, alpha);
            ok = 1;
            break;
    }
    
    //free the strings
    _free_string_array(result, count);
    
    return ok;
}


//returns the filepath of a resource
static ALLEGRO_USTR *_get_resource_filepath(const ALLEGRO_USTR *skin_filepath, const char *resource_filename) {
    ALLEGRO_USTR *result;            
    int pos, end_pos;
    ALLEGRO_USTR *temp;
    ALLEGRO_USTR_INFO temp_info;

    //duplicate the resource filepath in order to replace '\' by '/'
    result = al_ustr_dup(skin_filepath);
    
    //replace '\' with '/'
    al_ustr_find_replace_cstr(result, 0, "\\", "/");
    
    //find the end of the string
    end_pos = al_ustr_size(result);
    
    //find the last occurrence of '/'
    pos = al_ustr_rfind_chr(result, end_pos, '/');
    
    //find the position after the '/'
    pos += al_utf8_width('/');   
    
    //the replace function needs a ustr
    temp = al_ref_cstr(&temp_info, resource_filename);
    
    //replace the string after the '/' with the resource filename
    al_ustr_replace_range(result, pos, end_pos, temp);
    
    return result;
}


//converts a string to a font 
static int _string_to_font(
    const char *str, 
    unsigned int def_size, 
    unsigned int def_flags, 
    ALLEGRO_USTR *filename, 
    unsigned int *size, 
    unsigned int *flags)
{
    ALLEGRO_USTR *result[3];
    int count, ok = 0;
    
    //if the string is null, the result is invalid
    if (!str) return 0;
    
    //split the string by ','
    count = _split_string_by_char(str, ',', result, 3);
    
    switch (count) {
        //read the filename; set the size and flags from the defaults
        case 1:
            al_ustr_assign(filename, result[0]);
            *size = def_size;
            *flags = def_flags;
            ok = 1;
            break;
            
        //read the filename and the size; set the flags from the defaults
        case 2:
            al_ustr_assign(filename, result[0]);
            if (_string_to_uint(al_cstr(result[1]), size)) {
                *flags = def_flags;            
                ok = 1;
            }
            break;
            
        //read the filename, the size and the flags
        case 3:
            al_ustr_assign(filename, result[0]);
            if (_string_to_uint(al_cstr(result[1]), size)) {
                if (_string_to_uint(al_cstr(result[2]), flags)) {
                    ok = 1;
                }
            }
            break;
    }
    
    //free the strings
    _free_string_array(result, count);
    
    return ok;
}


//loads a bitmap from a file
static ALLEGRO_BITMAP *_load_bitmap(ALGUI_SKIN *skin, const char *filename) {
    ALLEGRO_USTR *filepath;
    ALLEGRO_BITMAP *bmp;
    
    //if the filename is null
    if (!filename) return NULL;
    
    //get the resource's filepath
    filepath = _get_resource_filepath(skin->filename, filename);
    
    //empty filepath
    if (!filepath) return NULL;
    
    //load the bitmap
    bmp = al_load_bitmap(al_cstr(filepath));
    
    //bitmap cannot be loaded
    if (!bmp) {
        al_ustr_free(filepath);
        return NULL;
    }        
    
    //install a resource
    if (!algui_install_resource(bmp, al_cstr(filepath), algui_bitmap_resource_destructor)) {
        al_destroy_bitmap(bmp);
        al_ustr_free(filepath);
        return NULL;
    }
    
    //success
    return bmp;
}
 
 
//loads a font from a file
static ALLEGRO_FONT *_load_font(ALGUI_SKIN *skin, const char *filename, unsigned int size, unsigned int flags) {
    ALLEGRO_USTR *filepath;
    ALLEGRO_FONT *font;
    
    //if the filename is null
    if (!filename) return NULL;
    
    //get the resource's filepath
    filepath = _get_resource_filepath(skin->filename, filename);
    
    //empty filepath
    if (!filepath) return NULL;
    
    //load the font
    font = al_load_font(al_cstr(filepath), size, flags);
    
    //the font cannot be loaded
    if (!font) {
        al_ustr_free(filepath);
        return NULL;
    }        
    
    //install a resource
    if (!algui_install_resource(font, al_cstr(filepath), algui_font_resource_destructor)) {
        al_destroy_font(font);
        al_ustr_free(filepath);
        return NULL;
    }
    
    //success
    return font;
}
 
 
/******************************************************************************
    PUBLIC FUNCTIONS
 ******************************************************************************/
 
 
/** returns the current filename of a skin.    
    @param skin skin to get the filename of.
    @return the filename of a skin; a pointer to the internal character buffer (UTF-8 string).
 */
const char *algui_get_skin_filename(ALGUI_SKIN *skin) {
    assert(skin);
    return al_cstr(skin->filename);
}


/** returns the allegro config of a skin structure.
    @param skin skin to get the config of.
    @return the config of the skin.
 */
ALLEGRO_CONFIG *algui_get_skin_config(ALGUI_SKIN *skin) {
    assert(skin);
    return skin->config;
}


/** returns an integer value from a skin.
    @param skin skin to get the value from.
    @param wgt widget name (UTF-8 string).
    @param res resource name (UTF-8 string).
    @param def default value.
    @return the value from the config or the default if not found.
 */
int algui_get_skin_int(ALGUI_SKIN *skin, const char *wgt, const char *res, int def) {
    const char *valstr;
    int result;
    
    assert(skin);
    assert(skin->config);
    
    //get the config value
    valstr = al_get_config_value(skin->config, wgt, res);
    
    //convert the string to integer; if the conversion fails, return the default
    if (!_string_to_int(valstr, &result)) return def;
    
    //success
    return result;
}


/** returns an unsigned integer value from a skin.
    @param skin skin to get the value from.
    @param wgt widget name (UTF-8 string).
    @param res resource name (UTF-8 string).
    @param def default value.
    @return the value from the config or the default if not found.
 */
unsigned int algui_get_skin_uint(ALGUI_SKIN *skin, const char *wgt, const char *res, unsigned int def) {
    const char *valstr;
    unsigned int result;
    
    assert(skin);
    assert(skin->config);
    
    //get the config value    
    valstr = al_get_config_value(skin->config, wgt, res);
    
    //convert the string to unsigned integer; if the conversion fails, return the default
    if (!_string_to_uint(valstr, &result)) return def;    
    
    //success
    return result;
} 


/** returns a double value from a skin.
    @param skin skin to get the value from.
    @param wgt widget name (UTF-8 string).
    @param res resource name (UTF-8 string).
    @param def default value.
    @return the value from the config or the default if not found.
 */
double algui_get_skin_double(ALGUI_SKIN *skin, const char *wgt, const char *res, double def) {
    const char *valstr;
    double result;
    
    assert(skin);
    assert(skin->config);
    
    //get the config value    
    valstr = al_get_config_value(skin->config, wgt, res);
    
    //convert the string to double; if the conversion fails, return the default
    if (!_string_to_double(valstr, &result)) return def;    
    
    //success
    return result;
}


/** returns a string from a skin.
    The string must be freed by the caller.
    @param skin skin to get the value from.
    @param wgt widget name (UTF-8 string).
    @param res resource name (UTF-8 string).
    @param def default value (UTF-8 string).
    @return the value from the config file or the default if not found.
 */
const char *algui_get_skin_str(ALGUI_SKIN *skin, const char *wgt, const char *res, const char *def) {
    const char *valstr;
    
    assert(skin);
    assert(skin->config);
    
    //get the config value    
    valstr = al_get_config_value(skin->config, wgt, res);

    //return the config value or the default if the config value is null    
    return valstr ? valstr : def;
}


/** returns a color from a skin.
    @param skin skin to get the color from.
    @param wgt widget name (UTF-8 string).
    @param res resource name (UTF-8 string).
    @param def default value.
    @return the value from the config or the default if not found.
 */
ALLEGRO_COLOR algui_get_skin_color(ALGUI_SKIN *skin, const char *wgt, const char *res, ALLEGRO_COLOR def) {
    const char *valstr;    
    ALLEGRO_COLOR color;
    
    assert(skin);
    assert(skin->config);
    
    //get the config value
    valstr = al_get_config_value(skin->config, wgt, res);
    
    //convert the string to a color; if the conversion fails, return the default
    if (!_string_to_color(valstr, &color)) return def;
    
    //success
    return color;
}


/** loads a bitmap from a skin.
    The bitmap is managed via the resource manager.
    @param skin skin.
    @param wgt widget name (UTF-8 string).
    @param res resource name (UTF-8 string).
    @param def default resource.
    @return the loaded resource or the default resource if the resource is not found.
 */
ALLEGRO_BITMAP *algui_get_skin_bitmap(ALGUI_SKIN *skin, const char *wgt, const char *res, ALLEGRO_BITMAP *def) {
    const char *valstr;
    
    assert(skin);
    assert(skin->config);
    
    //get the config value
    valstr = al_get_config_value(skin->config, wgt, res);

    //load the bitmap    
    return _load_bitmap(skin, valstr);
}


/** loads a font from a skin.
    The font is managed via the resource manager.
    @param skin skin.
    @param wgt widget name (UTF-8 string).
    @param res resource name (UTF-8 string).
    @param def default resource.
    @param def_size def size of font, in case the size of the font is not found.
    @param def_flags def size of font, in case the flags of the font are not found.
    @return the loaded resource or the default resource if the resource is not found.
 */
ALLEGRO_FONT *algui_get_skin_font(ALGUI_SKIN *skin, const char *wgt, const char *res, ALLEGRO_FONT *def, unsigned int def_size, unsigned int def_flags) {
    const char *valstr;
    unsigned int size, flags;
    ALLEGRO_USTR *filename;
    ALLEGRO_FONT *font;
    
    assert(skin);
    assert(skin->config);
    
    //get the config value
    valstr = al_get_config_value(skin->config, wgt, res);
    
    //the filename string
    filename = al_ustr_new("");
    
    //convert the string to font; if the conversion fails, return the default
    if (!_string_to_font(valstr, def_size, def_flags, filename, &size, &flags)) {
        al_ustr_free(filename);
        return def;
    }        
    
    //load the font
    font = _load_font(skin, al_cstr(filename), size, flags);
    
    //free the filename string
    al_ustr_free(filename);
    
    //if the font was loaded successfuly, return it, otherwise return the default
    return font ? font : def;
}



/** initializes a skin structure.
    Creates a new empty allegro config for the skin structure.
    @param skin skin structure to initialize.
 */
void algui_init_skin(ALGUI_SKIN *skin) {
    assert(skin);
    skin->filename = al_ustr_new("");
    skin->config = al_create_config();
}


/** cleans up a skin structure.
    Destroys the allegro config.
    @param skin skin structure to cleanup.
 */
void algui_cleanup_skin(ALGUI_SKIN *skin) {
    assert(skin);
    al_ustr_free(skin->filename);
    al_destroy_config(skin->config);
    skin->filename = NULL;
    skin->config = NULL;
}


/** creates a skin structure with an empty allegro config.
 */
ALGUI_SKIN *algui_create_skin() {
    ALGUI_SKIN *skin = (ALGUI_SKIN *)al_malloc(sizeof(ALGUI_SKIN));
    assert(skin);
    algui_init_skin(skin);
    return skin;
}


/** destroys a skin structure and its allegro config.
 */
void algui_destroy_skin(ALGUI_SKIN *skin) {
    assert(skin);
    algui_cleanup_skin(skin);
    al_free(skin);
}


/** loads a skin from disk.
    @param skin skin to load.
    @param filename filename of the skin's config file (UTF-8 string).
    @return the skin or NULL if the skin could not be loaded.
 */
ALGUI_SKIN *algui_load_skin(const char *filename) {
    ALLEGRO_CONFIG *config;
    ALGUI_SKIN *skin;
    
    assert(filename);
    
    //load config
    config = al_load_config_file(filename);
    if (!config) return NULL;
    
    //create a skin
    skin = (ALGUI_SKIN *)al_malloc(sizeof(ALGUI_SKIN));
    assert(skin);
    
    //copy the filename
    skin->filename = al_ustr_new(filename);
    
    //set the config
    skin->config = config;
    
    return skin;
}


/** saves a skin to disk.
    @param skin skin to save.
    @param filename filename (UTF-8 string).
    @return non-zero on success, zero on failure.
 */
int algui_save_skin(ALGUI_SKIN *skin, const char *filename) {
    ALLEGRO_USTR *new_filename;

    assert(skin);
    assert(skin->config);
    assert(filename);
    
    //save the config
    if (!al_save_config_file(filename, skin->config)) return 0;
    
    //copy the new filename
    new_filename = al_ustr_new(filename);
    
    //free the previous filename (which might be the given filename,
    //and therefore we duplicate it before freeing the current filename)
    al_ustr_free(skin->filename);
    
    //set the new filename
    skin->filename = new_filename;
 
    //success   
    return 1;
}


/** adds or sets an integer value of a skin.
    @param skin skin to set the value of.
    @param wgt widget name (UTF-8 string).
    @param res resource name (UTF-8 string).
    @param val resource value.
    @return non-zero on success, zero on failure.
 */
int algui_set_skin_int(ALGUI_SKIN *skin, const char *wgt, const char *res, int val) {
    ALLEGRO_USTR *temp;
    int result;
    
    //use printf style to add the value
    temp = al_ustr_newf("%i", val);
    
    //set the string
    result = algui_set_skin_str(skin, wgt, res, al_cstr(temp));
    
    //free the temp string
    al_ustr_free(temp);
    
    return result;
}


/** adds or sets an unsigned integer value of a skin.
    @param skin skin to set the value of.
    @param wgt widget name.
    @param res resource name (UTF-8 string).
    @param val resource value (UTF-8 string).
    @return non-zero on success, zero on failure.
 */
int algui_set_skin_uint(ALGUI_SKIN *skin, const char *wgt, const char *res, unsigned int val) {
    ALLEGRO_USTR *temp;
    int result;
    
    //use printf style to add the value
    temp = al_ustr_newf("%ui", val);
    
    //set the string
    result = algui_set_skin_str(skin, wgt, res, al_cstr(temp));
    
    //free the temp string
    al_ustr_free(temp);
    
    return result;
} 


/** adds or sets a double value of a skin.
    @param skin skin to set the value of.
    @param wgt widget name (UTF-8 string).
    @param res resource name (UTF-8 string).
    @param val resource value.
    @return non-zero on success, zero on failure.
 */
int algui_set_skin_double(ALGUI_SKIN *skin, const char *wgt, const char *res, double val) {
    ALLEGRO_USTR *temp;
    int result;
    
    //use printf style to add the value
    temp = al_ustr_newf("%g", val);
    
    //set the string
    result = algui_set_skin_str(skin, wgt, res, al_cstr(temp));
    
    //free the temp string
    al_ustr_free(temp);
    
    return result;
}  


/** adds or sets a string of a skin.
    @param skin skin to set the value of.
    @param wgt widget name (UTF-8 string).
    @param res resource name (UTF-8 string).
    @param val resource value (UTF-8 string).
    @return non-zero on success, zero on failure.
 */
int algui_set_skin_str(ALGUI_SKIN *skin, const char *wgt, const char *res, const char *val) {
    assert(skin);
    assert(skin->config);
    al_set_config_value(skin->config, wgt, res, val);
    return 1;
}


/** adds or sets a color of a skin.
    @param skin skin to set the color of.
    @param wgt widget name (UTF-8 string).
    @param res resource name (UTF-8 string).
    @param val resource value.
    @return non-zero on success, zero on failure.
 */
int algui_set_skin_color(ALGUI_SKIN *skin, const char *wgt, const char *res, ALLEGRO_COLOR color) {
    ALLEGRO_USTR *temp;
    unsigned char red, green, blue, alpha;
    int result;
    
    //convert the color to a string
    al_unmap_rgba(color, &red, &green, &blue, &alpha);
    temp = al_ustr_newf("%ui, %ui, %ui, %ui", (unsigned int)red, (unsigned int)green, (unsigned int)blue, (unsigned int)alpha);
    
    //set the skin
    result = algui_set_skin_str(skin, wgt, res, al_cstr(temp));
    
    //free the temp string
    al_ustr_free(temp);
    
    return result;
}


/** adds or sets a bitmap of a skin.
    The bitmap must be placed manually in the skin's folder.
    @param skin skin to set the bitmap of.
    @param wgt widget name (UTF-8 string).
    @param res resource name (UTF-8 string).
    @param filename the filename of the bitmap (UTF-8 string).
    @return non-zero on success, zero on failure.
 */
int algui_set_skin_bitmap(ALGUI_SKIN *skin, const char *wgt, const char *res, const char *filename) {
    return algui_set_skin_str(skin, wgt, res, filename);
}


/** adds or sets a font of a skin.
    The font must be placed manually in the skin's folder.
    The font is not saved in 
    @param skin skin to set the bitmap of.
    @param wgt widget name (UTF-8 string).
    @param res resource name (UTF-8 string).
    @param filename the filename of the font (UTF-8 string).
    @param size the size of the font.
    @param flags font flags, as in al_load_font.
    @return non-zero on success, zero on failure.
 */
int algui_set_skin_font(ALGUI_SKIN *skin, const char *wgt, const char *res, const char *filename, unsigned int size, unsigned int flags) {
    ALLEGRO_USTR *temp;
    int result;
    
    //convert the font to a string
    temp = al_ustr_newf("%s, %ui, %ui", filename, size, flags);

    //set the string    
    result = algui_set_skin_str(skin, wgt, res, al_cstr(temp));
    
    //free the temp string
    al_ustr_free(temp);
    
    return result;
}
