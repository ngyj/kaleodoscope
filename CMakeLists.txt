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
    # set( CMAKE_CXX_EXTENSIONS OFF )

    set( CMAKE_CXX_COMPILER clang++ )
    set(       CMAKE_LINKER lld )
    set(      RESULT_BINARY mangekyou )
endif(UNIX)

include_directories( include )

project( mangekyou )

add_subdirectory( test )
add_subdirectory( lib/googletest )

set( BINARY ${CMAKE_PROJECT_NAME} )
file( GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.hpp *.cpp )
set( SOURCES src/core/name.hpp src/core/type.hpp src/core/name.cpp src/core/type.cpp )

add_library( ${BINARY}-lib STATIC ${SOURCES} )
add_executable( ${BINARY} src/main.cpp )
target_link_libraries( ${BINARY} ${BINARY}-lib )

add_custom_target( run
  COMMAND ${CMAKE_BINARY_DIR}/${BINARY}
  USES_TERMINAL
)
add_dependencies ( run mangekyou )

add_custom_target( test
  COMMAND ${CMAKE_BINARY_DIR}/test/${BINARY}-test
  USES_TERMINAL
)
add_dependencies ( test mangekyou-test )


install( TARGETS mangekyou DESTINATION bin )
