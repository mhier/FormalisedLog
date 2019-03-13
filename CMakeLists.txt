cmake_minimum_required(VERSION 2.6)
project(FormalisedLog)

set(${PROJECT_NAME}_MAJOR_VERSION 01)
set(${PROJECT_NAME}_MINOR_VERSION 00)
set(${PROJECT_NAME}_PATCH_VERSION 00)
include(cmake/set_version_numbers.cmake)

include(cmake/enable_latest_cxx_support.cmake)

set(WT_CONNECTOR "Wt::HTTP" CACHE STRING "Connector used (Wt::HTTP or Wt::FCGI)")

set(TARGET FormalisedLog.wt)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-psabi")

find_package(Boost REQUIRED COMPONENTS program_options signals system filesystem thread)
find_package(wt REQUIRED COMPONENTS http fcgi dbo dbosqlite3 dbomysql)
find_package(CURL REQUIRED)

include_directories(${CURL_INCLUDE_DIRS})

add_custom_target(link_target ALL
                  COMMAND ${CMAKE_COMMAND} -E create_symlink /usr/share/Wt/resources ${CMAKE_BINARY_DIR}/resources)

configure_file(${CMAKE_SOURCE_DIR}/FormalisedLog ${CMAKE_BINARY_DIR}/FormalisedLog COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/FormalisedLog.css ${CMAKE_BINARY_DIR}/FormalisedLog COPYONLY)

aux_source_directory(${CMAKE_SOURCE_DIR}/src SOURCE)
include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(${TARGET} ${SOURCE})
target_link_libraries(${TARGET} ${WT_CONNECTOR} Wt::DboMySQL Wt::DboSqlite3 ${Boost_LIBRARIES} ${CURL_LIBRARIES})

configure_file(include/Version.h.in ${PROJECT_BINARY_DIR}/version/Version.h)
include_directories(${PROJECT_BINARY_DIR}/version)

FILE( COPY ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}.css DESTINATION ${PROJECT_BINARY_DIR})
FILE( COPY ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}.cfg.template DESTINATION ${PROJECT_BINARY_DIR})
FILE( COPY ${CMAKE_SOURCE_DIR}/${PROJECT_NAME} DESTINATION ${PROJECT_BINARY_DIR}
      FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
