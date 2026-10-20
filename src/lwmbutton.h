#ifndef LWMTOOLS_BUTTON_H
#define LWMTOOLS_BUTTON_H

#include "common_defs.h"

lwmt_button_t lwmt_button_init_d(lwmt_button_read_pin_cb read_pin_cb,
                                 lwmt_button_event_cb event_cb,
                                 lwmt_button_get_time_cb get_time_cb);

lwmt_button_t lwmt_button_init(lwmt_button_read_pin_cb read_pin_cb,
                               lwmt_button_event_cb event_cb,
                               lwmt_button_get_time_cb get_time_cb,
                               uint32_t debounce_ms,
                               uint32_t click_max_duration_ms,
                               uint32_t double_click_gap_ms,
                               uint32_t long_press_ms,
                               uint32_t long_press_hold_ms);

lwmt_button_status_e lwmt_button_status(lwmt_button_t const *h);

void lwmt_button_process(lwmt_button_t *h);

#endif