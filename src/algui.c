#include "algui.h"


/******************************************************************************
    INTERNAL VARIABLES
 ******************************************************************************/
 
 
//init flag
static int _init_flag = 0;


//cleanup flag
static int _cleanup_flag = 0; 
 
 
/******************************************************************************
    INTERNAL FUNCTIONS
 ******************************************************************************/
 
 
extern int _algui_init_log(); 
extern void _algui_cleanup_log(); 
extern int _algui_init_resource_manager(); 
extern void _algui_cleanup_resource_manager(); 
 
 
/******************************************************************************
    PUBLIC FUNCTIONS
 ******************************************************************************/
 
 
/** initializes the library.
    Allegro must be initialized before this call.
    @return non-zero if initialization suceeded, zero otherwise.
 */
int algui_init() {
    if (_init_flag) return 1;
    if (!_algui_init_log()) return 0;
    if (!_algui_init_resource_manager()) return 0;
    atexit(algui_cleanup);
    _init_flag = 1;
    return 1;
}


/** cleans up the library.
    If not invoked at the end of main, it is invoked automatically at exit.
    It destroys any resources in the resource manager.
    It must be invoked before the allegro cleanup functions.
 */ 
void algui_cleanup(void) {
    if (_cleanup_flag) return;
    _algui_cleanup_log();
    _algui_cleanup_resource_manager();    
    _cleanup_flag = 1;
}
