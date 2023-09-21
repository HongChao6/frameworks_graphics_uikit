/**
 * @file launcher.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "page.h"


#define APP_ENTRY_CNT 7
/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    const void * name;
    const char * text;
    uint32_t bg_color;
} app_entry_t;

typedef struct {
    lv_fragment_t base;
    lv_style_t cont_style;
    lv_style_t icon_style;
    lv_style_t label_style;
    app_entry_t app_entry[APP_ENTRY_CNT];
} page_ctx_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define APP_ICON_SIZE 100
#define APP_ICON_PAD_VER (APP_ICON_SIZE / 2)
#define APP_ICON_ANIM_TIME 200

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void app_icon_click_anim(lv_obj_t * img, bool ispress)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, img);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_img_set_zoom);
    lv_anim_set_values(
        &a,
        lv_img_get_zoom(img),
        ispress ? (int)(256 * 0.8f) : 256);
    lv_anim_set_time(&a, APP_ICON_ANIM_TIME);

    lv_anim_set_path_cb(&a, ispress ? lv_anim_path_ease_in_out : lv_anim_path_overshoot);

    lv_anim_start(&a);
}

static void on_app_icon_event(lv_event_t * e)
{
    page_ctx_t * ctx = lv_event_get_user_data(e);
    lv_obj_t * icon = lv_event_get_current_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * img = lv_obj_get_child(icon, 0);

    switch(code) {
        case LV_EVENT_PRESSED:
            app_icon_click_anim(img, true);
            break;
        case LV_EVENT_RELEASED:
        case LV_EVENT_PRESS_LOST:
            app_icon_click_anim(img, false);
            break;
        case LV_EVENT_CLICKED: {
                const app_entry_t * entry = lv_obj_get_user_data(icon);

                page_push(&ctx->base, entry->name, NULL);
            }
            break;
        default:
            break;
    }
}

static void app_icon_create(page_ctx_t * ctx, lv_obj_t * par, const app_entry_t * entry)
{
    lv_obj_t * cont = lv_obj_create(par);
    lv_obj_remove_style_all(cont);
    lv_obj_add_style(cont, &ctx->cont_style, 0);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t * icon = lv_obj_create(cont);
    lv_style_t * trans_style = resource_get_style("btn_trans");
    lv_obj_remove_style_all(icon);
    lv_obj_add_style(icon, &ctx->icon_style, 0);

    lv_obj_set_style_bg_color(icon, lv_palette_main(LV_PALETTE_GREY), LV_STATE_PRESSED);
    lv_obj_set_style_transform_width(icon, 5, LV_STATE_PRESSED);
    lv_obj_set_style_transform_height(icon, -5, LV_STATE_PRESSED);
    lv_obj_add_style(icon, trans_style, LV_STATE_PRESSED);
    lv_obj_add_style(icon, trans_style, LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(icon, lv_color_hex(entry->bg_color), 0);
    lv_obj_set_user_data(icon, (void *)entry);
    lv_obj_add_event(icon, on_app_icon_event, LV_EVENT_ALL, ctx);
    lv_obj_clear_flag(icon, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * img = lv_img_create(icon);
    lv_img_set_src(img, resource_get_img(entry->name));
    lv_obj_center(img);

    lv_obj_t * label = lv_label_create(cont);
    lv_obj_add_style(label, &ctx->label_style, 0);
    lv_label_set_text(label, entry->text);
}

static void on_page_construct(lv_fragment_t * self, void * args)
{
    LV_LOG_INFO("self: %p args: %p", self, args);
    page_ctx_t * ctx = (page_ctx_t *)self;

    lv_style_init(&ctx->cont_style);
    lv_style_set_width(&ctx->cont_style, LV_PCT(100));
    lv_style_set_height(&ctx->cont_style, LV_SIZE_CONTENT);

    lv_style_init(&ctx->icon_style);
    lv_style_set_bg_opa(&ctx->icon_style, LV_OPA_COVER);
    lv_style_set_size(&ctx->icon_style, APP_ICON_SIZE, APP_ICON_SIZE);
    lv_style_set_radius(&ctx->icon_style, 10);

    lv_style_init(&ctx->label_style);
    lv_style_set_text_font(&ctx->label_style, resource_get_font("erasbd_23"));
    lv_style_set_text_color(&ctx->label_style, lv_color_white());

    app_entry_t app_entry[APP_ENTRY_CNT] = {
        { "heart_rate", "Heart", 0xFF542A },
        { "music", "Music", 0xEE4C84 },
        { "stop_watch", "StWatch", 0x3CFFA7 },
        { "flashlight", "Light", 0xFFB428 },
        { "sleep", "Sleep", 0x001BC8 },
        { "sport", "Sport", 0x9B5DFF },
        { "settings", "Settings", 0x0089FF },
    };

    for(int i = 0; i < APP_ENTRY_CNT; i++) {
        ctx->app_entry[i] = app_entry[i];
    }

}

static void on_page_destruct(lv_fragment_t * self)
{
    LV_LOG_INFO("self: %p", self);
    page_ctx_t * ctx = (page_ctx_t *)self;
    lv_style_reset(&ctx->cont_style);
    lv_style_reset(&ctx->icon_style);
    lv_style_reset(&ctx->label_style);
}

static void on_page_attached(lv_fragment_t * self)
{
    LV_LOG_INFO("self: %p", self);
}

static void on_page_detached(lv_fragment_t * self)
{
    LV_LOG_INFO("self: %p", self);
}

static lv_obj_t * on_page_create(lv_fragment_t * self, lv_obj_t * container)
{
    LV_LOG_INFO("self: %p container: %p", self, container);

    lv_obj_t * root = lv_obj_create(container);
    lv_obj_remove_style_all(root);
    lv_obj_add_style(root, resource_get_style("root_def"), 0);
    return root;
}

static void on_page_created(lv_fragment_t * self, lv_obj_t * obj)
{
    LV_LOG_INFO("self: %p obj: %p", self, obj);

    page_ctx_t * ctx = (page_ctx_t *)self;

    /* root */
    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(obj, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(obj, 20, 0);
    lv_obj_set_style_pad_ver(obj, APP_ICON_PAD_VER, 0);

    /* apps */
    for(int i = 0; i < APP_ENTRY_CNT; i++) {
        app_icon_create(
            ctx,
            obj,
            &ctx->app_entry[i]);
    }

    /* shadow image */
    {
        lv_obj_t * img1 = lv_img_create(obj);
        lv_img_set_src(img1, resource_get_img("icon_shadow_up"));
        lv_obj_align(img1, LV_ALIGN_TOP_MID, 0, -APP_ICON_PAD_VER);
        lv_obj_add_flag(img1, LV_OBJ_FLAG_FLOATING);

        lv_obj_t * img2 = lv_img_create(obj);
        lv_img_set_src(img2, resource_get_img("icon_shadow_down"));
        lv_obj_align(img2, LV_ALIGN_BOTTOM_MID, 0, APP_ICON_PAD_VER);
        lv_obj_add_flag(img2, LV_OBJ_FLAG_FLOATING);
    }
}

static void on_page_will_delete(lv_fragment_t * self, lv_obj_t * obj)
{
    LV_LOG_INFO("self: %p obj: %p", self, obj);
}

static void on_page_deleted(lv_fragment_t * self, lv_obj_t * obj)
{
    LV_LOG_INFO("self: %p obj: %p", self, obj);
}

static bool on_page_event(lv_fragment_t * self, int code, void * user_data)
{
    LV_LOG_INFO("self: %p code: %d user_data: %p", self, code, user_data);
    return false;
}

PAGE_CLASS_DEF(launcher);