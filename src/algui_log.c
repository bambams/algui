#include "algui_log.h"
#include <stdarg.h>
#include <allegro5/allegro.h>


/******************************************************************************
    INTERNAL VARIABLES
 ******************************************************************************/
 
 
//sync mutex
static ALLEGRO_MUTEX *_mutex = NULL; 
 
 
//allegro file 
static ALLEGRO_FILE *_log_file = NULL;


//inits the log; invoked from algui_init.
int _algui_init_log() {    
    _mutex = al_create_mutex();
    if (!_mutex) return 0;
    return 1;
}


//cleans up the log; invoked from algui_cleanup.
void _algui_cleanup_log() {    
    if (_log_file) al_fclose(_log_file);
    if (_mutex) al_destroy_mutex(_mutex);
}


/******************************************************************************
    PUBLIC FUNCTIONS
 ******************************************************************************/  


/** logging function.
    It writes the output in a file algui.log.
    @param format output format string.
    @param ... parameters.
    @return non-zero if the operation succeeded, zero otherwise.
 */
int algui_log(const char *format, ...) {
    va_list args;    
    ALLEGRO_USTR *str;
    const char *cstr;
    int r;
    
    //lock
    al_lock_mutex(_mutex);
    
    //open file if needed
    if (!_log_file) {
        _log_file = al_fopen("algui.log", "wt");
        if (!_log_file) {
            al_unlock_mutex(_mutex);
            return 0;
        }
    }
    
    //begin varargs
    va_start(args, format);
    
    //create a utf-8 string
    str = al_ustr_new("");
    
    //append the varargs
    r = al_ustr_vappendf(str, format, args);
    
    //write them to the file
    cstr = al_cstr(str);
    r &= al_fputs(_log_file, cstr);
    r &= al_fflush(_log_file);
    
    //delete the utf-8 string
    al_ustr_free(str);
    
    //end varargs
    va_end(args);
    
    //unlock
    al_unlock_mutex(_mutex);
    
    return r;
}
