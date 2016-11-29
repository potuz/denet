cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
set(CMAKE_USER_MAKE_RULES_OVERRIDE
   ${CMAKE_CURRENT_SOURCE_DIR}/c_flag_overrides.cmake)
set(CMAKE_USER_MAKE_RULES_OVERRIDE_CXX
   ${CMAKE_CURRENT_SOURCE_DIR}/cxx_flag_overrides.cmake)
project (denet VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
if(WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" "/MT" )
	set(CMAKE_CXX_FLAGS_DEBUG 
		"${CMAKE_CXX_FLAGS_DEBUG}" "/MT" )
	set(CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_CXX_STANDARD_LIBRARIES} imm32.lib Ws2_32.lib opengl32.lib")
else(WIN32)
	set(CMAKE_INSTALL_PREFIX "/usr")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-switch-enum" )
	set(CMAKE_CXX_FLAGS_DEBUG 
  "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wno-switch-enum" )
endif(WIN32)

configure_file ( README.md.in "${CMAKE_SOURCE_DIR}/README.md")

if("${CMAKE_BUILD_TYPE}_" STREQUAL "DEBUG_")
  set ( DATAFILE_PATH "${PROJECT_BINARY_DIR}/" )
  set ( IMAGEFILE_PATH "${PROJECT_BINARY_DIR}/images/" )
  set ( PROJECT_VERSION_COMPLETE
    "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}_alpha")
  configure_file ( COPYING 
    "${PROJECT_BINARY_DIR}/COPYING" COPYONLY)
  configure_file ( src/genet/genet_pt.qm 
    "${PROJECT_BINARY_DIR}/genet_pt.qm" COPYONLY)
  configure_file ( data/tickers.csv 
    "${PROJECT_BINARY_DIR}/tickers.csv" COPYONLY)
  configure_file ( data/companies.csv
    "${PROJECT_BINARY_DIR}/companies.csv" COPYONLY)
  configure_file ( src/genet/images/document-open-symbolic.png
    "${PROJECT_BINARY_DIR}/images/document-open-symbolic.png" COPYONLY)
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
else("${CMAKE_BUILD_TYPE}_" STREQUAL "DEBUG_")
	set ( DATAFILE_PATH "${CMAKE_INSTALL_PREFIX}/share/" )
	set ( IMAGEFILE_PATH "${DATAFILE_PATH}images/" )
	set ( PROJECT_VERSION_COMPLETE
		"${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}_alpha")
add_definitions(-DQT_NO_DEBUG_OUTPUT)
endif("${CMAKE_BUILD_TYPE}_" STREQUAL "DEBUG_")

if(WIN32)
	add_definitions(-DLIBARCHIVE_STATIC)
	add_definitions(-DCURL_STATICLIB)
	add_definitions(-DCPPCONN_PUBLIC_FUNC=)
	set(MYSQLCONNECTORCPP_ROOT_DIR "$ENV{PROGRAMFILES}\\MySQL\\Connector.C++\ 1.1\\")
	set(MYPRG "PROGRAMFILES(x86)")
	set(CMAKE_PREFIX_PATH "C:\\Qt\\5.7\\qtbase")
	set(CMAKE_INCLUDE_PATH "$ENV{${MYPRG}}" 
		"$ENV{${MYPRG}}/libarchive/")
	set(CMAKE_LIBRARY_PATH "$ENV{${MYPRG}}"
		"$ENV{${MYPRG}}/libarchive/Release/"
		"$ENV{PROGRAMFILES}/mysql/mysql server 5.7/lib"
		"C:\\Qt\\5.7\\qtbase\\lib"
		"C:\\Qt\\5.7\\qtbase\\plugins\\platforms")
	set(CURL_INCLUDE_DIR  "C:\\Users\\heluani\\Source\\Repos\\curl-7.51.0\\builds\\libcurl-vc14-x64-release-static-ipv6-sspi-winssl\\include" )
	set(CURL_LIBRARY   "C:\\Users\\heluani\\Source\\Repos\\curl-7.51.0\\builds\\libcurl-vc14-x64-release-static-ipv6-sspi-winssl\\lib\\libcurl_a.lib")
endif(WIN32)

configure_file ( src/config.h.in "${PROJECT_BINARY_DIR}/config.h" )

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(Qt5Charts REQUIRED)
find_package(Qt5Concurrent REQUIRED)
find_package(MySQL REQUIRED)
find_package(MySQLConnectorCpp REQUIRED)
message("MYSQLCONNECTORCPP_INCLUDE_DIRS: " ${MYSQLCONNECTORCPP_INCLUDE_DIRS})
message("MYSQLCONNECTORCPP_INCLUDE_LIBRARIES: " ${MYSQLCONNECTORCPP_LIBRARIES})
find_package(LibArchive REQUIRED)
find_package(CURL REQUIRED)
if(WIN32)
set(MYPRG "PROGRAMFILES(x86)")
set(BOOST_ROOT "$ENV{${MYPRG}}/boost_1_62_0")
find_package(Boost REQUIRED)
endif(WIN32)
if(WIN32)
add_library (dfp STATIC
	src/pugixml/pugixml.cpp 
	src/dfp/dfp_cvm_connection.cpp 
	src/dfp/dfp_database.cpp 
	src/dfp/dfp_cvm_file.cpp 
	src/dfp/dfp_company.cpp 
	src/dfp/dfp_utils.cpp )
else(WIN32)
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
endif(WIN32)
target_include_directories(dfp 
	PRIVATE 
	${MYSQL_INCLUDE_DIRS}
	${MYSQLCONNECTORCPP_INCLUDE_DIRS}
	${LibArchive_INCLUDE_DIRS} 
	${CURL_INCLUDE_DIRS} 
	${PROJECT_BINARY_DIR}
	PUBLIC 
	"${CMAKE_SOURCE_DIR}/src/dfp"
	"${CMAKE_SOURCE_DIR}/src")
if(WIN32)
	target_include_directories(dfp PUBLIC ${Boost_INCLUDE_DIRS})
endif(WIN32)
target_link_libraries(dfp  ${MYSQL_LIBRARIES} ${CURL_LIBRARIES}
  ${MYSQLCONNECTORCPP_LIBRARIES} ${LibArchive_LIBRARIES})


add_executable (denet src/denet/denet.cpp)
target_link_libraries (denet dfp)
target_include_directories (denet PRIVATE ${PROJECT_BINARY_DIR} 
	${MYSQLCONNECTORCPP_INCLUDE_DIRS} 
	"${CMAKE_SOURCE_DIR}/src/ezOptionParser")

add_executable (cvmfetch src/cvmfetch/cvmfetch.cpp)
target_link_libraries (cvmfetch dfp)
target_include_directories (cvmfetch PRIVATE ${PROJECT_BINARY_DIR} 
	${MYSQLCONNECTORCPP_INCLUDE_DIRS})
if(WIN32)
add_executable ( genet WIN32
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
	src/genet/balance_item.cpp
	src/genet/balance_model.cpp
	src/genet/balance_view.cpp
	src/genet/date_button_view.cpp
	src/genet/chart_view.cpp
	src/genet/report.cpp
	src/genet/report_dialog.cpp )
else(WIN32)
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
  src/genet/balance_item.cpp
  src/genet/balance_model.cpp
  src/genet/balance_view.cpp
  src/genet/date_button_view.cpp
  src/genet/chart_view.cpp
  src/genet/report.cpp
  src/genet/report_dialog.cpp
  )
endif(WIN32)
target_include_directories(genet
	PRIVATE
	${MYSQLCONNECTORCPP_INCLUDE_DIRS}
	${MYSQL_INCLUDE_DIRS})
if(WIN32)
find_library (QtConcurrent Qt5Concurrent)
find_library (QtPlatformSupport Qt5PlatformSupport)
find_library (QtFreeType qtfreetype)
find_library (QtHarfBuzz qtharfbuzzng)
find_library (QWindows qwindows)
find_library (QtPCRE qtpcre)
target_link_libraries ( genet Qt5::Widgets
  Qt5::Charts 
  Qt5::PrintSupport
  ${QtFreeType}
  ${QtHarfBuzz}
  ${QtPlatformSupport}
  ${QWindows}
  ${QtConcurrent} 
  ${QtPCRE}
  dfp )
else(WIN32)
target_link_libraries ( genet Qt5::Widgets
  Qt5::Charts 
  Qt5Concurrent 
  Qt5::PrintSupport
  dfp )
endif(WIN32)

install ( TARGETS dfp denet cvmfetch genet
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib 
  ARCHIVE DESTINATION lib)
install ( FILES data/companies.csv data/tickers.csv COPYING
  src/genet/genet_pt.qm
  DESTINATION share/denet )
install ( FILES 
  src/genet/images/application-exit.png
  src/genet/images/document-save.png
  src/genet/images/document-open-symbolic.png
  src/genet/images/banner.png
  src/genet/images/background.png
  src/genet/images/watermark1.png
  src/genet/images/watermark2.png
  DESTINATION share/denet/images )

SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
SET(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_RPATH} "${CMAKE_INSTALL_PREFIX}/lib")


