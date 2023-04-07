message(STATUS "Server CMakeList")

# Project/Library Names
set(EXECUTABLE_NAME_SERVER ${PROJECT_NAME}_SERVER_${PROJECT_VERSION})

set(ALL_HEADERS 
  clientmonitor/clientmonitor.h
  dataservice/dataservice.h
  logger/logger.h
)

set(ALL_SOURCES 
  clientmonitor/clientmonitor.cpp
  dataservice/dataservice.cpp
  logger/logger.cpp
  main.cpp
)


set(PROTO_GENERATED_INCLUDES
    "${CMAKE_BINARY_DIR}/proto/myproto")

add_executable(${EXECUTABLE_NAME_SERVER} ${ALL_SOURCES} ${ALL_HEADERS} )

target_include_directories(${EXECUTABLE_NAME_SERVER}
  PUBLIC
    ${PROTO_GENERATED_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}
  )
  
target_link_libraries(${EXECUTABLE_NAME_SERVER}
PRIVATE
myproto
rt
pthread
)
