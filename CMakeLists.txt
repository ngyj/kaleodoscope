cmake_minimum_required( VERSION 3.10 )

set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON" FORCE)
set( CMAKE_CXX_STANDARD 17           )
if (WIN32)
    set(   CMAKE_C_COMPILER clang-cl.exe )
    set( CMAKE_CXX_COMPILER clang-cl.exe )
    set(       CMAKE_LINKER lld-link.exe )
endif (WIN32)

include_directories( include )

project( mangekyou )

add_executable( mangekyou src/main.cpp )

add_custom_target( run 
    COMMAND ${CMAKE_BINARY_DIR}/mangekyou.exe
    USES_TERMINAL
)

add_dependencies ( run mangekyou )

install( TARGETS mangekyou DESTINATION bin )

