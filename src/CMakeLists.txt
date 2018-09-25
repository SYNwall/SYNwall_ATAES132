
file(GLOB SOURCES "*.c")
add_library(ataes STATIC
    ${SOURCES}
)
 
target_include_directories(ataes PUBLIC
    ../include
)