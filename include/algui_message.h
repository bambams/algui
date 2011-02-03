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
    
    ///inform widget to set its preferred rectangle.
    ALGUI_MSG_SET_PREFERRED_RECT,
    
    ///inform widget to position its children.
    ALGUI_MSG_DO_LAYOUT,
    
    ///left button down.
    ALGUI_MSG_LEFT_BUTTON_DOWN,
    
    ///left button up.
    ALGUI_MSG_LEFT_BUTTON_UP,
    
    ///middle button down.
    ALGUI_MSG_MIDDLE_BUTTON_DOWN,
    
    ///middle button up.
    ALGUI_MSG_MIDDLE_BUTTON_UP,
    
    ///right button down.
    ALGUI_MSG_RIGHT_BUTTON_DOWN,
    
    ///right button up.
    ALGUI_MSG_RIGHT_BUTTON_UP,
    
    ///mouse enter
    ALGUI_MSG_MOUSE_ENTER,
    
    ///mouse move
    ALGUI_MSG_MOUSE_MOVE,
    
    ///mouse leave
    ALGUI_MSG_MOUSE_LEAVE,
    
    ///mouse wheel
    ALGUI_MSG_MOUSE_WHEEL,
    
    ///key down
    ALGUI_MSG_KEY_DOWN,
    
    ///key up
    ALGUI_MSG_KEY_UP,
    
    ///unused key down
    ALGUI_MSG_UNUSED_KEY_DOWN,
    
    ///unused key up
    ALGUI_MSG_UNUSED_KEY_UP,
    
    ///character
    ALGUI_MSG_KEY_CHAR,
    
    ///unused key char
    ALGUI_MSG_UNUSED_KEY_CHAR,
    
    ///hit test
    ALGUI_MSG_HIT_TEST,
    
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


/** the set-preferred rect message.
 */
typedef struct ALGUI_SET_PREFERRED_RECT_MESSAGE {
    ///base message.
    ALGUI_MESSAGE message;
} ALGUI_SET_PREFERRED_RECT_MESSAGE;


/** the do-layout message.
 */
typedef struct ALGUI_DO_LAYOUT_MESSAGE {
    ///base message.
    ALGUI_MESSAGE message;
} ALGUI_DO_LAYOUT_MESSAGE;


/** left button down message.
 */
typedef struct ALGUI_LEFT_BUTTON_DOWN_MESSAGE {
    ///base message.
    ALGUI_MESSAGE message;
    
    ///screen mouse x
    int screen_x;
    
    ///screen mouse y
    int screen_y;
    
    ///mouse z
    int z;
    
    ///mouse w
    int w;
    
    ///mouse button
    unsigned button;
    
    ///local mouse x (relative to widget)
    int x;
    
    ///local mouse y (relative to widget)
    int y;
} ALGUI_LEFT_BUTTON_DOWN_MESSAGE;


/** left button up message.
 */
typedef ALGUI_LEFT_BUTTON_DOWN_MESSAGE ALGUI_LEFT_BUTTON_UP_MESSAGE;


/** middle button down message.
 */
typedef ALGUI_LEFT_BUTTON_DOWN_MESSAGE ALGUI_MIDDLE_BUTTON_DOWN_MESSAGE;


/** middle button up message.
 */
typedef ALGUI_LEFT_BUTTON_DOWN_MESSAGE ALGUI_MIDDLE_BUTTON_UP_MESSAGE;


/** right button down message.
 */
typedef ALGUI_LEFT_BUTTON_DOWN_MESSAGE ALGUI_RIGHT_BUTTON_DOWN_MESSAGE;


/** right button up message.
 */
typedef ALGUI_LEFT_BUTTON_DOWN_MESSAGE ALGUI_RIGHT_BUTTON_UP_MESSAGE;


/** mouse enter message.
 */
typedef ALGUI_LEFT_BUTTON_DOWN_MESSAGE ALGUI_MOUSE_ENTER_MESSAGE;


/** mouse move message.
 */
typedef ALGUI_LEFT_BUTTON_DOWN_MESSAGE ALGUI_MOUSE_MOVE_MESSAGE;


/** mouse leave message.
 */
typedef ALGUI_LEFT_BUTTON_DOWN_MESSAGE ALGUI_MOUSE_LEAVE_MESSAGE;


/** mouse wheel message.
 */
typedef ALGUI_LEFT_BUTTON_DOWN_MESSAGE ALGUI_MOUSE_WHEEL_MESSAGE;


///key down message.
typedef struct ALGUI_KEY_DOWN_MESSAGE {
    ///base message.
    ALGUI_MESSAGE message;    
    
    ///key code
    int keycode;
} ALGUI_KEY_DOWN_MESSAGE;  


///key up message.
typedef ALGUI_KEY_DOWN_MESSAGE ALGUI_KEY_UP_MESSAGE;


///unused key down message.
typedef ALGUI_KEY_DOWN_MESSAGE ALGUI_UNUSED_KEY_DOWN_MESSAGE;


///unused key up message.
typedef ALGUI_KEY_DOWN_MESSAGE ALGUI_UNUSED_KEY_UP_MESSAGE;


/** key char message.
 */
typedef struct ALGUI_KEY_CHAR_MESSAGE {
    ///base message.
    ALGUI_MESSAGE message;    
    
    ///key code
    int keycode;
    
    ///unicode character
    int unichar;
    
    ///modifiers
    unsigned modifiers;
    
    ///repeat flag
    int repeat;
} ALGUI_KEY_CHAR_MESSAGE; 


///unused key char message.
typedef ALGUI_KEY_CHAR_MESSAGE ALGUI_UNUSED_KEY_CHAR_MESSAGE;


///hit test message
typedef struct ALGUI_HIT_TEST_MESSAGE {
    ///base message.
    ALGUI_MESSAGE message;    
    
    ///horizontal coordinate, relative to widget
    int x;
    
    ///vertical coordinate, relative to widget
    int y;
    
    ///hit test success
    int ok;
} ALGUI_HIT_TEST_MESSAGE;


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
    
    //set preferred rect message
    ALGUI_SET_PREFERRED_RECT_MESSAGE set_preferred_rect;
    
    //do layout message
    ALGUI_DO_LAYOUT_MESSAGE do_layout;    
    
    ///left button down message
    ALGUI_LEFT_BUTTON_DOWN_MESSAGE left_button_down;
    
    ///left button up message
    ALGUI_LEFT_BUTTON_UP_MESSAGE left_button_up;
    
    ///middle button down message
    ALGUI_MIDDLE_BUTTON_DOWN_MESSAGE middle_button_down;
    
    ///middle button up message
    ALGUI_MIDDLE_BUTTON_UP_MESSAGE middle_button_up;
    
    ///right button down message
    ALGUI_RIGHT_BUTTON_DOWN_MESSAGE right_button_down;
    
    ///right button up message
    ALGUI_RIGHT_BUTTON_UP_MESSAGE right_button_up;
    
    ///mouse enter message
    ALGUI_MOUSE_ENTER_MESSAGE mouse_enter;
    
    ///mouse move message
    ALGUI_MOUSE_MOVE_MESSAGE mouse_move;
    
    ///mouse leave message
    ALGUI_MOUSE_LEAVE_MESSAGE mouse_leave;
    
    ///mouse wheel message
    ALGUI_MOUSE_WHEEL_MESSAGE mouse_wheel;
    
    ///key down message
    ALGUI_KEY_DOWN_MESSAGE key_down;
    
    ///key up message
    ALGUI_KEY_UP_MESSAGE key_up;
    
    ///unused key down message
    ALGUI_UNUSED_KEY_DOWN_MESSAGE unused_key_down;
    
    ///unused key up message
    ALGUI_UNUSED_KEY_UP_MESSAGE unused_key_up;
    
    ///key char message
    ALGUI_KEY_CHAR_MESSAGE key_char;
    
    ///unused key char message
    ALGUI_UNUSED_KEY_CHAR_MESSAGE unused_key_char;
    
    ///hit_test message
    ALGUI_HIT_TEST_MESSAGE hit_test;
} ALGUI_MESSAGE_UNION;


#endif //ALGUI_MESSAGE_H
