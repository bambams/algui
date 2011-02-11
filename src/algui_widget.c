#include "algui_widget.h"
#include <assert.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>


/******************************************************************************
    INTERNAL CONSTANTS
 ******************************************************************************/
 
 
//mouse buttons
#define _LEFT_BUTTON         1
#define _RIGHT_BUTTON        2
#define _MIDDLE_BUTTON       3 


/******************************************************************************
    INTERNAL FUNCTIONS
 ******************************************************************************/
 
 
//calculates the screen rectangle of a widget
static void _calc_screen_rect(ALGUI_WIDGET *wgt) {
    ALGUI_WIDGET *parent, *child;
    
    parent = algui_get_parent_widget(wgt);
    
    //for child
    if (parent) {
        wgt->screen_rect = wgt->rect;
        algui_offset_rect(&wgt->screen_rect, parent->screen_rect.left, parent->screen_rect.top);
    }

    //for root    
    else {
        wgt->screen_rect = wgt->rect;
    }
    
    //calculate children
    for(child = algui_get_lowest_child_widget(wgt); child; child = algui_get_higher_sibling_widget(child)) {
        _calc_screen_rect(child);
    }
} 


//checks if a widget is being managed
static int _manages_layout(ALGUI_WIDGET *wgt) {
    for(; wgt; wgt = algui_get_parent_widget(wgt)) {
        if (wgt->layout) return 1;
    }
    return 0;
}
 
 
//sets the preferred size to a widget, children first
static void _set_preferred_size(ALGUI_WIDGET *wgt) {
    ALGUI_SET_PREFERRED_RECT_MESSAGE msg;
    ALGUI_WIDGET *child;
    
    //avoid hidden widgets
    if (!wgt->visible_tree) return;
    
    //begin layout management
    wgt->layout = 1;
    
    //notify children
    for(child = algui_get_lowest_child_widget(wgt); child; child = algui_get_higher_sibling_widget(child)) {
        _set_preferred_size(child);
    }
    
    //send message to widget
    msg.message.id = ALGUI_MSG_SET_PREFERRED_RECT;
    algui_send_message(wgt, &msg.message);
    
    //end layout management
    wgt->layout = 0;
}
 
 
//do layout, parent first
static void _do_layout(ALGUI_WIDGET *wgt) {
    ALGUI_DO_LAYOUT_MESSAGE msg;
    ALGUI_WIDGET *child;
    
    //avoid hidden widgets
    if (!wgt->visible_tree) return;
    
    //begin layout management
    wgt->layout = 1;
    
    //send message to widget
    msg.message.id = ALGUI_MSG_DO_LAYOUT;
    algui_send_message(wgt, &msg.message);
    
    //notify children
    for(child = algui_get_lowest_child_widget(wgt); child; child = algui_get_higher_sibling_widget(child)) {
        _do_layout(child);
    }

    //end layout management
    wgt->layout = 0;
}
 
 
//if the widget's layout is invalid, then initialize the layout    
static void _init_layout(ALGUI_WIDGET *wgt) {
    _set_preferred_size(wgt);
    _do_layout(wgt);    
    _calc_screen_rect(wgt);
}


//updates the layout of a widget
static void _update_layout(ALGUI_WIDGET *wgt) {
    ALGUI_SET_PREFERRED_RECT_MESSAGE msg;
    ALGUI_WIDGET *last;
    ALGUI_RECT prev_rect;
    
    do {    
        last = wgt;
        
        //keep the rect of the widget
        prev_rect = wgt->rect;

        //ask the widget to recalculate its preferred rect
        msg.message.id = ALGUI_MSG_SET_PREFERRED_RECT;
        algui_send_message(wgt, &msg.message);
        
        //if the rect didn't change, there is no need to propagate the calculation further
        if (algui_is_rect_equal_to_rect(&prev_rect, &wgt->rect)) break;
        
        //propagate the calculation to the parent
        wgt = algui_get_parent_widget(wgt);
    } while (wgt);    
    
    //recalculate the layout from the last widget (where the propagation stopped)
    _do_layout(last);
    _calc_screen_rect(last);
}


//updates the widgets' flags
static void _update_flags(ALGUI_WIDGET *wgt, int drawn) {
    ALGUI_WIDGET *parent, *child;    
    parent = algui_get_parent_widget(wgt);
    wgt->drawn = drawn;
    wgt->visible_tree = wgt->visible && (!parent || parent->visible_tree);
    wgt->enabled_tree = wgt->enabled && (!parent || parent->enabled_tree);
    if (!drawn) {
        wgt->focus = 0;
        wgt->mouse = 0;
        wgt->data_source = 0;
    }        
    for(child = algui_get_highest_child_widget(wgt); child; child = algui_get_higher_sibling_widget(child)) {
        _update_flags(child, drawn);
    }
} 
 
 
//recursively draw widgets
static void _draw(ALGUI_WIDGET *wgt, ALGUI_RECT *rect) {
    ALGUI_PAINT_MESSAGE msg;
    ALGUI_WIDGET *child;
    
    //if the widget is not drawn yet, then initialize the widgets
    if (!wgt->drawn) {
        _update_flags(wgt, 1);
        _init_layout(wgt);
    }

    //avoid invisible widgets
    if (!wgt->visible_tree) return;

    //calculate the actual clip between the widget rect and the given rect
    algui_get_rect_intersection(&wgt->screen_rect, rect, &msg.paint_rect);
    
    //if the widget lies beyond the clip area, then don't draw anything else
    if (!algui_is_rect_normalized(&msg.paint_rect)) return;
    
    //prepare the paint message
    msg.message.id = ALGUI_MSG_PAINT;
    msg.widget_rect = wgt->screen_rect;
    
    //clip the screen as needed, so as that each widget doesn't draw outside its area
    al_set_clipping_rectangle(msg.paint_rect.left, msg.paint_rect.top, algui_get_rect_width(&msg.paint_rect), algui_get_rect_height(&msg.paint_rect));
    
    //send the paint message to the widget
    algui_send_message(wgt, &msg.message);
    
    //paint children from lowest to highest
    for(child = algui_get_lowest_child_widget(wgt); child; child = algui_get_higher_sibling_widget(child)) {
        _draw(child, &msg.paint_rect);
    }
}
 
 
//recursively destroys a bunch of widgets
static void _destroy(ALGUI_WIDGET *wgt) {
    ALGUI_WIDGET *child, *next;
    for(child = algui_get_highest_child_widget(wgt); child; ) {
        next = algui_get_lower_sibling_widget(child);
        _destroy(child);
        child = next;
    }
    algui_cleanup_widget(wgt);
    al_free(wgt);
}


//removes focus from a widget
static int _remove_focus(ALGUI_WIDGET *wgt) {
    ALGUI_LOSE_FOCUS_MESSAGE lose_focus_msg;
    ALGUI_LOST_FOCUS_MESSAGE lost_focus_msg;
    
    //ask widget if it accepts losing the focus
    lose_focus_msg.message.id = ALGUI_MSG_LOSE_FOCUS;
    lose_focus_msg.ok = 0;
    algui_send_message(wgt, &lose_focus_msg.message);
    if (!lose_focus_msg.ok) return 0;

    //notify the widget that it has lost the input focus
    wgt->focus = 0;
    lost_focus_msg.message.id = ALGUI_MSG_LOST_FOCUS;
    algui_send_message(wgt, &lost_focus_msg.message);
    
    //success
    return 1;
}


//sends a message to the widget, but only if it is enabled
static int _send_message_to_enabled(ALGUI_WIDGET *wgt, ALGUI_MESSAGE *msg) {
    return wgt->enabled_tree ? algui_send_message(wgt, msg) : 0;
}
    
    
//dispatches a message to a widget tree
static int _broadcast_message_to_enabled(ALGUI_WIDGET *wgt, ALGUI_MESSAGE *msg) {
    ALGUI_WIDGET *child;
    
    //try the widget
    if (_send_message_to_enabled(wgt, msg)) return 1;
    
    //try the children
    for(child = algui_get_lowest_child_widget(wgt); child; child = algui_get_higher_sibling_widget(child)) {
        if (_broadcast_message_to_enabled(child, msg)) return 1;
    }
    
    //message not processed
    return 0;
}


//helper function
static void _get_capture_widget_helper(ALGUI_WIDGET *wgt, ALGUI_WIDGET **result) {
    ALGUI_WIDGET *child;
    
    //set the result from this widget
    if (wgt->capture > (*result)->capture) {
        *result = wgt;
    }
    
    //find the capture amongst children
    for(child = algui_get_lowest_child_widget(wgt); child; child = algui_get_higher_sibling_widget(child)) {
        _get_capture_widget_helper(child, result);        
    }
} 
 
 
//returns the widget with the current capture
static ALGUI_WIDGET *_get_capture_widget(ALGUI_WIDGET *wgt) {
    ALGUI_WIDGET *result = wgt;
    _get_capture_widget_helper(wgt, &result);
    return result;
} 


//returns the widget that has the mouse
static ALGUI_WIDGET *_get_mouse_widget(ALGUI_WIDGET *wgt) {
    ALGUI_WIDGET *child, *result;
    if (wgt->mouse) return wgt;
    for(child = algui_get_highest_child_widget(wgt); child; child = algui_get_lower_sibling_widget(child)) {
        result = _get_mouse_widget(child);
        if (result) return result;
    }
    return 0;
}


//sets a key char message from an event
static void _set_key_char_message(ALGUI_KEY_CHAR_MESSAGE *msg, int id, ALLEGRO_EVENT *ev) {
    msg->message.id = id;
    msg->event = ev;
    msg->keycode = ev->keyboard.keycode;
    msg->unichar = ev->keyboard.unichar;
    msg->modifiers = ev->keyboard.modifiers;
    msg->repeat = ev->keyboard.repeat;
}


//sets a mouse message structure from an event
static void _set_mouse_message(ALGUI_WIDGET *wgt, ALGUI_MOUSE_MESSAGE *msg, int id, ALLEGRO_EVENT *ev) {
    msg->message.id = id;
    msg->event = ev;
    msg->z = ev->mouse.z;
    msg->w = ev->mouse.w;
    msg->button = ev->mouse.button;
    msg->x = ev->mouse.x - wgt->screen_rect.left;
    msg->y = ev->mouse.y - wgt->screen_rect.top;
}


//returns the widget with the data source
static ALGUI_WIDGET *_get_data_source(ALGUI_WIDGET *wgt) {
    ALGUI_WIDGET *child, *result;
    
    //check widget
    if (wgt->data_source) return wgt;
    
    //check children
    for(child = algui_get_lowest_child_widget(wgt); child; child = algui_get_higher_sibling_widget(child)) {
        result = _get_data_source(child);
        if (result) return result;
    }
    
    return NULL;
}
 
 
//sets a drag message structure from an event
static void _set_drag_message(ALGUI_WIDGET *wgt, ALGUI_DRAG_AND_DROP_MOUSE_MESSAGE *msg, int id, ALLEGRO_EVENT *ev, ALGUI_WIDGET *source) {
    msg->message.id = id;
    msg->event = ev;
    msg->z = ev->mouse.z;
    msg->w = ev->mouse.w;
    msg->button = ev->mouse.button;
    msg->x = ev->mouse.x - wgt->screen_rect.left;
    msg->y = ev->mouse.y - wgt->screen_rect.top;
    msg->source = source;
}


//locates the node of a timer
static ALGUI_LIST_NODE *_find_timer(ALGUI_LIST *timers, ALLEGRO_TIMER *timer) {
    ALGUI_LIST_NODE *node;    
    for(node = algui_get_first_list_node(timers); node; node = algui_get_next_list_node(node)) {
        if (node->data == timer) return node;
    }
    return NULL;
}


//removes a timer node from a timer list, stops and destroys the timer,
//and then free the node
static void _destroy_timer(ALGUI_LIST *timers, ALGUI_LIST_NODE *node) {
    ALLEGRO_TIMER *timer = (ALLEGRO_TIMER *)algui_get_list_node_data(node);
    al_destroy_timer(timer);
    algui_remove_list_node(timers, node);
    al_free(node);
}


//manages the input focus according to key pressed
static int _manage_focus_via_keyboard(ALGUI_WIDGET *wgt, ALLEGRO_EVENT *ev) {
    switch (ev->keyboard.keycode) {
        case ALLEGRO_KEY_LEFT:
        case ALLEGRO_KEY_UP:
            return algui_move_focus_backward(wgt);
            
        case ALLEGRO_KEY_RIGHT:
        case ALLEGRO_KEY_DOWN:
            return algui_move_focus_forward(wgt);
            
        case ALLEGRO_KEY_TAB:
            if (ev->keyboard.modifiers & (ALLEGRO_KEYMOD_SHIFT)) {
                return algui_move_focus_backward(wgt);
            }
            else {
                return algui_move_focus_forward(wgt);
            }
            break;
            
    }
    
    return 0;
}


//returns the child widget with a lower tab order than the given one
static ALGUI_WIDGET *_get_lower_tab_child(ALGUI_WIDGET *wgt, ALGUI_WIDGET *tab_child) {
    ALGUI_WIDGET *result = NULL, *child;    
    int min_tab_order = INT_MIN;
    int max_tab_order = tab_child ? tab_child->tab_order : INT_MAX;
    for(child = algui_get_highest_child_widget(wgt); child; child = algui_get_lower_sibling_widget(child)) {
        if (child->tab_order < max_tab_order && child->tab_order > min_tab_order) {            
            result = child;
            min_tab_order = child->tab_order;
        }
    }
    return result;
}


//moves focus back in children
static int _move_focus_backward_in_children(ALGUI_WIDGET *wgt, ALGUI_WIDGET *focus_child) {
    ALGUI_WIDGET *child;
    for(child = _get_lower_tab_child(wgt, focus_child); child; child = _get_lower_tab_child(wgt, child)) {
        if (_move_focus_backward_in_children(child, NULL)) return 1;
        if (algui_set_focus_widget(child)) return 1;
    }
    return 0;
}


//returns the child widget with a higher tab order than the given one
static ALGUI_WIDGET *_get_higher_tab_child(ALGUI_WIDGET *wgt, ALGUI_WIDGET *tab_child) {
    ALGUI_WIDGET *result = NULL, *child;    
    int max_tab_order = INT_MAX;
    int min_tab_order = tab_child ? tab_child->tab_order : -1;
    for(child = algui_get_lowest_child_widget(wgt); child; child = algui_get_higher_sibling_widget(child)) {
        if (child->tab_order > min_tab_order && child->tab_order < max_tab_order) {            
            result = child;
            max_tab_order = child->tab_order;
        }
    }
    return result;
}


//moves focus forward in children
static int _move_focus_forward_in_children(ALGUI_WIDGET *wgt, ALGUI_WIDGET *focus_child) {
    ALGUI_WIDGET *child;
    for(child = _get_higher_tab_child(wgt, focus_child); child; child = _get_higher_tab_child(wgt, child)) {
        if (_move_focus_forward_in_children(child, NULL)) return 1;
        if (algui_set_focus_widget(child)) return 1;
    }
    return 0;
}


/******************************************************************************
    INTERNAL MESSAGE HANDLERS
 ******************************************************************************/
     

//cleanup widget
static int _msg_cleanup(ALGUI_WIDGET *wgt, ALGUI_CLEANUP_MESSAGE *msg) {
    algui_destroy_widget_timers(wgt);
    return 1;
} 


//insert widget
static int _msg_insert_widget(ALGUI_WIDGET *wgt, ALGUI_INSERT_WIDGET_MESSAGE *msg) {
    assert(wgt);
    assert(msg);
    assert(msg->child);
    msg->ok = algui_insert_tree(&wgt->tree, &msg->child->tree, msg->next ? &msg->next->tree : NULL);
    if (msg->ok) {
        msg->child->tab_order = algui_get_tree_child_count(&wgt->tree);
        _update_flags(msg->child, wgt->drawn);
        if (wgt->drawn) {
            _init_layout(msg->child);
            _update_layout(wgt);
        }
    }        
    return 1;
} 


//remove widget
static int _msg_remove_widget(ALGUI_WIDGET *wgt, ALGUI_REMOVE_WIDGET_MESSAGE *msg) {
    assert(wgt);
    assert(msg);
    assert(msg->child);
    msg->ok = algui_remove_tree(&wgt->tree, &msg->child->tree);
    if (msg->ok) {
        _update_flags(msg->child, 0);
        if (wgt->drawn) {
            _update_layout(wgt);
        }
    }        
    return 1;
} 


//set rect
static int _msg_set_rect(ALGUI_WIDGET *wgt, ALGUI_SET_RECT_MESSAGE *msg) {
    assert(wgt);
    assert(msg);
    if (algui_is_rect_equal_to_rect(&wgt->rect, &msg->rect)) return 1;
    wgt->rect = msg->rect;
    if (wgt->drawn && !_manages_layout(wgt)) _update_layout(wgt);
    return 1;
} 


//set visible
static int _msg_set_visible(ALGUI_WIDGET *wgt, ALGUI_SET_VISIBLE_MESSAGE *msg) {
    ALGUI_WIDGET *focus, *parent;

    assert(wgt);
    assert(msg);
    
    if (wgt->visible == msg->visible) return 1;
    
    //a widget that contains the input focus cannot be hidden
    if (!msg->visible) {
        focus = algui_get_focus_widget(wgt);
        if (focus) {
            msg->ok = 0;
            return 1;
        }
    }
    
    wgt->visible = msg->visible;
    _update_flags(wgt, wgt->drawn);
    
    msg->ok = 1;
    
    //if the widget has a parent, then update the layout of the parent
    if (wgt->drawn) {
        parent = algui_get_parent_widget(wgt);
        if (parent) _update_layout(parent);
    }
    
    return 1;
} 


//set enabled
static int _msg_set_enabled(ALGUI_WIDGET *wgt, ALGUI_SET_ENABLED_MESSAGE *msg) {
    ALGUI_WIDGET *focus;

    assert(wgt);
    assert(msg);
    
    if (wgt->enabled == msg->enabled) return 1;
        
    //a widget that contains the input focus cannot be disabled
    if (!msg->enabled) {
        focus = algui_get_focus_widget(wgt);
        if (focus) {
            msg->ok = 0;
            return 1;
        }
    }
    
    wgt->enabled = msg->enabled;
    _update_flags(wgt, wgt->drawn);
    msg->ok = 1;
    
    return 1;
} 


//get focus
static int _msg_get_focus(ALGUI_WIDGET *wgt, ALGUI_GET_FOCUS_MESSAGE *msg) {
    assert(wgt);
    assert(msg);
    
    //accept getting focus
    msg->ok = 1;
    
    return 1;
} 


//lose focus
static int _msg_lose_focus(ALGUI_WIDGET *wgt, ALGUI_LOSE_FOCUS_MESSAGE *msg) {
    assert(wgt);
    assert(msg);
    
    //accept losing focus
    msg->ok = 1;
    
    return 1;
} 


//hit test message
static int _msg_hit_test(ALGUI_WIDGET *wgt, ALGUI_HIT_TEST_MESSAGE *msg) {
    assert(wgt);
    assert(msg);
    
    //accept the hit test if the point is inside the widget
    msg->ok = 
        msg->x >= 0 && msg->x < algui_get_widget_width(wgt) &&
        msg->y >= 0 && msg->y < algui_get_widget_height(wgt);
    
    return 1;
} 


/******************************************************************************
    INTERNAL EVENT HANDLERS
 ******************************************************************************/
     

//dispatches the relevant events
static int _event_key_down(ALGUI_WIDGET *wgt, ALLEGRO_EVENT *ev) {
    ALGUI_KEY_DOWN_MESSAGE key_down_msg;
    ALGUI_UNUSED_KEY_DOWN_MESSAGE unused_key_down_msg;
    ALGUI_WIDGET *capture, *focus;

    //dispatch events to the current capture
    capture = _get_capture_widget(wgt);
    
    //get the focus widget
    focus = algui_get_focus_widget(capture);
    
    //if there is a focus widget, dispatch the event to it
    if (focus) {
        //prepare the keydown message
        key_down_msg.message.id = ALGUI_MSG_KEY_DOWN;
        key_down_msg.event = ev;
        key_down_msg.keycode = ev->keyboard.keycode;

        //if the focus widget processes the message, then do nothing else        
        if (_send_message_to_enabled(focus, &key_down_msg.message)) return 1;
    }
    
    //prepare the unused key down message
    unused_key_down_msg.message.id = ALGUI_MSG_UNUSED_KEY_DOWN;
    unused_key_down_msg.event = ev;
    unused_key_down_msg.keycode = ev->keyboard.keycode;
    
    //dispatch the unused key down message
    if (_broadcast_message_to_enabled(capture, &unused_key_down_msg.message)) return 1;
    
    //event not processed
    return 0;
}


//dispatches the relevant events
static int _event_key_up(ALGUI_WIDGET *wgt, ALLEGRO_EVENT *ev) {
    ALGUI_KEY_UP_MESSAGE key_up_msg;
    ALGUI_UNUSED_KEY_UP_MESSAGE unused_key_up_msg;
    ALGUI_WIDGET *capture, *focus;

    //dispatch events to the current capture
    capture = _get_capture_widget(wgt);
    
    //get the focus widget
    focus = algui_get_focus_widget(capture);
    
    //if there is a focus widget, dispatch the event to it
    if (focus) {
        //prepare the keyup message
        key_up_msg.message.id = ALGUI_MSG_KEY_UP;
        key_up_msg.event = ev;
        key_up_msg.keycode = ev->keyboard.keycode;

        //if the focus widget processes the message, then do nothing else        
        if (_send_message_to_enabled(focus, &key_up_msg.message)) return 1;
    }
    
    //prepare the unused key up message
    unused_key_up_msg.message.id = ALGUI_MSG_UNUSED_KEY_UP;
    unused_key_up_msg.event = ev;
    unused_key_up_msg.keycode = ev->keyboard.keycode;
    
    //dispatch the unused key up message
    if (_broadcast_message_to_enabled(capture, &unused_key_up_msg.message)) return 1;
    
    //event not processed
    return 0;
}


//dispatches the relevant events
static int _event_key_char(ALGUI_WIDGET *wgt, ALLEGRO_EVENT *ev) {
    ALGUI_KEY_CHAR_MESSAGE key_char_msg;
    ALGUI_UNUSED_KEY_CHAR_MESSAGE unused_key_char_msg;
    ALGUI_WIDGET *capture, *focus;

    //dispatch events to the current capture
    capture = _get_capture_widget(wgt);
    
    //get the focus widget
    focus = algui_get_focus_widget(capture);
    
    //if there is a focus widget, dispatch the event to it
    if (focus) {
        //prepare the keychar message
        _set_key_char_message(&key_char_msg, ALGUI_MSG_KEY_CHAR, ev);

        //if the focus widget processes the message, then do nothing else        
        if (_send_message_to_enabled(focus, &key_char_msg.message)) return 1;
    }
    
    //prepare the unused key char message
    _set_key_char_message(&unused_key_char_msg, ALGUI_MSG_UNUSED_KEY_CHAR, ev);
    
    //dispatch the unused key char message
    if (_broadcast_message_to_enabled(capture, &unused_key_char_msg.message)) return 1; 
        
    //manage focus via the keyboard
    return _manage_focus_via_keyboard(capture, ev);
}


//dispatches the relevant events
static int _event_mouse_move(ALGUI_WIDGET *wgt, ALLEGRO_EVENT *ev) {
    ALGUI_WIDGET *capture, *new_mouse, *old_mouse;
    ALGUI_MOUSE_LEAVE_MESSAGE leave_msg;
    ALGUI_MOUSE_ENTER_MESSAGE enter_msg;
    ALGUI_MOUSE_MOVE_MESSAGE move_msg;
    int processed = 0;

    //dispatch events to the current capture
    capture = _get_capture_widget(wgt);    
    
    //get the old widget under mouse (from the root, not from the capture,
    //because the old mouse may be outside of the capture tree)
    old_mouse = _get_mouse_widget(wgt);
    
    //get the new widget under mouse
    new_mouse = algui_get_widget_from_point(
        capture, 
        ev->mouse.x - capture->screen_rect.left, 
        ev->mouse.y - capture->screen_rect.top);
    
    //if the mouse changed widgets
    if (new_mouse != old_mouse) {        
        //send a mouse leave to the old mouse
        if (old_mouse) {
            old_mouse->mouse = 0;
            _set_mouse_message(old_mouse, &leave_msg, ALGUI_MSG_MOUSE_LEAVE, ev);
            processed |= _send_message_to_enabled(old_mouse, &leave_msg.message);
        }
        
        //send a mouse enter to the new nouse
        if (new_mouse) {
            new_mouse->mouse = 1;
            _set_mouse_message(new_mouse, &enter_msg, ALGUI_MSG_MOUSE_ENTER, ev);
            processed |= _send_message_to_enabled(new_mouse, &enter_msg.message);        
        }
    }
    
    else {
        if (!new_mouse) new_mouse = capture;                
        _set_mouse_message(new_mouse, &move_msg, ALGUI_MSG_MOUSE_MOVE, ev);
        processed |= _send_message_to_enabled(new_mouse, &move_msg.message);                    
    }
    
    return processed;
}


//dispatches the relevant events
static int _event_mouse_wheel(ALGUI_WIDGET *wgt, ALLEGRO_EVENT *ev) {
    ALGUI_MOUSE_WHEEL_MESSAGE wheel_msg;
    ALGUI_WIDGET *capture, *focus;
    
    capture = _get_capture_widget(wgt);

    //dispatch the event to the focus widget
    focus = algui_get_focus_widget(capture);    
    
    //if there is no focus widget, send the message to the capture widget
    if (!focus) focus = capture;
    
    //send the event        
    _set_mouse_message(focus, &wheel_msg, ALGUI_MSG_MOUSE_WHEEL, ev);
    return _send_message_to_enabled(focus, &wheel_msg.message);        
}


//dispatches the relevant events
static int _event_button_down(ALGUI_WIDGET *wgt, ALLEGRO_EVENT *ev) {
    ALGUI_MOUSE_MESSAGE button_msg;
    ALGUI_WIDGET *capture, *mouse;
    int id;

    //dispatch events to the current capture
    capture = _get_capture_widget(wgt);    
    
    //dispatch the event to the widget under the mouse coordinates
    mouse = algui_get_widget_from_point(
        capture, 
        ev->mouse.x - capture->screen_rect.left, 
        ev->mouse.y - capture->screen_rect.top);
        
    //if the event is outside of the capture, send it to the capture
    if (!mouse) mouse = capture;
    
    //identify the id according to the button
    switch (ev->mouse.button) {
        //left
        case _LEFT_BUTTON: 
            id = ALGUI_MSG_LEFT_BUTTON_DOWN;
            break;
            
        //right
        case _RIGHT_BUTTON: 
            id = ALGUI_MSG_RIGHT_BUTTON_DOWN;
            break;
            
        //middle
        case _MIDDLE_BUTTON: 
            id = ALGUI_MSG_MIDDLE_BUTTON_DOWN;
            break;
            
        //no other buttons are supported                
        default:                
            return 0;
    }

    _set_mouse_message(mouse, &button_msg, id, ev);
    return _send_message_to_enabled(mouse, &button_msg.message);        
}


//dispatches the relevant events
static int _event_button_up(ALGUI_WIDGET *wgt, ALLEGRO_EVENT *ev) {
    ALGUI_LEFT_BUTTON_UP_MESSAGE button_msg;
    ALGUI_WIDGET *capture, *mouse;
    int id;

    //dispatch events to the current capture
    capture = _get_capture_widget(wgt);    
    
    //dispatch the event to the widget under the mouse coordinates
    mouse = algui_get_widget_from_point(
        capture, 
        ev->mouse.x - capture->screen_rect.left, 
        ev->mouse.y - capture->screen_rect.top);
        
    //if the event is outside of the capture, send it to the capture
    if (!mouse) mouse = capture;
    
    //identify the id according to the button
    switch (ev->mouse.button) {
        //left
        case _LEFT_BUTTON: 
            id = ALGUI_MSG_LEFT_BUTTON_UP;
            break;
            
        //right
        case _RIGHT_BUTTON: 
            id = ALGUI_MSG_RIGHT_BUTTON_UP;
            break;
            
        //middle
        case _MIDDLE_BUTTON: 
            id = ALGUI_MSG_MIDDLE_BUTTON_UP;
            break;
            
        //no other buttons are supported                
        default:                
            return 0;
    }

    _set_mouse_message(mouse, &button_msg, id, ev);
    return _send_message_to_enabled(mouse, &button_msg.message);        
}


//dispatches the relevant events
static int _event_expose(ALGUI_WIDGET *wgt, ALLEGRO_EVENT *ev) {
    ALGUI_RECT rct;
    wgt = algui_get_root_widget(wgt);
    algui_move_and_resize_rect(&rct, ev->display.x, ev->display.y, ev->display.width, ev->display.height);
    algui_draw_widget_rect(wgt, &rct);
    return 1;
}


//dispatches the relevant events
static int _event_window_deactivated(ALGUI_WIDGET *wgt, ALLEGRO_EVENT *ev) {
    ALGUI_WIDGET *focus = algui_get_focus_widget(wgt);
    if (!focus) return 0;
    _remove_focus(focus);
    return 1;
}


//dispatches the relevant events
static int _event_window_activated(ALGUI_WIDGET *wgt, ALLEGRO_EVENT *ev) {
    //nothing at the moment
    return 0;
}


//dispatch the relevant event
static int _event_drag_key_down(ALGUI_WIDGET *wgt, ALLEGRO_EVENT *ev, ALGUI_WIDGET *source) {
    ALGUI_WIDGET *capture, *mouse;
    ALGUI_DRAG_KEY_DOWN_MESSAGE msg;
    
    //dispatch event in the capture tree
    capture = _get_capture_widget(wgt);
    
    //dispatch event to the mouse widget 
    mouse = _get_mouse_widget(capture);
    
    //if there is no mouse widget, dispatch it to the capture
    if (!mouse) mouse = capture;
    
    //prepare the message
    msg.message.id = ALGUI_MSG_DRAG_KEY_DOWN;
    msg.event = ev;
    msg.keycode = ev->keyboard.keycode;
    msg.source = source;
    
    //send the message to the widget
    return _send_message_to_enabled(mouse, &msg.message);
}

    
//dispatch the relevant event
static int _event_drag_key_up(ALGUI_WIDGET *wgt, ALLEGRO_EVENT *ev, ALGUI_WIDGET *source) {
    ALGUI_WIDGET *capture, *mouse;
    ALGUI_DRAG_KEY_UP_MESSAGE msg;
    
    //dispatch event in the capture tree
    capture = _get_capture_widget(wgt);
    
    //dispatch event to the mouse widget 
    mouse = _get_mouse_widget(capture);
    
    //if there is no mouse widget, dispatch it to the capture
    if (!mouse) mouse = capture;
    
    //prepare the message
    msg.message.id = ALGUI_MSG_DRAG_KEY_UP;
    msg.event = ev;
    msg.keycode = ev->keyboard.keycode;
    msg.source = source;
    
    //send the message to the widget
    return _send_message_to_enabled(mouse, &msg.message);
}

    
//dispatch the relevant event
static int _event_drag_key_char(ALGUI_WIDGET *wgt, ALLEGRO_EVENT *ev, ALGUI_WIDGET *source) {
    ALGUI_WIDGET *capture, *mouse;
    ALGUI_DRAG_KEY_CHAR_MESSAGE msg;
    
    //dispatch event in the capture tree
    capture = _get_capture_widget(wgt);
    
    //dispatch event to the mouse widget 
    mouse = _get_mouse_widget(capture);
    
    //if there is no mouse widget, dispatch it to the capture
    if (!mouse) mouse = capture;
    
    //prepare the message
    msg.message.id = ALGUI_MSG_DRAG_KEY_CHAR;
    msg.event = ev;
    msg.keycode = ev->keyboard.keycode;
    msg.unichar = ev->keyboard.unichar;
    msg.modifiers = ev->keyboard.modifiers;
    msg.repeat = ev->keyboard.repeat;
    msg.source = source;
    
    //send the message to the widget
    return _send_message_to_enabled(mouse, &msg.message);
}

    
//dispatches the relevant events
static int _event_drag_move(ALGUI_WIDGET *wgt, ALLEGRO_EVENT *ev, ALGUI_WIDGET *source) {
    ALGUI_WIDGET *capture, *new_mouse, *old_mouse;
    ALGUI_DRAG_LEAVE_MESSAGE leave_msg;
    ALGUI_DRAG_ENTER_MESSAGE enter_msg;    
    ALGUI_DRAG_MOVE_MESSAGE move_msg;
    int processed = 0;

    //dispatch events to the current capture
    capture = _get_capture_widget(wgt);    
    
    //get the old widget under mouse
    old_mouse = _get_mouse_widget(capture);
    
    //get the new widget under mouse
    new_mouse = algui_get_widget_from_point(
        capture, 
        ev->mouse.x - capture->screen_rect.left, 
        ev->mouse.y - capture->screen_rect.top);
    
    //if the mouse changed widgets
    if (new_mouse != old_mouse) {        
        //send a mouse leave to the old mouse
        if (old_mouse) {
            old_mouse->mouse = 0;
            _set_drag_message(old_mouse, &leave_msg, ALGUI_MSG_DRAG_LEAVE, ev, source);
            processed |= _send_message_to_enabled(old_mouse, &leave_msg.message);
        }
        
        //send a mouse enter to the new nouse
        if (new_mouse) {
            new_mouse->mouse = 1;
            _set_drag_message(new_mouse, &enter_msg, ALGUI_MSG_DRAG_ENTER, ev, source);
            processed |= _send_message_to_enabled(new_mouse, &enter_msg.message);        
        }
    }
    
    else {
        if (!new_mouse) new_mouse = capture;                
        _set_drag_message(new_mouse, &move_msg, ALGUI_MSG_DRAG_MOVE, ev, source);
        processed |= _send_message_to_enabled(new_mouse, &move_msg.message);        
    }
    
    return processed;
}


//dispatches the relevant events
static int _event_drag_wheel(ALGUI_WIDGET *wgt, ALLEGRO_EVENT *ev, ALGUI_WIDGET *source) {
    ALGUI_DRAG_WHEEL_MESSAGE wheel_msg;
    ALGUI_WIDGET *capture, *mouse;
    
    capture = _get_capture_widget(wgt);

    //dispatch the event to the focus widget
    mouse = algui_get_mouse_widget(capture);    
    
    //if there is no mouse widget, send the message to the capture widget
    if (!mouse) mouse = capture;
    
    //send the event        
    _set_drag_message(mouse, &wheel_msg, ALGUI_MSG_DRAG_WHEEL, ev, source);
    return _send_message_to_enabled(mouse, &wheel_msg.message);        
}


//dispatches the relevant events
static int _event_drop(ALGUI_WIDGET *wgt, ALLEGRO_EVENT *ev, ALGUI_WIDGET *source) {
    ALGUI_LEFT_DROP_MESSAGE drop_msg;
    ALGUI_WIDGET *capture, *mouse;
    int id, processed;

    //dispatch events to the current capture
    capture = _get_capture_widget(wgt);    
    
    //dispatch the event to the widget under the mouse coordinates
    mouse = algui_get_widget_from_point(
        capture, 
        ev->mouse.x - capture->screen_rect.left, 
        ev->mouse.y - capture->screen_rect.top);
        
    //if the event is outside of the capture, send it to the capture
    if (!mouse) mouse = capture;
    
    //identify the id according to the button
    switch (ev->mouse.button) {
        //left
        case _LEFT_BUTTON: 
            id = ALGUI_MSG_LEFT_DROP;
            break;
            
        //right
        case _RIGHT_BUTTON: 
            id = ALGUI_MSG_RIGHT_DROP;
            break;
            
        //middle
        case _MIDDLE_BUTTON: 
            id = ALGUI_MSG_MIDDLE_DROP;
            break;
            
        //no other buttons are supported                
        default:                
            return 0;
    }

    _set_drag_message(mouse, &drop_msg, id, ev, source);
    processed |= _send_message_to_enabled(mouse, &drop_msg.message);        
    
    //end drag and drop
    algui_end_drag_and_drop(source);
    
    return processed;
}


//timer event
static int  _event_timer(ALGUI_WIDGET *wgt, ALLEGRO_EVENT *ev) {
    ALGUI_LIST_NODE *node;
    ALGUI_TIMER_MESSAGE msg;
    ALGUI_WIDGET *child;
    
    //find the node that corresponds to the timer
    node = _find_timer(&wgt->timers, ev->timer.source);
    
    //if found, dispatch the event to the widget
    if (node) {
        //prepare the message
        msg.message.id = ALGUI_MSG_TIMER;
        msg.event = ev;
        msg.timer = ev->timer.source;
        
        //send the message
        _send_message_to_enabled(wgt, &msg.message);
        
        //timer event processed
        return 1;
    }
    
    //try the children
    for(child = algui_get_highest_child_widget(wgt); child; child = algui_get_lower_sibling_widget(child)) {
        if (_event_timer(child, ev)) return 1;
    }
    
    //timer event not processed
    return 0;
}


//display resized
static int _event_display_resized(ALGUI_WIDGET *wgt, ALLEGRO_EVENT *ev) {
    ALGUI_DISPLAY_RESIZED_MESSAGE msg;
    wgt = algui_get_root_widget(wgt);
    msg.message.id = ALGUI_MSG_DISPLAY_RESIZED;
    msg.event = ev;
    return algui_send_message(wgt, &msg.message);
}


//default event dispatch
static int _event_dispatch_default(ALGUI_WIDGET *wgt, ALLEGRO_EVENT *ev) {
    switch (ev->type) {        
        //expose
        case ALLEGRO_EVENT_DISPLAY_EXPOSE:
            return _event_expose(wgt, ev);
        
        //window deactivated
        case ALLEGRO_EVENT_DISPLAY_SWITCH_OUT:
            return _event_window_deactivated(wgt, ev);
        
        //window activated
        case ALLEGRO_EVENT_DISPLAY_SWITCH_IN:
            return _event_window_activated(wgt, ev);
            
        //timer
        case ALLEGRO_EVENT_TIMER:
            return _event_timer(wgt, ev);
            
        //display resized event
        case ALLEGRO_EVENT_DISPLAY_RESIZE:            
            return _event_display_resized(wgt, ev);
    }
    
    //event not processed
    return 0;
}


//normal event dispatch
static int _event_dispatch(ALGUI_WIDGET *wgt, ALLEGRO_EVENT *ev) {
    int processed = 0;

    switch (ev->type) {
        //key down
        case ALLEGRO_EVENT_KEY_DOWN:
            return _event_key_down(wgt, ev);
        
        //key up
        case ALLEGRO_EVENT_KEY_UP:
            return _event_key_up(wgt, ev);
        
        //key char
        case ALLEGRO_EVENT_KEY_CHAR:
            return _event_key_char(wgt, ev);
        
        //mouse moved
        case ALLEGRO_EVENT_MOUSE_AXES:
        case ALLEGRO_EVENT_MOUSE_WARPED:
            if (ev->mouse.dx || ev->mouse.dy) {
                processed |= _event_mouse_move(wgt, ev);
            }
            if (ev->mouse.dz || ev->mouse.dw) {
                processed |= _event_mouse_wheel(wgt, ev);
            }
            return processed;
        
        //button down
        case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
            return _event_button_down(wgt, ev);
        
        //button up
        case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
            return _event_button_up(wgt, ev);
    }            

    //default dispatch            
    return _event_dispatch_default(wgt, ev);            
}


//drag and drop dispatch
static int _event_dispatch_drag_and_drop(ALGUI_WIDGET *wgt, ALLEGRO_EVENT *ev, ALGUI_WIDGET *source) {
    int processed = 0;

    switch (ev->type) {
        //key down
        case ALLEGRO_EVENT_KEY_DOWN:
            return _event_drag_key_down(wgt, ev, source);
        
        //key up
        case ALLEGRO_EVENT_KEY_UP:
            return _event_drag_key_up(wgt, ev, source);
        
        //key char
        case ALLEGRO_EVENT_KEY_CHAR:
            return _event_drag_key_char(wgt, ev, source);
        
        //mouse moved
        case ALLEGRO_EVENT_MOUSE_AXES:
        case ALLEGRO_EVENT_MOUSE_WARPED:
            if (ev->mouse.dx || ev->mouse.dy) {
                processed |= _event_drag_move(wgt, ev, source);
            }
            if (ev->mouse.dz || ev->mouse.dw) {
                processed |= _event_drag_wheel(wgt, ev, source);
            }
            return processed;
        
        //drop
        case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
            return _event_drop(wgt, ev, source);

    }            

    //default dispatch     
    return _event_dispatch_default(wgt, ev);            
}

    
/******************************************************************************
    PUBLIC
 ******************************************************************************/


/** the default widget procedure.
    TODO explain which messages it processes.
    @param wgt widget.
    @param msg message.
    @return non-zero if the message was processed, zero otherwise.
 */
int algui_widget_proc(ALGUI_WIDGET *wgt, ALGUI_MESSAGE *msg) {
    switch (msg->id) {
        case ALGUI_MSG_CLEANUP      : return _msg_cleanup(wgt, (ALGUI_CLEANUP_MESSAGE *)msg);
        case ALGUI_MSG_INSERT_WIDGET: return _msg_insert_widget(wgt, (ALGUI_INSERT_WIDGET_MESSAGE *)msg);
        case ALGUI_MSG_REMOVE_WIDGET: return _msg_remove_widget(wgt, (ALGUI_REMOVE_WIDGET_MESSAGE *)msg);
        case ALGUI_MSG_SET_RECT     : return _msg_set_rect(wgt, (ALGUI_SET_RECT_MESSAGE *)msg);
        case ALGUI_MSG_SET_VISIBLE  : return _msg_set_visible(wgt, (ALGUI_SET_VISIBLE_MESSAGE *)msg);
        case ALGUI_MSG_SET_ENABLED  : return _msg_set_enabled(wgt, (ALGUI_SET_ENABLED_MESSAGE *)msg);
        case ALGUI_MSG_GET_FOCUS    : return _msg_get_focus(wgt, (ALGUI_GET_FOCUS_MESSAGE *)msg);
        case ALGUI_MSG_LOSE_FOCUS   : return _msg_lose_focus(wgt, (ALGUI_LOSE_FOCUS_MESSAGE *)msg);
        case ALGUI_MSG_HIT_TEST     : return _msg_hit_test(wgt, (ALGUI_HIT_TEST_MESSAGE *)msg);
    }
    return 0;
}


/** sends a message to a widget.
    It returns after the message has been processed by the widget.
    @param wgt target widget.
    @param msg message to send to the widget.
    @return non-zero if the message was processed, zero otherwise.
 */
int algui_send_message(ALGUI_WIDGET *wgt, ALGUI_MESSAGE *msg) {
    assert(wgt);
    assert(wgt->proc);
    return wgt->proc(wgt, msg);
}


/** sends a message to all widgets in the widget tree.
    It returns after the message has been processed by the widgets.
    @param wgt target widget.
    @param msg message to send to the widget.
    @return non-zero if the message was processed by any widget, zero otherwise.
 */
int algui_broadcast_message(ALGUI_WIDGET *wgt, ALGUI_MESSAGE *msg) {
    ALGUI_WIDGET *child;
    int r = 0;
    assert(wgt);
    r |= algui_send_message(wgt, msg);
    for(child = algui_get_lowest_child_widget(wgt); child; child = algui_get_higher_sibling_widget(child)) {
        r |= algui_broadcast_message(child, msg);
    }
    return r;
}


/** returns a widget's procedure.
    @param wgt widget to get the procedure of.
    @return the widget procedure.
 */
ALGUI_WIDGET_PROC algui_get_widget_proc(ALGUI_WIDGET *wgt) {
    assert(wgt);
    return wgt->proc;
}


/** returns the parent widget.
    @param wgt widget to get the parent of.
    @return the parent widget; it may be null.
 */
ALGUI_WIDGET *algui_get_parent_widget(ALGUI_WIDGET *wgt) {
    ALGUI_TREE *tree;
    assert(wgt);
    tree = algui_get_parent_tree(&wgt->tree);
    return tree ? (ALGUI_WIDGET *)algui_get_tree_data(tree) : NULL;
}


/** returns the previous sibling.
    The sibling is lower in z-order.
    @param wgt widget to get the sibling of.
    @return the sibling; it may be null.
 */
ALGUI_WIDGET *algui_get_lower_sibling_widget(ALGUI_WIDGET *wgt) {
    ALGUI_TREE *tree;
    assert(wgt);
    tree = algui_get_prev_sibling_tree(&wgt->tree);
    return tree ? (ALGUI_WIDGET *)algui_get_tree_data(tree) : NULL;
}


/** returns the next sibling.
    The sibling is higher in z-order.
    @param wgt widget to get the sibling of.
    @return the sibling; it may be null.
 */
ALGUI_WIDGET *algui_get_higher_sibling_widget(ALGUI_WIDGET *wgt) {
    ALGUI_TREE *tree;
    assert(wgt);
    tree = algui_get_next_sibling_tree(&wgt->tree);
    return tree ? (ALGUI_WIDGET *)algui_get_tree_data(tree) : NULL;
}


/** returns the first child widget.
    The sibling is lowest in the z-order of children.
    @param wgt widget to get the child of.
    @return the child; it may be null.
 */
ALGUI_WIDGET *algui_get_lowest_child_widget(ALGUI_WIDGET *wgt) {
    ALGUI_TREE *tree;
    assert(wgt);
    tree = algui_get_first_child_tree(&wgt->tree);
    return tree ? (ALGUI_WIDGET *)algui_get_tree_data(tree) : NULL;
}


/** returns the last child widget.
    The sibling is highest in the z-order of children.
    @param wgt widget to get the child of.
    @return the child; it may be null.
 */
ALGUI_WIDGET *algui_get_highest_child_widget(ALGUI_WIDGET *wgt) {
    ALGUI_TREE *tree;
    assert(wgt);
    tree = algui_get_last_child_tree(&wgt->tree);
    return tree ? (ALGUI_WIDGET *)algui_get_tree_data(tree) : NULL;
}


/** returns the number of children widgets.
    @param wgt widget to get the number of children of.
    @return the number of children widgets.
 */
unsigned long algui_get_widget_child_count(ALGUI_WIDGET *wgt) {
    assert(wgt);
    return algui_get_tree_child_count(&wgt->tree);
}


/** returns the root widget.
    @param wgt widget to get the root of.
    @return the root widget.
 */
ALGUI_WIDGET *algui_get_root_widget(ALGUI_WIDGET *wgt) {
    ALGUI_TREE *tree;
    assert(wgt);
    tree = algui_get_root_tree(&wgt->tree);
    return tree ? (ALGUI_WIDGET *)algui_get_tree_data(tree) : NULL;
}


/** returns the local rectangle of a widget.
    @param wgt widget to get the rectangle of.
    @return the widget's rectangle.
 */
ALGUI_RECT algui_get_widget_rect(ALGUI_WIDGET *wgt) {
    assert(wgt);
    return wgt->rect;
}


/** returns the horizontal position of the widget.
    @param wgt widget to get the coordinate of.
    @return the horizontal position of the widget.
 */
int algui_get_widget_x(ALGUI_WIDGET *wgt) {
    assert(wgt);
    return wgt->rect.left;
}


/** returns the vertical position of the widget.
    @param wgt widget to get the coordinate of.
    @return the vertical position of the widget.
 */
int algui_get_widget_y(ALGUI_WIDGET *wgt) {
    assert(wgt);
    return wgt->rect.top;
}


/** returns the width of the widget.
    @param wgt widget to get the size of.
    @return the width of the widget.
 */
int algui_get_widget_width(ALGUI_WIDGET *wgt) {
    assert(wgt);
    return algui_get_rect_width(&wgt->rect);
}


/** returns the height of the widget.
    @param wgt widget to get the size of.
    @return the height of the widget.
 */
int algui_get_widget_height(ALGUI_WIDGET *wgt) {
    assert(wgt);
    return algui_get_rect_height(&wgt->rect);
}


/** returns the screen rectangle of a widget.
    @param wgt widget to get the rectangle of.
    @return the widget's rectangle.
 */
ALGUI_RECT algui_get_widget_screen_rect(ALGUI_WIDGET *wgt) {
    assert(wgt);
    return wgt->screen_rect;
}


/** returns a widget's visible status.
    @param wgt widget to get the status of.
    @return the widget status.
 */
int algui_is_widget_visible(ALGUI_WIDGET *wgt) {
    assert(wgt);
    return wgt->visible;
}


/** returns a widget's enabled status.
    @param wgt widget to get the status of.
    @return the widget status.
 */
int algui_is_widget_enabled(ALGUI_WIDGET *wgt) {
    assert(wgt);
    return wgt->enabled;
}


/** returns a widget's enabled tree status.
    Returns true if this and all ancestor widgets are enabled.
    @param wgt widget to get the status of.
    @return the widget status.
 */
int algui_is_widget_tree_enabled(ALGUI_WIDGET *wgt) {
    assert(wgt);
    return wgt->enabled_tree;
}


/** returns true if the given widget has the input focus.
    @param wgt widget to get the focus status of.
    @return non-zero if the widget has the focus, zero otherwise.
 */
int algui_widget_has_focus(ALGUI_WIDGET *wgt) {
    assert(wgt);
    return wgt->focus;
}


/** get the widget that has the focus.
    @param wgt widget to start the search from.
    @return the widget that has the focus or NULL if no widget has the focus.
 */
ALGUI_WIDGET *algui_get_focus_widget(ALGUI_WIDGET *wgt) {
    ALGUI_WIDGET *child, *result;
    assert(wgt);
    if (wgt->focus) return wgt;
    for(child = algui_get_highest_child_widget(wgt); child; child = algui_get_lower_sibling_widget(child)) {
        result = algui_get_focus_widget(child);
        if (result) return result;
    }
    return NULL;
}


/** returns true if the given widget has the mouse.
    @param wgt widget to get the mouse status of.
    @return non-zero if the widget has the mouse, zero otherwise.
 */
int algui_widget_has_mouse(ALGUI_WIDGET *wgt) {
    assert(wgt);
    return wgt->mouse;
}


/** get the widget that has the mouse.
    @param wgt widget to start the search from.
    @return the widget that has the mouse or NULL if no widget has the mouse.
 */
ALGUI_WIDGET *algui_get_mouse_widget(ALGUI_WIDGET *wgt) {
    ALGUI_WIDGET *child, *result;
    assert(wgt);
    if (wgt->mouse) return wgt;
    for(child = algui_get_highest_child_widget(wgt); child; child = algui_get_lower_sibling_widget(child)) {
        result = algui_get_mouse_widget(child);
        if (result) return result;
    }
    return NULL;
}


/** returns the widget that is under the given point.
    Widgets receive a hit-test message.
    Invisible widgets are ignored.
    @param wgt root of widget tree to search.
    @param x horizontal coordinate, relative to the given widget.
    @param y vertical coordinate, relative to the given widget.
    @return the widget under the given coordinates, or NULL if there is one.
 */
ALGUI_WIDGET *algui_get_widget_from_point(ALGUI_WIDGET *wgt, int x, int y) {
    ALGUI_WIDGET *child, *result;
    ALGUI_HIT_TEST_MESSAGE msg;

    assert(wgt);
    
    //if the widget is not visible, or if the coordinates lie beyond the widget's rect, then do nothing
    if (!wgt->visible_tree || 
        x < 0 || 
        x >= algui_get_widget_width(wgt) || 
        y < 0 || 
        y >= algui_get_widget_height(wgt))
    {        
        return NULL;
    }        
    
    //search the children, from higher to lower
    for(child = algui_get_highest_child_widget(wgt); child; child = algui_get_lower_sibling_widget(child)) {
        result = algui_get_widget_from_point(child, x - child->rect.left, y - child->rect.top);
        if (result) return result;
    }
    
    //ask the widget
    msg.message.id = ALGUI_MSG_HIT_TEST;
    msg.x = x;
    msg.y = y;
    msg.ok = 0;
    algui_send_message(wgt, &msg.message);
    
    return msg.ok ? wgt : NULL;
}


/** returns the id of a widget.
    @param wgt widget to get the id of.
    @return the id of a widget.
 */
const char *algui_get_widget_id(ALGUI_WIDGET *wgt) {
    assert(wgt);
    return wgt->id;
}


/** retrieves the tab order of a widget.
    @param wgt widget to get the tab order of.
    @return the widget's tab order.
 */
int algui_get_widget_tab_order(ALGUI_WIDGET *wgt) {
    assert(wgt);
    return wgt->tab_order;
}


/** retrieves the z-order of a widget.
    @param wgt widget to get the z-order of.
    @return the widget's z-order; 0 means that the widget is below all its siblings;
        -1 means the input parameter was null.
 */
int algui_get_widget_z_order(ALGUI_WIDGET *wgt) {
    int result = -1;
    for(; wgt; ++result, wgt = algui_get_lower_sibling_widget(wgt));
    return result;
}


/** initializes a widget structure.
    @param wgt widget to initialize.
    @param proc widget proc.
    @param id widget id; statically allocated text that identifies the widget; 
        normally, it is the widget class, which is used for skinning widget classes,
        but specific instances can modify the widget id to a unique identifier
        in order to specify a unique skin appearance.
 */
void algui_init_widget(ALGUI_WIDGET *wgt, ALGUI_WIDGET_PROC proc, const char *id) {
    assert(wgt);
    assert(proc);
    wgt->proc = proc;
    algui_init_tree(&wgt->tree, wgt);
    algui_set_rect(&wgt->rect, 0, 0, 0, 0);
    algui_set_rect(&wgt->screen_rect, 0, 0, 0, 0);
    algui_init_list(&wgt->timers);
    wgt->id = id;
    wgt->capture = 0;
    wgt->tab_order = 0;
    wgt->visible = 1;
    wgt->visible_tree = 1;
    wgt->enabled = 1;
    wgt->enabled_tree = 1;
    wgt->focus = 0;
    wgt->layout = 0;
    wgt->drawn = 0;
    wgt->data_source = 0;
}


/** cleans up a widget and its children.
    Every widget in the tree gets the cleanup message.
    Parent-child widget relationships are maintained.
    @param wgt widget to cleanup.
 */
void algui_cleanup_widget(ALGUI_WIDGET *wgt) {
    ALGUI_CLEANUP_MESSAGE msg;
    msg.message.id = ALGUI_MSG_CLEANUP;
    algui_broadcast_message(wgt, &msg.message);
}


/** cleans up a widget and its children, and then frees the memory it occupies.
    Every widget in the tree is cleaned up, and then widgets are freed using the al_free function.
    @param wgt widget to cleanup.
 */
void algui_destroy_widget(ALGUI_WIDGET *wgt) {
    algui_cleanup_widget(wgt);
    _destroy(wgt);
}


/** translates a point from the coordinate system of one widget to the other.
    @param src source widget; if null, the input point is considered to be in screen coordinates.
    @param src_x input x in the source coordinate system; can be the source.
    @param src_y input y in the source coordinate system; can be the source.
    @param dst destination widget; if null, the output point is considered to be in screen coordinates.
    @param dst_x output x in the destination coordinate system; can be the source.
    @param dst_y output y in the destination coordinate system; can be the source.
 */
void algui_translate_point(ALGUI_WIDGET *src, int src_x, int src_y, ALGUI_WIDGET *dst, int *dst_x, int *dst_y) {
    assert(dst_x);
    assert(dst_y);
    
    //translate coordinates from source to screen    
    if (src) {
        src_x += src->screen_rect.left;
        src_y += src->screen_rect.top;
    }
    
    //translate coordinates from screen to destination
    if (dst) {
        src_x -= dst->screen_rect.left;
        src_y -= dst->screen_rect.top;
    }
    
    *dst_x = src_x;
    *dst_y = src_y;
}


/** translates a rectangle from the coordinate system of one widget to the other.
    @param src source widget; if null, the input rect is considered to be in screen coordinates.
    @param src_rct source rectangle.
    @param dst destination widget; if null, the output rect is considered to be in screen coordinates.
    @param dst_rct destination rectangle; it may be the source.
 */
void algui_translate_rect(ALGUI_WIDGET *src, ALGUI_RECT *src_rct, ALGUI_WIDGET *dst, ALGUI_RECT *dst_rct) {
    ALGUI_RECT tmp;
    
    assert(src_rct);
    assert(dst_rct);
    
    tmp = *src_rct;

    //translate coordinates from source to screen    
    if (src) {
        algui_offset_rect(&tmp, src->screen_rect.left, src->screen_rect.top);
    }
    
    //translate coordinates from screen to destination
    if (dst) {
        algui_offset_rect(&tmp, -dst->screen_rect.left, -dst->screen_rect.top);
    }
    
    *dst_rct = tmp;
}


/** draws a part of a widget.
    Every widget in the tree gets the paint message.
    @param wgt widget to draw; its children are also drawn.
    @param rct local rectangle of widget to draw.
 */
void algui_draw_widget_rect(ALGUI_WIDGET *wgt, ALGUI_RECT *rct) {
    ALGUI_RECT screen_rect;
    int cx, cy, cw, ch;
    assert(wgt);
    assert(rct);
    
    //translate coordinates from widget to screen
    algui_translate_rect(wgt, rct, NULL, &screen_rect);
    
    //keep the clipping rect in order to restore it later
    al_get_clipping_rectangle(&cx, &cy, &cw, &ch);
    
    //draw every widget in the tree
    _draw(wgt, &screen_rect);
    
    //restore the clipping
    al_set_clipping_rectangle(cx, cy, cw, ch);
}


/** draws a whole widget.
    @param wgt widget to draw; its children are also drawn.
 */
void algui_draw_widget(ALGUI_WIDGET *wgt) {
    ALGUI_RECT rct;
    assert(wgt);
    algui_move_and_resize_rect(&rct, 0, 0, algui_get_widget_width(wgt), algui_get_widget_height(wgt));
    algui_draw_widget_rect(wgt, &rct);
}


/** inserts a widget in another widget as a child.
    @param parent parent; it receives the insert-widget message.
    @param child child.
    @param next next sibling; it can be null.
    @return non-zero if the operation succeeded, zero otherwise.
 */
int algui_insert_widget(ALGUI_WIDGET *parent, ALGUI_WIDGET *child, ALGUI_WIDGET *next) {
    ALGUI_INSERT_WIDGET_MESSAGE msg;
    msg.message.id = ALGUI_MSG_INSERT_WIDGET;
    msg.child = child;
    msg.next = next;
    msg.ok = 0;
    algui_send_message(parent, &msg.message);
    return msg.ok;
}


/** adds a widget in another widget as a child.
    @param parent parent; it receives the insert-widget message.
    @param child child.
    @return non-zero if the operation succeeded, zero otherwise.
 */
int algui_add_widget(ALGUI_WIDGET *parent, ALGUI_WIDGET *child) {
    return algui_insert_widget(parent, child, NULL);
}


/** removes a widget from its parent.
    @param parent parent; it receives the remove-widget message.
    @param child child to remove.
    @return non-zero if the operation succeeded, zero otherwise.
 */
int algui_remove_widget(ALGUI_WIDGET *parent, ALGUI_WIDGET *child) {
    ALGUI_REMOVE_WIDGET_MESSAGE msg;
    msg.message.id = ALGUI_MSG_REMOVE_WIDGET;
    msg.child = child;
    msg.ok = 0;
    algui_send_message(parent, &msg.message);
    return msg.ok;
}


/** removes a widget from its parent.
    @param child widget to remove from its parent, if there is one.
 */
void algui_detach_widget(ALGUI_WIDGET *child) {
    ALGUI_WIDGET *parent = algui_get_parent_widget(child);
    if (parent) algui_remove_widget(parent, child);
}


/** sets the local rectangle of a widget.
    The widget receives the set-rect message.
    @param wgt widget to set.
    @param rct the widget's new local rectangle.
 */
void algui_set_widget_rect(ALGUI_WIDGET *wgt, ALGUI_RECT *rct) {
    ALGUI_SET_RECT_MESSAGE msg;    
    msg.message.id = ALGUI_MSG_SET_RECT;
    msg.rect = *rct;
    algui_send_message(wgt, &msg.message);    
}


/** sets the widget's rectangle by setting its position and size.
    The widget receives the set-rect message.
    @param wgt widget to set.
    @param x new horizontal position.
    @param y new vertical position.
    @param w new width.
    @param h new height.
 */
void algui_move_and_resize_widget(ALGUI_WIDGET *wgt, int x, int y, int w, int h) {
    ALGUI_RECT rct;
    algui_move_and_resize_rect(&rct, x, y, w, h);
    algui_set_widget_rect(wgt, &rct);
}


/** sets the widget's rectangle by setting its position.
    The widget receives the set-rect message.
    @param wgt widget to set.
    @param x new horizontal position.
    @param y new vertical position.
 */
void algui_move_widget(ALGUI_WIDGET *wgt, int x, int y) {
    ALGUI_RECT rct = algui_get_widget_rect(wgt);
    algui_move_rect(&rct, x, y);
    algui_set_widget_rect(wgt, &rct);
}


/** sets the widget's rectangle by setting its size.
    The widget receives the set-rect message.
    @param wgt widget to set.
    @param w new width.
    @param h new height.
 */
void algui_resize_widget(ALGUI_WIDGET *wgt, int w, int h) {
    ALGUI_RECT rct = algui_get_widget_rect(wgt);
    algui_resize_rect(&rct, w, h);
    algui_set_widget_rect(wgt, &rct);
}


/** Calculates the layout of the given widget and its children.
    This may lead to further changes in other widgets up and down the tree, depending on layout.
    This function may be invoked after a widget modifies a visual attribute
    that changes the widget's position or size.
    There is no need to invoke this function before drawing the widgets
    for the first time; they will be packed automatically.
    @param wgt widget to start the layout management from.
 */
void algui_pack_widget(ALGUI_WIDGET *wgt) {
    assert(wgt);
    if (wgt->drawn) _update_layout(wgt);
}


/** sets the widget's visible state.
    The widget receives the set-visible message.
    @param wgt widget.
    @param state new state; non-zero for visible, zero for invisible.
    @return non-zero if the operation succeeded, zero otherwise.
 */
int algui_set_widget_visible(ALGUI_WIDGET *wgt, int visible) {
    ALGUI_SET_VISIBLE_MESSAGE msg;
    msg.message.id = ALGUI_MSG_SET_VISIBLE;
    msg.visible = visible;
    msg.ok = 0;
    algui_send_message(wgt, &msg.message);
    return msg.ok;
}


/** sets the widget's visible state to true.
    The widget receives the set-visible message.
    @param wgt widget.
    @return non-zero if the operation succeeded, zero otherwise.
 */ 
int algui_show_widget(ALGUI_WIDGET *wgt) {
    return algui_set_widget_visible(wgt, 1);
}


/** sets the widget's visible state to false.
    The widget receives the set-visible message.
    @param wgt widget.
    @return non-zero if the operation succeeded, zero otherwise.
 */ 
int algui_hide_widget(ALGUI_WIDGET *wgt) {
    return algui_set_widget_visible(wgt, 0);
}


/** sets the widget's enabled state.
    The widget receives the set-enabled message.
    @param wgt widget.
    @param state new state; non-zero for enabled, zero for disabled.
    @return non-zero if the operation succeeded, zero otherwise.
 */
int algui_set_widget_enabled(ALGUI_WIDGET *wgt, int enabled) {
    ALGUI_SET_ENABLED_MESSAGE msg;
    msg.message.id = ALGUI_MSG_SET_ENABLED;
    msg.enabled = enabled;
    msg.ok = 0;
    algui_send_message(wgt, &msg.message);
    return msg.ok;
}


/** sets the widget's enabled state to true.
    The widget receives the set-enabled message.
    @param wgt widget.
    @return non-zero if the operation succeeded, zero otherwise.
 */ 
int algui_enable_widget(ALGUI_WIDGET *wgt) {
    return algui_set_widget_enabled(wgt, 1);
}


/** sets the widget's enabled state to false.
    The widget receives the set-enabled message.
    @param wgt widget.
    @return non-zero if the operation succeeded, zero otherwise.
 */ 
int algui_disable_widget(ALGUI_WIDGET *wgt) {
    return algui_set_widget_enabled(wgt, 0);
}


/** sets the widget proc of a widget.
    @param wgt widget to set.
    @param proc new widget proc.
 */
void algui_set_widget_proc(ALGUI_WIDGET *wgt, ALGUI_WIDGET_PROC proc) {
    assert(wgt);
    assert(proc);
    wgt->proc = proc;
}


/** sets the input focus to the given widget.
    The given widget receives a get-focus message;
    if the widget accepts the focus, then the previous focus widget, if there is one,
    receives a lose-focus message.
    If both widgets return ok, then the previous widget get a lost-focus message,
    and the new widget gets a got-focus message.
    @param wgt wgt to set the focus to.
    @return non-zero if the operation is successful, zero otherwise.
 */
int algui_set_focus_widget(ALGUI_WIDGET *wgt) {
    ALGUI_WIDGET *focus;
    ALGUI_GET_FOCUS_MESSAGE get_focus_msg;
    ALGUI_GOT_FOCUS_MESSAGE got_focus_msg;

    assert(wgt);
    
    //a disabled or invisible widget cannot get the input focus
    if (!wgt->enabled_tree || !wgt->visible_tree) return 0;
    
    //ask the widget if it wants the focus
    get_focus_msg.message.id = ALGUI_MSG_GET_FOCUS;
    get_focus_msg.ok = 0;
    algui_send_message(wgt, &get_focus_msg.message);
    if (!get_focus_msg.ok) return 0;
    
    //try to remove the current focus
    focus = algui_get_focus_widget(algui_get_root_widget(wgt));
    if (focus && !_remove_focus(focus)) return 0;
    
    //notify the widget that it has the input focus
    wgt->focus = 1;
    got_focus_msg.message.id = ALGUI_MSG_GOT_FOCUS;
    algui_send_message(wgt, &got_focus_msg.message);
        
    //success
    return 1;
}


/** dispatch an Allegro event to a widget tree.
    The event is dispatched to the given widget or its children
    or to the widget that has captured events or its children.
    @param wgt root of widget tree to dispatch the event to.
    @param ev allegro event.
    @return non-zero if the event was processed, zero otherwise.
 */
int algui_dispatch_event(ALGUI_WIDGET *wgt, ALLEGRO_EVENT *ev) {
    ALGUI_WIDGET *drag_and_drop_source;    
    drag_and_drop_source = algui_get_drag_and_drop_source(wgt);
    if (!drag_and_drop_source) return _event_dispatch(wgt, ev);
    return _event_dispatch_drag_and_drop(wgt, ev, drag_and_drop_source);
}


/** captures events.
    Until events are released, events are dispatched to the given
    widget or its children.
    Events must be released with the function algui_release_events.
    @param wgt widget to capture events.
    @return non-zero if the operation succeeded, 
        zero if it failed due to too many captures or 
        failure to remove the focus if the focus lies outside of the capture tree.
 */
int algui_capture_events(ALGUI_WIDGET *wgt) {
    ALGUI_WIDGET *root;

    assert(wgt);
    
    root = algui_get_root_widget(wgt);
        
    //set the widget's capture value from the value of the current capture widget
    wgt->capture = _get_capture_widget(root)->capture + 1;
    
    //if the value rolled below zero, then we have too many captures
    if (wgt->capture < 0) {
        wgt->capture = 0;
        return 0;
    }
    
    //success
    return 1;
}


/** releases events.
    Event control is given back to the previous widget
    that had captured events, or to the root widget of
    the widget tree that events are dispatched to.
    @param wgt widget to release events from.
    @return non-zero if the operation succeeded, zero if it failed due to the widget not having captured the events.
 */
int algui_release_events(ALGUI_WIDGET *wgt) {
    assert(wgt);
    if (!wgt->capture) return 0;
    wgt->capture = 0;
    return 1;
}


/** begins drag and drop with the given widget as the data source.
    After this call, widgets receive drag and drop events,
    until drag and drop ends.
    If there is a capture, then drag and drop events are delivered within the capture widget.
    The widget receives a begin-drag-and-drop message.
    @param source widget to initiate drag-and-drop.
    @return non-zero if the widget accepted the operation and there is no other source widget, zero otherwise.
 */
int algui_begin_drag_and_drop(ALGUI_WIDGET *source) {
    ALGUI_BEGIN_DRAG_AND_DROP_MESSAGE msg;
    ALGUI_WIDGET *other_source;
    
    assert(source);
    
    //check if there is another drag and drop in progress
    other_source = algui_get_drag_and_drop_source(source);
    assert(other_source == 0);
    if (other_source) return 0;
    
    //set up the message
    msg.message.id = ALGUI_MSG_BEGIN_DRAG_AND_DROP;
    msg.ok = 0;
    
    //ask the widget
    algui_send_message(source, &msg.message);
    
    //if the widget refused
    if (!msg.ok) return 0;
    
    //set up the flag
    source->data_source = 1;
    
    //success
    return 1;
}


/** Ends a drag-and-drop session.
    This function is called automatically when a mouse button is released
    during a drag-and-drop session.    
    The source widget receives a drag-and-drop-ended message.
    @param source the source widget.
    @return non-zero if the operation suceeded, zero if the widget was not the data source.
 */
int algui_end_drag_and_drop(ALGUI_WIDGET *source) {    
    ALGUI_DRAG_AND_DROP_ENDED_MESSAGE msg;
    
    assert(source);
    assert(source->data_source != 0);
    
    //if the widget is not the data source, fail
    if (!source->data_source) return 0;
    
    //prepare the message
    msg.message.id = ALGUI_MSG_DRAG_AND_DROP_ENDED;
    
    //inform the widget
    algui_send_message(source, &msg.message);
    
    //no more drag and drop
    source->data_source = 0;
    
    //success
    return 1;
}


/** retrieves the data source widget.
    The data source widget must be in the same widget tree as the given widget.
    @param wgt widget tree to get the data source widget from.
    @return the data soruce widget.
 */
ALGUI_WIDGET *algui_get_drag_and_drop_source(ALGUI_WIDGET *wgt) {
    return _get_data_source(algui_get_root_widget(wgt));
}


/** queries the data source widget about the data type and operation.
    The data source widget receives a query-drag-and-drop message.
    @param source data source widget.
    @param format data format description (UTF-8 string).
    @param type drag-and-drop type.
    @return non-zero if the data source widget supports the given format and type, zero otherwise.
 */
int algui_query_dragged_data(ALGUI_WIDGET *source, const char *format, ALGUI_DRAG_AND_DROP_TYPE type) {
    ALGUI_QUERY_DRAGGED_DATA_MESSAGE msg;    
    assert(format);
    assert(type == ALGUI_DRAG_AND_DROP_COPY || type == ALGUI_DRAG_AND_DROP_MOVE);    
    msg.message.id = ALGUI_MSG_QUERY_DRAGGED_DATA;
    msg.format = format;
    msg.type = type;
    msg.ok = 0;
    algui_send_message(source, &msg.message);
    return msg.ok;
}


/** Retrieves the dragged data from the data source widget.
    The data source widget receives a get-drag-and-drop message.
    @param source data source widget.
    @param format data format description (UTF-8 string).
    @param type drag-and-drop type.
    @return pointer to the data or NULL if the source widget does not support the format.        
        The source widget must supply a copy of the data, which is freed by the destination widget.
 */
void *algui_get_dragged_data(ALGUI_WIDGET *source, const char *format, ALGUI_DRAG_AND_DROP_TYPE type) {
    ALGUI_GET_DRAGGED_DATA_MESSAGE msg;    
    assert(format);
    assert(type == ALGUI_DRAG_AND_DROP_COPY || type == ALGUI_DRAG_AND_DROP_MOVE);    
    msg.message.id = ALGUI_MSG_GET_DRAGGED_DATA;
    msg.format = format;
    msg.type = type;
    msg.data = NULL;
    algui_send_message(source, &msg.message);
    return msg.data;
}


/** creates and starts an allegro timer for a widget.
    @param wgt widget to add the timer to.
    @param secs seconds, in timeout.
    @param queue event queue to associate the timer with.
    @return the allegro timer or NULL if the timer could not be created.
 */
ALLEGRO_TIMER *algui_create_widget_timer(ALGUI_WIDGET *wgt, double secs, ALLEGRO_EVENT_QUEUE *queue) {
    ALGUI_LIST_NODE *node;
    ALLEGRO_TIMER *timer;
    assert(wgt);
    assert(secs > 0);
    assert(queue);
    timer = al_create_timer(secs);
    if (!timer) return NULL;
    node = (ALGUI_LIST_NODE *)al_malloc(sizeof(ALGUI_LIST_NODE));
    assert(node);
    algui_init_list_node(node, timer);
    algui_append_list_node(&wgt->timers, node);
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_start_timer(timer);
    return timer;
}


/** stops, removes and destroys an allegro timer from a widget.
    @param wgt widget to remove the timer from.
    @param timer allegro timer to remove from the widget and then destroy.
    @return non-zero if the operation is successful, zero otherwise.
 */
int algui_destroy_widget_timer(ALGUI_WIDGET *wgt, ALLEGRO_TIMER *timer) {
    ALGUI_LIST_NODE *node;
    assert(wgt);
    node = _find_timer(&wgt->timers, timer);
    if (!node) return 0;
    _destroy_timer(&wgt->timers, node);
    return 1;
}


/** removes and destroys all timers in a widget.
    This function is called automatically from the default cleanup message handler.
    @param wgt widget to remove the timers from.
 */
void algui_destroy_widget_timers(ALGUI_WIDGET *wgt) {
    ALGUI_LIST_NODE *node, *next;
    assert(wgt);    
    for(node = algui_get_first_list_node(&wgt->timers); node;) {
        next = algui_get_next_list_node(node);
        _destroy_timer(&wgt->timers, node);
        node = next;
    }
}


/** allows the widgets in the tree to set themselves up from the given skin.
    Widgets receive a set-skin message.
    @param wgt root of tree to skin.
    @param skin skin.
 */
void algui_skin_widget(ALGUI_WIDGET *wgt, ALGUI_SKIN *skin) {
    ALGUI_SET_SKIN_MESSAGE msg;
    assert(skin);
    msg.message.id = ALGUI_MSG_SET_SKIN;
    msg.skin = skin;
    algui_broadcast_message(wgt, &msg.message);
}


/** sets the id of a widget.
    @param wgt widget to get the id of.
    @param id the new id of a widget.
 */
void algui_set_widget_id(ALGUI_WIDGET *wgt, const char *id) {
    assert(wgt);
    wgt->id = id;
}


/** sets the text of all widgets in the tree from a config file that contains translations.
    Widgets receive the set-translation message.
    @param wgt root of widget tree to set the translations of.
    @param config allegro config file with translations.
 */
void algui_set_translation(ALGUI_WIDGET *wgt, ALLEGRO_CONFIG *config) {
    ALGUI_SET_TRANSLATION_MESSAGE msg;
    msg.message.id = ALGUI_MSG_SET_TRANSLATION;
    msg.config = config;
    algui_broadcast_message(wgt, &msg.message);
}


/** sets the tab order of a widget.
    @param wgt widget to set the tab order of.
    @param tbo the widget's tab order.
 */
void algui_set_widget_tab_order(ALGUI_WIDGET *wgt, int tbo) {
    assert(wgt);
    wgt->tab_order = tbo;
}


/** moves the input focus backward within the given widget tree,
    depending on the tab order of widgets.
    @param wgt root of widget tree to move the focus backward into.
    @return non-zero if focus was changed, zero otherwise.
 */
int algui_move_focus_backward(ALGUI_WIDGET *wgt) {
    ALGUI_WIDGET *focus, *child_focus, *end;    
    focus = algui_get_focus_widget(wgt);    
    if (!focus) focus = wgt;    
    child_focus = NULL;    
    end = algui_get_parent_widget(wgt);
    while (focus != end) {
        if (_move_focus_backward_in_children(focus, child_focus)) return 1;        
        child_focus = focus;
        focus = algui_get_parent_widget(focus);
    }    
    return algui_set_focus_widget(wgt);
}


/** moves the input focus forward within the given widget tree,
    depending on the tab order of widgets.
    @param wgt root of widget tree to move the focus forward into.
    @return non-zero if focus was changed, zero otherwise.
 */
int algui_move_focus_forward(ALGUI_WIDGET *wgt) {
    ALGUI_WIDGET *focus, *child_focus, *end;    
    focus = algui_get_focus_widget(wgt);    
    if (!focus) focus = wgt;    
    child_focus = NULL;    
    end = algui_get_parent_widget(wgt);
    while (focus != end) {
        if (_move_focus_forward_in_children(focus, child_focus)) return 1;        
        child_focus = focus;
        focus = algui_get_parent_widget(focus);
    }    
    return algui_set_focus_widget(wgt);
}
