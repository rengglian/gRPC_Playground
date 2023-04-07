message(STATUS "Server CMakeList")

# Project/Library Names
set(EXECUTABLE_NAME_SERVER ${PROJECT_NAME}_SERVER_${PROJECT_VERSION})

set(PROTO_GENERATED_INCLUDES
    "${CMAKE_BINARY_DIR}/proto/myproto") 

message(STATUS "PROTO_GENERATED_INCLUDES: ${PROTO_GENERATED_INCLUDES}")

add_executable(${EXECUTABLE_NAME_SERVER} src/main.cpp )


target_include_directories(${EXECUTABLE_NAME_SERVER}
  PUBLIC
    ${PROTO_GENERATED_INCLUDES}
  )
  
target_link_libraries(${EXECUTABLE_NAME_SERVER}
PRIVATE
myproto
rt
pthread
)
