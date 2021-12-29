idf_component_register(SRCS "main.c"
                            "rest_server.c"
                    INCLUDE_DIRS ".")
set(WEB_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")
if(EXISTS ${WEB_SRC_DIR}/front)
        spiffs_create_partition_image(www ${WEB_SRC_DIR}/front FLASH_IN_PROJECT)
else()
        message(FATAL_ERROR "${WEB_SRC_DIR}/front doesn't exist.")
endif()
