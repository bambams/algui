#include "algui_widget.h"
#include <assert.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>


/******************************************************************************
    PRIVATE
 ******************************************************************************/
 
 
//updates the screen rectangle of a widget, if needed
static void _update_screen_rect(ALGUI_WIDGET *wgt) {
    ALGUI_WIDGET *parent, *child;

    //the screen rect is already valid
    if (wgt->screen_rect_valid) return;
    
    parent = algui_get_parent_widget(wgt);
    
    //if the widget is child
    if (parent) {
        wgt->screen_rect = wgt->rect;
        algui_offset_rect(&wgt->screen_rect, parent->screen_rect.left, parent->screen_rect.top);
    }
    
    //else it is a root widget
    else {
        wgt->screen_rect = wgt->rect;
    }
    
    wgt->screen_rect_valid = 1;
    
    //invalidate children
    for(child = algui_get_lowest_child_widget(wgt); child; child = algui_get_higher_sibling_widget(child)) {
        child->screen_rect_valid = 0;
    }
}


//recursively draw widgets
static void _draw(ALGUI_WIDGET *wgt, ALGUI_RECT *rect) {
    ALGUI_PAINT_MESSAGE msg;
    ALGUI_WIDGET *child;

    //avoid invisible widgets
    if (!wgt->visible_tree) return;
    
    //update the widget's screen rect, if needed
    _update_screen_rect(wgt);
    
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


//updates the visible and enabled tree flags of the widget tree
static void _update_visible_and_enabled_tree(ALGUI_WIDGET *wgt) {
    ALGUI_WIDGET *parent, *child;    
    parent = algui_get_parent_widget(wgt);
    wgt->visible_tree = wgt->visible && (!parent || parent->visible_tree);
    wgt->enabled_tree = wgt->enabled && (!parent || parent->enabled_tree);
    for(child = algui_get_highest_child_widget(wgt); child; child = algui_get_higher_sibling_widget(child)) {
        _update_visible_and_enabled_tree(child);
    }
} 
 
 
//cleanup
static int _cleanup(ALGUI_WIDGET *wgt, ALGUI_CLEANUP_MESSAGE *msg) {
    assert(wgt);
    assert(msg);
    return 1;
} 


//paint
static int _paint(ALGUI_WIDGET *wgt, ALGUI_PAINT_MESSAGE *msg) {
    ALGUI_RECT *pos = &msg->widget_rect;
    ALLEGRO_COLOR bg = algui_has_widget_focus(wgt) ? al_map_rgb(255, 255, 0) : al_map_rgb(255, 255, 255);
    int border = algui_has_widget_focus(wgt) ? 3 : 1;

    //draw a white box
    al_draw_filled_rectangle(pos->left + 0.5f, pos->top + 0.5f, pos->right + 0.5f, pos->bottom + 0.5f, bg);
    
    //draw a black border
    al_draw_rectangle(pos->left + 0.5f, pos->top + 0.5f, pos->right + 0.5f, pos->bottom + 0.5f, al_map_rgb(0, 0, 0), border);
    
    return 1;
} 


//insert widget
static int _insert_widget(ALGUI_WIDGET *wgt, ALGUI_INSERT_WIDGET_MESSAGE *msg) {
    assert(wgt);
    assert(msg);
    assert(msg->child);
    msg->ok = algui_insert_tree(&wgt->tree, &msg->child->tree, msg->next ? &msg->next->tree : NULL);
    if (msg->ok) {
        wgt->screen_rect_valid = 0;
        _update_visible_and_enabled_tree(wgt);
    }        
    return 1;
} 


//remove widget
static int _remove_widget(ALGUI_WIDGET *wgt, ALGUI_REMOVE_WIDGET_MESSAGE *msg) {
    assert(wgt);
    assert(msg);
    assert(msg->child);
    msg->ok = algui_remove_tree(&wgt->tree, &msg->child->tree);
    if (msg->ok) {
        wgt->screen_rect_valid = 0;
        _update_visible_and_enabled_tree(wgt);
    }        
    return 1;
} 


//set rect
static int _set_rect(ALGUI_WIDGET *wgt, ALGUI_SET_RECT_MESSAGE *msg) {
    assert(wgt);
    assert(msg);
    if (algui_is_rect_equal_to_rect(&wgt->rect, &msg->rect)) return 1;
    wgt->rect = msg->rect;
    wgt->screen_rect_valid = 0;
    return 1;
} 


//set visible
static int _set_visible(ALGUI_WIDGET *wgt, ALGUI_SET_VISIBLE_MESSAGE *msg) {
    ALGUI_WIDGET *focus;

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
    _update_visible_and_enabled_tree(wgt);
    msg->ok = 1;
    
    return 1;
} 


//set enabled
static int _set_enabled(ALGUI_WIDGET *wgt, ALGUI_SET_ENABLED_MESSAGE *msg) {
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
    _update_visible_and_enabled_tree(wgt);
    msg->ok = 1;
    
    return 1;
} 


//get focus
static int _get_focus(ALGUI_WIDGET *wgt, ALGUI_GET_FOCUS_MESSAGE *msg) {
    assert(wgt);
    assert(msg);
    msg->ok = 1;
    return 1;
} 


//lose focus
static int _lose_focus(ALGUI_WIDGET *wgt, ALGUI_LOSE_FOCUS_MESSAGE *msg) {
    assert(wgt);
    assert(msg);
    msg->ok = 1;
    return 1;
} 


//got focus
static int _got_focus(ALGUI_WIDGET *wgt, ALGUI_GOT_FOCUS_MESSAGE *msg) {
    assert(wgt);
    assert(msg);
    return 1;
} 


//lost focus
static int _lost_focus(ALGUI_WIDGET *wgt, ALGUI_LOST_FOCUS_MESSAGE *msg) {
    assert(wgt);
    assert(msg);
    return 1;
} 


/******************************************************************************
    PUBLIC
 ******************************************************************************/


/** the default widget procedure.
    It provides the default implementation for all the basic widget messages.
    @param wgt widget.
    @param msg message.
    @return non-zero if the message was processed, zero otherwise.
 */
int algui_widget_proc(ALGUI_WIDGET *wgt, ALGUI_MESSAGE *msg) {
    switch (msg->id) {
        case ALGUI_MSG_CLEANUP      : return _cleanup(wgt, (ALGUI_CLEANUP_MESSAGE *)msg);
        case ALGUI_MSG_PAINT        : return _paint(wgt, (ALGUI_PAINT_MESSAGE *)msg);
        case ALGUI_MSG_INSERT_WIDGET: return _insert_widget(wgt, (ALGUI_INSERT_WIDGET_MESSAGE *)msg);
        case ALGUI_MSG_REMOVE_WIDGET: return _remove_widget(wgt, (ALGUI_REMOVE_WIDGET_MESSAGE *)msg);
        case ALGUI_MSG_SET_RECT     : return _set_rect(wgt, (ALGUI_SET_RECT_MESSAGE *)msg);
        case ALGUI_MSG_SET_VISIBLE  : return _set_visible(wgt, (ALGUI_SET_VISIBLE_MESSAGE *)msg);
        case ALGUI_MSG_SET_ENABLED  : return _set_enabled(wgt, (ALGUI_SET_ENABLED_MESSAGE *)msg);
        case ALGUI_MSG_GET_FOCUS    : return _get_focus(wgt, (ALGUI_GET_FOCUS_MESSAGE *)msg);
        case ALGUI_MSG_LOSE_FOCUS   : return _lose_focus(wgt, (ALGUI_LOSE_FOCUS_MESSAGE *)msg);
        case ALGUI_MSG_GOT_FOCUS    : return _got_focus(wgt, (ALGUI_GOT_FOCUS_MESSAGE *)msg);
        case ALGUI_MSG_LOST_FOCUS   : return _lost_focus(wgt, (ALGUI_LOST_FOCUS_MESSAGE *)msg);
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
int algui_has_widget_focus(ALGUI_WIDGET *wgt) {
    assert(wgt);
    return wgt->has_focus;
}


/** get the widget that has the focus.
    @param wgt widget to start the search from.
    @return the widget that has the focus or NULL if no widget has the focus.
 */
ALGUI_WIDGET *algui_get_focus_widget(ALGUI_WIDGET *wgt) {
    ALGUI_WIDGET *child, *result;
    assert(wgt);
    if (wgt->has_focus) return wgt;
    for(child = algui_get_highest_child_widget(wgt); child; child = algui_get_lower_sibling_widget(child)) {
        result = algui_get_focus_widget(child);
        if (result) return result;
    }
    return NULL;
}


/** initializes a widget structure.
    @param wgt widget to initialize.
    @param proc widget proc.
 */
void algui_init_widget(ALGUI_WIDGET *wgt, ALGUI_WIDGET_PROC proc) {
    assert(wgt);
    assert(proc);
    wgt->proc = proc;
    algui_init_tree(&wgt->tree, wgt);
    algui_set_rect(&wgt->rect, 0, 0, 0, 0);
    algui_set_rect(&wgt->screen_rect, 0, 0, 0, 0);
    wgt->visible = 1;
    wgt->visible_tree = 1;
    wgt->enabled = 1;
    wgt->enabled_tree = 1;
    wgt->screen_rect_valid = 0;    
    wgt->has_focus = 0;
}


/** cleans up a widget and its children.
    Every widget in the tree gets the cleanup message.
    Parent-child widget relationships are maintained.
    @param wgt widget to cleanup.
 */
void algui_cleanup_widget(ALGUI_WIDGET *wgt) {
    ALGUI_CLEANUP_MESSAGE msg;
    msg.message.id = ALGUI_MSG_CLEANUP;
    algui_send_message(wgt, &msg.message);
}


/** cleans up a widget and its children, and then frees the memory it occupies.
    Every widget in the tree is cleaned up, and then widgets are freed using the al_free function.
    @param wgt widget to cleanup.
 */
void algui_destroy_widget(ALGUI_WIDGET *wgt) {
    algui_cleanup_widget(wgt);
    _destroy(wgt);
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
        _update_screen_rect(src);
        algui_offset_rect(&tmp, src->screen_rect.left, src->screen_rect.top);
    }
    
    //translate coordinates from screen to destination
    if (dst) {
        _update_screen_rect(dst);
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
    assert(wgt);
    _update_screen_rect(wgt);
    _draw(wgt, &wgt->screen_rect);
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
    ALGUI_WIDGET *root, *prev;
    ALGUI_GET_FOCUS_MESSAGE get_focus_msg;
    ALGUI_LOSE_FOCUS_MESSAGE lose_focus_msg;
    ALGUI_GOT_FOCUS_MESSAGE got_focus_msg;
    ALGUI_LOST_FOCUS_MESSAGE lost_focus_msg;

    assert(wgt);
    
    //a disabled or invisible widget cannot get the input focus
    if (!wgt->enabled_tree || !wgt->visible_tree) return 0;
    
    //ask the widget if it accepts the input focus
    get_focus_msg.message.id = ALGUI_MSG_GET_FOCUS;
    get_focus_msg.ok = 0;
    algui_send_message(wgt, &get_focus_msg.message);
    if (!get_focus_msg.ok) return 0;
    
    //get the widget that currently has the input focus
    root = algui_get_root_widget(wgt);
    prev = algui_get_focus_widget(root);
    
    //if another widget has the input focus, then ask it if it accepts losing the input focus
    if (prev) {
        lose_focus_msg.message.id = ALGUI_MSG_LOSE_FOCUS;
        lose_focus_msg.ok = 0;
        algui_send_message(wgt, &lose_focus_msg.message);
        if (!lose_focus_msg.ok) return 0;

        //notify the previous widget that it has lost the input focus
        prev->has_focus = 0;
        lost_focus_msg.message.id = ALGUI_MSG_LOST_FOCUS;
        algui_send_message(wgt, &lost_focus_msg.message);
    }

    //notify the current widget that it has the input focus
    wgt->has_focus = 1;
    got_focus_msg.message.id = ALGUI_MSG_GOT_FOCUS;
    algui_send_message(wgt, &got_focus_msg.message);
        
    //success
    return 1;
}
