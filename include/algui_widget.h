#ifndef ALGUI_WIDGET_H
#define ALGUI_WIDGET_H


#include <allegro5/allegro.h>
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
    ALGUI_LIST timers;
    int capture:8;
    int drawn:1;
    int layout:1;
    int visible:1;
    int visible_tree:1;
    int enabled:1;
    int enabled_tree:1;
    int focus:1;
    int mouse:1;
    int data_source:1;
} ALGUI_WIDGET;


/** the default widget procedure.
    TODO explain which messages it processes.
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


/** sends a message to all widgets in the widget tree.
    It returns after the message has been processed by the widgets.
    @param wgt target widget.
    @param msg message to send to the widget.
    @return non-zero if the message was processed by any widget, zero otherwise.
 */
int algui_broadcast_message(ALGUI_WIDGET *wgt, ALGUI_MESSAGE *msg); 


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


/** returns the horizontal position of the widget.
    @param wgt widget to get the coordinate of.
    @return the horizontal position of the widget.
 */
int algui_get_widget_x(ALGUI_WIDGET *wgt); 


/** returns the vertical position of the widget.
    @param wgt widget to get the coordinate of.
    @return the vertical position of the widget.
 */
int algui_get_widget_y(ALGUI_WIDGET *wgt); 


/** returns the width of the widget.
    @param wgt widget to get the size of.
    @return the width of the widget.
 */
int algui_get_widget_width(ALGUI_WIDGET *wgt); 


/** returns the height of the widget.
    @param wgt widget to get the size of.
    @return the height of the widget.
 */
int algui_get_widget_height(ALGUI_WIDGET *wgt); 


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
int algui_widget_has_focus(ALGUI_WIDGET *wgt);


/** get the widget that has the focus.
    @param wgt widget to start the search from.
    @return the widget that has the focus or NULL if no widget has the focus.
 */
ALGUI_WIDGET *algui_get_focus_widget(ALGUI_WIDGET *wgt); 


/** returns true if the given widget has the mouse.
    @param wgt widget to get the mouse status of.
    @return non-zero if the widget has the mouse, zero otherwise.
 */
int algui_widget_has_mouse(ALGUI_WIDGET *wgt);


/** get the widget that has the mouse.
    @param wgt widget to start the search from.
    @return the widget that has the mouse or NULL if no widget has the mouse.
 */
ALGUI_WIDGET *algui_get_mouse_widget(ALGUI_WIDGET *wgt); 


/** returns the widget that is under the given point.
    Widgets receive a hit-test message.
    Invisible widgets are ignored.
    @param wgt root of widget tree to search.
    @param x horizontal coordinate, relative to the given widget.
    @param y vertical coordinate, relative to the given widget.
    @return the widget under the given coordinates, or NULL if there is one.
 */
ALGUI_WIDGET *algui_get_widget_from_point(ALGUI_WIDGET *wgt, int x, int y); 


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


/** translates a point from the coordinate system of one widget to the other.
    @param src source widget; if null, the input point is considered to be in screen coordinates.
    @param src_x input x in the source coordinate system; can be the source.
    @param src_y input y in the source coordinate system; can be the source.
    @param dst destination widget; if null, the output point is considered to be in screen coordinates.
    @param dst_x output x in the destination coordinate system; can be the source.
    @param dst_y output y in the destination coordinate system; can be the source.
 */
void algui_translate_point(ALGUI_WIDGET *src, int src_x, int src_y, ALGUI_WIDGET *dst, int *dst_x, int *dst_y); 


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


/** Calculates the layout of the given widget and its children.
    This may lead to further changes in other widgets up and down the tree, depending on layout.
    This function may be invoked after a widget modifies a visual attribute
    that changes the widget's position or size.
    There is no need to invoke this function before drawing the widgets
    for the first time; they will be packed automatically.
    @param wgt widget to start the layout management from.
 */
void algui_pack_widget(ALGUI_WIDGET *wgt); 


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


/** dispatch an Allegro event to a widget tree.
    The event is dispatched to the given widget or its children
    or to the widget that has captured events or its children.
    @param wgt root of widget tree to dispatch the event to.
    @param ev allegro event.
 */
void algui_dispatch_event(ALGUI_WIDGET *wgt, ALLEGRO_EVENT *ev); 


/** captures events.
    Until events are released, events are dispatched to the given
    widget or its children.
    Events must be released with the function algui_release_events.
    @param wgt widget to capture events.
    @return non-zero if the operation succeeded, 
        zero if it failed due to too many captures or 
        failure to remove the focus if the focus lies outside of the capture tree.
 */
int algui_capture_events(ALGUI_WIDGET *wgt);


/** releases events.
    Event control is given back to the previous widget
    that had captured events, or to the root widget of
    the widget tree that events are dispatched to.
    @param wgt widget to release events from.
    @return non-zero if the operation succeeded, zero if it failed due to the widget not having captured the events.
 */
int algui_release_events(ALGUI_WIDGET *wgt);


/** begins drag and drop with the given widget as the data source.
    After this call, widgets receive drag and drop events,
    until drag and drop ends.
    If there is a capture, then drag and drop events are delivered within the capture widget.
    The widget receives a begin-drag-and-drop message.
    @param source widget to initiate drag-and-drop.
    @return non-zero if the widget accepted the operation and there is no other source widget, zero otherwise.
 */
int algui_begin_drag_and_drop(ALGUI_WIDGET *source); 


/** Ends a drag-and-drop session.
    This function is called automatically when a mouse button is released
    during a drag-and-drop session.    
    The source widget receives a drag-and-drop-ended message.
    @param source the source widget.
    @return non-zero if the operation suceeded, zero if the widget was not the data source.
 */
int algui_end_drag_and_drop(ALGUI_WIDGET *source);


/** retrieves the data source widget.
    The data source widget must be in the same widget tree as the given widget.
    @param wgt widget tree to get the data source widget from.
    @return the data soruce widget.
 */
ALGUI_WIDGET *algui_get_drag_and_drop_source(ALGUI_WIDGET *wgt); 


/** queries the data source widget about the data type and operation.
    The data source widget receives a query-drag-and-drop message.
    @param source data source widget.
    @param format data format description (simple ASCII string).
    @param type drag-and-drop type.
    @return non-zero if the data source widget supports the given format and type, zero otherwise.
 */
int algui_query_dragged_data(ALGUI_WIDGET *source, const char *format, ALGUI_DRAG_AND_DROP_TYPE type); 


/** Retrieves the dragged data from the data source widget.
    The data source widget receives a get-drag-and-drop message.
    @param source data source widget.
    @param format data format description (simple ASCII string).
    @param type drag-and-drop type.
    @return pointer to the data or NULL if the source widget does not support the format.        
        The source widget must supply a copy of the data, which is freed by the destination widget.
 */
void *algui_get_dragged_data(ALGUI_WIDGET *source, const char *format, ALGUI_DRAG_AND_DROP_TYPE type); 


/** creates and starts an allegro timer for a widget.
    @param wgt widget to add the timer to.
    @param secs seconds, in timeout.
    @param queue event queue to associate the timer with.
    @return the allegro timer or NULL if the timer could not be created.
 */
ALLEGRO_TIMER *algui_create_widget_timer(ALGUI_WIDGET *wgt, double secs, ALLEGRO_EVENT_QUEUE *queue);


/** stops, removes and destroys an allegro timer from a widget.
    @param wgt widget to remove the timer from.
    @param timer allegro timer to remove from the widget and then destroy.
    @return non-zero if the operation is successful, zero otherwise.
 */
int algui_destroy_widget_timer(ALGUI_WIDGET *wgt, ALLEGRO_TIMER *timer);


/** stops, removes and destroys all timers in a widget.
    This function is called automatically from the default cleanup message handler.
    @param wgt widget to remove the timers from.
 */
void algui_destroy_widget_timers(ALGUI_WIDGET *wgt);


#endif //ALGUI_WIDGET_H
