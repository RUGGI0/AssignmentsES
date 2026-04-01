include("${CMAKE_CURRENT_LIST_DIR}/rule.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/file.cmake")

set(Assignment2_default_library_list )

# Handle files with suffix s, for group default-XC16
if(Assignment2_default_default_XC16_FILE_TYPE_assemble)
add_library(Assignment2_default_default_XC16_assemble OBJECT ${Assignment2_default_default_XC16_FILE_TYPE_assemble})
    Assignment2_default_default_XC16_assemble_rule(Assignment2_default_default_XC16_assemble)
    list(APPEND Assignment2_default_library_list "$<TARGET_OBJECTS:Assignment2_default_default_XC16_assemble>")

endif()

# Handle files with suffix S, for group default-XC16
if(Assignment2_default_default_XC16_FILE_TYPE_assemblePreproc)
add_library(Assignment2_default_default_XC16_assemblePreproc OBJECT ${Assignment2_default_default_XC16_FILE_TYPE_assemblePreproc})
    Assignment2_default_default_XC16_assemblePreproc_rule(Assignment2_default_default_XC16_assemblePreproc)
    list(APPEND Assignment2_default_library_list "$<TARGET_OBJECTS:Assignment2_default_default_XC16_assemblePreproc>")

endif()

# Handle files with suffix c, for group default-XC16
if(Assignment2_default_default_XC16_FILE_TYPE_compile)
add_library(Assignment2_default_default_XC16_compile OBJECT ${Assignment2_default_default_XC16_FILE_TYPE_compile})
    Assignment2_default_default_XC16_compile_rule(Assignment2_default_default_XC16_compile)
    list(APPEND Assignment2_default_library_list "$<TARGET_OBJECTS:Assignment2_default_default_XC16_compile>")

endif()

# Handle files with suffix s, for group default-XC16
if(Assignment2_default_default_XC16_FILE_TYPE_dependentObject)
add_library(Assignment2_default_default_XC16_dependentObject OBJECT ${Assignment2_default_default_XC16_FILE_TYPE_dependentObject})
    Assignment2_default_default_XC16_dependentObject_rule(Assignment2_default_default_XC16_dependentObject)
    list(APPEND Assignment2_default_library_list "$<TARGET_OBJECTS:Assignment2_default_default_XC16_dependentObject>")

endif()

# Handle files with suffix elf, for group default-XC16
if(Assignment2_default_default_XC16_FILE_TYPE_bin2hex)
add_library(Assignment2_default_default_XC16_bin2hex OBJECT ${Assignment2_default_default_XC16_FILE_TYPE_bin2hex})
    Assignment2_default_default_XC16_bin2hex_rule(Assignment2_default_default_XC16_bin2hex)
    list(APPEND Assignment2_default_library_list "$<TARGET_OBJECTS:Assignment2_default_default_XC16_bin2hex>")

endif()


# Main target for this project
add_executable(Assignment2_default_image_3BdpTXja ${Assignment2_default_library_list})

set_target_properties(Assignment2_default_image_3BdpTXja PROPERTIES
    OUTPUT_NAME "default"
    SUFFIX ".elf"
    RUNTIME_OUTPUT_DIRECTORY "${Assignment2_default_output_dir}")
target_link_libraries(Assignment2_default_image_3BdpTXja PRIVATE ${Assignment2_default_default_XC16_FILE_TYPE_link})

# Add the link options from the rule file.
Assignment2_default_link_rule( Assignment2_default_image_3BdpTXja)

# Call bin2hex function from the rule file
Assignment2_default_bin2hex_rule(Assignment2_default_image_3BdpTXja)

