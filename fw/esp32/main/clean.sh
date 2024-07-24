#!/bin/bash
# -------------------------------------------------------
#  ▞▚ ▚▗▘▛▚ ▛▀ ▛▚ ▛▚ ▌▐ ▙▐ ▌▞  ▞▚ ▞▚ ▙▟ ▛▚ ▞▚ ▙▐ ▚▗▘
#  ▌▗  ▌ ▛▚ ▛  ▛▚ ▛▘ ▌▐ ▌▜ ▛▖  ▌▗ ▌▐ ▌▐ ▛▘ ▛▜ ▌▜  ▌
#  ▝▘  ▘ ▀▘ ▀▀ ▘▝ ▘  ▝▘ ▘▝ ▘▝ ▘▝▘ ▝▘ ▘▝ ▘  ▘▝ ▘▝  ▘
# Copyright© 2024 - Cyberpunk.Company - GPLv3
# This program is free software
# See LICENSE.txt - https://cyberpunk.company/tixlegeek
# -------------------------------------------------------
# Simple HTML/JS/CSS simple Minifier.

INPUT_FILE="${1}"
OUTPUT_FILE="${2}"
# Check if input file is provided
function usage () {
  echo "Usage: ./script.sh INPUT_FILE OUTPUT_FILE"
}
if [ -z "${INPUT_FILE}" ]; then
    echo "ERROR: No input file specified."
    usage
    exit 1
fi
if [ -z "${OUTPUT_FILE}" ]; then
    echo "ERROR: No output file specified."
    usage
    exit 1
fi

# Check if the input file exists
if [ ! -f "${INPUT_FILE}" ]; then
    echo "ERROR: File ${INPUT_FILE} does not exist."
    exit 1
fi
    # Set of rules that results in a minifying of about any web file
    # Removing comments, redundant characters, and unefficient stuffs
    echo -e "[c] \033[32mCompressing ${INPUT_FILE} into ${OUTPUT_FILE}\033[0m"
    cat ${INPUT_FILE} | \
    sed -e '/<!--/,/-->/d' \
    -e "s|/\*\(\\\\\)\?\*/|/~\1~/|g"  \
    -e "s|/\*[^*]*\*\+\([^/][^*]*\*\+\)*/||g"  \
    -e "s|\([^:/]\)//.*$|\1|" -e "s|^//.*$||" | \
    tr '\n' ' ' |  \
    sed -e "s|/\*[^*]*\*\+\([^/][^*]*\*\+\)*/||g"  \
    -e "s|/\~\(\\\\\)\?\~/|/*\1*/|g"  \
    -e "s|\s\+| |g"  \
    -e "s| \([{;:,]\)|\1|g"  \
    -e "s|\([{;:,]\) |\1|g" > "${OUTPUT_FILE}"
