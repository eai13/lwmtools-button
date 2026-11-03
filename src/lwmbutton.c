#include "lwmbutton.h"

static inline lwmt_button_status_e status_from_pinstate(lwmt_button_t *h)
{
    return (h->read_pin_cb()) ? (LWMT_BUTTON_STATUS_PRESSED) : (LWMT_BUTTON_STATUS_RELEASED);
}

static inline void emit(lwmt_button_t *h, lwmt_button_event_e event)
{
    if (h->event_cb)
    {
        h->event_cb(h, event);
    }
}

lwmt_button_t lwmt_button_init_d(lwmt_button_read_pin_cb read_pin_cb,
                                 lwmt_button_event_cb event_cb,
                                 lwmt_button_get_time_cb get_time_cb)
{
    return lwmt_button_init(read_pin_cb, event_cb, get_time_cb,
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
}

lwmt_button_t lwmt_button_init(lwmt_button_read_pin_cb read_pin_cb,
                               lwmt_button_event_cb event_cb,
                               lwmt_button_get_time_cb get_time_cb,
                               uint32_t debounce_ms
#ifdef LWMT_BUTTON_SUPPORT_CLICK_DETECTION
                               ,
                               uint32_t click_max_duration_ms
#endif
#ifdef LWMT_BUTTON_SUPPORT_DOUBLE_CLICK_DETECTION
                               ,
                               uint32_t double_click_gap_ms
#endif
#ifdef LWMT_BUTTON_SUPPORT_LONG_PRESS_DETECTION
                               ,
                               uint32_t long_press_ms,
                               uint32_t long_press_hold_ms
#endif
)
{
    lwmt_button_t h = {
        .read_pin_cb = read_pin_cb,
        .event_cb = event_cb,
        .get_time_cb = get_time_cb};

    h.s_debounce_ms = debounce_ms;

#ifdef LWMT_BUTTON_SUPPORT_CLICK_DETECTION
    h.s_click_max_duration_ms = click_max_duration_ms;
#endif
#ifdef LWMT_BUTTON_SUPPORT_DOUBLE_CLICK_DETECTION
    h.s_double_click_gap_ms = double_click_gap_ms;
#endif
#ifdef LWMT_BUTTON_SUPPORT_LONG_PRESS_DETECTION
    h.s_long_press_ms = long_press_ms;
    h.s_long_press_hold_ms = long_press_hold_ms;
#endif

    h.stable_status = status_from_pinstate(&h);
    h.last_raw_status = h.stable_status;
    h.last_raw_status_change_ms = 0;

    h.press_start_ms = 0;
    h.release_start_ms = 0;

#ifdef LWMT_BUTTON_SUPPORT_DOUBLE_CLICK_DETECTION
    h.double_click_pending = 0;
#endif

#ifdef LWMT_BUTTON_SUPPORT_LONG_PRESS_DETECTION
    h.long_press_started = 0;
    h.long_press_start_ms = 0;
#endif

    return h;
}

lwmt_button_status_e lwmt_button_status(lwmt_button_t const *h)
{
    return h->stable_status;
}

void lwmt_button_process(lwmt_button_t *h)
{
    uint32_t now_ms = h->get_time_cb();

    lwmt_button_status_e raw_status = status_from_pinstate(h);

    // Debouncing, drop status change if button status changed in h->s_debounce_ms
    if (raw_status != h->last_raw_status)
    {
        h->last_raw_status = raw_status;
        h->last_raw_status_change_ms = now_ms;
    }

    uint32_t last_raw_status_changed = now_ms - h->last_raw_status_change_ms;
    uint8_t status_updated = 0;
    if ((last_raw_status_changed >= h->s_debounce_ms) && (raw_status != h->stable_status))
    {
        status_updated = 1;
        h->stable_status = raw_status;
    }

    // Button debounce status updated
    if (status_updated)
    {
        switch (raw_status)
        {
        case LWMT_BUTTON_STATUS_PRESSED: // From released to pressed
        {
            h->press_start_ms = now_ms;
            emit(h, LWMT_BUTTON_EVENT_DOWN);
            break;
        }
        case LWMT_BUTTON_STATUS_RELEASED: // From pressed to released
        {
            h->release_start_ms = now_ms;
            emit(h, LWMT_BUTTON_EVENT_UP);
#ifdef LWMT_BUTTON_SUPPORT_LONG_PRESS_DETECTION
            if (h->long_press_started || h->long_press_hold_started)
            {
                h->long_press_hold_started = 0;
                h->long_press_started = 0;
                emit(h, LWMT_BUTTON_EVENT_LONG_PRESS_END);
            }
            else
#endif
            {
#ifdef LWMT_BUTTON_SUPPORT_CLICK_DETECTION
                uint32_t click_duration = now_ms - h->press_start_ms;
                if (click_duration <= h->s_click_max_duration_ms)
                {
#ifdef LWMT_BUTTON_SUPPORT_DOUBLE_CLICK_DETECTION
                    if (h->double_click_pending)
                    {
                        h->double_click_pending = 0;
                        emit(h, LWMT_BUTTON_EVENT_DOUBLE_CLICK);
                    }
                    else
                    {
                        h->double_click_pending = 1;
#endif
                        emit(h, LWMT_BUTTON_EVENT_CLICK);
#ifdef LWMT_BUTTON_SUPPORT_DOUBLE_CLICK_DETECTION
                    }
#endif
                }
#endif
            }
            break;
        }
        default:
        {
            break;
        }
        }
    }
    else
    {
        switch (raw_status)
        {
        case LWMT_BUTTON_STATUS_PRESSED:
        {
#ifdef LWMT_BUTTON_SUPPORT_LONG_PRESS_DETECTION
            uint32_t hold_time = now_ms - h->press_start_ms;
            if (!h->long_press_started && (hold_time >= h->s_long_press_ms))
            {
                h->long_press_started = 1;
                emit(h, LWMT_BUTTON_EVENT_LONG_PRESS_START);
            }
            else if (!h->long_press_hold_started && (hold_time >= h->s_long_press_hold_ms))
            {
                h->long_press_hold_started = 1;
                emit(h, LWMT_BUTTON_EVENT_LONG_PRESS_HOLD);
            }
#endif
            break;
        }
        case LWMT_BUTTON_STATUS_RELEASED:
        {
#ifdef LWMT_BUTTON_SUPPORT_DOUBLE_CLICK_DETECTION
            uint32_t since_last_release = now_ms - h->release_start_ms;
            if (h->double_click_pending && (since_last_release > h->s_double_click_gap_ms))
            {
                h->double_click_pending = 0;
            }
#endif
            break;
        }
        default:
        {
            break;
        }
        }
    }

    // // Detect stable status change
    // if (((now_ms - h->last_raw_status_change_ms) >= h->s_debounce_ms) &&
    //     (raw_status != h->stable_status))
    // {
    //     h->stable_status = raw_status;

    //     switch (raw_status)
    //     {
    //     case (LWMT_BUTTON_STATUS_PRESSED):
    //     {
    //         h->press_start_ms = now_ms;
    //         h->long_press_started = 0;
    //         emit(h, LWMT_BUTTON_EVENT_DOWN);
    //         break;
    //     }
    //     case (LWMT_BUTTON_STATUS_RELEASED):
    //     {
    //         uint32_t press_len = now_ms - h->press_start_ms;

    //         if (h->long_press_started)
    //         {
    //             emit(h, LWMT_BUTTON_EVENT_LONG_PRESS_END);
    //             h->double_click_pending = 0;
    //         }
    //         else if (press_len <= h->s_click_max_duration_ms)
    //         {
    //             if (h->double_click_pending)
    //             {
    //                 emit(h, LWMT_BUTTON_EVENT_DOUBLE_CLICK);
    //                 h->double_click_pending = 0;
    //             }
    //             else
    //             {
    //                 h->double_click_pending = 1;
    //                 h->release_start_ms = now_ms;
    //             }
    //         }
    //         else
    //         {
    //             emit(h, LWMT_BUTTON_EVENT_UP);
    //         }
    //         break;
    //     }
    //     }
    // }

    // if (h->stable_status == LWMT_BUTTON_STATUS_PRESSED)
    // {
    //     uint32_t hold_time = now_ms - h->press_start_ms;
    //     if (!h->long_press_started && hold_time >= h->s_long_press_ms)
    //     {
    //         h->long_press_started = 1;
    //         h->long_press_start_ms = now_ms;
    //         h->double_click_pending = 0;
    //         emit(h, LWMT_BUTTON_EVENT_LONG_PRESS_START);
    //     }
    //     else if ((h->long_press_started) &&
    //              ((now_ms - h->long_press_start_ms) >= h->s_long_press_hold_ms))
    //     {
    //         h->long_press_start_ms = now_ms;
    //         emit(h, LWMT_BUTTON_EVENT_LONG_PRESS_HOLD);
    //     }
    // }

    // // If no second click obtained
    // if (h->double_click_pending &&
    //     (h->stable_status == LWMT_BUTTON_STATUS_RELEASED) &&
    //     ((now_ms - h->release_start_ms) > h->s_double_click_gap_ms))
    // {
    //     h->double_click_pending = 0;
    //     emit(h, LWMT_BUTTON_EVENT_CLICK);
    // }
}
