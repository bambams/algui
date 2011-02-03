#include "algui_rect.h"
#include <assert.h>


/******************************************************************************
    PRIVATE    
 ******************************************************************************/
 
 
#ifndef MIN
#define MIN(A, B)  ((A) < (B) ? (A) : (B))
#endif


#ifndef MAX
#define MAX(A, B)  ((A) > (B) ? (A) : (B))
#endif


/******************************************************************************
    PUBLIC
 ******************************************************************************/


/** calculates the width of a rectangle.
    @param rct rectangle to calculate the width of.
    @return the rectangle's width.
 */
int algui_get_rect_width(ALGUI_RECT *rct) {
    assert(rct);
    return rct->right - rct->left + 1;
}


/** calculates the height of a rectangle.
    @param rct rectangle to calculate the height of.
    @return the rectangle's height.
 */
int algui_get_rect_height(ALGUI_RECT *rct) {
    assert(rct);
    return rct->bottom - rct->top + 1;
}


/** calculates the horizontal center of a rectangle.
    @param rct rectangle to calculate the center of.
    @return the rectangle's horizontal center.
 */
int algui_get_rect_cx(ALGUI_RECT *rct) {
    assert(rct);
    return rct->left + algui_get_rect_width(rct) / 2;
} 


/** calculates the vertical center of a rectangle.
    @param rct rectangle to calculate the center of.
    @return the rectangle's vertical center.
 */
int algui_get_rect_cy(ALGUI_RECT *rct) {
    assert(rct);
    return rct->top + algui_get_rect_height(rct) / 2;
}  


/** checks if a point intersects a rectangle.
    @param rct rectangle to check.
    @param x horizontal point coordinate.
    @param y vertical point coordinate.
    @return non-zero if the point is inside the rectangle, zero otherwise.
 */
int algui_rect_intersects_point(ALGUI_RECT *rct, int x, int y) {
    assert(rct);
    return x >= rct->left && y >= rct->top && x <= rct->right && y <= rct->bottom;
}


/** checks if a rectangle intersects a rectangle.
    @param r1 1st rectangle to check.
    @param r2 2nd rectangle to check.
    @return non-zero if the two rectangles intersect, zero otherwise.
 */
int algui_rect_intersects_rect(ALGUI_RECT *r1, ALGUI_RECT *r2) {
    assert(r1);
    assert(r2);
    return r1->left <= r2->right && r1->top <= r2->bottom && r1->right >= r2->left && r1->bottom >= r2->top;
}


/** checks if a rectangle is normalized,
    i.e. if left and top less than or equal right and bottom coordinates,
    respectively.
    @param rct rectangle to check.
    @return non-zero if the rectangle is normalized, zero otherwise.
 */
int algui_is_rect_normalized(ALGUI_RECT *rct) {
    assert(rct);
    return rct->left <= rct->right && rct->top <= rct->bottom;
}


/** checks if a rectangle is equal to another rectangle.
    @param r1 1st rectangle.
    @param r2 2nd rectangle.
    @return non-zero if the rectangles are equal, zero otherwise.
 */
int algui_is_rect_equal_to_rect(ALGUI_RECT *r1, ALGUI_RECT *r2) {
    assert(r1);
    assert(r2);
    return r1->left   == r2->left  && 
           r1->top    == r2->top   && 
           r1->right  == r2->right && 
           r1->bottom == r2->bottom;
}


/** sets a rectangle.
    @param rct rectangle to set.
    @param left new left coordinate.
    @param top new top coordinate.
    @param right new right coordinate.
    @param bottom new bottom coordinate.
 */
void algui_set_rect(ALGUI_RECT *rct, int left, int top, int right, int bottom) {
    assert(rct);
    rct->left   = left  ;
    rct->top    = top   ;
    rct->right  = right ;
    rct->bottom = bottom;
}


/** moves and resizes a rectangle.
    @param rct rectangle to set.
    @param x new left coordinate.
    @param y new top coordinate.
    @param w new width.
    @param h new height.
 */
void algui_move_and_resize_rect(ALGUI_RECT *rct, int x, int y, int w, int h) {
    assert(rct);
    rct->left   = x;
    rct->top    = y;
    rct->right  = x + w - 1;
    rct->bottom = y + h - 1;
}


/** moves a rectangle.
    @param rct rectangle to set.
    @param x new left coordinate.
    @param y new top coordinate.
 */
void algui_move_rect(ALGUI_RECT *rct, int x, int y) {
    algui_move_and_resize_rect(rct, x, y, algui_get_rect_width(rct), algui_get_rect_height(rct));
}


/** resizes a rectangle.
    @param rct rectangle to set.
    @param w new width.
    @param h new height.
 */
void algui_resize_rect(ALGUI_RECT *rct, int w, int h) {
    assert(rct);
    rct->right  = rct->left + w - 1;
    rct->bottom = rct->top  + h - 1;
}


/** moves a rectangle relative to its current position.
    @param rct rectangle to set.
    @param x new left coordinate.
    @param y new top coordinate.
 */
void algui_offset_rect(ALGUI_RECT *rct, int x, int y) {
    assert(rct);
    rct->left   += x;
    rct->top    += y;
    rct->right  += x;
    rct->bottom += y; 
}


/** calculates the intersection of two rectangles.
    @param r1 1st rectangle; it can be the result.
    @param r2 2nd rectangle; it can be the result.
    @param r result.
 */
void algui_get_rect_intersection(ALGUI_RECT *r1, ALGUI_RECT *r2, ALGUI_RECT *r) {
    assert(r1);
    assert(r2);
    assert(r);
    r->left   = MAX(r1->left  , r2->left  );
    r->top    = MAX(r1->top   , r2->top   );
    r->right  = MIN(r1->right , r2->right );
    r->bottom = MIN(r1->bottom, r2->bottom);
}


/** calculates the union of two rectangles.
    @param r1 1st rectangle; it can be the result.
    @param r2 2nd rectangle; it can be the result.
    @param r result.
 */
void algui_get_rect_union(ALGUI_RECT *r1, ALGUI_RECT *r2, ALGUI_RECT *r) {
    assert(r1);
    assert(r2);
    assert(r);
    r->left   = MIN(r1->left  , r2->left  );
    r->top    = MIN(r1->top   , r2->top   );
    r->right  = MAX(r1->right , r2->right );
    r->bottom = MAX(r1->bottom, r2->bottom);
}

