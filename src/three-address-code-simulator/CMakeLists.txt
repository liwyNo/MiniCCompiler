include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_compile_options(-m32)
set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} -m32)
ADD_EXECUTABLE(sim sim.cpp op.cpp)
