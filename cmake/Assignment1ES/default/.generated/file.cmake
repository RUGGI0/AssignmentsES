# The following variables contains the files used by the different stages of the build process.
set(Assignment1ES_default_default_XC16_FILE_TYPE_assemble)
set_source_files_properties(${Assignment1ES_default_default_XC16_FILE_TYPE_assemble} PROPERTIES LANGUAGE ASM)

# For assembly files, add "." to the include path for each file so that .include with a relative path works
foreach(source_file ${Assignment1ES_default_default_XC16_FILE_TYPE_assemble})
        set_source_files_properties(${source_file} PROPERTIES INCLUDE_DIRECTORIES "$<PATH:NORMAL_PATH,$<PATH:REMOVE_FILENAME,${source_file}>>")
endforeach()

set(Assignment1ES_default_default_XC16_FILE_TYPE_assemblePreproc)
set_source_files_properties(${Assignment1ES_default_default_XC16_FILE_TYPE_assemblePreproc} PROPERTIES LANGUAGE ASM)

# For assembly files, add "." to the include path for each file so that .include with a relative path works
foreach(source_file ${Assignment1ES_default_default_XC16_FILE_TYPE_assemblePreproc})
        set_source_files_properties(${source_file} PROPERTIES INCLUDE_DIRECTORIES "$<PATH:NORMAL_PATH,$<PATH:REMOVE_FILENAME,${source_file}>>")
endforeach()

set(Assignment1ES_default_default_XC16_FILE_TYPE_compile
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../Assignment1ES.X/newmainXC16.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../Assignment4.X/functions4.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../Assignment4.X/newmainXC16_4.c")
set_source_files_properties(${Assignment1ES_default_default_XC16_FILE_TYPE_compile} PROPERTIES LANGUAGE C)
set(Assignment1ES_default_default_XC16_FILE_TYPE_link)
set(Assignment1ES_default_default_XC16_FILE_TYPE_bin2hex)
set(Assignment1ES_default_image_name "default.elf")
set(Assignment1ES_default_image_base_name "default")

# The output directory of the final image.
set(Assignment1ES_default_output_dir "${CMAKE_CURRENT_SOURCE_DIR}/../../../out/Assignment1ES")

# The full path to the final image.
set(Assignment1ES_default_full_path_to_image ${Assignment1ES_default_output_dir}/${Assignment1ES_default_image_name})
