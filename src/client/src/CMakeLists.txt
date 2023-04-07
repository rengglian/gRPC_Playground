message(STATUS "Client CMakeList")

# Project/Library Names
set(EXECUTABLE_NAME_CLIENT ${PROJECT_NAME}_CLIENT_${PROJECT_VERSION})

set(ALL_HEADERS dataclient/dataclient.h
  serverstreamclient/serverstreamclient.h
)

set(ALL_SOURCES dataclient/dataclient.cpp 
  main.cpp
  serverstreamclient/serverstreamclient.cpp
)

set(PROTO_GENERATED_INCLUDES
    "${CMAKE_BINARY_DIR}/proto/myproto") 

add_executable(${EXECUTABLE_NAME_CLIENT} ${ALL_SOURCES} ${ALL_HEADERS} )

target_include_directories(${EXECUTABLE_NAME_CLIENT}
  PUBLIC
  ${PROTO_GENERATED_INCLUDES}
  )  
  
target_link_libraries(${EXECUTABLE_NAME_CLIENT}
PRIVATE
myproto
rt
pthread
)