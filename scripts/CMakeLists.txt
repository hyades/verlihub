SET(SCRIPT_FILES
	vh_daemon
	vh_lib
	vh_gui
	vh
	vh_multihub
	vh_regimporter
	vh_migration_0.9.8eto1.0
)

# Autotool variables that will be replaced
SET(prefix ${CMAKE_INSTALL_PREFIX})
SET(exec_prefix ${CMAKE_INSTALL_PREFIX})
SET(libdir ${CMAKE_INSTALL_PREFIX}/lib)
SET(bindir ${CMAKE_INSTALL_PREFIX}/bin)
SET(datadir ${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME})

FOREACH(VH_SCRIPT ${SCRIPT_FILES})
	MESSAGE(STATUS "Generating ${VH_SCRIPT}")
	CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/scripts/${VH_SCRIPT}.in ${CMAKE_BINARY_DIR}/scripts/${VH_SCRIPT} IMMEDIATE @ONLY)
	LIST(APPEND GENERATED_SCRIPTS ${VERLIHUB_BINARY_DIR}/scripts/${VH_SCRIPT})
ENDFOREACH(VH_SCRIPT ${SCRIPT_FILES})

INSTALL(
	FILES ${GENERATED_SCRIPTS}
	DESTINATION bin
	PERMISSIONS
		OWNER_READ OWNER_EXECUTE
		GROUP_READ GROUP_EXECUTE
	WORLD_READ WORLD_EXECUTE
)
