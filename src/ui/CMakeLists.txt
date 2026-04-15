cmake_minimum_required(VERSION 3.19)
project(NoteFlow LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Widgets WebSockets PrintSupport)

qt_standard_project_setup()

qt_add_executable(NoteFlow
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    channelitemwidget.cpp
    channelitemwidget.h
    chatview.cpp
    chatview.h
    connectdialog.cpp
    connectdialog.h
    Message.h
    messagebubble.cpp
    messagebubble.h
    wsclient.cpp
    wsclient.h
    fileEditor.cpp
    fileEditor.h
    fileitemwidget.h
    fileitemwidget.cpp
)

target_link_libraries(NoteFlow
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt6::Gui
        Qt6::PrintSupport
        Qt6::WebSockets
        "C:/QScintilla_src/src/release/libqscintilla2_qt6.a" # your directory here!
)

install(TARGETS NoteFlow
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET NoteFlow
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
