#!/usr/bin/env bash
# -------------------------------------------------------
#  ▞▚ ▚▗▘▛▚ ▛▀ ▛▚ ▛▚ ▌▐ ▙▐ ▌▞  ▞▚ ▞▚ ▙▟ ▛▚ ▞▚ ▙▐ ▚▗▘
#  ▌▗  ▌ ▛▚ ▛  ▛▚ ▛▘ ▌▐ ▌▜ ▛▖  ▌▗ ▌▐ ▌▐ ▛▘ ▛▜ ▌▜  ▌
#  ▝▘  ▘ ▀▘ ▀▀ ▘▝ ▘  ▝▘ ▘▝ ▘▝ ▘▝▘ ▝▘ ▘▝ ▘  ▘▝ ▘▝  ▘
# Copyright© 2024 - Cyberpunk.Company - GPLv3
# This program is free software
# See LICENSE.txt - https://cyberpunk.company/tixlegeek
# -------------------------------------------------------
# PATHectories
WEB_PATH="./web"
COMPRESSED_PATH="./compressed"

pushd main >& /dev/null

# Ensure the compressed PATHectory exists
mkdir -p "${COMPRESSED_PATH}"
echo "--- COMPRESSING ...."
# Process each HTML and CSS file found in the web PATHectory
find "${WEB_PATH}" -type f -regex ".+css\|.+html\|.+js" -print0 | while IFS= read -r -d $'\0' FILE; do
    # Extract filename
    FILE_NAME=$(basename "$FILE")

    # Construct the output file path
    OUTPUT_FILE="$COMPRESSED_PATH/$FILE_NAME"
    # Call clean.sh with the current file and its intended output path
    ./clean.sh "$FILE" "$OUTPUT_FILE"
done
echo "All HTML files processed."
popd > /dev/null
idf.py flash
