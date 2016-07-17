project (fuzzy-k-means)

#Bring headers
include_directories(include)

#Define sources
file(GLOB SOURCES "src/*.cpp")

add_executable(fkm ${SOURCES})
