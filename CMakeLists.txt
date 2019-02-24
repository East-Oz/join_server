cmake_minimum_required( VERSION 3.2 )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

project( join_server VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER} )

find_package(Boost COMPONENTS system REQUIRED)

add_executable( join_server main.cpp DataBase.h DataBase.cpp CommandObject.h )

set_target_properties( bulk_server PROPERTIES
	CXX_STANDARD 14
	CXX_STANDARD_REQUIRED ON
	COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra"
)

set_target_properties(join_server PROPERTIES
  INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR}
)

target_link_libraries(join_server
  ${Boost_LIBRARIES}
)

install( TARGETS join_server RUNTIME DESTINATION bin)

set( CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}" )
set( CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}" )
set( CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}" )

set( CPACK_GENERATOR DEB )

set( CPACK_PACKAGE_CONTACT surdukov@gmail.com )

set( CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON )

include( CPack )

