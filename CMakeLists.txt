# AQEMU CMake Project

# Authors:
# Andrey Rijov (AQEMU developer)
# Timothy Redaelli (Fix for build in a different folder)

PROJECT( AQEMU )
CMAKE_MINIMUM_REQUIRED( VERSION 2.4.0 )

# Available options
OPTION( DEBUG "Build with debug information" )
OPTION( WITHOUT_EMBEDDED_DISPLAY "Build without support Embedded VNC Display" )
OPTION( UPDATE_TRANSLATIONS "Update source translation files (WARNING: make clean will delete the source .ts files!)" )
OPTION( INSTALL_MAN "Install MAN page file" "ON" )

# Debug mode
IF( DEBUG )
	# Verbose compiler output
	SET( CMAKE_VERBOSE_MAKEFILE ON )
	
	# GCC
	ADD_DEFINITIONS( -Wall -g )
ENDIF( DEBUG )

# For find VNC libs
SET( CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules )

# Files
SET( aqemu_headers
	About_Window.h
	Add_New_Device_Window.h
	Add_Port_Window.h
	Add_USB_Port_Window.h
	Advanced_Settings_Window.h
	Boot_Device_Window.h
	Convert_HDD_Image_Window.h
	Copy_VM_Window.h
	Create_HDD_Image_Window.h
	Create_Template_Window.h
	Delete_VM_Files_Window.h
	Device_Manager_Widget.h
	Edit_Emulator_Version_Window.h
	Emulator_Control_Window.h
	Error_Log_Window.h
	First_Start_Wizard.h
	HDD_Image_Info.h
	Main_Window.h
	Monitor_Window.h
	Network_Widget.h
	Old_Network_Widget.h
	Ports_Tab_Widget.h
	Properties_Window.h
	Emulator_Options_Window.h
	Select_Icon_Window.h
	Settings_Window.h
	SMP_Settings_Window.h
	Snapshot_Properties_Window.h
	Snapshots_Window.h
	SPICE_Settings_Widget.h
	System_Info.h
	Utils.h
	VM.h
	VM_Devices.h
	VM_Wizard_Window.h
	VNC_Password_Window.h
)

SET( aqemu_sources
	About_Window.cpp
	Add_New_Device_Window.cpp
	Add_Port_Window.cpp
	Add_USB_Port_Window.cpp
	Advanced_Settings_Window.cpp
	Boot_Device_Window.cpp
	Convert_HDD_Image_Window.cpp
	Copy_VM_Window.cpp
	Create_HDD_Image_Window.cpp
	Create_Template_Window.cpp
	Delete_VM_Files_Window.cpp
	Device_Manager_Widget.cpp
	Edit_Emulator_Version_Window.cpp
	Emulator_Control_Window.cpp
	Error_Log_Window.cpp
	First_Start_Wizard.cpp
	HDD_Image_Info.cpp
	main.cpp
	Main_Window.cpp
	Monitor_Window.cpp
	Network_Widget.cpp
	Old_Network_Widget.cpp
	Ports_Tab_Widget.cpp
	Properties_Window.cpp
	Emulator_Options_Window.cpp
	Select_Icon_Window.cpp
	Settings_Window.cpp
	SMP_Settings_Window.cpp
	Snapshot_Properties_Window.cpp
	Snapshots_Window.cpp
	SPICE_Settings_Widget.cpp
	System_Info.cpp
	Utils.cpp
	VM.cpp
	VM_Devices.cpp
	VM_Wizard_Window.cpp
	VNC_Password_Window.cpp
)

SET( aqemu_ui
	About_Window.ui
	Add_New_Device_Window.ui
	Add_Port_Window.ui
	Add_USB_Port_Window.ui
	Advanced_Settings_Window.ui
	Boot_Device_Window.ui
	Convert_HDD_Image_Window.ui
	Copy_VM_Window.ui
	Create_HDD_Image_Window.ui
	Create_Template_Window.ui
	Delete_VM_Files_Window.ui
	Device_Manager_Widget.ui
	Edit_Emulator_Version_Window.ui
	Emulator_Control_Window.ui
	Error_Log_Window.ui
	First_Start_Wizard.ui
	Main_Window.ui
	Monitor_Window.ui
	Network_Widget.ui
	Old_Network_Widget.ui
	Ports_Tab_Widget.ui
	Properties_Window.ui
	Emulator_Options_Window.ui
	Select_Icon_Window.ui
	Settings_Window.ui
	SMP_Settings_Window.ui
	Snapshot_Properties_Window.ui
	Snapshots_Window.ui
	SPICE_Settings_Widget.ui
	VM_Wizard_Window.ui
	VNC_Password_Window.ui
)

SET( aqemu_resouces shared_images.qrc )
SET( translations German.ts Russian.ts Ukrainian.ts French.ts English.ts )
SET( files_to_translate ${files_to_translate} ${aqemu_headers} ${aqemu_sources} ${aqemu_ui} )


# Check Qt
SET( QT_MIN_VERSION "4.4.2" )
FIND_PACKAGE( Qt4 REQUIRED )
INCLUDE( ${QT_USE_FILE} )
SET( QT_USE_QTXML TRUE )
SET( QT_USE_QTNETWORK TRUE )
SET( QT_USE_QTTEST TRUE )


# Embedded VNC display
IF( NOT WITHOUT_EMBEDDED_DISPLAY )
	FIND_PACKAGE( LibVNCServer REQUIRED )
	
	SET( aqemu_headers ${aqemu_headers} Embedded_Display/Machine_View.h Embedded_Display/remoteview.h
										Embedded_Display/vncclientthread.h Embedded_Display/vncview.h )
	
	SET( aqemu_sources ${aqemu_sources} Embedded_Display/Machine_View.cpp Embedded_Display/remoteview.cpp
										Embedded_Display/vncclientthread.cpp Embedded_Display/vncview.cpp )
	
	ADD_DEFINITIONS( -DVNC_DISPLAY -DQTONLY )
	ADD_LIBRARY( libvncclient SHARED ${aqemu_headers} ${aqemu_sources} )
	
	INCLUDE_DIRECTORIES( ${VNCCLIENT_INCLUDE_DIRS} ${LIBVNCCLIENT_INCLUDE_DIR} )
ENDIF( NOT WITHOUT_EMBEDDED_DISPLAY )

# Add Qt definitions
ADD_DEFINITIONS( ${QT_DEFINITIONS} )

# MOC and other Qt stuff
QT4_WRAP_UI( ui_headers ${aqemu_ui} )
QT4_WRAP_CPP( moc_srcs ${aqemu_headers} )
QT4_ADD_RESOURCES( moc_srcs ${aqemu_resouces} )


# For translation
IF( UPDATE_TRANSLATIONS )
	QT4_CREATE_TRANSLATION( qm_files ${CMAKE_CURRENT_SOURCE_DIR} ${files_to_translate} ${translations} )
ELSE( UPDATE_TRANSLATIONS )
	QT4_ADD_TRANSLATION( qm_files ${translations} )
ENDIF( UPDATE_TRANSLATIONS )

ADD_CUSTOM_TARGET( translations_target DEPENDS ${qm_files} )


# Building
INCLUDE_DIRECTORIES(
	${QT_INCLUDES}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
	${QT_INCLUDE_DIR}
	${CMAKE_BINARY_DIR}
	${CMAKE_SOURCE_DIR}
	${QT_QTNETWORK_INCLUDE_DIR}
	${QT_QTTEST_INCLUDE_DIR}
)

IF( COMMAND CMAKE_POLICY )
	CMAKE_POLICY( SET CMP0003 NEW )
ENDIF( COMMAND CMAKE_POLICY )

ADD_EXECUTABLE( aqemu
	${aqemu_sources}
	${ui_headers}
	${moc_srcs}
	${qm_files}
)

TARGET_LINK_LIBRARIES( aqemu
	${QT_LIBRARIES}
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${QT_QTNETWORK_LIBRARIES}
	${QT_QTTEST_LIBRARIES}
	${QT_QTXML_LIBRARY}
)

# Embedded VNC display
IF( NOT WITHOUT_EMBEDDED_DISPLAY )
	TARGET_LINK_LIBRARIES( aqemu ${LIBVNCCLIENT_LIBRARIES} )
ENDIF( NOT WITHOUT_EMBEDDED_DISPLAY )

# Install files
INSTALL( TARGETS aqemu DESTINATION bin )

INSTALL( FILES ${qm_files} DESTINATION share/aqemu )

FILE( GLOB os_icons "${CMAKE_CURRENT_SOURCE_DIR}/os_icons/*.*" )
INSTALL( FILES ${os_icons} DESTINATION share/aqemu/os_icons )

FILE( GLOB os_templates "${CMAKE_CURRENT_SOURCE_DIR}/os_templates/*.*")
INSTALL( FILES ${os_templates} DESTINATION share/aqemu/os_templates )

FILE( GLOB links "${CMAKE_CURRENT_SOURCE_DIR}/aqemu_links.html" )
INSTALL( FILES ${links} DESTINATION share/aqemu )

FILE( GLOB pixmaps "${CMAKE_CURRENT_SOURCE_DIR}/menu_data/*.png" )
INSTALL( FILES ${pixmaps} DESTINATION share/pixmaps )

FILE( GLOB app_menu "${CMAKE_CURRENT_SOURCE_DIR}/menu_data/aqemu.desktop" )
INSTALL( FILES ${app_menu} DESTINATION share/applications )

# Create docs .bz2 files
MACRO( create_docs _sources )
	FOREACH( curFile ${ARGN} )
		GET_FILENAME_COMPONENT( _in ${curFile} ABSOLUTE )
		GET_FILENAME_COMPONENT( _basename ${curFile} NAME_WE )
		
		SET( _out ${CMAKE_CURRENT_SOURCE_DIR}/${_basename}.bz2 )
		
		ADD_CUSTOM_COMMAND( OUTPUT ${_out} COMMAND bzip2 ARGS -zkf9 ${_in} DEPENDS ${_in} )
		
		SET( ${_sources} ${${_sources}} ${_out} )
	ENDFOREACH( curFile )
ENDMACRO( create_docs )

CREATE_DOCS( docs_bz2 AUTHORS CHANGELOG README TODO )
ADD_CUSTOM_TARGET( docs ALL DEPENDS ${docs_bz2} )
INSTALL( FILES ${docs_bz2} DESTINATION share/doc/aqemu )

# Build RCC files (GUI icons)
MACRO( pack_icons _sources )
	FOREACH( curFile ${ARGN} )
		GET_FILENAME_COMPONENT( _in ${curFile} ABSOLUTE )
		GET_FILENAME_COMPONENT( _basename ${curFile} NAME_WE )
		
		SET( _out ${CMAKE_CURRENT_SOURCE_DIR}/${_basename}.rcc )
		
		ADD_CUSTOM_COMMAND( OUTPUT ${_out} COMMAND rcc ARGS ${_in} -binary -o ${_out} DEPENDS ${_in} )
		
		SET( ${_sources} ${${_sources}} ${_out} )
	ENDFOREACH( curFile )
ENDMACRO( pack_icons )

PACK_ICONS( gui_icons crystalsvg_icons.qrc oxygen_icons.qrc )
ADD_CUSTOM_TARGET( icons ALL DEPENDS ${gui_icons} )
INSTALL( FILES ${gui_icons} DESTINATION share/aqemu )

# Compress and install MAN page file
IF( NOT MAN_PAGE_COMPRESSOR )
	MESSAGE( STATUS "MAN page will be installed is not packed." )
ELSEIF( MAN_PAGE_COMPRESSOR STREQUAL "gzip" )
	SET( COMPRESSOR_SUFFIX gz )
ELSEIF( MAN_PAGE_COMPRESSOR STREQUAL "bzip2" )
	SET( COMPRESSOR_SUFFIX bz2 )
ELSEIF( MAN_PAGE_COMPRESSOR STREQUAL "lzma" )
	SET( COMPRESSOR_SUFFIX lzma )
ELSE( NOT MAN_PAGE_COMPRESSOR )
	MESSAGE( STATUS "${MAN_PAGE_COMPRESSOR} not supported!" )
	MESSAGE( FATAL_ERROR "Correct values: gzip, bzip2, lzma" )
ENDIF( NOT MAN_PAGE_COMPRESSOR )

# Compress MAN file?
IF( INSTALL_MAN )
	IF( COMPRESSOR_SUFFIX )
		# Find gzip, bzip or lzma
		FIND_PROGRAM( COMPRESSOR_BIN NAMES ${MAN_PAGE_COMPRESSOR} )
		
		IF( ${COMPRESSOR_BIN} MATCHES COMPRESSOR_BIN-NOTFOUND )
			MESSAGE( STATUS "${MAN_PAGE_COMPRESSOR} - NOT FOUND!" )
			MESSAGE( FATAL_ERROR "Please install ${MAN_PAGE_COMPRESSOR} package!" )
		ENDIF( ${COMPRESSOR_BIN} MATCHES COMPRESSOR_BIN-NOTFOUND )
		
		# Compress
		EXECUTE_PROCESS( OUTPUT_FILE aqemu.1.${COMPRESSOR_SUFFIX}
						COMMAND "${COMPRESSOR_BIN}" -9
						INPUT_FILE aqemu.1
						WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} )
		
		# Install MAN
		INSTALL( FILES ${CMAKE_SOURCE_DIR}/aqemu.1.${COMPRESSOR_SUFFIX} DESTINATION share/man/man1 )
	ELSE( COMPRESSOR_SUFFIX )
		MESSAGE( STATUS "MAN_PAGE_COMPRESSOR flag not set. Available values: gzip, bzip2, lzma" )
		MESSAGE( STATUS "Will be installed not compressed MAN pages" )
		
		INSTALL( FILES ${CMAKE_SOURCE_DIR}/aqemu.1 DESTINATION share/man/man1 )
	ENDIF( COMPRESSOR_SUFFIX )
ENDIF( INSTALL_MAN )
