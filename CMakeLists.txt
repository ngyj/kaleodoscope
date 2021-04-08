cmake_minimum_required( VERSION 3.10 )

set( CMAKE_CXX_STANDARD 20 )
if (WIN32)
    set(   CMAKE_C_COMPILER clang-cl.exe )
    set( CMAKE_CXX_COMPILER clang-cl.exe )
    set(       CMAKE_LINKER lld-link.exe )
    set(      RESULT_BINARY mangekyou.exe )
    set(    CMAKE_CXX_FLAGS_INIT "${CMAKE_CXX_FLAGS} -Wno-return-type" )
endif (WIN32)
if (UNIX)
    set( CMAKE_CXX_COMPILER clang++ )
    set(      RESULT_BINARY mangekyou )
endif(UNIX)

include_directories( include )

project( mangekyou )

add_library( mangekyou-lib src/core/name.cpp src/core/type.cpp)
add_executable( mangekyou src/main.cpp )
target_link_libraries( mangekyou mangekyou-lib )

add_custom_target( run
  COMMAND ${CMAKE_BINARY_DIR}/${RESULT_BINARY}
  USES_TERMINAL
)

add_dependencies ( run mangekyou )

install( TARGETS mangekyou DESTINATION bin )
