# AQEMU CMake Project

# Authors:
# Andrey Rijov (AQEMU developer)
# Timothy Redaelli (Fix for build in a different folder)
# Tobias Gläßer (Qt5 port)

PROJECT( AQEMU CXX )
CMAKE_MINIMUM_REQUIRED( VERSION 3.1.0 FATAL_ERROR )

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

# For finding VNC libs
SET( CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/resources/cmake/modules )

# Files
SET( aqemu_headers
    src/No_Boot_Device.h
    src/Highlighted_Label.h
    src/Service.h
    src/Run_Guard.h
    src/QEMU_Docopt.h
    src/IP_Edit.h
    src/Dialog_Button_Box.h
	src/docopt/docopt.h
    	src/Settings_Widget.h
    	src/QDom.h
    	src/tinyxml2.h
	src/About_Window.h
	src/Add_New_Device_Window.h
	src/Add_Port_Window.h
	src/Add_USB_Port_Window.h
	src/Advanced_Settings_Window.h
	src/Boot_Device_Window.h
	src/Convert_HDD_Image_Window.h
	src/Copy_VM_Window.h
	src/Create_HDD_Image_Window.h
	src/Create_Template_Window.h
	src/Delete_VM_Files_Window.h
	src/Device_Manager_Widget.h
	src/Edit_Emulator_Version_Window.h
	src/Emulator_Control_Window.h
	src/Error_Log_Window.h
	src/First_Start_Wizard.h
	src/Folder_Sharing_Widget.h
	src/HDD_Image_Info.h
	src/Main_Window.h
	src/Monitor_Window.h
	src/Network_Widget.h
	src/Old_Network_Widget.h
	src/Ports_Tab_Widget.h
	src/Properties_Window.h
	src/Emulator_Options_Window.h
	src/Select_Icon_Window.h
	src/SMP_Settings_Window.h
	src/Snapshot_Properties_Window.h
	src/Snapshots_Window.h
	src/SPICE_Settings_Widget.h
	src/System_Info.h
	src/Utils.h
	src/VM.h
	src/VM_Devices.h
	src/VM_Wizard_Window.h
	src/VNC_Password_Window.h
)

SET( aqemu_sources
    src/No_Boot_Device.cpp
    src/Highlighted_Label.cpp
    src/Service.cpp
    src/Run_Guard.cpp
    src/QEMU_Docopt.cpp
    src/IP_Edit.cpp
    src/Dialog_Button_Box.cpp
	src/docopt/docopt.cpp
	src/Settings_Widget.cpp
    	src/QDom.cpp
    	src/tinyxml2.cpp
	src/About_Window.cpp
	src/Add_New_Device_Window.cpp
	src/Add_Port_Window.cpp
	src/Add_USB_Port_Window.cpp
	src/Advanced_Settings_Window.cpp
	src/Boot_Device_Window.cpp
	src/Convert_HDD_Image_Window.cpp
	src/Copy_VM_Window.cpp
	src/Create_HDD_Image_Window.cpp
	src/Create_Template_Window.cpp
	src/Delete_VM_Files_Window.cpp
	src/Device_Manager_Widget.cpp
	src/Edit_Emulator_Version_Window.cpp
	src/Emulator_Control_Window.cpp
	src/Error_Log_Window.cpp
	src/First_Start_Wizard.cpp
	src/Folder_Sharing_Widget.cpp
	src/HDD_Image_Info.cpp
	src/main.cpp
	src/Main_Window.cpp
	src/Monitor_Window.cpp
	src/Network_Widget.cpp
	src/Old_Network_Widget.cpp
	src/Ports_Tab_Widget.cpp
	src/Properties_Window.cpp
	src/Emulator_Options_Window.cpp
	src/Select_Icon_Window.cpp
	src/SMP_Settings_Window.cpp
	src/Snapshot_Properties_Window.cpp
	src/Snapshots_Window.cpp
	src/SPICE_Settings_Widget.cpp
	src/System_Info.cpp
	src/Utils.cpp
	src/VM.cpp
	src/VM_Devices.cpp
	src/VM_Wizard_Window.cpp
	src/VNC_Password_Window.cpp
)

SET( aqemu_ui
    src/No_Boot_Device.ui
	src/About_Window.ui
	src/Add_New_Device_Window.ui
	src/Add_Port_Window.ui
	src/Add_USB_Port_Window.ui
	src/Advanced_Settings_Window.ui
	src/Boot_Device_Window.ui
	src/Convert_HDD_Image_Window.ui
	src/Copy_VM_Window.ui
	src/Create_HDD_Image_Window.ui
	src/Create_Template_Window.ui
	src/Delete_VM_Files_Window.ui
	src/Device_Manager_Widget.ui
	src/Edit_Emulator_Version_Window.ui
	src/Emulator_Control_Window.ui
	src/Error_Log_Window.ui
	src/First_Start_Wizard.ui
	src/Folder_Sharing_Widget.ui
	src/Main_Window.ui
	src/Monitor_Window.ui
	src/Network_Widget.ui
	src/Old_Network_Widget.ui
	src/Ports_Tab_Widget.ui
	src/Properties_Window.ui
	src/Emulator_Options_Window.ui
	src/Select_Icon_Window.ui
	src/Settings_Window.ui
	src/SMP_Settings_Window.ui
	src/Snapshot_Properties_Window.ui
	src/Snapshots_Window.ui
	src/SPICE_Settings_Widget.ui
	src/VM_Wizard_Window.ui
	src/VNC_Password_Window.ui
)

SET( aqemu_qrc 
    resources/shared_images.qrc
    resources/icons.qrc
)

SET( aqemu_man_page
    resources/docs/aqemu.1 )

SET( translations 
     translations/German.ts
     transtations/Russian.ts
     translations/Ukrainian.ts
     translations/French.ts
     translations/English.ts )

SET( files_to_translate ${files_to_translate} ${aqemu_headers} ${aqemu_sources} ${aqemu_ui} )

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
# Instruct CMake to run uic automatically when needed.
SET(CMAKE_AUTOUIC ON)
# Instruct CMake to run rcc automatically when needed.
SET(CMAKE_AUTORCC ON)

# Check Qt
FIND_PACKAGE( Qt5Core )
FIND_PACKAGE( Qt5Widgets )
FIND_PACKAGE( Qt5Network )
FIND_PACKAGE( Qt5Test )
FIND_PACKAGE( Qt5PrintSupport )
FIND_PACKAGE( Qt5DBus )

# Embedded VNC display
IF( NOT WITHOUT_EMBEDDED_DISPLAY )
	FIND_PACKAGE( LibVNCServer REQUIRED )
	
	SET( aqemu_headers ${aqemu_headers} src/Embedded_Display/Machine_View.h src/Embedded_Display/remoteview.h
										src/Embedded_Display/vncclientthread.h src/Embedded_Display/vncview.h )
	
	SET( aqemu_sources ${aqemu_sources} src/Embedded_Display/Machine_View.cpp src/Embedded_Display/remoteview.cpp
										src/Embedded_Display/vncclientthread.cpp src/Embedded_Display/vncview.cpp )
	
	ADD_DEFINITIONS( -DVNC_DISPLAY -DQTONLY )
ENDIF( NOT WITHOUT_EMBEDDED_DISPLAY )

# TODO: port from Qt4 to Qt5
# For translation
#IF( UPDATE_TRANSLATIONS )
#	QT5_CREATE_TRANSLATION( qm_files ${CMAKE_CURRENT_SOURCE_DIR} ${files_to_translate} ${translations} )
#ELSE( UPDATE_TRANSLATIONS )
#	QT5_ADD_TRANSLATION( qm_files ${translations} )
#ENDIF( UPDATE_TRANSLATIONS )

ADD_CUSTOM_TARGET( translations_target DEPENDS ${qm_files} )

IF( COMMAND CMAKE_POLICY )
	CMAKE_POLICY( SET CMP0003 NEW )
ENDIF( COMMAND CMAKE_POLICY )

ADD_EXECUTABLE( aqemu
	${aqemu_sources}
	${ui_headers}
	${moc_srcs}
	${qm_files}
    ${aqemu_qrc}
)

target_include_directories(aqemu PUBLIC src/ )

# Require C++-11 standard
SET_PROPERTY(TARGET aqemu PROPERTY CXX_STANDARD 11)
SET_PROPERTY(TARGET aqemu PROPERTY CXX_STANDARD_REQUIRED ON)

TARGET_LINK_LIBRARIES( aqemu
	Qt5::Widgets
    Qt5::Network
    Qt5::Test
    Qt5::PrintSupport
    Qt5::DBus
)

# Embedded VNC display
IF( NOT WITHOUT_EMBEDDED_DISPLAY )
	TARGET_LINK_LIBRARIES( aqemu ${LIBVNCCLIENT_LIBRARIES} )
ENDIF( NOT WITHOUT_EMBEDDED_DISPLAY )

# Install files
INSTALL( TARGETS aqemu DESTINATION bin )

INSTALL( FILES ${qm_files} DESTINATION share/aqemu )

FILE( GLOB os_icons "${CMAKE_CURRENT_SOURCE_DIR}/resources/os_icons/*.*" )
INSTALL( FILES ${os_icons} DESTINATION share/aqemu/os_icons )

FILE( GLOB os_templates "${CMAKE_CURRENT_SOURCE_DIR}/resources/os_templates/*.*")
INSTALL( FILES ${os_templates} DESTINATION share/aqemu/os_templates )

FILE( GLOB links "${CMAKE_CURRENT_SOURCE_DIR}/resources/docs/links.html" )
INSTALL( FILES ${links} DESTINATION share/aqemu/docs )

FILE( GLOB pixmaps "${CMAKE_CURRENT_SOURCE_DIR}/resources/menu_data/*.png" )
INSTALL( FILES ${pixmaps} DESTINATION share/pixmaps )

FILE( GLOB app_menu "${CMAKE_CURRENT_SOURCE_DIR}/resources/menu_data/aqemu.desktop" )
INSTALL( FILES ${app_menu} DESTINATION share/applications )

FILE( GLOB app_menu "${CMAKE_CURRENT_SOURCE_DIR}/resources/menu_data/aqemu.appdata.xml" )
INSTALL( FILES ${app_menu} DESTINATION share/appdata )

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
		EXECUTE_PROCESS( OUTPUT_FILE ${aqemu_man_page}.${COMPRESSOR_SUFFIX}
						COMMAND "${COMPRESSOR_BIN}" -9
						INPUT_FILE ${aqemu_man_page}
						WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} )
		
		# Install MAN
		INSTALL( FILES ${CMAKE_SOURCE_DIR}/${aqemu_man_page}.${COMPRESSOR_SUFFIX} DESTINATION share/man/man1 )
	ELSE( COMPRESSOR_SUFFIX )
		MESSAGE( STATUS "MAN_PAGE_COMPRESSOR flag not set. Available values: gzip, bzip2, lzma" )
		MESSAGE( STATUS "Will be installed not compressed MAN pages" )
		
		INSTALL( FILES ${CMAKE_SOURCE_DIR}/${aqemu_man_page} DESTINATION share/man/man1 )
	ENDIF( COMPRESSOR_SUFFIX )
ENDIF( INSTALL_MAN )
