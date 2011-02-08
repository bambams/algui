#ifndef ALGUI_MESSAGE_H
#define ALGUI_MESSAGE_H


#include "algui_rect.h"
#include "algui_skin.h"


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
    
    ///hit test
    ALGUI_MSG_HIT_TEST,
    
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
    
    ///drop with left buton
    ALGUI_MSG_LEFT_DROP,
    
    ///middle drop
    ALGUI_MSG_MIDDLE_DROP,
    
    ///right drop
    ALGUI_MSG_RIGHT_DROP,
    
    ///drag enter
    ALGUI_MSG_DRAG_ENTER,
    
    ///drag move
    ALGUI_MSG_DRAG_MOVE,
    
    ///drag leave
    ALGUI_MSG_DRAG_LEAVE,
    
    ///drag wheel
    ALGUI_MSG_DRAG_WHEEL,
    
    ///drag key down
    ALGUI_MSG_DRAG_KEY_DOWN,
    
    ///drag key up
    ALGUI_MSG_DRAG_KEY_UP,
    
    ///drag key char
    ALGUI_MSG_DRAG_KEY_CHAR,
    
    ///query dragged data.
    ALGUI_MSG_QUERY_DRAGGED_DATA,
    
    ///get dragged data.
    ALGUI_MSG_GET_DRAGGED_DATA,
    
    ///begin drag and drop.
    ALGUI_MSG_BEGIN_DRAG_AND_DROP,
        
    ///ended drag and drop.
    ALGUI_MSG_DRAG_AND_DROP_ENDED,
    
    ///timer event
    ALGUI_MSG_TIMER,
    
    ///set the UI skin
    ALGUI_MSG_SET_SKIN,
        
    ///1st user message
    ALGUI_MSG_USER = 0x10000
} ALGUI_MSG_ID; 


///drag and drop type.
typedef enum ALGUI_DRAG_AND_DROP_TYPE {
    ///copy
    ALGUI_DRAG_AND_DROP_COPY,

    ///move
    ALGUI_DRAG_AND_DROP_MOVE,
} ALGUI_DRAG_AND_DROP_TYPE;


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


/** mouse message.
 */
typedef struct ALGUI_MOUSE_MESSAGE {
    ///base message.
    ALGUI_MESSAGE message;
    
    ///timestamp, same as in allegro event
    double timestamp;
    
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
} ALGUI_MOUSE_MESSAGE;


/** left button down message.
 */
typedef ALGUI_MOUSE_MESSAGE ALGUI_LEFT_BUTTON_DOWN_MESSAGE;

/** left button up message.
 */
typedef ALGUI_MOUSE_MESSAGE ALGUI_LEFT_BUTTON_UP_MESSAGE;


/** middle button down message.
 */
typedef ALGUI_MOUSE_MESSAGE ALGUI_MIDDLE_BUTTON_DOWN_MESSAGE;


/** middle button up message.
 */
typedef ALGUI_MOUSE_MESSAGE ALGUI_MIDDLE_BUTTON_UP_MESSAGE;


/** right button down message.
 */
typedef ALGUI_MOUSE_MESSAGE ALGUI_RIGHT_BUTTON_DOWN_MESSAGE;


/** right button up message.
 */
typedef ALGUI_MOUSE_MESSAGE ALGUI_RIGHT_BUTTON_UP_MESSAGE;


/** mouse enter message.
 */
typedef ALGUI_MOUSE_MESSAGE ALGUI_MOUSE_ENTER_MESSAGE;


/** mouse move message.
 */
typedef ALGUI_MOUSE_MESSAGE ALGUI_MOUSE_MOVE_MESSAGE;


/** mouse leave message.
 */
typedef ALGUI_MOUSE_MESSAGE ALGUI_MOUSE_LEAVE_MESSAGE;


/** mouse wheel message.
 */
typedef ALGUI_MOUSE_MESSAGE ALGUI_MOUSE_WHEEL_MESSAGE;


///key message.
typedef struct ALGUI_KEY_MESSAGE {
    ///base message.
    ALGUI_MESSAGE message;    
    
    ///timestamp, same as in allegro event
    double timestamp;
    
    ///key code.
    int keycode;
} ALGUI_KEY_MESSAGE;  


///key down message.
typedef ALGUI_KEY_MESSAGE ALGUI_KEY_DOWN_MESSAGE;


///key up message.
typedef ALGUI_KEY_MESSAGE ALGUI_KEY_UP_MESSAGE;


///unused key down message.
typedef ALGUI_KEY_MESSAGE ALGUI_UNUSED_KEY_DOWN_MESSAGE;


///unused key up message.
typedef ALGUI_KEY_MESSAGE ALGUI_UNUSED_KEY_UP_MESSAGE;


/** key char message.
 */
typedef struct ALGUI_KEY_CHAR_MESSAGE {
    ///base message.
    ALGUI_MESSAGE message;    
    
    ///timestamp, same as in allegro event
    double timestamp;
    
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


/** drag'n'drop mouse message.
 */
typedef struct ALGUI_DRAG_AND_DROP_MOUSE_MESSAGE {
    ///base message.
    ALGUI_MESSAGE message;
    
    ///timestamp, same as in allegro event
    double timestamp;
    
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
    
    ///pointer to widget that has the dragged data
    struct ALGUI_WIDGET *source;
} ALGUI_DRAG_AND_DROP_MOUSE_MESSAGE;


/** left drop message.
 */
typedef ALGUI_DRAG_AND_DROP_MOUSE_MESSAGE ALGUI_LEFT_DROP_MESSAGE;


/** middle drop message.
 */
typedef ALGUI_DRAG_AND_DROP_MOUSE_MESSAGE ALGUI_MIDDLE_DROP_MESSAGE;


/** right drop message.
 */
typedef ALGUI_DRAG_AND_DROP_MOUSE_MESSAGE ALGUI_RIGHT_DROP_MESSAGE;


/** drag enter message.
 */
typedef ALGUI_DRAG_AND_DROP_MOUSE_MESSAGE ALGUI_DRAG_ENTER_MESSAGE;


/** drag move message.
 */
typedef ALGUI_DRAG_AND_DROP_MOUSE_MESSAGE ALGUI_DRAG_MOVE_MESSAGE;


/** drag leave message.
 */
typedef ALGUI_DRAG_AND_DROP_MOUSE_MESSAGE ALGUI_DRAG_LEAVE_MESSAGE;


/** drag wheel message.
 */
typedef ALGUI_DRAG_AND_DROP_MOUSE_MESSAGE ALGUI_DRAG_WHEEL_MESSAGE;


///key message for drag and drop.
typedef struct ALGUI_DRAG_AND_DROP_KEY_MESSAGE {
    ///base message.
    ALGUI_MESSAGE message;    
    
    ///timestamp, same as in allegro event
    double timestamp;
    
    ///key code.
    int keycode;
        
    ///pointer to widget that has the dragged data
    struct ALGUI_WIDGET *source;
} ALGUI_DRAG_AND_DROP_KEY_MESSAGE;  


///drag key down message.
typedef ALGUI_DRAG_AND_DROP_KEY_MESSAGE ALGUI_DRAG_KEY_DOWN_MESSAGE;


///drag key up message.
typedef ALGUI_DRAG_AND_DROP_KEY_MESSAGE ALGUI_DRAG_KEY_UP_MESSAGE;


/** key char message for drag and drop.
 */
typedef struct ALGUI_DRAG_KEY_CHAR_MESSAGE {
    ///base message.
    ALGUI_MESSAGE message;    
    
    ///timestamp, same as in allegro event
    double timestamp;
    
    ///key code
    int keycode;
    
    ///unicode character
    int unichar;
    
    ///modifiers
    unsigned modifiers;
    
    ///repeat flag
    int repeat;

    ///pointer to widget that has the dragged data
    struct ALGUI_WIDGET *source;
} ALGUI_DRAG_KEY_CHAR_MESSAGE; 


///query dragged data message.
typedef struct ALGUI_QUERY_DRAGGED_DATA_MESSAGE {
    ///base message.
    ALGUI_MESSAGE message;    
    
    ///data format.
    const char *format;
    
    ///drag and drop type
    ALGUI_DRAG_AND_DROP_TYPE type;
    
    ///operation result.
    int ok;
} ALGUI_QUERY_DRAGGED_DATA_MESSAGE;


///get dragged data message.
typedef struct ALGUI_GET_DRAGGED_DATA_MESSAGE {
    ///base message.
    ALGUI_MESSAGE message;    
    
    ///data format.
    const char *format;
    
    ///drag and drop type
    ALGUI_DRAG_AND_DROP_TYPE type;
    
    ///operation result.
    void *data;
} ALGUI_GET_DRAGGED_DATA_MESSAGE;


///begin drag and drop message.
typedef struct ALGUI_BEGIN_DRAG_AND_DROP_MESSAGE {
    ///base message.
    ALGUI_MESSAGE message;    
    
    ///accepted status
    int ok;
} ALGUI_BEGIN_DRAG_AND_DROP_MESSAGE;


///drag and drop ended message.
typedef struct ALGUI_DRAG_AND_DROP_ENDED_MESSAGE {
    ///base message.
    ALGUI_MESSAGE message;    
} ALGUI_DRAG_AND_DROP_ENDED_MESSAGE;


///timer message.
typedef struct ALGUI_TIMER_MESSAGE {
    ///base message.
    ALGUI_MESSAGE message;    
    
    ///timestamp, same as in allegro event
    double timestamp;
        
    ///allegro timer
    ALLEGRO_TIMER *timer;
} ALGUI_TIMER_MESSAGE;


///set skin message.
typedef struct ALGUI_SET_SKIN_MESSAGE {
    ///base message.
    ALGUI_MESSAGE message;    
    
    ///skin
    ALGUI_SKIN *skin;
} ALGUI_SET_SKIN_MESSAGE;


/** union of all messages.
 */
typedef union ALGUI_MESSAGE_UNION {
    ///message id.
    int id;
    
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
    
    ///hit_test message
    ALGUI_HIT_TEST_MESSAGE hit_test;
    
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
    
    ///left drop.        
    ALGUI_LEFT_DROP_MESSAGE left_drop;

    ///middle drop.
    ALGUI_MIDDLE_DROP_MESSAGE middle_drop;

    ///right drop.
    ALGUI_RIGHT_DROP_MESSAGE right_drop;

    ///drag enter.
    ALGUI_DRAG_ENTER_MESSAGE drag_enter;

    ///drag move.
    ALGUI_DRAG_MOVE_MESSAGE drag_move;

    ///drag leave.
    ALGUI_DRAG_LEAVE_MESSAGE drag_leave;

    ///drag wheel.
    ALGUI_DRAG_WHEEL_MESSAGE drag_wheel;

    ///drag key down.
    ALGUI_DRAG_KEY_DOWN_MESSAGE drag_key_down;

    ///drag key up.
    ALGUI_DRAG_KEY_UP_MESSAGE drag_key_up;

    //drag key char.
    ALGUI_DRAG_KEY_CHAR_MESSAGE drag_key_char;
    
    ///query dragged data.
    ALGUI_QUERY_DRAGGED_DATA_MESSAGE query_dragged_data;
    
    ///get dragged data.
    ALGUI_GET_DRAGGED_DATA_MESSAGE get_dragged_data;    
    
    ///begin drag-and-drop
    ALGUI_BEGIN_DRAG_AND_DROP_MESSAGE begin_drag_and_drop;
    
    ///drag-and-drop ended
    ALGUI_DRAG_AND_DROP_ENDED_MESSAGE drag_and_drop_ended;
    
    ///timer event
    ALGUI_TIMER_MESSAGE timer;
    
    ///set skin
    ALGUI_SET_SKIN_MESSAGE set_skin;
} ALGUI_MESSAGE_UNION;


#endif //ALGUI_MESSAGE_H
