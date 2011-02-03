#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include "algui_widget.h"


static int _paint(ALGUI_WIDGET *wgt, ALGUI_PAINT_MESSAGE *msg) {
    ALGUI_RECT *pos = &msg->widget_rect;
    ALLEGRO_COLOR bg = algui_has_widget_focus(wgt) ? al_map_rgb(255, 255, 0) : al_map_rgb(255, 255, 255);
    int border = algui_has_widget_focus(wgt) ? 3 : 1;
    al_draw_filled_rectangle(pos->left + 0.5f, pos->top + 0.5f, pos->right + 0.5f, pos->bottom + 0.5f, bg);
    al_draw_rectangle(pos->left + 0.5f, pos->top + 0.5f, pos->right + 0.5f, pos->bottom + 0.5f, al_map_rgb(0, 0, 0), border);    
    return 1;
} 


static int test_widget_proc(ALGUI_WIDGET *wgt, ALGUI_MESSAGE *msg) {
    switch (msg->id) {
        case ALGUI_MSG_PAINT:
            return _paint(wgt, (ALGUI_PAINT_MESSAGE *)msg);
    }
    return algui_widget_proc(wgt, msg);
}


static int _set_preferred_rect(ALGUI_WIDGET *wgt, ALGUI_SET_PREFERRED_RECT_MESSAGE *msg) {
    int w = 0, h = 0;
    ALGUI_WIDGET *child;
    
    for(child = algui_get_lowest_child_widget(wgt); child; child = algui_get_higher_sibling_widget(child)) {
        w += algui_get_widget_width(child);
        h = h > algui_get_widget_height(child) ? h : algui_get_widget_height(child);
    }
    
    algui_resize_widget(wgt, w+2, h+2);
    
    return 1;
}


static int _do_layout(ALGUI_WIDGET *wgt, ALGUI_DO_LAYOUT_MESSAGE *msg) {
    ALGUI_WIDGET *child;
    int x = 1, y = 1, w, h = algui_get_widget_height(wgt) - 2;

    for(child = algui_get_lowest_child_widget(wgt); child; child = algui_get_higher_sibling_widget(child)) {
        w = algui_get_widget_width(child);
        algui_move_and_resize_widget(child, x, 1, w, h);
        x += w;
    }    
    
    return 1;
}


static int box_widget_proc(ALGUI_WIDGET *wgt, ALGUI_MESSAGE *msg) {
    switch (msg->id) {
        case ALGUI_MSG_SET_PREFERRED_RECT:
            return _set_preferred_rect(wgt, (ALGUI_SET_PREFERRED_RECT_MESSAGE *)msg);
        case ALGUI_MSG_DO_LAYOUT:
            return _do_layout(wgt, (ALGUI_DO_LAYOUT_MESSAGE *)msg);
    }
    return test_widget_proc(wgt, msg);
}


int main() {
    ALLEGRO_DISPLAY *display;
    ALLEGRO_EVENT_QUEUE *queue;
    ALLEGRO_EVENT event;
    ALGUI_WIDGET root, form1, form2, form3, btn1, btn2, btn3;
    
    al_init();
    al_install_keyboard();
    al_install_mouse();
    al_init_image_addon();
    al_init_font_addon();
    al_init_primitives_addon();
    
    al_set_new_display_flags(al_get_new_display_flags() | ALLEGRO_GENERATE_EXPOSE_EVENTS);
    display = al_create_display(640, 480);
    assert(display);
    
    queue = al_create_event_queue();
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));
    
    algui_init_widget(&root, test_widget_proc);
    algui_init_widget(&form1, test_widget_proc);
    algui_init_widget(&form2, box_widget_proc);
    algui_init_widget(&form3, test_widget_proc);
    algui_init_widget(&btn1, test_widget_proc);
    algui_init_widget(&btn2, test_widget_proc);
    algui_init_widget(&btn3, test_widget_proc);
    
    algui_add_widget(&root, &form1);
    algui_add_widget(&root, &form2);
    algui_add_widget(&root, &form3);
    algui_add_widget(&form2, &btn1);
    algui_add_widget(&form2, &btn2);
    algui_add_widget(&form2, &btn3);
    
    algui_move_and_resize_widget(&root, 0, 0, 640, 480);
    algui_move_and_resize_widget(&form1, 100, 50, 250, 200);
    algui_move_and_resize_widget(&form2, 200, 150, 250, 200);
    algui_move_and_resize_widget(&form3, 300, 250, 250, 200);
    algui_move_and_resize_widget(&btn1, 50, 40, 50, 40);
    algui_move_and_resize_widget(&btn2, 70, 60, 50, 40);
    algui_move_and_resize_widget(&btn3, 90, 80, 50, 40);
    
    for(;;) {
        al_wait_for_event(queue, &event);
        
        switch (event.type) {
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                goto END;
                
            case ALLEGRO_EVENT_KEY_DOWN:
                if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) goto END;
                if (event.keyboard.keycode == ALLEGRO_KEY_1) {
                    algui_set_focus_widget(&btn1);    
                    algui_draw_widget(&root);
                    al_flip_display();
                }
                if (event.keyboard.keycode == ALLEGRO_KEY_2) {
                    algui_set_focus_widget(&btn2);    
                    algui_draw_widget(&root);
                    al_flip_display();
                }
                if (event.keyboard.keycode == ALLEGRO_KEY_3) {
                    algui_set_focus_widget(&btn3);    
                    algui_draw_widget(&root);
                    al_flip_display();
                }
                break;

            case ALLEGRO_EVENT_DISPLAY_EXPOSE:
                algui_draw_widget(&root);
                al_flip_display();
                break;                            
        }
    }
    
    END:
   
    al_destroy_event_queue(queue);  
    al_destroy_display(display);
   
    return 0;
}
