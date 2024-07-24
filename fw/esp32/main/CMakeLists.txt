# -------------------------------------------------------
#  ▞▚ ▚▗▘▛▚ ▛▀ ▛▚ ▛▚ ▌▐ ▙▐ ▌▞  ▞▚ ▞▚ ▙▟ ▛▚ ▞▚ ▙▐ ▚▗▘
#  ▌▗  ▌ ▛▚ ▛  ▛▚ ▛▘ ▌▐ ▌▜ ▛▖  ▌▗ ▌▐ ▌▐ ▛▘ ▛▜ ▌▜  ▌
#  ▝▘  ▘ ▀▘ ▀▀ ▘▝ ▘  ▝▘ ▘▝ ▘▝ ▘▝▘ ▝▘ ▘▝ ▘  ▘▝ ▘▝  ▘
# Copyright© 2024 - Cyberpunk.Company - GPLv3
# This program is free software
# See LICENSE.txt - https://cyberpunk.company/tixlegeek
# -------------------------------------------------------

set(COMPRESSED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/compressed")

# Integrate WebPages handlers
set(GET_HANDLERS_SOURCES "")
file(GLOB GET_HANDLERS_SOURCES "handlers/get/*.c")
set(POST_HANDLERS_SOURCES "")
file(GLOB POST_HANDLERS_SOURCES "handlers/post/*.c")
set(WS_HANDLERS_SOURCES "")
file(GLOB WS_HANDLERS_SOURCES "handlers/ws/*.c")

file(GLOB HTML_FILES "${COMPRESSED_DIR}/*.html")
file(GLOB CSS_FILES "${COMPRESSED_DIR}/*.css")
file(GLOB JS_FILES "${COMPRESSED_DIR}/*.js")

list(APPEND EMBED_FILES ${HTML_FILES})
list(APPEND EMBED_FILES ${CSS_FILES})
list(APPEND EMBED_FILES ${JS_FILES})


idf_component_register(SRCS "main.c"
                            "drivers/led_strip_encoder.c"
                            "keep_alive.c"
                            "coolkids.c"
                            "httpd.c"
                            "dnsd.c"
                            "led.c"
                            "sntp.c"
                            "wifi.c"
                            "nvs_db.c"
                            "board.c"
                            "${GET_HANDLERS_SOURCES}"
                            "${POST_HANDLERS_SOURCES}"
                            "${WS_HANDLERS_SOURCES}"
                    INCLUDE_DIRS "." "./drivers"
                    # Embedded certs and files
                    EMBED_TXTFILES ${EMBED_FILES}
                    		"certs/servercert.pem"
                        "certs/prvtkey.pem"
                    )
