######################################################################
# Qt4 qmake project file for AQEMU.
######################################################################

TEMPLATE = app
TARGET = 
INCLUDEPATH += .

HEADERS += About_Window.h \
           Add_New_Device_Window.h \
           Add_Port_Window.h \
           Add_USB_Port_Window.h \
           Advanced_Settings_Window.h \
           Boot_Device_Window.h \
           Convert_HDD_Image_Window.h \
           Copy_VM_Window.h \
           Create_HDD_Image_Window.h \
           Create_Template_Window.h \
           Delete_VM_Files_Window.h \
           Device_Manager_Widget.h \
           Edit_Emulator_Version_Window.h \
           Emulator_Control_Window.h \
           Emulator_Options_Window.h \
           Error_Log_Window.h \
           First_Start_Wizard.h \
           HDD_Image_Info.h \
           Main_Window.h \
           Monitor_Window.h \
           Network_Widget.h \
           Old_Network_Widget.h \
           Ports_Tab_Widget.h \
           Properties_Window.h \
           Select_Icon_Window.h \
           Settings_Window.h \
           SMP_Settings_Window.h \
           Snapshot_Properties_Window.h \
           Snapshots_Window.h \
           SPICE_Settings_Widget.h \
           System_Info.h \
           Utils.h \
           VM.h \
           VM_Devices.h \
           VM_Wizard_Window.h \
           VNC_Password_Window.h

FORMS += About_Window.ui \
         Add_New_Device_Window.ui \
         Add_Port_Window.ui \
         Add_USB_Port_Window.ui \
         Advanced_Settings_Window.ui \
         Boot_Device_Window.ui \
         Convert_HDD_Image_Window.ui \
         Copy_VM_Window.ui \
         Create_HDD_Image_Window.ui \
         Create_Template_Window.ui \
         Delete_VM_Files_Window.ui \
         Device_Manager_Widget.ui \
         Edit_Emulator_Version_Window.ui \
         Emulator_Control_Window.ui \
         Emulator_Options_Window.ui \
         Error_Log_Window.ui \
         First_Start_Wizard.ui \
         Main_Window.ui \
         Monitor_Window.ui \
         Network_Widget.ui \
         Old_Network_Widget.ui \
         Ports_Tab_Widget.ui \
         Properties_Window.ui \
         Select_Icon_Window.ui \
         Settings_Window.ui \
         SMP_Settings_Window.ui \
         Snapshot_Properties_Window.ui \
         Snapshots_Window.ui \
         SPICE_Settings_Widget.ui \
         VM_Wizard_Window.ui \
         VNC_Password_Window.ui

SOURCES += About_Window.cpp \
           Add_New_Device_Window.cpp \
           Add_Port_Window.cpp \
           Add_USB_Port_Window.cpp \
           Advanced_Settings_Window.cpp \
           Boot_Device_Window.cpp \
           Convert_HDD_Image_Window.cpp \
           Copy_VM_Window.cpp \
           Create_HDD_Image_Window.cpp \
           Create_Template_Window.cpp \
           Delete_VM_Files_Window.cpp \
           Device_Manager_Widget.cpp \
           Edit_Emulator_Version_Window.cpp \
           Emulator_Control_Window.cpp \
           Emulator_Options_Window.cpp \
           Error_Log_Window.cpp \
           First_Start_Wizard.cpp \
           HDD_Image_Info.cpp \
           main.cpp \
           Main_Window.cpp \
           Monitor_Window.cpp \
           Network_Widget.cpp \
           Old_Network_Widget.cpp \
           Ports_Tab_Widget.cpp \
           Properties_Window.cpp \
           Select_Icon_Window.cpp \
           Settings_Window.cpp \
           SMP_Settings_Window.cpp \
           Snapshot_Properties_Window.cpp \
           Snapshots_Window.cpp \
           SPICE_Settings_Widget.cpp \
           System_Info.cpp \
           Utils.cpp \
           VM.cpp \
           VM_Devices.cpp \
           VM_Wizard_Window.cpp \
           VNC_Password_Window.cpp

RESOURCES += oxygen_icons.qrc shared_images.qrc
TRANSLATIONS += German.ts Russian.ts Ukrainian.ts French.ts English.ts
QT += xml network
#CONFIG += debug console
CONFIG += release
