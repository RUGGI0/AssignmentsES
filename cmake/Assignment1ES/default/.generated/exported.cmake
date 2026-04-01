set(DEPENDENT_MP_BIN2HEXAssignment1ES_default_BJoHZvk8 "/opt/microchip/xc16/v2.10/bin/xc16-bin2hex")
set(DEPENDENT_DEPENDENT_TARGET_ELFAssignment1ES_default_BJoHZvk8 ${CMAKE_CURRENT_LIST_DIR}/../../../../out/Assignment1ES/default.elf)
set(DEPENDENT_TARGET_DIRAssignment1ES_default_BJoHZvk8 ${CMAKE_CURRENT_LIST_DIR}/../../../../out/Assignment1ES)
set(DEPENDENT_BYPRODUCTSAssignment1ES_default_BJoHZvk8 ${DEPENDENT_TARGET_DIRAssignment1ES_default_BJoHZvk8}/${sourceFileNameAssignment1ES_default_BJoHZvk8}.s)
add_custom_command(
    OUTPUT ${DEPENDENT_TARGET_DIRAssignment1ES_default_BJoHZvk8}/${sourceFileNameAssignment1ES_default_BJoHZvk8}.s
    COMMAND ${DEPENDENT_MP_BIN2HEXAssignment1ES_default_BJoHZvk8} ${DEPENDENT_DEPENDENT_TARGET_ELFAssignment1ES_default_BJoHZvk8} --image ${sourceFileNameAssignment1ES_default_BJoHZvk8} ${addressAssignment1ES_default_BJoHZvk8} ${modeAssignment1ES_default_BJoHZvk8} -mdfp=/home/ruggio/.mchp_packs/Microchip/dsPIC33E-GM-GP-MC-GU-MU_DFP/1.6.297/xc16 
    WORKING_DIRECTORY ${DEPENDENT_TARGET_DIRAssignment1ES_default_BJoHZvk8}
    DEPENDS ${DEPENDENT_DEPENDENT_TARGET_ELFAssignment1ES_default_BJoHZvk8})
add_custom_target(
    dependent_produced_source_artifactAssignment1ES_default_BJoHZvk8 
    DEPENDS ${DEPENDENT_TARGET_DIRAssignment1ES_default_BJoHZvk8}/${sourceFileNameAssignment1ES_default_BJoHZvk8}.s
    )
