cmake_minimum_required(VERSION 3.16)
project(QHotkey LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 自动处理MOC和头文件
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# 平台检测
if(WIN32)
    set(PLATFORM_SPECIFIC_SRC
        qhotkey_win.cpp
    )
    add_definitions(-DQHOTKEY_SHARED -DQHOTKEY_LIBRARY)
elseif(APPLE)
    set(PLATFORM_SPECIFIC_SRC
        qhotkey_mac.cpp
    )
else()
    set(PLATFORM_SPECIFIC_SRC
        qhotkey_x11.cpp
    )
endif()

# 源文件集合
set(QHOTKEY_SOURCES
    qhotkey.cpp
    ${PLATFORM_SPECIFIC_SRC}
)

# 头文件配置
set(QHOTKEY_HEADERS
    qhotkey.h
    qhotkey_p.h
)

# 创建库目标
add_library(QHotkey STATIC ${QHOTKEY_SOURCES} ${QHOTKEY_HEADERS})

# 包含目录
target_include_directories(QHotkey
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# 链接依赖
target_link_libraries(QHotkey
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
)

# Windows特殊处理
if(WIN32)
    target_sources(QHotkey PRIVATE qhotkey_win.cpp)
    target_link_libraries(QHotkey PRIVATE user32)
    set_target_properties(QHotkey PROPERTIES
        WINDOWS_EXPORT_ALL_SYMBOLS ON  # 自动导出符号
    )
endif()

# 安装配置（可选）
install(TARGETS QHotkey
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES QHotkey/qhotkey.h
    DESTINATION include/QHotkey
)

# 版本信息
set_target_properties(QHotkey PROPERTIES
    VERSION 1.5.0
    SOVERSION 1
)
