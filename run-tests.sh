#!/bin/bash

CEEDLING_FLAGS="-v normal"

# No functionality macros
echo ""
echo "====================="
echo "== TEST: No Macros =="
echo "====================="
echo ""
ceedling test:all --mixin test-mixins/mixin_default.yaml ${CEEDLING_FLAGS}

# All functionality macros
echo ""
echo "======================"
echo "== TEST: All Macros =="
echo "======================"
echo ""
ceedling test:all --mixin test-mixins/mixin_full_support.yaml ${CEEDLING_FLAGS}

# Click support macros
echo ""
echo "==============================================="
echo "== TEST: LWMT_BUTTON_SUPPORT_CLICK_DETECTION =="
echo "==============================================="
echo ""
ceedling test:all --mixin test-mixins/mixin_support_click.yaml ${CEEDLING_FLAGS}

# Double click support macros
echo ""
echo "============================================================================================"
echo "== TEST: LWMT_BUTTON_SUPPORT_CLICK_DETECTION + LWMT_BUTTON_SUPPORT_DOUBLE_CLICK_DETECTION =="
echo "============================================================================================"
echo ""
ceedling test:all --mixin test-mixins/mixin_support_double_click.yaml ${CEEDLING_FLAGS}

# Long press support macros
echo ""
echo "===================================================="
echo "== TEST: LWMT_BUTTON_SUPPORT_LONG_PRESS_DETECTION =="
echo "===================================================="
echo ""
ceedling test:all --mixin test-mixins/mixin_support_long_press.yaml ${CEEDLING_FLAGS}