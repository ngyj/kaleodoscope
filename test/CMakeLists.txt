set( BINARY ${CMAKE_PROJECT_NAME}-test )

file( GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.hpp *.cpp )
set( SOURCES ${TEST_SOURCES} )

include_directories( ../src/core )
add_executable( ${BINARY} ${TEST_SOURCES} )
add_test( NAME ${BINARY} COMMAND ${BINARY} )

target_link_libraries( ${BINARY} PUBLIC ${CMAKE_PROJECT_NAME}-lib gtest )

