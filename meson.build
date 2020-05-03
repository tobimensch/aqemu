# meson.build is a fast and furious build system
# minimum meson version required for best results: 0.32.0

project('aqemu', 'cpp', default_options : ['cpp_std=c++11', 'warning_level=3'])

qt5 = import('qt5')
qt5dep = dependency('qt5', modules : ['Core', 'Gui', 'Widgets', 'Network', 'Test', 'PrintSupport', 'DBus'])
vncclient = dependency('libvncclient')

incdir = include_directories('src')

add_global_arguments( ['-DVNC_DISPLAY', '-DQTONLY'] , language : 'cpp')

aqemu_headers =    ['src/No_Boot_Device.h',
                    'src/Highlighted_Label.h',
                    'src/main.h',
                    'src/Service.h',
                    'src/Run_Guard.h',
                    'src/QEMU_Docopt.h',
                    'src/IP_Edit.h',
                    'src/Dialog_Button_Box.h',
                    'src/docopt/docopt.h',
                    'src/Settings_Widget.h',
                    'src/QDom.h',
                    'src/tinyxml2.h',
                    'src/About_Window.h',
                    'src/Add_New_Device_Window.h',
                    'src/Add_Port_Window.h',
                    'src/Add_USB_Port_Window.h',
                    'src/Advanced_Settings_Window.h',
                    'src/Boot_Device_Window.h',
                    'src/Convert_HDD_Image_Window.h',
                    'src/Copy_VM_Window.h',
                    'src/Create_HDD_Image_Window.h',
                    'src/Create_Template_Window.h',
                    'src/Delete_VM_Files_Window.h',
                    'src/Device_Manager_Widget.h',
                    'src/Edit_Emulator_Version_Window.h',
                    'src/Emulator_Control_Window.h',
                    'src/Error_Log_Window.h',
                    'src/First_Start_Wizard.h',
                    'src/Folder_Sharing_Widget.h',
                    'src/HDD_Image_Info.h',
                    'src/Main_Window.h',
                    'src/Monitor_Window.h',
                    'src/Network_Widget.h',
                    'src/Old_Network_Widget.h',
                    'src/Ports_Tab_Widget.h',
                    'src/Properties_Window.h',
                    'src/Emulator_Options_Window.h',
                    'src/Select_Icon_Window.h',
                    'src/SMP_Settings_Window.h',
                    'src/Snapshot_Properties_Window.h',
                    'src/Snapshots_Window.h',
                    'src/SPICE_Settings_Widget.h',
                    'src/System_Info.h',
                    'src/Utils.h',
                    'src/VM.h',
                    'src/VM_Devices.h',
                    'src/VM_Wizard_Window.h',
                    'src/VNC_Password_Window.h',
                    'src/Embedded_Display/Machine_View.h',
                    'src/Embedded_Display/remoteview.h',
                    'src/Embedded_Display/vncclientthread.h',
                    'src/Embedded_Display/vncview.h',
                    'src/Embedded_Display/krdc_debug.h']

aqemu_sources =    ['src/No_Boot_Device.cpp',
                    'src/Highlighted_Label.cpp',
                    'src/Service.cpp',
                    'src/Run_Guard.cpp',
                    'src/QEMU_Docopt.cpp',
                    'src/IP_Edit.cpp',
                    'src/Dialog_Button_Box.cpp',
                    'src/docopt/docopt.cpp',
                    'src/Settings_Widget.cpp',
                    'src/QDom.cpp',
                    'src/tinyxml2.cpp',
                    'src/About_Window.cpp',
                    'src/Add_New_Device_Window.cpp',
                    'src/Add_Port_Window.cpp',
                    'src/Add_USB_Port_Window.cpp',
                    'src/Advanced_Settings_Window.cpp',
                    'src/Boot_Device_Window.cpp',
                    'src/Convert_HDD_Image_Window.cpp',
                    'src/Copy_VM_Window.cpp',
                    'src/Create_HDD_Image_Window.cpp',
                    'src/Create_Template_Window.cpp',
                    'src/Delete_VM_Files_Window.cpp',
                    'src/Device_Manager_Widget.cpp',
                    'src/Edit_Emulator_Version_Window.cpp',
                    'src/Emulator_Control_Window.cpp',
                    'src/Error_Log_Window.cpp',
                    'src/First_Start_Wizard.cpp',
                    'src/Folder_Sharing_Widget.cpp',
                    'src/HDD_Image_Info.cpp',
                    'src/main.cpp',
                    'src/Main_Window.cpp',
                    'src/Monitor_Window.cpp',
                    'src/Network_Widget.cpp',
                    'src/Old_Network_Widget.cpp',
                    'src/Ports_Tab_Widget.cpp',
                    'src/Properties_Window.cpp',
                    'src/Emulator_Options_Window.cpp',
                    'src/Select_Icon_Window.cpp',
                    'src/SMP_Settings_Window.cpp',
                    'src/Snapshot_Properties_Window.cpp',
                    'src/Snapshots_Window.cpp',
                    'src/SPICE_Settings_Widget.cpp',
                    'src/System_Info.cpp',
                    'src/Utils.cpp',
                    'src/VM.cpp',
                    'src/VM_Devices.cpp',
                    'src/VM_Wizard_Window.cpp',
                    'src/VNC_Password_Window.cpp',
                    'src/Embedded_Display/Machine_View.cpp',
                    'src/Embedded_Display/remoteview.cpp',
                    'src/Embedded_Display/vncclientthread.cpp',
                    'src/Embedded_Display/vncview.cpp']

aqemu_resources =  ['resources/icons.qrc',
                    'resources/shared_images.qrc']

aqemu_ui =         ['src/No_Boot_Device.ui',
                    'src/About_Window.ui',
                    'src/Add_New_Device_Window.ui',
                    'src/Add_Port_Window.ui',
                    'src/Add_USB_Port_Window.ui',
                    'src/Advanced_Settings_Window.ui',
                    'src/Advanced_Options.ui',
                    'src/Architecture_Options.ui',
                    'src/Boot_Device_Window.ui',
                    'src/Convert_HDD_Image_Window.ui',
                    'src/Copy_VM_Window.ui',
                    'src/Create_HDD_Image_Window.ui',
                    'src/Create_Template_Window.ui',
                    'src/Delete_VM_Files_Window.ui',
                    'src/Device_Manager_Widget.ui',
                    'src/Edit_Emulator_Version_Window.ui',
                    'src/Emulator_Control_Window.ui',
                    'src/Error_Log_Window.ui',
                    'src/First_Start_Wizard.ui',
                    'src/Folder_Sharing_Widget.ui',
                    'src/KVM_Options.ui',
                    'src/Main_Window.ui',
                    'src/Monitor_Window.ui',
                    'src/Network_Widget.ui',
                    'src/Old_Network_Widget.ui',
                    'src/Ports_Tab_Widget.ui',
                    'src/Properties_Window.ui',
                    'src/Emulator_Options_Window.ui',
                    'src/Select_Icon_Window.ui',
                    'src/Settings_Window.ui',
                    'src/SMP_Settings_Window.ui',
                    'src/Snapshot_Properties_Window.ui',
                    'src/Snapshots_Window.ui',
                    'src/SPICE_Settings_Widget.ui',
                    'src/VM_Wizard_Window.ui',
                    'src/VNC_Password_Window.ui']

prep = qt5.preprocess(
                    moc_headers : aqemu_headers,
                    ui_files : aqemu_ui,
                    qresources : aqemu_resources,
                    include_directories : incdir)

q5exe = executable('aqemu',
                    sources : [aqemu_sources, prep],
                    dependencies : [qt5dep, vncclient],
                    include_directories : incdir, install : true)

install_man('resources/docs/aqemu.1')

install_subdir('resources/os_icons/', install_dir : 'share/aqemu')
install_subdir('resources/os_templates/', install_dir : 'share/aqemu')
install_subdir('resources/docs/', install_dir : 'share/aqemu')

install_data('TODO', install_dir : 'share/doc/aqemu')
install_data('CHANGELOG', install_dir : 'share/doc/aqemu')
install_data('AUTHORS', install_dir : 'share/doc/aqemu')
install_data('README', install_dir : 'share/doc/aqemu')

install_data('resources/menu_data/aqemu.png', install_dir : 'share/pixmaps')
install_data('resources/menu_data/aqemu.desktop', install_dir : 'share/applications')
install_data('resources/menu_data/aqemu.appdata.xml', install_dir : 'share/appdata')

# TODO: qt5 translations

