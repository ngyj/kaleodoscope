cmake_minimum_required( VERSION 3.10 )

set( CMAKE_CXX_STANDARD          17  )

project( mangekyou )

add_executable( mangekyou src/main.cpp )

add_custom_target( run 
    COMMAND ${CMAKE_BINARY_DIR}/mangekyou.exe
    USES_TERMINAL
)

add_dependencies ( run mangekyou )

install( TARGETS mangekyou DESTINATION bin )

