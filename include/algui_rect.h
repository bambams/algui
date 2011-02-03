#ifndef ALGUI_RECT_H
#define ALGUI_RECT_H


/** rectangle.
 */
typedef struct ALGUI_RECT {
    ///left coordinate.
    int left;
    
    ///top coordinate
    int top;
    
    ///right coordinate
    int right;
    
    ///bottom coordinate
    int bottom;
} ALGUI_RECT; 


/** calculates the width of a rectangle.
    @param rct rectangle to calculate the width of.
    @return the rectangle's width.
 */
int algui_get_rect_width(ALGUI_RECT *rct); 


/** calculates the height of a rectangle.
    @param rct rectangle to calculate the height of.
    @return the rectangle's height.
 */
int algui_get_rect_height(ALGUI_RECT *rct); 


/** calculates the horizontal center of a rectangle.
    @param rct rectangle to calculate the center of.
    @return the rectangle's horizontal center.
 */
int algui_get_rect_cx(ALGUI_RECT *rct); 


/** calculates the vertical center of a rectangle.
    @param rct rectangle to calculate the center of.
    @return the rectangle's vertical center.
 */
int algui_get_rect_cy(ALGUI_RECT *rct); 


/** checks if a point intersects a rectangle.
    @param rct rectangle to check.
    @param x horizontal point coordinate.
    @param y vertical point coordinate.
    @return non-zero if the point is inside the rectangle, zero otherwise.
 */
int algui_rect_intersects_point(ALGUI_RECT *rct, int x, int y); 


/** checks if a rectangle intersects a rectangle.
    @param r1 1st rectangle to check.
    @param r2 2nd rectangle to check.
    @return non-zero if the two rectangles intersect, zero otherwise.
 */
int algui_rect_intersects_rect(ALGUI_RECT *r1, ALGUI_RECT *r2); 


/** checks if a rectangle is normalized,
    i.e. if left and top less than or equal right and bottom coordinates,
    respectively.
    @param rct rectangle to check.
    @return non-zero if the rectangle is normalized, zero otherwise.
 */
int algui_is_rect_normalized(ALGUI_RECT *rct); 


/** checks if a rectangle is equal to another rectangle.
    @param r1 1st rectangle.
    @param r2 2nd rectangle.
    @return non-zero if the rectangles are equal, zero otherwise.
 */
int algui_is_rect_equal_to_rect(ALGUI_RECT *r1, ALGUI_RECT *r2);


/** sets a rectangle.
    @param rct rectangle to set.
    @param left new left coordinate.
    @param top new top coordinate.
    @param right new right coordinate.
    @param bottom new bottom coordinate.
 */
void algui_set_rect(ALGUI_RECT *rct, int left, int top, int right, int bottom);


/** moves and resizes a rectangle.
    @param rct rectangle to set.
    @param x new left coordinate.
    @param y new top coordinate.
    @param w new width.
    @param h new height.
 */
void algui_move_and_resize_rect(ALGUI_RECT *rct, int x, int y, int w, int h); 


/** moves a rectangle.
    @param rct rectangle to set.
    @param x new left coordinate.
    @param y new top coordinate.
 */
void algui_move_rect(ALGUI_RECT *rct, int x, int y); 


/** resizes a rectangle.
    @param rct rectangle to set.
    @param w new width.
    @param h new height.
 */
void algui_resize_rect(ALGUI_RECT *rct, int w, int h); 


/** moves a rectangle relative to its current position.
    @param rct rectangle to set.
    @param x new left coordinate.
    @param y new top coordinate.
 */
void algui_offset_rect(ALGUI_RECT *rct, int x, int y); 


/** calculates the intersection of two rectangles.
    @param r1 1st rectangle; it can be the result.
    @param r2 2nd rectangle; it can be the result.
    @param r result.
 */
void algui_get_rect_intersection(ALGUI_RECT *r1, ALGUI_RECT *r2, ALGUI_RECT *r); 


/** calculates the union of two rectangles.
    @param r1 1st rectangle; it can be the result.
    @param r2 2nd rectangle; it can be the result.
    @param r result.
 */
void algui_get_rect_union(ALGUI_RECT *r1, ALGUI_RECT *r2, ALGUI_RECT *r); 


#endif //ALGUI_RECT_H
