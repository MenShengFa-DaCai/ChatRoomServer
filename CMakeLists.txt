cmake_minimum_required(VERSION 3.31)
project(ChatRoomServer)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_PREFIX_PATH "C:/Qt/Qt6/6.9.1/mingw_64")

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        Network
        Sql
        REQUIRED)

qt_add_resources(RESOURCE_FILES
        icon/icon.qrc)

add_executable(ChatRoomServer main.cpp
        server.cpp
        server.h
        server.ui
        ${RESOURCE_FILES})

target_link_libraries(ChatRoomServer
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Network
        Qt::Sql
)

# ========== Windows 平台 DLL 复制逻辑 ==========
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

    # 创建临时目录用于存放运行时文件
    set(RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/runtime")
    file(MAKE_DIRECTORY "${RUNTIME_OUTPUT_DIRECTORY}")
    file(MAKE_DIRECTORY "${RUNTIME_OUTPUT_DIRECTORY}/plugins/platforms")
    file(MAKE_DIRECTORY "${RUNTIME_OUTPUT_DIRECTORY}/plugins/sqldrivers")

    # 复制平台插件
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        file(COPY "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                DESTINATION "${RUNTIME_OUTPUT_DIRECTORY}/plugins/platforms")
    endif ()

    # 复制Qt核心DLL
    foreach (QT_LIB Core Gui Widgets Network Sql)
        file(COPY "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
                DESTINATION "${RUNTIME_OUTPUT_DIRECTORY}")
    endforeach ()

    # 复制SQLite驱动
    file(COPY "${QT_INSTALL_PATH}/plugins/sqldrivers/qsqlite${DEBUG_SUFFIX}.dll"
            DESTINATION "${RUNTIME_OUTPUT_DIRECTORY}/plugins/sqldrivers")

    # 复制可执行文件
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "$<TARGET_FILE:${PROJECT_NAME}>"
            "${RUNTIME_OUTPUT_DIRECTORY}")
endif ()

# ========== 安装规则 ==========
# 1. 安装可执行文件
install(FILES "${RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.exe"
        DESTINATION .
        RENAME "${PROJECT_NAME}.exe"
)

# 2. 安装 Qt 平台插件
install(DIRECTORY "${RUNTIME_OUTPUT_DIRECTORY}/plugins/platforms/"
        DESTINATION ./plugins/platforms
        FILES_MATCHING PATTERN "*.dll"
)

# 3. 安装 Qt 数据库插件
install(DIRECTORY "${RUNTIME_OUTPUT_DIRECTORY}/plugins/sqldrivers/"
        DESTINATION ./plugins/sqldrivers
        FILES_MATCHING PATTERN "*.dll"
)

# 4. 安装 Qt 核心 DLL
file(GLOB QT_DLLS "${RUNTIME_OUTPUT_DIRECTORY}/Qt6*.dll")
install(FILES ${QT_DLLS}
        DESTINATION .
)

# ========== CPACK 打包配置 ==========
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)  # 防止创建额外的顶层目录
set(CPACK_PACKAGE_NAME "ChatRoomServer")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "聊天服务器程序")
set(CPACK_PACKAGE_VENDOR "Your Name")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "ChatRoomServer")

set(CPACK_GENERATOR "NSIS")

set(ICON_INSTALL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/icon/install.ico")
set(ICON_UNINSTALL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/icon/uninstall.ico")

set(CPACK_NSIS_MUI_ICON "${ICON_INSTALL_PATH}")
set(CPACK_NSIS_MUI_UNIICON "${ICON_UNINSTALL_PATH}")
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
set(CPACK_NSIS_MODIFY_PATH OFF)
set(CPACK_NSIS_PACKAGE_NAME "${CPACK_PACKAGE_NAME}")
set(CPACK_NSIS_INSTALLED_ICON_NAME "ChatRoomServer.exe")

# 自定义快捷方式
set(CPACK_NSIS_CREATE_ICONS_EXTRA
        "CreateShortCut '$DESKTOP\\\\${CPACK_PACKAGE_NAME}.lnk' '$INSTDIR\\\\ChatRoomServer.exe'")
set(CPACK_NSIS_DELETE_ICONS_EXTRA
        "Delete '$DESKTOP\\\\${CPACK_PACKAGE_NAME}.lnk'")

include(CPack)