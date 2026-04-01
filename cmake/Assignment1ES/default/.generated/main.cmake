include("${CMAKE_CURRENT_LIST_DIR}/rule.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/file.cmake")

set(Assignment1ES_default_library_list )

# Handle files with suffix s, for group default-XC16
if(Assignment1ES_default_default_XC16_FILE_TYPE_assemble)
add_library(Assignment1ES_default_default_XC16_assemble OBJECT ${Assignment1ES_default_default_XC16_FILE_TYPE_assemble})
    Assignment1ES_default_default_XC16_assemble_rule(Assignment1ES_default_default_XC16_assemble)
    list(APPEND Assignment1ES_default_library_list "$<TARGET_OBJECTS:Assignment1ES_default_default_XC16_assemble>")

endif()

# Handle files with suffix S, for group default-XC16
if(Assignment1ES_default_default_XC16_FILE_TYPE_assemblePreproc)
add_library(Assignment1ES_default_default_XC16_assemblePreproc OBJECT ${Assignment1ES_default_default_XC16_FILE_TYPE_assemblePreproc})
    Assignment1ES_default_default_XC16_assemblePreproc_rule(Assignment1ES_default_default_XC16_assemblePreproc)
    list(APPEND Assignment1ES_default_library_list "$<TARGET_OBJECTS:Assignment1ES_default_default_XC16_assemblePreproc>")

endif()

# Handle files with suffix c, for group default-XC16
if(Assignment1ES_default_default_XC16_FILE_TYPE_compile)
add_library(Assignment1ES_default_default_XC16_compile OBJECT ${Assignment1ES_default_default_XC16_FILE_TYPE_compile})
    Assignment1ES_default_default_XC16_compile_rule(Assignment1ES_default_default_XC16_compile)
    list(APPEND Assignment1ES_default_library_list "$<TARGET_OBJECTS:Assignment1ES_default_default_XC16_compile>")

endif()

# Handle files with suffix s, for group default-XC16
if(Assignment1ES_default_default_XC16_FILE_TYPE_dependentObject)
add_library(Assignment1ES_default_default_XC16_dependentObject OBJECT ${Assignment1ES_default_default_XC16_FILE_TYPE_dependentObject})
    Assignment1ES_default_default_XC16_dependentObject_rule(Assignment1ES_default_default_XC16_dependentObject)
    list(APPEND Assignment1ES_default_library_list "$<TARGET_OBJECTS:Assignment1ES_default_default_XC16_dependentObject>")

endif()

# Handle files with suffix elf, for group default-XC16
if(Assignment1ES_default_default_XC16_FILE_TYPE_bin2hex)
add_library(Assignment1ES_default_default_XC16_bin2hex OBJECT ${Assignment1ES_default_default_XC16_FILE_TYPE_bin2hex})
    Assignment1ES_default_default_XC16_bin2hex_rule(Assignment1ES_default_default_XC16_bin2hex)
    list(APPEND Assignment1ES_default_library_list "$<TARGET_OBJECTS:Assignment1ES_default_default_XC16_bin2hex>")

endif()


# Main target for this project
add_executable(Assignment1ES_default_image_BJoHZvk8 ${Assignment1ES_default_library_list})

set_target_properties(Assignment1ES_default_image_BJoHZvk8 PROPERTIES
    OUTPUT_NAME "default"
    SUFFIX ".elf"
    RUNTIME_OUTPUT_DIRECTORY "${Assignment1ES_default_output_dir}")
target_link_libraries(Assignment1ES_default_image_BJoHZvk8 PRIVATE ${Assignment1ES_default_default_XC16_FILE_TYPE_link})

# Add the link options from the rule file.
Assignment1ES_default_link_rule( Assignment1ES_default_image_BJoHZvk8)

# Call bin2hex function from the rule file
Assignment1ES_default_bin2hex_rule(Assignment1ES_default_image_BJoHZvk8)

