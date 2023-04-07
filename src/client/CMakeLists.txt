message(STATUS "Client CMakeList")

# Project/Library Names
set(EXECUTABLE_NAME_CLIENT ${PROJECT_NAME}_CLIENT_${PROJECT_VERSION})

set(PROTO_GENERATED_INCLUDES
    "${CMAKE_BINARY_DIR}/proto/myproto") 

message(STATUS "PROTO_GENERATED_INCLUDES: ${PROTO_GENERATED_INCLUDES}")

add_executable(${EXECUTABLE_NAME_CLIENT} src/main.cpp )

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