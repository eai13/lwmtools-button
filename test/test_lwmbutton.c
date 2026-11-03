#include "unity.h"
#include "lwmbutton.h"

uint32_t time = 0;
uint32_t get_time_cb(void)
{
    return time;
}
uint8_t pin_state = 0;
uint8_t read_pin_cb(void)
{
    return pin_state;
}
lwmt_button_event_e current_event = LWMT_BUTTON_EVENT_NONE;
lwmt_button_event_e events_history[16];
uint32_t event_updated_times = 0;
void event_cb(lwmt_button_t *h, lwmt_button_event_e e)
{
    current_event = e;
    events_history[event_updated_times++] = e;
}
void reset_events()
{
    current_event = LWMT_BUTTON_EVENT_NONE;
    event_updated_times = 0;
}
void reset_all()
{
    reset_events();
    time = 0;
    pin_state = 0;
}

lwmt_button_t create_default_button(void)
{
    return lwmt_button_init_d(read_pin_cb, event_cb, get_time_cb);
}

void test_lwmbutton_init(void)
{
    lwmt_button_t button_1 = lwmt_button_init_d(read_pin_cb, event_cb, get_time_cb);
    lwmt_button_t button_2 = lwmt_button_init(read_pin_cb, event_cb, get_time_cb,
                                              LWMT_BUTTON_SETTING_DEFAULT_DEBOUNCE_MS
#ifdef LWMT_BUTTON_SUPPORT_CLICK_DETECTION
                                              ,
                                              LWMT_BUTTON_SETTING_DEFAULT_CLICK_MAX_DURATION_MS
#endif
#ifdef LWMT_BUTTON_SUPPORT_DOUBLE_CLICK_DETECTION
                                              ,
                                              LWMT_BUTTON_SETTING_DEFAULT_DOUBLE_CLICK_GAP_MS
#endif
#ifdef LWMT_BUTTON_SUPPORT_LONG_PRESS_DETECTION
                                              ,
                                              LWMT_BUTTON_SETTING_DEFAULT_LONG_PRESS_MS,
                                              LWMT_BUTTON_SETTING_DEFAULT_LONG_PRESS_HOLD_MS
#endif
    );
    TEST_ASSERT_EQUAL(button_1.get_time_cb, button_2.get_time_cb);
    TEST_ASSERT_EQUAL(button_1.read_pin_cb, button_2.read_pin_cb);
    TEST_ASSERT_EQUAL(button_1.event_cb, button_2.event_cb);
    TEST_ASSERT_EQUAL(button_1.s_debounce_ms, button_2.s_debounce_ms);
#ifdef LWMT_BUTTON_SUPPORT_CLICK_DETECTION
    TEST_ASSERT_EQUAL(button_1.s_click_max_duration_ms, button_2.s_click_max_duration_ms);
#endif
#ifdef LWMT_BUTTON_SUPPORT_DOUBLE_CLICK_DETECTION
    TEST_ASSERT_EQUAL(button_1.s_double_click_gap_ms, button_2.s_double_click_gap_ms);
#endif
#ifdef LWMT_BUTTON_SUPPORT_LONG_PRESS_DETECTION
    TEST_ASSERT_EQUAL(button_1.s_long_press_hold_ms, button_2.s_long_press_hold_ms);
    TEST_ASSERT_EQUAL(button_1.s_long_press_ms, button_2.s_long_press_ms);
#endif
}

// Test quick press-release
void test_lwmbutton_press_release_debounce(void)
{
    reset_all();
    lwmt_button_t b = create_default_button();

    reset_events();
    lwmt_button_process(&b);

    TEST_ASSERT_EQUAL(b.last_raw_status_change_ms, 0);
    TEST_ASSERT_EQUAL(b.last_raw_status, LWMT_BUTTON_STATUS_RELEASED);
    TEST_ASSERT_EQUAL(b.stable_status, LWMT_BUTTON_STATUS_RELEASED);
    TEST_ASSERT_EQUAL(lwmt_button_status(&b), LWMT_BUTTON_STATUS_RELEASED);
    TEST_ASSERT_EQUAL(event_updated_times, 0);
    TEST_ASSERT_EQUAL(current_event, LWMT_BUTTON_EVENT_NONE);

    reset_events();
    time = 10;
    pin_state = 1;
    lwmt_button_process(&b);
    TEST_ASSERT_EQUAL(b.last_raw_status_change_ms, 10);
    TEST_ASSERT_EQUAL(b.last_raw_status, LWMT_BUTTON_STATUS_PRESSED);
    TEST_ASSERT_EQUAL(b.stable_status, LWMT_BUTTON_STATUS_RELEASED);
    TEST_ASSERT_EQUAL(event_updated_times, 0);
    TEST_ASSERT_EQUAL(current_event, LWMT_BUTTON_EVENT_NONE);

    reset_events();
    time += b.s_debounce_ms + 1;
    pin_state = 1;
    lwmt_button_process(&b);
    TEST_ASSERT_EQUAL(b.last_raw_status_change_ms, 10);
    TEST_ASSERT_EQUAL(b.last_raw_status, LWMT_BUTTON_STATUS_PRESSED);
    TEST_ASSERT_EQUAL(b.stable_status, LWMT_BUTTON_STATUS_PRESSED);
    TEST_ASSERT_EQUAL(event_updated_times, 1);
    TEST_ASSERT_EQUAL(current_event, LWMT_BUTTON_EVENT_DOWN);

    reset_events();
    time += 1;
    pin_state = 0;
    lwmt_button_process(&b);
    TEST_ASSERT_EQUAL(b.last_raw_status_change_ms, time);
    TEST_ASSERT_EQUAL(b.last_raw_status, LWMT_BUTTON_STATUS_RELEASED);
    TEST_ASSERT_EQUAL(b.stable_status, LWMT_BUTTON_STATUS_PRESSED);
    TEST_ASSERT_EQUAL(event_updated_times, 0);
    TEST_ASSERT_EQUAL(current_event, LWMT_BUTTON_EVENT_NONE);

    reset_events();
    time += 1;
    pin_state = 1;
    lwmt_button_process(&b);
    TEST_ASSERT_EQUAL(b.last_raw_status_change_ms, time);
    TEST_ASSERT_EQUAL(b.last_raw_status, LWMT_BUTTON_STATUS_PRESSED);
    TEST_ASSERT_EQUAL(b.stable_status, LWMT_BUTTON_STATUS_PRESSED);
    TEST_ASSERT_EQUAL(event_updated_times, 0);
    TEST_ASSERT_EQUAL(current_event, LWMT_BUTTON_EVENT_NONE);

    reset_events();
    time += 1;
    pin_state = 0;
    lwmt_button_process(&b);
    uint32_t last_time_change = time;

    reset_events();
#ifdef LWMT_BUTTON_SUPPORT_CLICK_DETECTION
    time += b.s_click_max_duration_ms + 1;
#else
    time += b.s_debounce_ms + 1;
#endif
    pin_state = 0;
    lwmt_button_process(&b);
    TEST_ASSERT_EQUAL(b.last_raw_status_change_ms, last_time_change);
    TEST_ASSERT_EQUAL(b.last_raw_status, LWMT_BUTTON_STATUS_RELEASED);
    TEST_ASSERT_EQUAL(b.stable_status, LWMT_BUTTON_STATUS_RELEASED);
    TEST_ASSERT_EQUAL(event_updated_times, 1);
    TEST_ASSERT_EQUAL(current_event, LWMT_BUTTON_EVENT_UP);
}

void test_lwmbutton_press_release_time(void)
{
    reset_all();

    uint32_t time_press_start = 0;
    uint32_t time_release_start = 0;

    lwmt_button_t b = create_default_button();

    pin_state = 1;
    time += 1;
    lwmt_button_process(&b);

    reset_events();
    time += b.s_debounce_ms + 1;
    time_press_start = time;
    lwmt_button_process(&b);
    TEST_ASSERT_EQUAL(b.press_start_ms, time_press_start);
    TEST_ASSERT_EQUAL(b.release_start_ms, time_release_start);

    reset_events();
    time += 1;
    pin_state = 0;
    lwmt_button_process(&b);
    TEST_ASSERT_EQUAL(b.press_start_ms, time_press_start);
    TEST_ASSERT_EQUAL(b.release_start_ms, time_release_start);

    reset_events();
    time += 1;
    pin_state = 1;
    lwmt_button_process(&b);
    TEST_ASSERT_EQUAL(b.press_start_ms, time_press_start);
    TEST_ASSERT_EQUAL(b.release_start_ms, time_release_start);

    reset_events();
    time += 1;
    pin_state = 0;
    lwmt_button_process(&b);

    reset_events();
    time += b.s_debounce_ms + 1;
    time_release_start = time;
    lwmt_button_process(&b);
    TEST_ASSERT_EQUAL(b.press_start_ms, time_press_start);
    TEST_ASSERT_EQUAL(b.release_start_ms, time_release_start);
}

void test_lwmbutton_click(void)
{
#ifdef LWMT_BUTTON_SUPPORT_CLICK_DETECTION
    reset_all();
    lwmt_button_t b = create_default_button();

    pin_state = 1;
    time += 1;
    lwmt_button_process(&b);

    reset_events();
    time += b.s_debounce_ms;
    lwmt_button_process(&b);
    TEST_ASSERT_EQUAL(event_updated_times, 1);
    TEST_ASSERT_EQUAL(current_event, LWMT_BUTTON_EVENT_DOWN);

    reset_events();
    time += 1;
    pin_state = 0;
    lwmt_button_process(&b);

    reset_events();
    time += b.s_click_max_duration_ms - 1;
    lwmt_button_process(&b);
    TEST_ASSERT_EQUAL(event_updated_times, 2);
    TEST_ASSERT_EQUAL(events_history[0], LWMT_BUTTON_EVENT_UP);
    TEST_ASSERT_EQUAL(events_history[1], LWMT_BUTTON_EVENT_CLICK);
#ifdef LWMT_BUTTON_SUPPORT_DOUBLE_CLICK_DETECTION
    TEST_ASSERT_EQUAL(b.double_click_pending, 1);
#endif

#ifdef LWMT_BUTTON_SUPPORT_DOUBLE_CLICK_DETECTION
    reset_events();
    time += 1;
    pin_state = 1;
    lwmt_button_process(&b);

    reset_events();
    time += b.s_double_click_gap_ms - 2;
    lwmt_button_process(&b);
    TEST_ASSERT_EQUAL(event_updated_times, 1);
    TEST_ASSERT_EQUAL(current_event, LWMT_BUTTON_EVENT_DOWN);

    reset_events();
    time += 1;
    pin_state = 0;
    lwmt_button_process(&b);

    reset_events();
    time += b.s_click_max_duration_ms - 1;
    lwmt_button_process(&b);
    TEST_ASSERT_EQUAL(event_updated_times, 2);
    TEST_ASSERT_EQUAL(events_history[0], LWMT_BUTTON_EVENT_UP);
    TEST_ASSERT_EQUAL(events_history[1], LWMT_BUTTON_EVENT_DOUBLE_CLICK);
#endif
#endif
}