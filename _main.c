#include <stdarg.h>
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include "algui.h"


int main() {
    //allegro stuff
    ALLEGRO_DISPLAY *al_display;
    ALLEGRO_EVENT_QUEUE *queue;
    ALLEGRO_EVENT event;    
    ALLEGRO_TIMER *loop_timer;
    int need_draw = 1;
    
    //widgets
    ALGUI_DISPLAY *display;
    
    //other
    ALGUI_SKIN *skin;
    
    /**** INIT ****/
    
    //init allegro
    al_init();
    al_install_keyboard();
    al_install_mouse();
    al_init_image_addon();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();
    
    //init algui
    algui_init();
    
    /**** CREATE ALLEGRO RESOURCES ****/
    
    //set flags
    al_set_new_display_flags(al_get_new_display_flags() | ALLEGRO_GENERATE_EXPOSE_EVENTS | ALLEGRO_RESIZABLE | ALLEGRO_WINDOWED);
    
    //create a al_display
    al_display = al_create_display(640, 480);
    assert(al_display);        
    
    //create timers and events
    queue = al_create_event_queue();    
    loop_timer = al_create_timer(1.0 / 60.0);    
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_display_event_source(al_display));
    al_register_event_source(queue, al_get_timer_event_source(loop_timer));    
    
    /**** CREATE WIDGETS ****/
    
    //create the display widget
    display = algui_create_display();
    algui_resize_widget(algui_get_display_widget(display), 640, 480);
    
    //skin widgets
    skin = algui_load_skin("test/test-skin/test-skin.txt");
    algui_skin_widget(algui_get_display_widget(display), skin);
        
    /**** MANAGE EVENTS ****/
    
    //event loop        
    al_start_timer(loop_timer);
    for(;;) {
        //wait for event
        al_wait_for_event(queue, &event);
        
        switch (event.type) {
            //close
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                goto END;
                
            //keydown                
            case ALLEGRO_EVENT_KEY_DOWN:
                if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) goto END;
                algui_dispatch_event(algui_get_display_widget(display), &event);
                break;                                

            //redraw
            case ALLEGRO_EVENT_DISPLAY_EXPOSE:
                need_draw = 1;
                break;            
                
            //remove the input focus                
            case ALLEGRO_EVENT_DISPLAY_SWITCH_OUT:
                algui_dispatch_event(algui_get_display_widget(display), &event);
                need_draw = 1;
                break;            
                        
            //handle timers
            case ALLEGRO_EVENT_TIMER:
                if (event.timer.source == loop_timer) need_draw = 1;
                else algui_dispatch_event(algui_get_display_widget(display), &event);
                break;
                
            //dispatch event to the widgets                
			default:                                
                algui_dispatch_event(algui_get_display_widget(display), &event);
                break;
        }

        if (need_draw && al_is_event_queue_empty(queue)) {
            algui_draw_widget(algui_get_display_widget(display));
            al_flip_display();
            need_draw = false;
        }                
    }
    
    /**** CLEANUP ****/
    
    END:

    //cleanup the gui
    algui_destroy_skin(skin);
    algui_destroy_widget(algui_get_display_widget(display));
    algui_cleanup();
    
    //cleanup allegro
    al_destroy_timer(loop_timer);
    al_destroy_event_queue(queue);  
    al_destroy_display(al_display);
   
    return 0;
}
