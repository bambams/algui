#ifndef ALGUI_H
#define ALGUI_H


#include "algui_log.h"
#include "algui_display.h"


/** initializes the library.
    Allegro must be initialized before this call.
    @return non-zero if initialization suceeded, zero otherwise.
 */
int algui_init();


/** cleans up the library.
    If not invoked at the end of main, it is invoked automatically at exit.
    It destroys any resources in the resource manager.
    It must be invoked before the allegro cleanup functions.
 */ 
void algui_cleanup(void); 


#endif //ALGUI_H
