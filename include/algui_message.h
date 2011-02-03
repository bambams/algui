#ifndef ALGUI_MESSAGE_H
#define ALGUI_MESSAGE_H


#include "algui_rect.h"


/** widget messages enumeration.
 */
typedef enum ALGUI_MSG_ID {
    ///cleanup widget.
    ALGUI_MSG_CLEANUP = 1,
    
    ///paint widget.
    ALGUI_MSG_PAINT,
    
    ///insert widget.
    ALGUI_MSG_INSERT_WIDGET,
    
    ///remove widget.
    ALGUI_MSG_REMOVE_WIDGET,
    
    ///set widget rectangle.
    ALGUI_MSG_SET_RECT,
    
    ///set widget visible state.
    ALGUI_MSG_SET_VISIBLE,
    
    ///set widget enabled state.
    ALGUI_MSG_SET_ENABLED,
    
    ///ask widget if it wants the input focus.
    ALGUI_MSG_GET_FOCUS,
    
    ///ask widget if it can lose the input focus.
    ALGUI_MSG_LOSE_FOCUS,
    
    ///widget got the input focus.
    ALGUI_MSG_GOT_FOCUS,
    
    ///widget lost the input focus.
    ALGUI_MSG_LOST_FOCUS,
    
    ///1st user message
    ALGUI_MSG_USER = 0x10000
} ALGUI_MSG_ID; 


/** message base struct.
 */
typedef struct ALGUI_MESSAGE {
    //message id
    int id;
} ALGUI_MESSAGE; 


/** the cleanup message.
 */
typedef struct ALGUI_CLEANUP_MESSAGE {
    ///base message.
    ALGUI_MESSAGE message;
} ALGUI_CLEANUP_MESSAGE;


/** the paint message.
 */
typedef struct ALGUI_PAINT_MESSAGE {
    ///base message.
    ALGUI_MESSAGE message;
    
    ///widget rectangle to paint, relative to the screen.
    ALGUI_RECT paint_rect;
    
    ///widget position rectangle, relative to the screen
    ALGUI_RECT widget_rect;
} ALGUI_PAINT_MESSAGE;


/** the insert-widget message.
 */
typedef struct ALGUI_INSERT_WIDGET_MESSAGE {
    ///base message.
    ALGUI_MESSAGE message;
    
    ///widget to insert.
    struct ALGUI_WIDGET *child;
    
    ///next sibling widget.
    struct ALGUI_WIDGET *next;
    
    ///success.
    int ok;
} ALGUI_INSERT_WIDGET_MESSAGE;


/** the remove-widget message.
 */
typedef struct ALGUI_REMOVE_WIDGET_MESSAGE {
    ///base message.
    ALGUI_MESSAGE message;
    
    ///widget to remove.
    struct ALGUI_WIDGET *child;
    
    ///success.
    int ok;    
} ALGUI_REMOVE_WIDGET_MESSAGE;


/** the set-rect message.
 */
typedef struct ALGUI_SET_RECT_MESSAGE {
    ///base message.
    ALGUI_MESSAGE message;
    
    ///new rectangle.
    ALGUI_RECT rect;
} ALGUI_SET_RECT_MESSAGE;


/** the set-visible message.
 */
typedef struct ALGUI_SET_VISIBLE_MESSAGE {
    ///base message.
    ALGUI_MESSAGE message;
    
    ///new visible state.
    int visible : 1;
    
    ///success flag.
    int ok : 1;
} ALGUI_SET_VISIBLE_MESSAGE;


/** the set-enabled message.
 */
typedef struct ALGUI_SET_ENABLED_MESSAGE {
    ///base message.
    ALGUI_MESSAGE message;
    
    ///new enabled state.
    int enabled : 1;
    
    ///success flag.
    int ok : 1;
} ALGUI_SET_ENABLED_MESSAGE;


/** the get focus message.
 */
typedef struct ALGUI_GET_FOCUS_MESSAGE {
    ///base message.
    ALGUI_MESSAGE message;
    
    ///ok.
    int ok : 1;
} ALGUI_GET_FOCUS_MESSAGE;


/** the lose focus message.
 */
typedef struct ALGUI_LOSE_FOCUS_MESSAGE {
    ///base message.
    ALGUI_MESSAGE message;
    
    ///ok.
    int ok : 1;
} ALGUI_LOSE_FOCUS_MESSAGE;


/** the got focus message.
 */
typedef struct ALGUI_GOT_FOCUS_MESSAGE {
    ///base message.
    ALGUI_MESSAGE message;
} ALGUI_GOT_FOCUS_MESSAGE;


/** the lost focus message.
 */
typedef struct ALGUI_LOST_FOCUS_MESSAGE {
    ///base message.
    ALGUI_MESSAGE message;
} ALGUI_LOST_FOCUS_MESSAGE;


/** union of all messages.
 */
typedef union ALGUI_MESSAGE_UNION {
    ///base message.
    ALGUI_MESSAGE message;    
    
    ///cleanup message.
    ALGUI_CLEANUP_MESSAGE cleanup;
    
    ///paint message.
    ALGUI_PAINT_MESSAGE paint;
    
    ///insert widget message
    ALGUI_INSERT_WIDGET_MESSAGE insert_widget;
    
    ///remove widget message
    ALGUI_REMOVE_WIDGET_MESSAGE remove_widget;
    
    ///set rect message
    ALGUI_SET_RECT_MESSAGE set_rect;
    
    ///set visible message
    ALGUI_SET_VISIBLE_MESSAGE set_visible;
    
    ///set enabled message
    ALGUI_SET_ENABLED_MESSAGE set_enabled;
    
    //get focus message
    ALGUI_GET_FOCUS_MESSAGE get_focus;
    
    //lose focus message
    ALGUI_LOSE_FOCUS_MESSAGE lose_focus;
    
    //got focus message
    ALGUI_GOT_FOCUS_MESSAGE got_focus;
    
    //lost focus message
    ALGUI_LOST_FOCUS_MESSAGE lost_focus;
} ALGUI_MESSAGE_UNION;


#endif //ALGUI_MESSAGE_H
