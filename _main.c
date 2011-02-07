#include <stdarg.h>
#include <stdio.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include "algui_widget.h"


static int rnd(int i) {
	return rand() % i;
}


//test widget
typedef struct TEST_WIDGET {
	ALGUI_WIDGET widget;
	unsigned keycode;
	unsigned unichar;
} TEST_WIDGET;


//font to draw the widget
ALLEGRO_FONT *font = 0;


//draws text with a background color
static void draw_text(ALLEGRO_FONT *font, ALLEGRO_COLOR fg, ALLEGRO_COLOR bg, float x1, float y1, float x2, float y2, const char *buf) {
	al_draw_filled_rectangle(x1, y1, x2, y2, bg);
	al_draw_text(font, fg, x1, y1, 0, buf);
}


//get the message line from message id
static int _msg_line(int id) {
    if (id >= ALGUI_MSG_LEFT_BUTTON_DOWN && id <= ALGUI_MSG_UNUSED_KEY_CHAR)
        return id - ALGUI_MSG_LEFT_BUTTON_DOWN;

    if (id >= ALGUI_MSG_LEFT_DROP && id <= ALGUI_MSG_DRAG_KEY_CHAR)
        return id - ALGUI_MSG_LEFT_DROP;
        
    return -1;        
}


//draws a message in a widget
static void draw_msg(ALGUI_WIDGET *wgt, ALGUI_MESSAGE *msg, const char *format, ...) {
	char buf[512];
	va_list params;	
	ALGUI_RECT rct;

	//get params	
	va_start(params, format);
	vsnprintf(buf, sizeof(buf), format, params);
	va_end(params);

	//get rect
	rct = algui_get_widget_screen_rect(wgt);
	
	//set rect
	rct.top += al_get_font_line_height(font) * _msg_line(msg->id);
	rct.right = rct.left + 200;
	rct.bottom = rct.top + al_get_font_line_height(font) - 1;
	
 	//draw text
	draw_text(
		font, 
		al_map_rgb(128+rnd(128), 128+rnd(128), 128+rnd(128)), al_map_rgb(0, 0, 0), 
		rct.left, rct.top, rct.right, rct.bottom,
		buf);

	//update the display		
	al_flip_display();
}


//test paint
static int _test_paint(ALGUI_WIDGET *wgt, ALGUI_PAINT_MESSAGE *msg) {
	TEST_WIDGET *test = (TEST_WIDGET *)wgt;
    ALGUI_RECT *pos = &msg->widget_rect;
    ALLEGRO_COLOR bg = algui_widget_has_focus(wgt) ? al_map_rgb(255, 255, 0) : al_map_rgb(255, 255, 255);
    int border = algui_widget_has_focus(wgt) ? 3 : 1;
    al_draw_filled_rectangle(pos->left + 0.5f, pos->top + 0.5f, pos->right + 0.5f, pos->bottom + 0.5f, bg);
    al_draw_rectangle(pos->left + 0.5f, pos->top + 0.5f, pos->right + 0.5f, pos->bottom + 0.5f, al_map_rgb(0, 0, 0), border);    
    al_draw_textf(font, al_map_rgb(0, 0, 0), pos->left, pos->top, 0, "%i %c", test->keycode - ALLEGRO_KEY_1 + 1, test->unichar);
    return 1;
} 


//test left button down.
static int _test_left_button_down(ALGUI_WIDGET *wgt, ALGUI_LEFT_BUTTON_DOWN_MESSAGE *msg) {	
	draw_msg(wgt, &msg->message, "LEFT BUTTON DOWN(x=%i,y=%i)", msg->x, msg->y);	
	algui_set_focus_widget(wgt);
	algui_capture_events(wgt);
	return 1;
}


//test left button up.
static int _test_left_button_up(ALGUI_WIDGET *wgt, ALGUI_LEFT_BUTTON_UP_MESSAGE *msg) {
	draw_msg(wgt, &msg->message, "LEFT BUTTON UP(x=%i,y=%i)", msg->x, msg->y);	
	algui_release_events(wgt);
	return 1;
}


//test middle button down.
static int _test_middle_button_down(ALGUI_WIDGET *wgt, ALGUI_MIDDLE_BUTTON_DOWN_MESSAGE *msg) {
	draw_msg(wgt, &msg->message, "MIDDLE BUTTON DOWN(x=%i,y=%i)", msg->x, msg->y);	
	algui_set_focus_widget(wgt);
	algui_capture_events(wgt);
	return 1;
}


//test middle button up.
static int _test_middle_button_up(ALGUI_WIDGET *wgt, ALGUI_MIDDLE_BUTTON_UP_MESSAGE *msg) {
	draw_msg(wgt, &msg->message, "MIDDLE BUTTON UP(x=%i,y=%i)", msg->x, msg->y);	
	algui_release_events(wgt);
	return 1;
}


//test right button down.
static int _test_right_button_down(ALGUI_WIDGET *wgt, ALGUI_RIGHT_BUTTON_DOWN_MESSAGE *msg) {
	draw_msg(wgt, &msg->message, "RIGHT BUTTON DOWN(x=%i,y=%i)", msg->x, msg->y);	
	algui_begin_drag_and_drop(wgt);
	return 1;
}


//test right button up.
static int _test_right_button_up(ALGUI_WIDGET *wgt, ALGUI_RIGHT_BUTTON_UP_MESSAGE *msg) {
	draw_msg(wgt, &msg->message, "RIGHT BUTTON UP(x=%i,y=%i)", msg->x, msg->y);	
	return 1;
}


//test mouse enter
static int _test_mouse_enter(ALGUI_WIDGET *wgt, ALGUI_MOUSE_ENTER_MESSAGE *msg) {
	draw_msg(wgt, &msg->message, "MOUSE ENTER(x=%i,y=%i)", msg->x, msg->y);	
	return 1;
}


//test mouse move
static int _test_mouse_move(ALGUI_WIDGET *wgt, ALGUI_MOUSE_MOVE_MESSAGE *msg) {
	draw_msg(wgt, &msg->message, "MOUSE MOVE(x=%i,y=%i)", msg->x, msg->y);	
	return 1;
}


//test mouse leave
static int _test_mouse_leave(ALGUI_WIDGET *wgt, ALGUI_MOUSE_LEAVE_MESSAGE *msg) {
	draw_msg(wgt, &msg->message, "MOUSE LEAVE(x=%i,y=%i)", msg->x, msg->y);	
	return 1;
}


//test mouse wheel
static int _test_mouse_wheel(ALGUI_WIDGET *wgt, ALGUI_MOUSE_WHEEL_MESSAGE *msg) {
	draw_msg(wgt, &msg->message, "MOUSE WHEEL(z=%i,w=%i)", msg->z, msg->w);	
	return 1;
}


//test key down
static int _test_key_down(ALGUI_WIDGET *wgt, ALGUI_KEY_DOWN_MESSAGE *msg) {
	TEST_WIDGET *test = (TEST_WIDGET *)wgt;
	if (msg->keycode == test->keycode) {
		draw_msg(wgt, &msg->message, "KEY DOWN(keycode=%i)", msg->keycode);	
		return 1;
	}
	return 0;
}


//test key up
static int _test_key_up(ALGUI_WIDGET *wgt, ALGUI_KEY_UP_MESSAGE *msg) {
	TEST_WIDGET *test = (TEST_WIDGET *)wgt;
	if (msg->keycode == test->keycode) {
		draw_msg(wgt, &msg->message, "KEY UP(keycode=%i)", msg->keycode);	
		return 1;
	}
	return 0;
}


//test unused key down
static int _test_unused_key_down(ALGUI_WIDGET *wgt, ALGUI_UNUSED_KEY_DOWN_MESSAGE *msg) {
	TEST_WIDGET *test = (TEST_WIDGET *)wgt;
	if (msg->keycode == test->keycode) {
		draw_msg(wgt, &msg->message, "UNUSED KEY DOWN(keycode=%i)", msg->keycode);	
		algui_set_focus_widget(wgt);		
		return 1;
	}
	return 0;
}


//test unused key up
static int _test_unused_key_up(ALGUI_WIDGET *wgt, ALGUI_UNUSED_KEY_UP_MESSAGE *msg) {
	TEST_WIDGET *test = (TEST_WIDGET *)wgt;
	if (msg->keycode == test->keycode) {
		draw_msg(wgt, &msg->message, "UNUSED KEY UP(keycode=%i)", msg->keycode);	
		return 1;
	}
	return 0;
}


//test character
static int _test_key_char(ALGUI_WIDGET *wgt, ALGUI_KEY_CHAR_MESSAGE *msg) {
	TEST_WIDGET *test = (TEST_WIDGET *)wgt;
	if (msg->unichar == test->unichar) {
		draw_msg(wgt, &msg->message, "KEY CHAR(unichar=%c)", msg->unichar);	
		return 1;
	}
	return 0;
}


//test unused key char
static int _test_unused_key_char(ALGUI_WIDGET *wgt, ALGUI_UNUSED_KEY_CHAR_MESSAGE *msg) {
	TEST_WIDGET *test = (TEST_WIDGET *)wgt;
	if (msg->unichar == test->unichar) {
		draw_msg(wgt, &msg->message, "UNUSED KEY CHAR(unichar=%c)", msg->unichar);	
		algui_set_focus_widget(wgt);		
		return 1;
	}
	return 0;
}


//test left drop
static int _test_left_drop(ALGUI_WIDGET *wgt, ALGUI_LEFT_DROP_MESSAGE *msg) {
	draw_msg(wgt, &msg->message, "LEFT DROP(x=%i,y=%i)", msg->x, msg->y);	
    return 1;
}


//test middle drop.
static int _test_middle_drop(ALGUI_WIDGET *wgt, ALGUI_MIDDLE_DROP_MESSAGE *msg) {
	draw_msg(wgt, &msg->message, "MIDDLE DROP(x=%i,y=%i)", msg->x, msg->y);	
    return 1;
}


//test drop
static int _test_right_drop(ALGUI_WIDGET *wgt, ALGUI_RIGHT_DROP_MESSAGE *msg) {
	draw_msg(wgt, &msg->message, "RIGHT DROP(x=%i,y=%i)", msg->x, msg->y);	
    return 1;
}


//test drag enter
static int _test_drag_enter(ALGUI_WIDGET *wgt, ALGUI_DRAG_ENTER_MESSAGE *msg) {
	draw_msg(wgt, &msg->message, "DRAG ENTER(x=%i,y=%i)", msg->x, msg->y);	
    return 1;
}


//test drag move
static int _test_drag_move(ALGUI_WIDGET *wgt, ALGUI_DRAG_MOVE_MESSAGE *msg) {
	draw_msg(wgt, &msg->message, "DRAG MOVE(x=%i,y=%i)", msg->x, msg->y);	
    return 1;
}


//test drag leave
static int _test_drag_leave(ALGUI_WIDGET *wgt, ALGUI_DRAG_LEAVE_MESSAGE *msg) {
	draw_msg(wgt, &msg->message, "DRAG LEAVE(x=%i,y=%i)", msg->x, msg->y);	
    return 1;
}


//test drag wheel
static int _test_drag_wheel(ALGUI_WIDGET *wgt, ALGUI_DRAG_WHEEL_MESSAGE *msg) {
	draw_msg(wgt, &msg->message, "DRAG WHEEL(x=%i,y=%i)", msg->x, msg->y);	
    return 1;
}


//test drag key down
static int _test_drag_key_down(ALGUI_WIDGET *wgt, ALGUI_DRAG_KEY_DOWN_MESSAGE *msg) {
	draw_msg(wgt, &msg->message, "DRAG KEY DOWN(keycode=%i)", msg->keycode);	
    return 1;
}


//test drag-and-drop key up
static int _test_drag_key_up(ALGUI_WIDGET *wgt, ALGUI_DRAG_KEY_UP_MESSAGE *msg) {
	draw_msg(wgt, &msg->message, "DRAG KEY UP(keycode=%i)", msg->keycode);	
    return 1;
}


//test drag-and-drop character
static int _test_drag_key_char(ALGUI_WIDGET *wgt, ALGUI_DRAG_KEY_CHAR_MESSAGE *msg) {
	draw_msg(wgt, &msg->message, "DRAG KEY CHAR(unichar=%c)", msg->unichar);	
    return 1;
}


//begin drag and drop
static int _test_begin_drag_and_drop(ALGUI_WIDGET *wgt, ALGUI_BEGIN_DRAG_AND_DROP_MESSAGE *msg) {
    msg->ok = 1;
    return 1;
}


//test widget proc
static int test_widget_proc(ALGUI_WIDGET *wgt, ALGUI_MESSAGE *msg) {
    switch (msg->id) {
		//paint
        case ALGUI_MSG_PAINT:        
            return _test_paint(wgt, (ALGUI_PAINT_MESSAGE *)msg);
            
		//left button down.
		case ALGUI_MSG_LEFT_BUTTON_DOWN:
			return _test_left_button_down(wgt, (ALGUI_LEFT_BUTTON_DOWN_MESSAGE *)msg);
	    
		//left button up.
		case ALGUI_MSG_LEFT_BUTTON_UP:
			return _test_left_button_up(wgt, (ALGUI_LEFT_BUTTON_UP_MESSAGE *)msg);
	    
		//middle button down.
		case ALGUI_MSG_MIDDLE_BUTTON_DOWN:
			return _test_middle_button_down(wgt, (ALGUI_MIDDLE_BUTTON_DOWN_MESSAGE *)msg);
	    
		//middle button up.
		case ALGUI_MSG_MIDDLE_BUTTON_UP:
			return _test_middle_button_up(wgt, (ALGUI_MIDDLE_BUTTON_UP_MESSAGE *)msg);
	    
		//right button down.
		case ALGUI_MSG_RIGHT_BUTTON_DOWN:
			return _test_right_button_down(wgt, (ALGUI_RIGHT_BUTTON_DOWN_MESSAGE *)msg);
	    
		//right button up.
		case ALGUI_MSG_RIGHT_BUTTON_UP:
			return _test_right_button_up(wgt, (ALGUI_RIGHT_BUTTON_UP_MESSAGE *)msg);
	    
		//mouse enter
		case ALGUI_MSG_MOUSE_ENTER:
			return _test_mouse_enter(wgt, (ALGUI_MOUSE_ENTER_MESSAGE *)msg);
	    
		//mouse move
		case ALGUI_MSG_MOUSE_MOVE:
			return _test_mouse_move(wgt, (ALGUI_MOUSE_MOVE_MESSAGE *)msg);
	    
		//mouse leave
		case ALGUI_MSG_MOUSE_LEAVE:
			return _test_mouse_leave(wgt, (ALGUI_MOUSE_LEAVE_MESSAGE *)msg);
	    
		//mouse wheel
		case ALGUI_MSG_MOUSE_WHEEL:
			return _test_mouse_wheel(wgt, (ALGUI_MOUSE_WHEEL_MESSAGE *)msg);
	    
		//key down
		case ALGUI_MSG_KEY_DOWN:
			return _test_key_down(wgt, (ALGUI_KEY_DOWN_MESSAGE *)msg);
	    
		//key up
		case ALGUI_MSG_KEY_UP:
			return _test_key_up(wgt, (ALGUI_KEY_UP_MESSAGE *)msg);
	    
		//unused key down
		case ALGUI_MSG_UNUSED_KEY_DOWN:
			return _test_unused_key_down(wgt, (ALGUI_UNUSED_KEY_DOWN_MESSAGE *)msg);
	    
		//unused key up
		case ALGUI_MSG_UNUSED_KEY_UP:
			return _test_unused_key_up(wgt, (ALGUI_UNUSED_KEY_UP_MESSAGE *)msg);
	    
		//character
		case ALGUI_MSG_KEY_CHAR:
			return _test_key_char(wgt, (ALGUI_KEY_CHAR_MESSAGE *)msg);
	    
		//unused key char
		case ALGUI_MSG_UNUSED_KEY_CHAR:
			return _test_unused_key_char(wgt, (ALGUI_UNUSED_KEY_CHAR_MESSAGE *)msg);

		//left drop.
		case ALGUI_MSG_LEFT_DROP:
			return _test_left_drop(wgt, (ALGUI_LEFT_DROP_MESSAGE *)msg);
	    
		//middle drop.
		case ALGUI_MSG_MIDDLE_DROP:
			return _test_middle_drop(wgt, (ALGUI_MIDDLE_DROP_MESSAGE *)msg);
	    
		//right drop
		case ALGUI_MSG_RIGHT_DROP:
			return _test_right_drop(wgt, (ALGUI_RIGHT_DROP_MESSAGE *)msg);
	    
		//drag enter
		case ALGUI_MSG_DRAG_ENTER:
			return _test_drag_enter(wgt, (ALGUI_DRAG_ENTER_MESSAGE *)msg);
	    
		//drag move
		case ALGUI_MSG_DRAG_MOVE:
			return _test_drag_move(wgt, (ALGUI_DRAG_MOVE_MESSAGE *)msg);
	    
		//drag leave
		case ALGUI_MSG_DRAG_LEAVE:
			return _test_drag_leave(wgt, (ALGUI_DRAG_LEAVE_MESSAGE *)msg);
	    
		//drag wheel
		case ALGUI_MSG_DRAG_WHEEL:
			return _test_drag_wheel(wgt, (ALGUI_DRAG_WHEEL_MESSAGE *)msg);
	    
		//drag key down
		case ALGUI_MSG_DRAG_KEY_DOWN:
			return _test_drag_key_down(wgt, (ALGUI_DRAG_KEY_DOWN_MESSAGE *)msg);
	    
		//drag-and-drop key up
		case ALGUI_MSG_DRAG_KEY_UP:
			return _test_drag_key_up(wgt, (ALGUI_DRAG_KEY_UP_MESSAGE *)msg);
	    
		//drag-and-drop character
		case ALGUI_MSG_DRAG_KEY_CHAR:
			return _test_drag_key_char(wgt, (ALGUI_DRAG_KEY_CHAR_MESSAGE *)msg);	    
			
        //begin drag and drop
        case ALGUI_MSG_BEGIN_DRAG_AND_DROP:
            return _test_begin_drag_and_drop(wgt, (ALGUI_BEGIN_DRAG_AND_DROP_MESSAGE *)msg);			
    }
    return algui_widget_proc(wgt, msg);
}


//init the test widget
static void init_test_widget(TEST_WIDGET *wgt, unsigned keycode, unsigned unichar) {
	algui_init_widget(&wgt->widget, test_widget_proc);
	wgt->keycode = keycode;
	wgt->unichar = unichar;
}


int main() {
    ALLEGRO_DISPLAY *display;
    ALLEGRO_EVENT_QUEUE *queue;
    ALLEGRO_EVENT event;
    TEST_WIDGET root, form1, form2, form3, btn1, btn2, btn3;
    
    al_init();
    al_install_keyboard();
    al_install_mouse();
    al_init_image_addon();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();
    
    al_set_new_display_flags(al_get_new_display_flags() | ALLEGRO_GENERATE_EXPOSE_EVENTS);
    display = al_create_display(640, 480);
    assert(display);        
    
    queue = al_create_event_queue();
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));
    
    font = al_load_ttf_font("test/data/LiberationMono-Regular.ttf", 12, ALLEGRO_TTF_MONOCHROME);
    assert(font);
    
    init_test_widget(&root, ALLEGRO_KEY_1, 'A');
    init_test_widget(&form1, ALLEGRO_KEY_2, 'B');
    init_test_widget(&form2, ALLEGRO_KEY_3, 'C');
    init_test_widget(&form3, ALLEGRO_KEY_4, 'D');
    init_test_widget(&btn1, ALLEGRO_KEY_5, 'E');
    init_test_widget(&btn2, ALLEGRO_KEY_6, 'F');
    init_test_widget(&btn3, ALLEGRO_KEY_7, 'G');
    
    algui_add_widget(&root.widget, &form1.widget);
    algui_add_widget(&root.widget, &form2.widget);
    algui_add_widget(&root.widget, &form3.widget);
    algui_add_widget(&form2.widget, &btn1.widget);
    algui_add_widget(&form2.widget, &btn2.widget);
    algui_add_widget(&form2.widget, &btn3.widget);
    
    algui_move_and_resize_widget(&root.widget, 0, 0, 640, 480);
    algui_move_and_resize_widget(&form1.widget, 100, 50, 250, 200);
    algui_move_and_resize_widget(&form2.widget, 200, 150, 250, 200);
    algui_move_and_resize_widget(&form3.widget, 300, 250, 250, 200);
    algui_move_and_resize_widget(&btn1.widget, 50, 40, 50, 40);
    algui_move_and_resize_widget(&btn2.widget, 70, 60, 50, 40);
    algui_move_and_resize_widget(&btn3.widget, 90, 80, 50, 40);
    
    for(;;) {
        al_wait_for_event(queue, &event);
        
        switch (event.type) {
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                goto END;
                
            case ALLEGRO_EVENT_KEY_DOWN:
                if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) goto END;
                algui_dispatch_event(&root.widget, &event);
                break;

            case ALLEGRO_EVENT_DISPLAY_EXPOSE:
                algui_dispatch_event(&root.widget, &event);
                al_flip_display();
                break;            
                
			default:                                
                algui_dispatch_event(&root.widget, &event);
        }
    }
    
    END:
   
    al_destroy_event_queue(queue);  
    al_destroy_display(display);
   
    return 0;
}
