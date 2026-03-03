#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <gst/gst.h>
#include <unistd.h>
#include "lvgl.h"
#include <SDL2/SDL.h>

#include "anim/lv_example_anim.h"
#include "event/lv_example_event.h"
#include "get_started/lv_example_get_started.h"
#include "layouts/lv_example_layout.h"
#include "libs/lv_example_libs.h"
#include "others/lv_example_others.h"
#include "porting/osal/lv_example_osal.h"
#include "scroll/lv_example_scroll.h"
#include "styles/lv_example_style.h"
#include "widgets/lv_example_widgets.h"
#include "grad/lv_example_grad.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#if LV_BUILD_EXAMPLES

#if LV_USE_GSTREAMER
typedef struct {
    lv_obj_t * streamer;
    lv_obj_t * pp_button;
    lv_obj_t * button_label;
    lv_obj_t * position_label;
    lv_obj_t * duration_label;
    lv_subject_t position_subject;
} event_data_t;

static void play_pause_pressed(lv_event_t * e);
static void stream_state_changed(lv_event_t * e);

/**
 * Loads a video from the internet using the gstreamer widget
 */
void lv_example_gstreamer_1(void)
{
    static event_data_t event_data;

    event_data.streamer = lv_gstreamer_create(lv_screen_active());

    /* the gstreamer widget inherits the `lv_image` widget,
     * meaning you can also provide it lv_image functions, like
    lv_image_set_scale(event_data.streamer, 100);
    lv_image_set_rotation(event_data.streamer, 100);
    */

    /* Set the current src of the streamer.
     * Using the `URI` "factory", we can
     * specify various URI schemes as media sources including local files (file://),
     * web streams (http://, https://), RTSP streams (rtsp://), UDP streams (udp://),
     * and many others. GStreamer's uridecodebin automatically selects the appropriate
     * source element and decoder based on the URI scheme and media format. */
    lv_gstreamer_set_src(event_data.streamer, LV_GSTREAMER_FACTORY_WEBRTCSRC , LV_GSTREAMER_PROPERTY_WEBRTCSRC,
                         "");

//    lv_gstreamer_set_src(event_data.streamer, LV_GSTREAMER_FACTORY_WEBRTCSRC, NULL,"");


    lv_obj_center(event_data.streamer);

    /* The LV_EVENT_STATE_CHANGED will fire when the stream is ready at that point we can query the stream
     * information like its resolution and duration. See `streamer_ready` */
    lv_obj_add_event_cb(event_data.streamer, stream_state_changed, LV_EVENT_STATE_CHANGED, &event_data);

    /* Play the stream immediately */
    lv_gstreamer_play(event_data.streamer);

}


static void play_pause_pressed(lv_event_t * e)
{
    event_data_t * event_data = (event_data_t *)lv_event_get_user_data(e);

    if(lv_streq(lv_label_get_text(event_data->button_label), LV_SYMBOL_REFRESH)) {
        lv_gstreamer_set_position(event_data->streamer, 0);
        lv_gstreamer_play(event_data->streamer);
    }
    else if(lv_streq(lv_label_get_text(event_data->button_label), LV_SYMBOL_PLAY)) {
        lv_gstreamer_play(event_data->streamer);
    }
    else {
        lv_gstreamer_pause(event_data->streamer);
    }
}
static void stream_state_changed(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    event_data_t * event_data = (event_data_t *)lv_event_get_user_data(e);

    lv_obj_t * streamer = event_data->streamer;

    if(code != LV_EVENT_STATE_CHANGED) {
        return;
    }

    lv_gstreamer_stream_state_t stream_state = lv_gstreamer_get_stream_state(e);
    switch(stream_state) {
        case LV_GSTREAMER_STREAM_STATE_START: {
                uint32_t duration = lv_gstreamer_get_duration(streamer);
                LV_LOG_USER("Stream is starting");
                LV_LOG_USER("\tStream resolution %" LV_PRId32 "x%" LV_PRId32, lv_image_get_src_width(streamer),
                            lv_image_get_src_height(streamer));
                LV_LOG_USER("\tStream duration %" LV_PRIu32, duration);
                break;
            }
        case LV_GSTREAMER_STREAM_STATE_END:
            LV_LOG_USER("Stream is over");
            break;
        case LV_GSTREAMER_STREAM_STATE_PLAY:
            LV_LOG_USER("Stream is playing");
            break;
        case LV_GSTREAMER_STREAM_STATE_PAUSE:
            LV_LOG_USER("Stream is paused");
            break;
        case LV_GSTREAMER_STREAM_STATE_STOP:
            LV_LOG_USER("Stream is stopped");
            break;
    }
}

#endif
#endif

int main(int argc, char *argv[])
{
    /* Initialize LVGL */
    lv_init();

    
    /* Initialize display driver */
    lv_display_t * display = NULL;
    
    /* Create Wayland display  */
    display = lv_wayland_window_create(800, 600, "LVGL GStreamer Example", NULL);
    if (!display) {
        LV_LOG_USER("Failed to create display");
        return 1;
    }

    /* Create simple UI only if display is available */
    if (lv_screen_active()) {
        LV_LOG_USER("Screen is active, creating UI");
        lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x003a57), LV_PART_MAIN);

        lv_obj_t * label = lv_label_create(lv_screen_active());
        lv_label_set_text(label, "Hello LVGL with GStreamer");
        lv_obj_set_style_text_color(label, lv_color_hex(0xffffff), LV_PART_MAIN);
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
        
        /* Run the GStreamer example */
        lv_example_gstreamer_1();
        
        /* Main loop (simplified - in production you'd want a proper event loop) */
        LV_LOG_USER("Entering main loop");
        while(1) {
            lv_timer_handler();
        }
    }
    else {
        LV_LOG_USER("No active screen available");
    }
    
    return 0;
}
