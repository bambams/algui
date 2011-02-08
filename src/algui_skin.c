#include "algui_skin.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>


/******************************************************************************
    INTERNAL MACROS
 ******************************************************************************/
 
 
/******************************************************************************
    INTERNAL TYPES
 ******************************************************************************/
 
 
//conversion buffer
typedef char _BUFFER[4096]; 


//string position
typedef struct _STRPOS {
    const char *begin;
    const char *end;
} _STRPOS;
 
 
/******************************************************************************
    INTERNAL FUNCTIONS
 ******************************************************************************/
 
 
//skips whitespace in a string
static const char *_skip_whitespace(const char *str) {
    while (isspace(*str)) ++str;
    return str;
} 
 
 
//skips whitespace backwards
static const char *_skip_whitespace_reverse(const char *str) {
    while (isspace(*(str - 1))) --str;
    return str;
} 
 
 
//sets up a string pos entry, consuming whitespace in front and back of the string
static void _setup_strpos(_STRPOS *pos, const char *begin, const char *end) {
    pos->begin = _skip_whitespace(begin);
    pos->end = _skip_whitespace_reverse(end);
} 
 
 
//split a string by character
unsigned long _split_string(const char *str, char c, _STRPOS pos[], unsigned long pos_count) {
    //position in 'pos' array and final count of string positions
    unsigned long pos_index = 0;
    
    //next position in string
    char *next_str;

    //iterate the string while there are enough positions in the array
    while(*str && pos_index < pos_count) {
        //find character
        next_str = strchr(str, c);
        
        //character not found; setup the final entry
        if (!next_str) {            
            _setup_strpos(&pos[pos_index], str, strchr(str, '\0'));
            ++pos_index;
            break;
        }
        
        //setup the entry
        _setup_strpos(&pos[pos_index], str, next_str);
        ++pos_index;
        
        //skip the character
        str = next_str + 1;
    }

    //return number of entries    
    return pos_index;
}


//parse uint from a string position
static unsigned int _parse_uint(const char *pos, unsigned int v) {
    unsigned int r;
    char *endstr;    
    r = strtoul(pos, &endstr, 10);
    if (errno == ERANGE || endstr == pos) return v;
    return r;
}
 
 
//parse uchar from a string position
static unsigned char _parse_uchar(const char *pos, unsigned char v) {
    unsigned int r = _parse_uint(pos, v);
    return r <= 255 ? (unsigned char)r : v;
}
 
 
//converts a string to a color
static int _string_to_color(const char *str, ALLEGRO_COLOR *color) {
    _STRPOS pos[4];
    unsigned int count;
    unsigned char r = 0, g = 0, b = 0, a = 255;
    
    //split the string
    count = _split_string(str, ',', pos, 4);
    
    //check for invalid result; at least the RGB values must be specified
    if (count < 3 || count > 4) return 0;
    
    //setup the rgb values
    r = _parse_uchar(pos[0].begin, r);
    g = _parse_uchar(pos[1].begin, g);
    b = _parse_uchar(pos[2].begin, b);
    
    //setup the alpha value
    if (count == 4) {
        a = _parse_uchar(pos[3].begin, a);
    }
    
    //setup the color
    *color = al_map_rgba(r, g, b, a);
    
    //success
    return 1;
}


//converts a color to a string
static void _color_to_string(ALLEGRO_COLOR *color, char *str) {
    unsigned char r, g, b, a;
    al_unmap_rgba(*color, &r, &g, &b, &a);
    sprintf(str, "%ui,%ui,%ui,%ui", (unsigned int)r, (unsigned int)g, (unsigned int)b, (unsigned int)a);
}


//converts a font description to font attributes
static int _string_to_font(
    const char *str, 
    unsigned int def_size, 
    unsigned int def_flags, 
    char *filename, 
    unsigned int filename_size, 
    unsigned int *size, 
    unsigned int *flags)
{
    _STRPOS pos[3];
    unsigned int count;
    unsigned int len;

    //split the string
    count = _split_string(str, ',', pos, 3);
    
    //check for invalid result; 
    if (count < 1 || count > 3) return 0;
    
    //check the filename length
    len = pos[0].end - pos[0].begin;
    if (len >= filename_size - 1) return 0;
    
    //setup the filename
    memcpy(filename, pos[0].begin, len);
    filename[len] = '\0';
    
    //setup the size
    *size = def_size;    
    if (count >= 2) {
        *size = _parse_uint(pos[1].begin, def_size);
    }
    
    //setup the flags
    *flags = def_flags; 
    if (count == 3) {
        *flags = _parse_uint(pos[2].begin, def_flags);
    }
    
    //success
    return 1;
}


//converts font attributes to string
static void _font_to_string(const char *filename, unsigned int size, unsigned int flags, char *str) {
    sprintf(str, "%s,%ui,%ui", filename, size, flags);
}


//fix a path by replacing '\' to '/'
static int _fix_path(const char *filename, char *path, unsigned int path_size) {
    unsigned int pos = 0;
    
    while(filename[pos]) {
        //failure because the filename's length is greater than the path buffer
        if (pos == path_size - 1) 
            return 0;
        
        //if the character is not '\', copy it, else replace it
        if (filename[pos] != '\\') path[pos] = filename[pos]; else path[pos] = '/';
        
        //next position
        ++pos;
    }
    
    //setup the null terminating character
    path[pos] = '\0';
    
    //success
    return 1;
}


//returns the path of a file
static int _get_path(const char *filename, char *path, unsigned int path_size, unsigned int *len) {
    const char *filename_end, *path_end;
    _BUFFER buf;
    
    //convert '\' to '/'
    if (!_fix_path(filename, buf, sizeof(buf))) return 0;
    
    //find the end of the filename string
    filename_end = strchr(buf, '\0');
    
    //find the first '/' from the end
    path_end = strrchr(buf, '/');
    
    //find the path length, including '/'
    *len = path_end - buf + 1;
    
    //invalid len
    if (*len <= 2 || *len > path_size - 1) return 0;
    
    //copy the path
    memcpy(path, buf, *len);
    path[*len] = '\0';
    
    //success
    return 1;
}


//get the full filename of a resource
static int _get_full_filename(
    const char *skin_filename, 
    const char *filename, 
    char *full_filename, 
    unsigned int full_filename_size)
{
    _BUFFER skin_path;
    unsigned int skin_path_len, filename_len;
    
    //get path
    if (!_get_path(skin_filename, skin_path, sizeof(skin_path), &skin_path_len)) return 0;
    
    //compute the filename's length
    filename_len = strlen(filename);
    
    //if the buffer does not have enough space for the path and filename,
    //return nothing
    if (skin_path_len + filename_len >= full_filename_size - 1) return 0;
    
    //create the bitmap's path
    sprintf(full_filename, "%s%s\0", skin_path, filename);
    
    //success
    return 1;
}


//loads a bitmap from a skin directory
static ALLEGRO_BITMAP *_load_bitmap(ALGUI_SKIN *skin, const char *filename) {
    _BUFFER buf;
    if (!_get_full_filename(skin->filename, filename, buf, sizeof(buf))) return NULL;
    return al_load_bitmap(buf);
}
 
 
//loads a font from a skin directory
static ALLEGRO_FONT *_load_font(ALGUI_SKIN *skin, const char *filename, unsigned int size, unsigned int flags) {
    _BUFFER buf;
    if (!_get_full_filename(skin->filename, filename, buf, sizeof(buf))) return NULL;
    return al_load_font(buf, size, flags);
}
 
 
/******************************************************************************
    PUBLIC FUNCTIONS
 ******************************************************************************/
 
 
/** returns the current filename of a skin.    
    @param skin skin to get the filename of.
    @return the filename of a skin; a pointer to the internal character buffer.
 */
const char *algui_get_skin_filename(ALGUI_SKIN *skin) {
    assert(skin);
    return skin->filename;
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
    @param wgt widget name.
    @param res resource name.
    @param def default value.
    @return the value from the config or the default if not found.
 */
int algui_get_skin_int(ALGUI_SKIN *skin, const char *wgt, const char *res, int def) {
    const char *valstr;
    assert(skin);
    assert(skin->config);
    valstr = al_get_config_value(skin->config, wgt, res);
    if (!valstr) return def;
    return atoi(valstr);
}


/** returns an unsigned integer value from a skin.
    @param skin skin to get the value from.
    @param wgt widget name.
    @param res resource name.
    @param def default value.
    @return the value from the config or the default if not found.
 */
unsigned int algui_get_skin_uint(ALGUI_SKIN *skin, const char *wgt, const char *res, unsigned int def) {
    const char *valstr;
    assert(skin);
    assert(skin->config);
    valstr = al_get_config_value(skin->config, wgt, res);
    if (!valstr) return def;
    return (unsigned int)atof(valstr);
} 


/** returns a double value from a skin.
    @param skin skin to get the value from.
    @param wgt widget name.
    @param res resource name.
    @param def default value.
    @return the value from the config or the default if not found.
 */
double algui_get_skin_double(ALGUI_SKIN *skin, const char *wgt, const char *res, double def) {
    const char *valstr;
    assert(skin);
    assert(skin->config);
    valstr = al_get_config_value(skin->config, wgt, res);
    if (!valstr) return def;
    return atof(valstr);
}


/** returns a string from a skin.
    The string must be freed by the caller.
    @param skin skin to get the value from.
    @param wgt widget name.
    @param res resource name.
    @param def default value.
    @return the value from the config or the default if not found. The pointer returned must not be freed.
 */
const char *algui_get_skin_str(ALGUI_SKIN *skin, const char *wgt, const char *res, const char *def) {
    const char *valstr;
    assert(skin);
    assert(skin->config);
    valstr = al_get_config_value(skin->config, wgt, res);
    if (!valstr) return def;
    return valstr;
}


/** returns a color from a skin.
    @param skin skin to get the color from.
    @param wgt widget name.
    @param res resource name.
    @param def default value.
    @return the value from the config or the default if not found.
 */
ALLEGRO_COLOR algui_get_skin_color(ALGUI_SKIN *skin, const char *wgt, const char *res, ALLEGRO_COLOR def) {
    const char *valstr;
    ALLEGRO_COLOR color;
    int ok;
    assert(skin);
    assert(skin->config);
    valstr = al_get_config_value(skin->config, wgt, res);
    if (!valstr) return def;
    ok = _string_to_color(valstr, &color);
    return ok ? color : def;
}


/** loads a bitmap from a skin.
    @param skin skin.
    @param wgt widget name.
    @param res resource name.
    @param def default resource.
    @return the loaded resource or the default resource if the resource is not found.
 */
ALLEGRO_BITMAP *algui_get_skin_bitmap(ALGUI_SKIN *skin, const char *wgt, const char *res, ALLEGRO_BITMAP *def) {
    const char *valstr;
    assert(skin);
    assert(skin->config);
    valstr = al_get_config_value(skin->config, wgt, res);
    if (!valstr) return def;
    return _load_bitmap(skin, valstr);
}


/** loads a font from a skin.
    @param skin skin.
    @param wgt widget name.
    @param res resource name.
    @param def default resource.
    @param def_size def size of font, in case the size of the font is not found.
    @param def_flags def size of font, in case the flags of the font are not found.
    @return the loaded resource or the default resource if the resource is not found.
 */
ALLEGRO_FONT *algui_get_skin_font(ALGUI_SKIN *skin, const char *wgt, const char *res, ALLEGRO_FONT *def, unsigned int def_size, unsigned int def_flags) {
    const char *valstr;
    _BUFFER buf;
    unsigned int size, flags;
    int ok;
    assert(skin);
    assert(skin->config);
    valstr = al_get_config_value(skin->config, wgt, res);
    if (!valstr) return def;
    ok = _string_to_font(valstr, def_size, def_flags, buf, sizeof(buf), &size, &flags);
    return ok ? _load_font(skin, buf, size, flags) : def;
}



/** initializes a skin structure.
    Creates a new empty allegro config for the skin structure.
    @param skin skin structure to initialize.
 */
void algui_init_skin(ALGUI_SKIN *skin) {
    assert(skin);
    skin->filename = strdup("");
    skin->config = al_create_config();
}


/** cleans up a skin structure.
    Destroys the allegro config.
    @param skin skin structure to cleanup.
 */
void algui_cleanup_skin(ALGUI_SKIN *skin) {
    assert(skin);
    free(skin->filename);
    skin->filename = 0;
    al_destroy_config(skin->config);
    skin->config = 0;
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
    @param filename filename of the skin's config file.
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
    skin->filename = strdup(filename);
    
    //set the config
    skin->config = config;
    
    return skin;
}


/** saves a skin to disk.
    @param skin skin to save.
    @param filename filename.
    @return non-zero on success, zero on failure.
 */
int algui_save_skin(ALGUI_SKIN *skin, const char *filename) {
    assert(skin);
    assert(skin->config);
    assert(filename);
    
    //save the config
    if (!al_save_config_file(filename, skin->config)) return 0;
    
    //check if the filename needs to be set
    if (stricmp(filename, skin->filename) == 0) return 1;
    
    //set new filename
    free(skin->filename);
    skin->filename = strdup(filename);
    
    return 1;
}


/** adds or sets an integer value of a skin.
    @param skin skin to set the value of.
    @param wgt widget name.
    @param res resource name.
    @param val resource value.
    @return non-zero on success, zero on failure.
 */
int algui_set_skin_int(ALGUI_SKIN *skin, const char *wgt, const char *res, int val) {
    _BUFFER buf;
    sprintf(buf, "%i", val);
    return algui_set_skin_str(skin, wgt, res, buf);
}


/** adds or sets an unsigned integer value of a skin.
    @param skin skin to set the value of.
    @param wgt widget name.
    @param res resource name.
    @param val resource value.
    @return non-zero on success, zero on failure.
 */
int algui_set_skin_uint(ALGUI_SKIN *skin, const char *wgt, const char *res, unsigned int val) {
    _BUFFER buf;
    sprintf(buf, "%ui", val);
    return algui_set_skin_str(skin, wgt, res, buf);
} 


/** adds or sets a double value of a skin.
    @param skin skin to set the value of.
    @param wgt widget name.
    @param res resource name.
    @param val resource value.
    @return non-zero on success, zero on failure.
 */
int algui_set_skin_double(ALGUI_SKIN *skin, const char *wgt, const char *res, double val) {
    _BUFFER buf;
    sprintf(buf, "%g", val);
    return algui_set_skin_str(skin, wgt, res, buf);
}  


/** adds or sets a string of a skin.
    @param skin skin to set the value of.
    @param wgt widget name.
    @param res resource name.
    @param val resource value.
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
    @param wgt widget name.
    @param res resource name.
    @param val resource value.
    @return non-zero on success, zero on failure.
 */
int algui_set_skin_color(ALGUI_SKIN *skin, const char *wgt, const char *res, ALLEGRO_COLOR color) {
    _BUFFER buf;
    _color_to_string(&color, buf);
    return algui_set_skin_str(skin, wgt, res, buf);
}


/** adds or sets a bitmap of a skin.
    The bitmap must be placed manually in the skin's folder.
    @param skin skin to set the bitmap of.
    @param wgt widget name.
    @param res resource name.
    @param filename the filename of the bitmap.
    @return non-zero on success, zero on failure.
 */
int algui_set_skin_bitmap(ALGUI_SKIN *skin, const char *wgt, const char *res, const char *filename) {
    return algui_set_skin_str(skin, wgt, res, filename);
}


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
int algui_set_skin_font(ALGUI_SKIN *skin, const char *wgt, const char *res, const char *filename, unsigned int size, unsigned int flags) {
    _BUFFER buf;
    _font_to_string(filename, size, flags, buf);
    return algui_set_skin_str(skin, wgt, res, buf);
}
