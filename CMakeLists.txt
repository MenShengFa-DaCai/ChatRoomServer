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

# ========== 原有 Windows 平台 DLL 复制逻辑 ==========
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

# 安装可执行文件
install(TARGETS ChatRoomServer
        RUNTIME DESTINATION .  # 安装到目标目录的根目录
)

# 安装 Qt 平台插件（platforms/qwindows.dll 等）
install(DIRECTORY "$<TARGET_FILE_DIR:ChatRoomServer>/plugins/platforms/"
        DESTINATION ./plugins/platforms  # 保持原目录结构
        FILES_MATCHING PATTERN "*.dll"
)

# 安装 Qt 数据库插件（sqldrivers/qsqlite.dll 等）
install(DIRECTORY "$<TARGET_FILE_DIR:ChatRoomServer>/plugins/sqldrivers/"
        DESTINATION ./plugins/sqldrivers
        FILES_MATCHING PATTERN "*.dll"
)

# 安装 Qt 核心 DLL（Qt6Core.dll、Qt6Gui.dll 等）
install(DIRECTORY "$<TARGET_FILE_DIR:ChatRoomServer>/"
        DESTINATION .
        FILES_MATCHING PATTERN "Qt6*.dll"  # 仅复制 Qt 相关 DLL
)


# ========== 新增 CPACK 打包配置 ==========
# 启用 CPack 模块
include(CPack)

# 设置安装包基本信息
set(CPACK_PACKAGE_NAME "ChatRoomServer")                   # 安装包名称
set(CPACK_PACKAGE_VERSION "1.0.0")                         # 版本号
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "聊天服务器程序")       # 简短描述
set(CPACK_PACKAGE_VENDOR "Your Name")                       # 供应商名称
set(CPACK_PACKAGE_INSTALL_DIRECTORY "ChatRoomServer")       # 默认安装路径

# 指定生成器为 NSIS（Windows 安装程序）
set(CPACK_GENERATOR "NSIS")

# 设置 NSIS 特定配置（图标路径需使用绝对路径或相对于构建目录的路径）
# 注意：此处假设构建时图标文件会被复制到可执行文件目录，或直接使用源目录绝对路径
set(ICON_INSTALL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/icon/install.ico")   # 安装包图标路径
set(ICON_UNINSTALL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/icon/uninstall.ico") # 卸载程序图标路径

# 配置 NSIS 脚本参数
set(CPACK_NSIS_MUI_ICON "${ICON_INSTALL_PATH}")             # 安装包图标
set(CPACK_NSIS_MUI_UNIICON "${ICON_UNINSTALL_PATH}")        # 卸载程序图标
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)          # 安装前先卸载旧版本
set(CPACK_NSIS_MODIFY_PATH OFF)                             # 不修改系统 PATH
set(CPACK_NSIS_PACKAGE_NAME "${CPACK_PACKAGE_NAME}")         # 安装程序标题
set(CPACK_NSIS_INSTALLED_ICON_NAME "ChatRoomServer.exe")    # 开始菜单图标关联的程序

# 自定义安装后步骤（可选，如创建桌面快捷方式）
set(CPACK_NSIS_CREATE_ICONS_EXTRA
        "CreateShortCut '$DESKTOP\\\\${CPACK_PACKAGE_NAME}.lnk' '$INSTDIR\\\\ChatRoomServer.exe'")
set(CPACK_NSIS_DELETE_ICONS_EXTRA
        "Delete '$DESKTOP\\\\${CPACK_PACKAGE_NAME}.lnk'")