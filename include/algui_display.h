#ifndef ALGUI_DISPLAY_H
#define ALGUI_DISPLAY_H


#include "algui_widget.h"


/** A top level widget that covers the whole display.
 */
typedef struct ALGUI_DISPLAY {
    ALGUI_WIDGET widget;
    ALLEGRO_COLOR background_color;
    ALLEGRO_BITMAP *background_bitmap;
} ALGUI_DISPLAY;


/** the display widget message procedure.
    @param wgt target widget.
    @param msg message.
    @return non-zero if the message was processed, zero otherwise.
 */
int algui_display_proc(ALGUI_WIDGET *wgt, ALGUI_MESSAGE *msg); 


/** returns the widget from a display.
    @param dis display to get the widget of.
    @return the widget of the display.
 */
ALGUI_WIDGET *algui_get_display_widget(ALGUI_DISPLAY *dis); 


/** returns the background color of a display.
    @param dis display to get the background color of.
    @return the background color of a display.
 */
ALLEGRO_COLOR algui_get_display_background_color(ALGUI_DISPLAY *dis); 


/** returns the background bitmap of a display.
    @param dis display to get the background bitmap of.
    @return the background bitmap of a display.
 */
ALLEGRO_BITMAP *algui_get_display_background_bitmap(ALGUI_DISPLAY *dis); 


/** initializes a display widget.
    @param dis display widget to initialize.    
 */
void algui_init_display(ALGUI_DISPLAY *dis); 


/** creates a display widget.
    @return display widget.
 */
ALGUI_DISPLAY *algui_create_display(); 


/** sets the display's background color.
    @param dis display to set.
    @param bg background color.
 */
void algui_set_display_background_color(ALGUI_DISPLAY *dis, ALLEGRO_COLOR bg);


/** sets the display's background bitmap.
    @param dis display to set.
    @param bmp background bitmap.
 */
void algui_set_display_background_bitmap(ALGUI_DISPLAY *dis, ALLEGRO_BITMAP *bmp); 


#endif //ALGUI_DISPLAY_H
