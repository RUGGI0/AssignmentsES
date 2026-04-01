set(DEPENDENT_MP_BIN2HEXAssignment2_default_3BdpTXja "/opt/microchip/xc16/v2.10/bin/xc16-bin2hex")
set(DEPENDENT_DEPENDENT_TARGET_ELFAssignment2_default_3BdpTXja ${CMAKE_CURRENT_LIST_DIR}/../../../../out/Assignment2/default.elf)
set(DEPENDENT_TARGET_DIRAssignment2_default_3BdpTXja ${CMAKE_CURRENT_LIST_DIR}/../../../../out/Assignment2)
set(DEPENDENT_BYPRODUCTSAssignment2_default_3BdpTXja ${DEPENDENT_TARGET_DIRAssignment2_default_3BdpTXja}/${sourceFileNameAssignment2_default_3BdpTXja}.s)
add_custom_command(
    OUTPUT ${DEPENDENT_TARGET_DIRAssignment2_default_3BdpTXja}/${sourceFileNameAssignment2_default_3BdpTXja}.s
    COMMAND ${DEPENDENT_MP_BIN2HEXAssignment2_default_3BdpTXja} ${DEPENDENT_DEPENDENT_TARGET_ELFAssignment2_default_3BdpTXja} --image ${sourceFileNameAssignment2_default_3BdpTXja} ${addressAssignment2_default_3BdpTXja} ${modeAssignment2_default_3BdpTXja} -mdfp=/home/ruggio/.mchp_packs/Microchip/dsPIC33E-GM-GP-MC-GU-MU_DFP/1.6.297/xc16 
    WORKING_DIRECTORY ${DEPENDENT_TARGET_DIRAssignment2_default_3BdpTXja}
    DEPENDS ${DEPENDENT_DEPENDENT_TARGET_ELFAssignment2_default_3BdpTXja})
add_custom_target(
    dependent_produced_source_artifactAssignment2_default_3BdpTXja 
    DEPENDS ${DEPENDENT_TARGET_DIRAssignment2_default_3BdpTXja}/${sourceFileNameAssignment2_default_3BdpTXja}.s
    )
