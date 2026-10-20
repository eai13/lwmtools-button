#ifndef LWMTOOLS_BUTTON_COMMON_DEFS_H
#define LWMTOOLS_BUTTON_COMMON_DEFS_H

#include <stdint.h>

// Functionality support macros
#define LWMT_BUTTON_SUPPORT_CLICK_DETECTION
#define LWMT_BUTTON_SUPPORT_DOUBLE_CLICK_DETECTION
#define LWMT_BUTTON_SUPPORT_LONG_PRESS_DETECTION

// Default settings values
#define LWMT_BUTTON_SETTING_DEFAULT_DEBOUNCE_MS ((uint32_t)(10))
#ifdef LWMT_BUTTON_SUPPORT_CLICK_DETECTION
#define LWMT_BUTTON_SETTING_DEFAULT_CLICK_MAX_DURATION_MS ((uint32_t)(500))
#endif
#ifdef LWMT_BUTTON_SUPPORT_DOUBLE_CLICK_DETECTION
#define LWMT_BUTTON_SETTING_DEFAULT_DOUBLE_CLICK_GAP_MS ((uint32_t)(300))
#endif
#ifdef LWMT_BUTTON_SUPPORT_LONG_PRESS_DETECTION
#define LWMT_BUTTON_SETTING_DEFAULT_LONG_PRESS_MS ((uint32_t)(1500))
#define LWMT_BUTTON_SETTING_DEFAULT_LONG_PRESS_HOLD_MS ((uint32_t)(200))
#endif

typedef struct lwmt_button_s lwmt_button_t;

// Button press/release status
typedef enum
{
    LWMT_BUTTON_STATUS_RELEASED,
    LWMT_BUTTON_STATUS_PRESSED
} lwmt_button_status_e;

// Button event types
typedef enum
{
    LWMT_BUTTON_EVENT_NONE = 0,
    LWMT_BUTTON_EVENT_DOWN,
    LWMT_BUTTON_EVENT_UP,
#ifdef LWMT_BUTTON_SUPPORT_CLICK_DETECTION
    LWMT_BUTTON_EVENT_CLICK,
#endif
#ifdef LWMT_BUTTON_SUPPORT_DOUBLE_CLICK_DETECTION
    LWMT_BUTTON_EVENT_DOUBLE_CLICK,
#endif
#ifdef LWMT_BUTTON_SUPPORT_LONG_PRESS_DETECTION
    LWMT_BUTTON_EVENT_LONG_PRESS_START,
    LWMT_BUTTON_EVENT_LONG_PRESS_HOLD,
    LWMT_BUTTON_EVENT_LONG_PRESS_END,
#endif
} lwmt_button_event_e;

// Function typedefs
typedef uint8_t (*lwmt_button_read_pin_cb)(void);
typedef uint32_t (*lwmt_button_get_time_cb)(void);
typedef void (*lwmt_button_event_cb)(lwmt_button_t *, lwmt_button_event_e);

// Button structure
struct lwmt_button_s
{
    lwmt_button_get_time_cb get_time_cb;
    lwmt_button_read_pin_cb read_pin_cb;
    lwmt_button_event_cb event_cb;

    lwmt_button_status_e stable_status;
    lwmt_button_status_e last_raw_status;
    uint32_t last_raw_status_change_ms;

    uint32_t press_start_ms;
    uint32_t release_start_ms;

    uint8_t double_click_pending;

    uint8_t long_press_started;
    uint32_t long_press_start_ms;

    uint32_t s_debounce_ms;
#ifdef LWMT_BUTTON_SUPPORT_CLICK_DETECTION
    uint32_t s_click_max_duration_ms;
#endif
#ifdef LWMT_BUTTON_SUPPORT_DOUBLE_CLICK_DETECTION
    uint32_t s_double_click_gap_ms;
#endif
#ifdef LWMT_BUTTON_SUPPORT_LONG_PRESS_DETECTION
    uint32_t s_long_press_ms;
    uint32_t s_long_press_hold_ms;
#endif
};

#endif