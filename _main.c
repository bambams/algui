#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include "algui_widget.h"


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
    
    algui_init_widget(&root, algui_widget_proc);
    algui_init_widget(&form1, algui_widget_proc);
    algui_init_widget(&form2, algui_widget_proc);
    algui_init_widget(&form3, algui_widget_proc);
    algui_init_widget(&btn1, algui_widget_proc);
    algui_init_widget(&btn2, algui_widget_proc);
    algui_init_widget(&btn3, algui_widget_proc);
    
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
