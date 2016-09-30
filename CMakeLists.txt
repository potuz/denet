cmake_minimum_required(VERSION 3.0)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_INSTALL_PREFIX "/usr")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-switch-enum --std=c++14" )
set(CMAKE_CXX_FLAGS_DEBUG 
  "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wno-switch-enum --std=c++14" )
project (denet CXX)
set ( DATAFILE_PATH "${CMAKE_INSTALL_PREFIX}/share/denet/" )
configure_file ( src/config.h.in "${PROJECT_BINARY_DIR}/config.h" )
find_package(MySQL REQUIRED)
find_package(MySQLConnectorCpp REQUIRED)
find_package(LibArchive REQUIRED)
find_package(CURL REQUIRED)

add_library (dfp SHARED src/pugixml/pugixml.cpp src/dfp_database.cpp
  src/dfp_cvm_file.cpp src/dfp_company.cpp src/dfp_utils.cpp 
  src/dfp_cvm_connection.cpp)
set (dfp_VERSION_MAJOR 0)
set (dfp_VERSION_SOVERSION 1)
set (dfp_VERSION_MINOR 1)
set (dfp_VERSION_STRING ${dfp_VERSION_MAJOR}.${dfp_VERSION_MINOR})
set_target_properties (dfp PROPERTIES VERSION ${dfp_VERSION_STRING} 
  SOVERSION ${dfp_VERSION_SOVERSION})
target_include_directories(dfp PRIVATE ${MYSQL_INCLUDE_DIRS}
  ${MYSQLCONNECTORCPP_INCLUDES} ${LibArchive_INCLUDE_DIRS} 
  ${CURL_INCLUDE_DIRS} ${PROJECT_BINARY_DIR})
target_link_libraries(dfp  ${MYSQL_LIBRARIES} ${CURL_LIBRARIES}
  ${MYSQLCONNECTORCPP_LIBRARIES} ${LibArchive_LIBRARIES})


add_executable (denet src/denet.cpp)
target_link_libraries (denet dfp)
target_include_directories (denet PRIVATE ${PROJECT_BINARY_DIR})

add_executable (cvmfetch src/cvmfetch.cpp)
target_link_libraries (cvmfetch dfp)
target_include_directories (cvmfetch PRIVATE ${PROJECT_BINARY_DIR})

install ( TARGETS dfp denet cvmfetch
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib )
install ( FILES data/companies.csv data/tickers.csv 
  DESTINATION share/denet )

SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
SET(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_RPATH} "${CMAKE_INSTALL_PREFIX}/lib")


