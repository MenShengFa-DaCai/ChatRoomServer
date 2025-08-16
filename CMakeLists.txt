cmake_minimum_required(VERSION 3.31)
project(ChatRoomServer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "C:/Qt/Qt6/6.9.1/mingw_64")
set(RESOURCES icon/icon.qrc)  # 添加资源文件
find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        Network
        Sql
        REQUIRED)

add_executable(ChatRoomServer main.cpp
        server.cpp
        server.h
        server.ui
        ${RESOURCES})
target_link_libraries(ChatRoomServer
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Network
        Qt::Sql
)

if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    foreach (QT_LIB Core Gui Widgets Network Sql)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/sqldrivers/")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "${QT_INSTALL_PATH}/plugins/sqldrivers/qsqlite${DEBUG_SUFFIX}.dll"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/sqldrivers/")
endif ()

# 设置安装规则
install(TARGETS ChatRoomServer
        RUNTIME DESTINATION bin
        BUNDLE DESTINATION .
)

# 安装 Qt 运行时组件
if(WIN32)
    # 安装主程序依赖的 DLL
    install(FILES
            "${QT_INSTALL_PATH}/bin/Qt6Core.dll"
            "${QT_INSTALL_PATH}/bin/Qt6Gui.dll"
            "${QT_INSTALL_PATH}/bin/Qt6Widgets.dll"
            "${QT_INSTALL_PATH}/bin/Qt6Network.dll"
            "${QT_INSTALL_PATH}/bin/Qt6Sql.dll"
            DESTINATION bin
    )

    # 安装插件
    install(FILES
            "${QT_INSTALL_PATH}/plugins/platforms/qwindows.dll"
            DESTINATION bin/platforms
    )

    install(FILES
            "${QT_INSTALL_PATH}/plugins/sqldrivers/qsqlite.dll"
            DESTINATION bin/sqldrivers
    )

    # 安装图标资源
    install(FILES
            "${CMAKE_CURRENT_SOURCE_DIR}/icon/install.ico"
            "${CMAKE_CURRENT_SOURCE_DIR}/icon/uninstall.ico"
            DESTINATION bin/icon
    )
endif()

# 配置 CPack
include(CPack)
set(CPACK_GENERATOR "NSIS")
set(CPACK_PACKAGE_NAME "ChatRoomServer")
set(CPACK_PACKAGE_VENDOR "YourCompany")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_EXECUTABLES "ChatRoomServer" "Chat Room Server")
set(CPACK_CREATE_DESKTOP_LINKS "ChatRoomServer")
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
set(CPACK_NSIS_MODIFY_PATH OFF)

# 设置安装包图标
set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/icon/install.ico")
set(CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}/icon/uninstall.ico")

# 设置开始菜单名称
set(CPACK_NSIS_PACKAGE_NAME "Chat Room Server")
set(CPACK_NSIS_DISPLAY_NAME "Chat Room Server")
set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\ChatRoomServer.exe")

# 添加快捷方式图标设置
set(CPACK_NSIS_CREATE_ICONS_EXTRA "
    CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Chat Room Server.lnk' '$INSTDIR\\\\bin\\\\ChatRoomServer.exe' '' '$INSTDIR\\\\bin\\\\icon\\\\install.ico'
    CreateShortCut '$DESKTOP\\\\Chat Room Server.lnk' '$INSTDIR\\\\bin\\\\ChatRoomServer.exe' '' '$INSTDIR\\\\bin\\\\icon\\\\install.ico'
")

set(CPACK_NSIS_DELETE_ICONS_EXTRA "
    Delete '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Chat Room Server.lnk'
    Delete '$DESKTOP\\\\Chat Room Server.lnk'
")