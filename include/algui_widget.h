#ifndef ALGUI_WIDGET_H
#define ALGUI_WIDGET_H


#include "algui_message.h"
#include "algui_tree.h"


/** algui widget proc.
    @param wgt target widget.
    @param msg message.
    @return non-zero if message was processed, zero otherwise.
 */
typedef int (*ALGUI_WIDGET_PROC)(struct ALGUI_WIDGET *wgt, ALGUI_MESSAGE *msg); 


/** base struct for widgets.
 */
typedef struct ALGUI_WIDGET {
    ALGUI_WIDGET_PROC proc;
    ALGUI_TREE tree;
    ALGUI_RECT rect;
    ALGUI_RECT screen_rect;
    int visible:1;
    int visible_tree:1;
    int enabled:1;
    int enabled_tree:1;
    int screen_rect_valid:1;
    int has_focus:1;
} ALGUI_WIDGET;


/** the default widget procedure.
    It provides the default implementation for all the basic widget messages.
    @param wgt widget.
    @param msg message.
    @return non-zero if the message was processed, zero otherwise.
 */
int algui_widget_proc(ALGUI_WIDGET *wgt, ALGUI_MESSAGE *msg); 


/** sends a message to a widget.
    It returns after the message has been processed by the widget.
    @param wgt target widget.
    @param msg message to send to the widget.
    @return non-zero if the message was processed, zero otherwise.
 */
int algui_send_message(ALGUI_WIDGET *wgt, ALGUI_MESSAGE *msg); 


/** returns a widget's procedure.
    @param wgt widget to get the procedure of.
    @return the widget procedure.
 */
ALGUI_WIDGET_PROC algui_get_widget_proc(ALGUI_WIDGET *wgt);


/** returns the parent widget.
    @param wgt widget to get the parent of.
    @return the parent widget; it may be null.
 */
ALGUI_WIDGET *algui_get_parent_widget(ALGUI_WIDGET *wgt); 


/** returns the previous sibling.
    The sibling is lower in z-order.
    @param wgt widget to get the sibling of.
    @return the sibling; it may be null.
 */
ALGUI_WIDGET *algui_get_lower_sibling_widget(ALGUI_WIDGET *wgt); 


/** returns the next sibling.
    The sibling is higher in z-order.
    @param wgt widget to get the sibling of.
    @return the sibling; it may be null.
 */
ALGUI_WIDGET *algui_get_higher_sibling_widget(ALGUI_WIDGET *wgt); 


/** returns the first child widget.
    The sibling is lowest in the z-order of children.
    @param wgt widget to get the child of.
    @return the child; it may be null.
 */
ALGUI_WIDGET *algui_get_lowest_child_widget(ALGUI_WIDGET *wgt); 


/** returns the last child widget.
    The sibling is highest in the z-order of children.
    @param wgt widget to get the child of.
    @return the child; it may be null.
 */
ALGUI_WIDGET *algui_get_highest_child_widget(ALGUI_WIDGET *wgt); 


/** returns the root widget.
    @param wgt widget to get the root of.
    @return the root widget.
 */
ALGUI_WIDGET *algui_get_root_widget(ALGUI_WIDGET *wgt); 


/** returns the local rectangle of a widget.
    @param wgt widget to get the rectangle of.
    @return the widget's rectangle.
 */
ALGUI_RECT algui_get_widget_rect(ALGUI_WIDGET *wgt); 


/** returns the screen rectangle of a widget.
    @param wgt widget to get the rectangle of.
    @return the widget's rectangle.
 */
ALGUI_RECT algui_get_widget_screen_rect(ALGUI_WIDGET *wgt); 


/** returns a widget's visible status.
    @param wgt widget to get the status of.
    @return the widget status.
 */
int algui_is_widget_visible(ALGUI_WIDGET *wgt); 


/** returns a widget's enabled status.
    @param wgt widget to get the status of.
    @return the widget status.
 */
int algui_is_widget_enabled(ALGUI_WIDGET *wgt); 


/** returns a widget's enabled tree status.
    Returns true if this and all ancestor widgets are enabled.
    @param wgt widget to get the status of.
    @return the widget status.
 */
int algui_is_widget_tree_enabled(ALGUI_WIDGET *wgt); 


/** returns true if the given widget has the input focus.
    @param wgt widget to get the focus status of.
    @return non-zero if the widget has the focus, zero otherwise.
 */
int algui_has_widget_focus(ALGUI_WIDGET *wgt);


/** get the widget that has the focus.
    @param wgt widget to start the search from.
    @return the widget that has the focus or NULL if no widget has the focus.
 */
ALGUI_WIDGET *algui_get_focus_widget(ALGUI_WIDGET *wgt); 


/** initializes a widget structure.
    @param wgt widget to initialize.
    @param proc widget proc.
 */
void algui_init_widget(ALGUI_WIDGET *wgt, ALGUI_WIDGET_PROC proc);


/** cleans up a widget and its children.
    Every widget in the tree gets the cleanup message.
    Parent-child widget relationships are maintained.
    @param wgt widget to cleanup.
 */
void algui_cleanup_widget(ALGUI_WIDGET *wgt);


/** cleans up a widget and its children, and then frees the memory it occupies.
    Every widget in the tree is cleaned up, and then widgets are freed using the al_free function.
    @param wgt widget to cleanup.
 */
void algui_destroy_widget(ALGUI_WIDGET *wgt);


/** translates a rectangle from the coordinate system of one widget to the other.
    @param src source widget; if null, the input rect is considered to be in screen coordinates.
    @param src_rct source rectangle.
    @param dst destination widget; if null, the output rect is considered to be in screen coordinates.
    @param dst_rct destination rectangle; it may be the source.
 */
void algui_translate_rect(ALGUI_WIDGET *src, ALGUI_RECT *src_rct, ALGUI_WIDGET *dst, ALGUI_RECT *dst_rct); 


/** draws a part of a widget.
    Every widget in the tree gets the paint message.
    @param wgt widget to draw; its children are also drawn.
    @param rct local rectangle of widget to draw.
 */
void algui_draw_widget_rect(ALGUI_WIDGET *wgt, ALGUI_RECT *rct); 


/** draws a whole widget.
    Every widget in the tree gets the paint message.
    @param wgt widget to draw; its children are also drawn.
 */
void algui_draw_widget(ALGUI_WIDGET *wgt); 


/** inserts a widget in another widget as a child.
    @param parent parent; it receives the insert-widget message.
    @param child child.
    @param next next sibling; it can be null.
    @return non-zero if the operation succeeded, zero otherwise.
 */
int algui_insert_widget(ALGUI_WIDGET *parent, ALGUI_WIDGET *child, ALGUI_WIDGET *next); 


/** adds a widget in another widget as a child.
    @param parent parent; it receives the insert-widget message.
    @param child child.
    @return non-zero if the operation succeeded, zero otherwise.
 */
int algui_add_widget(ALGUI_WIDGET *parent, ALGUI_WIDGET *child);


/** removes a widget from its parent.
    @param parent parent; it receives the remove-widget message.
    @param child child to remove.
    @return non-zero if the operation succeeded, zero otherwise.
 */
int algui_remove_widget(ALGUI_WIDGET *parent, ALGUI_WIDGET *child); 


/** removes a widget from its parent.
    @param child widget to remove from its parent, if there is one.
 */
void algui_detach_widget(ALGUI_WIDGET *child); 


/** sets the local rectangle of a widget.
    The widget receives the set-rect message.
    @param wgt widget to set.
    @param rct the widget's new local rectangle.
 */
void algui_set_widget_rect(ALGUI_WIDGET *wgt, ALGUI_RECT *rct); 


/** sets the widget's rectangle by setting its position and size.
    The widget receives the set-rect message.
    @param wgt widget to set.
    @param x new horizontal position.
    @param y new vertical position.
    @param w new width.
    @param h new height.
 */
void algui_move_and_resize_widget(ALGUI_WIDGET *wgt, int x, int y, int w, int h); 


/** sets the widget's rectangle by setting its position.
    The widget receives the set-rect message.
    @param wgt widget to set.
    @param x new horizontal position.
    @param y new vertical position.
 */
void algui_move_widget(ALGUI_WIDGET *wgt, int x, int y); 


/** sets the widget's rectangle by setting its size.
    The widget receives the set-rect message.
    @param wgt widget to set.
    @param w new width.
    @param h new height.
 */
void algui_resize_widget(ALGUI_WIDGET *wgt, int w, int h); 


/** sets the widget's visible state.
    The widget receives the set-visible message.
    @param wgt widget.
    @param state new state; non-zero for visible, zero for invisible.
    @return non-zero if the operation succeeded, zero otherwise.
 */
int algui_set_widget_visible(ALGUI_WIDGET *wgt, int visible);


/** sets the widget's visible state to true.
    The widget receives the set-visible message.
    @param wgt widget.
    @return non-zero if the operation succeeded, zero otherwise.
 */ 
int algui_show_widget(ALGUI_WIDGET *wgt);


/** sets the widget's visible state to false.
    The widget receives the set-visible message.
    @param wgt widget.
    @return non-zero if the operation succeeded, zero otherwise.
 */ 
int algui_hide_widget(ALGUI_WIDGET *wgt);


/** sets the widget's enabled state.
    The widget receives the set-enabled message.
    @param wgt widget.
    @param state new state; non-zero for enabled, zero for disabled.
    @return non-zero if the operation succeeded, zero otherwise.
 */
int algui_set_widget_enabled(ALGUI_WIDGET *wgt, int enabled);


/** sets the widget's enabled state to true.
    The widget receives the set-enabled message.
    @param wgt widget.
    @return non-zero if the operation succeeded, zero otherwise.
 */ 
int algui_enable_widget(ALGUI_WIDGET *wgt);


/** sets the widget's enabled state to false.
    The widget receives the set-enabled message.
    @param wgt widget.
    @return non-zero if the operation succeeded, zero otherwise.
 */ 
int algui_disable_widget(ALGUI_WIDGET *wgt);


/** sets the widget proc of a widget.
    @param wgt widget to set.
    @param proc new widget proc.
 */
void algui_set_widget_proc(ALGUI_WIDGET *wgt, ALGUI_WIDGET_PROC proc); 


/** sets the input focus to the given widget.
    The given widget receives a get-focus message;
    if the widget accepts the focus, then the previous focus widget, if there is one,
    receives a lose-focus message.
    If both widgets return ok, then the previous widget get a lost-focus message,
    and the new widget gets a got-focus message.
    @param wgt wgt to set the focus to.
    @return non-zero if the operation is successful, zero otherwise.
 */
int algui_set_focus_widget(ALGUI_WIDGET *wgt); 


#endif //ALGUI_WIDGET_H
