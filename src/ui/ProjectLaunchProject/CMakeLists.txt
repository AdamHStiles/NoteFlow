cmake_minimum_required(VERSION 3.16)
project(Chat VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets WebSockets)

qt_add_executable(Chat
    main.cpp
    Message.h
    MessageBubble.h
    MessageBubble.cpp
    ChatView.h
    ChatView.cpp
    ConnectDialog.h
    ConnectDialog.cpp
    WsClient.h
    WsClient.cpp
    ChannelItemWidget.h
    ChannelItemWidget.cpp
    MainWindow.h
    MainWindow.cpp
)

target_link_libraries(Chat PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::WebSockets
)
