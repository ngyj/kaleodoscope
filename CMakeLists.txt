cmake_minimum_required( VERSION 3.10 )

set( PROJECT_BINARY_DIR "./build/default" )

set( CMAKE_CXX_STANDARD 20 )
if (WIN32)
    set(   CMAKE_C_COMPILER clang-cl.exe )
    set( CMAKE_CXX_COMPILER clang-cl.exe )
    set(       CMAKE_LINKER lld-link.exe )
    set(      RESULT_BINARY mangekyou.exe )
endif (WIN32)
if (UNIX)
    set(      RESULT_BINARY mangekyou )
endif(UNIX)


include_directories( include )

project( mangekyou )

add_executable( mangekyou src/main.cpp )

add_custom_target( dirs
  COMMAND which echo
  COMMAND echo "cmake_binary_dir         =" ${CMAKE_BINARY_DIR}
  COMMAND echo "cmake_current_binary_dir =" ${CMAKE_CURRENT_BINARY_DIR}
  COMMAND echo "cmake_source_dir         =" ${CMAKE_SOURCE_DIR}
  COMMAND echo "cmake_current_source_dir =" ${CMAKE_CURRENT_SOURCE_DIR}
  COMMAND echo "cmake_root               =" ${CMAKE_ROOT}
  COMMAND echo "project_binary_dir       =" ${PROJECT_BINARY_DIR}
  COMMAND echo "project_source_dir       =" ${PROJECT_SOURCE_DIR}
  USES_TERMINAL
)
add_custom_target( run
  COMMAND ${CMAKE_BINARY_DIR}/${RESULT_BINARY}
  USES_TERMINAL
)

add_dependencies ( run mangekyou )

install( TARGETS mangekyou DESTINATION bin )
