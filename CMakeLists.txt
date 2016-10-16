cmake_minimum_required(VERSION 3.0)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_INSTALL_PREFIX "/usr")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-switch-enum --std=c++14" )
set(CMAKE_CXX_FLAGS_DEBUG 
  "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wno-switch-enum --std=c++14" )
project (denet VERSION 1.0 LANGUAGES CXX)
if(${CMAKE_BUILD_TYPE} STREQUAL "DEBUG")
  set ( DATAFILE_PATH "${PROJECT_BINARY_DIR}/" )
  set ( IMAGEFILE_PATH "${PROJECT_BINARY_DIR}/images/" )
  set ( PROJECT_VERSION_COMPLETE
    "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}-pre")
  configure_file ( data/tickers.csv 
    "${PROJECT_BINARY_DIR}/tickers.csv" COPYONLY)
  configure_file ( data/companies.csv
    "${PROJECT_BINARY_DIR}/companies.csv" COPYONLY)
  configure_file ( src/genet/images/document-save.png
    "${PROJECT_BINARY_DIR}/images/document-save.png" COPYONLY)
  configure_file ( src/genet/images/banner.png
    "${PROJECT_BINARY_DIR}/images/banner.png" COPYONLY)
  configure_file ( src/genet/images/background.png
    "${PROJECT_BINARY_DIR}/images/background.png" COPYONLY)
  configure_file ( src/genet/images/watermark1.png
    "${PROJECT_BINARY_DIR}/images/watermark1.png" COPYONLY)
  configure_file ( src/genet/images/watermark2.png
    "${PROJECT_BINARY_DIR}/images/watermark2.png" COPYONLY)
  configure_file ( src/genet/images/application-exit.png
    "${PROJECT_BINARY_DIR}/images/application-exit.png" COPYONLY)
else(${CMAKE_BUILD_TYPE} STREQUAL "DEBUG")
  set ( DATAFILE_PATH "${CMAKE_INSTALL_PREFIX}/share/denet/" )
endif(${CMAKE_BUILD_TYPE} STREQUAL "DEBUG")

configure_file ( src/config.h.in "${PROJECT_BINARY_DIR}/config.h" )

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Concurrent REQUIRED)
find_package(MySQL REQUIRED)
find_package(MySQLConnectorCpp REQUIRED)
find_package(LibArchive REQUIRED)
find_package(CURL REQUIRED)

add_library (dfp SHARED 
  src/pugixml/pugixml.cpp 
  src/dfp/dfp_cvm_connection.cpp 
  src/dfp/dfp_database.cpp 
  src/dfp/dfp_cvm_file.cpp 
  src/dfp/dfp_company.cpp 
  src/dfp/dfp_utils.cpp )
set (dfp_VERSION_MAJOR 0)
set (dfp_VERSION_SOVERSION 1)
set (dfp_VERSION_MINOR 1)
set (dfp_VERSION_STRING ${dfp_VERSION_MAJOR}.${dfp_VERSION_MINOR})
set_target_properties (dfp PROPERTIES VERSION ${dfp_VERSION_STRING} 
  SOVERSION ${dfp_VERSION_SOVERSION})
target_include_directories(dfp 
  PRIVATE 
  ${MYSQL_INCLUDE_DIRS}
  ${MYSQLCONNECTORCPP_INCLUDES} ${LibArchive_INCLUDE_DIRS} 
  ${CURL_INCLUDE_DIRS} ${PROJECT_BINARY_DIR}
  PUBLIC 
  "${CMAKE_SOURCE_DIR}/src/dfp"
  "${CMAKE_SOURCE_DIR}/src" 
  )
target_link_libraries(dfp  ${MYSQL_LIBRARIES} ${CURL_LIBRARIES}
  ${MYSQLCONNECTORCPP_LIBRARIES} ${LibArchive_LIBRARIES})


add_executable (denet src/denet/denet.cpp)
target_link_libraries (denet dfp)
target_include_directories (denet PRIVATE ${PROJECT_BINARY_DIR})

add_executable (cvmfetch src/cvmfetch/cvmfetch.cpp)
target_link_libraries (cvmfetch dfp)
target_include_directories (cvmfetch PRIVATE ${PROJECT_BINARY_DIR})

add_executable ( genet 
  src/genet/genet.cpp 
  src/genet/mainwindow.cpp 
  src/genet/database_wizard.cpp
  src/genet/genet_database.cpp
  src/genet/indicator_model.cpp
  src/genet/indicator_view.cpp
  src/genet/import_model.cpp
  src/genet/import_view.cpp
  src/genet/import_item.cpp
  src/genet/import_progress_dialog.cpp
  )

target_link_libraries ( genet Qt5::Widgets Qt5Concurrent dfp )


install ( TARGETS dfp denet cvmfetch genet
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib )
install ( FILES data/companies.csv data/tickers.csv 
  DESTINATION share/denet )
install ( FILES 
  src/genet/images/application-exit.png
  src/genet/images/document-save.png
  src/genet/images/banner.png
  src/genet/images/background.png
  src/genet/images/watermark1.png
  src/genet/images/watermark2.png
  DESTINATION share/denet/images )

SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
SET(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_RPATH} "${CMAKE_INSTALL_PREFIX}/lib")


