#include "algui_display.h"
#include <allegro5/allegro_primitives.h>


/******************************************************************************
    INTERNAL MACROS
 ******************************************************************************/
 

//default background color 
#define _DEFAULT_BACKGROUND_COLOR      al_map_rgb(128, 128, 128)


/******************************************************************************
    INTERNAL MESSAGE HANDLERS
 ******************************************************************************/
 
 
//cleanup message
static int _msg_cleanup(ALGUI_DISPLAY *wgt, ALGUI_CLEANUP_MESSAGE *msg) {
    algui_release_resource(wgt->background_bitmap);
    return algui_widget_proc(&wgt->widget, &msg->message);
} 
 
 
//paint message 
static int _msg_paint(ALGUI_DISPLAY *wgt, ALGUI_PAINT_MESSAGE *msg) {
    //draw bitmap
    if (wgt->background_bitmap) {
        al_draw_scaled_bitmap(
            wgt->background_bitmap, 
            0,
            0,
            al_get_bitmap_width(wgt->background_bitmap),
            al_get_bitmap_height(wgt->background_bitmap),
            msg->widget_rect.left, 
            msg->widget_rect.top,
            algui_get_rect_width(&msg->widget_rect),
            algui_get_rect_height(&msg->widget_rect),
            0);
    }
    
    //else fill a rectangle
    else {
        al_draw_filled_rectangle(
            msg->widget_rect.left + 0.5f,
            msg->widget_rect.top + 0.5f,
            msg->widget_rect.right + 1.0f,
            msg->widget_rect.bottom + 1.0f,
            wgt->background_color);
    }
    
    return 1;
}
 
 
//set skin message
static int _msg_set_skin(ALGUI_DISPLAY *wgt, ALGUI_SET_SKIN_MESSAGE *msg) {
    //release resources
    algui_release_resource(wgt->background_bitmap);
    
    //acquire resources
    wgt->background_color = algui_get_skin_color(msg->skin, algui_get_widget_id(&wgt->widget), "background_color", _DEFAULT_BACKGROUND_COLOR);
    wgt->background_bitmap = algui_get_skin_bitmap(msg->skin, algui_get_widget_id(&wgt->widget), "background_bitmap", NULL);
    
    return 1;
}
 
 
/******************************************************************************
    PUBLIC FUNCTIONS
 ******************************************************************************/
  
  
/** the display widget message procedure.
    @param wgt target widget.
    @param msg message.
    @return non-zero if the message was processed, zero otherwise.
 */
int algui_display_proc(ALGUI_WIDGET *wgt, ALGUI_MESSAGE *msg) {
    //handle message
    switch (msg->id) {
        //cleanup
        case ALGUI_MSG_CLEANUP:
            return _msg_cleanup((ALGUI_DISPLAY *)wgt, (ALGUI_CLEANUP_MESSAGE *)msg);
    
        //paint
        case ALGUI_MSG_PAINT:
            return _msg_paint((ALGUI_DISPLAY *)wgt, (ALGUI_PAINT_MESSAGE *)msg);
            
        //skin
        case ALGUI_MSG_SET_SKIN:            
            return _msg_set_skin((ALGUI_DISPLAY *)wgt, (ALGUI_SET_SKIN_MESSAGE *)msg);
    }

    //default handling
    return algui_widget_proc(wgt, msg);
}


/** returns the widget from a display.
    @param dis display to get the widget of.
    @return the widget of the display.
 */
ALGUI_WIDGET *algui_get_display_widget(ALGUI_DISPLAY *dis) {
    assert(dis);
    return &dis->widget;
}


/** returns the background color of a display.
    @param dis display to get the background color of.
    @return the background color of a display.
 */
ALLEGRO_COLOR algui_get_display_background_color(ALGUI_DISPLAY *dis) {
    assert(dis);
    return dis->background_color;
}


/** returns the background bitmap of a display.
    @param dis display to get the background bitmap of.
    @return the background bitmap of a display.
 */
ALLEGRO_BITMAP *algui_get_display_background_bitmap(ALGUI_DISPLAY *dis) {
    assert(dis);
    return dis->background_bitmap;
}


/** initializes a display widget.
    @param dis display widget to initialize.    
 */
void algui_init_display(ALGUI_DISPLAY *dis) {
    assert(dis);
    algui_init_widget(&dis->widget, algui_display_proc, "display");
    dis->background_color = _DEFAULT_BACKGROUND_COLOR;
    dis->background_bitmap = NULL;
}


/** creates a display widget.
    @return display widget.
 */
ALGUI_DISPLAY *algui_create_display() {
    ALGUI_DISPLAY *dis = (ALGUI_DISPLAY *)al_malloc(sizeof(ALGUI_DISPLAY));
    algui_init_display(dis);
    return dis;
}


/** sets the display's background color.
    @param dis display to set.
    @param bg background color.
 */
void algui_set_display_background_color(ALGUI_DISPLAY *dis, ALLEGRO_COLOR bg) {
    assert(dis);
    dis->background_color = bg;
}


/** sets the display's background bitmap.
    @param dis display to set.
    @param bmp background bitmap.
 */
void algui_set_display_background_bitmap(ALGUI_DISPLAY *dis, ALLEGRO_BITMAP *bmp) {
    assert(dis);
    dis->background_bitmap = bmp;
}
