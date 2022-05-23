project(ked)
find_package(PkgConfig)
pkg_search_module(XPROTO REQUIRED xproto)
pkg_search_module(X11 REQUIRED x11)

# Change the following line to point to the directory
# where the application default resource files are kept.
#APPDEFAULTSDIR = /usr/local/lib/X11/app-defaults
#DEFINES = -DAPPDEFAULTSDIR=\"$(APPDEFAULTSDIR)\"
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DAPPDEFAULTSDIR=/usr/local/lib/X11/app-defaults")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DAPPDEFAULTSDIR=/usr/local/lib/X11/app-defaults")

list(APPEND KED_SOURCES
    main.cpp
    basic.cpp
    Kfont.cpp
    container.cpp
    base.cpp
    menu.cpp
    editor.cpp
    dialog.cpp
    scroll.cpp
    scrollbar.cpp
    button.cpp
    input_dialog.cpp
    load.cpp
    save.cpp
    cut.cpp
    print.cpp
)

add_executable(ked
    ${KED_SOURCES}
)
target_link_libraries(ked
    ${X11_LIBRARIES}
    ${XPROTO_LIBRARIES}
)
target_include_directories(ked PUBLIC
    ${X11_INCLUDE_DIRS}
    ${XPROTO_INCLUDE_DIRS}
)
install(TARGETS ked RUNTIME DESTINATION bin)
