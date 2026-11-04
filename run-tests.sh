#!/bin/bash

BUILD_DIR="ceedling-test-build"
if [ -d ${BUILD_DIR} ]; then
    echo "Cleanup build dir"
    rm -r ${BUILD_DIR}
fi

REPORTS_DIR="gcov-reports"
if [ -d ${REPORTS_DIR} ]; then
    echo "Cleanup gcov reports dir"
    rm -r ${REPORTS_DIR}
fi
mkdir -p ${REPORTS_DIR}

MIXIN="--mixin"
VERBOSITY="-v $1"
CEEDLING_GCOV="ceedling clobber gcov:all"
CEEDLING_TEST="ceedling clobber test:all" 

for f in test-mixins/*.yaml; do
    echo ""
    echo "=================================="
    echo ${f}
    echo "============== TEST =============="
    ${CEEDLING_GCOV} ${MIXIN} ${f} ${VERBOSITY}
    echo "=================================="
    echo ""

    bname=$(basename "$f" .yaml)
    variant=$(echo $bname | sed 's/^mixin_//')
    report_dir=${REPORTS_DIR}/${bname}

    mkdir ${report_dir}
    mv ${BUILD_DIR}/${variant}/artifacts/gcov/gcovr/* ${report_dir}/

    rm -r ${BUILD_DIR}
done

