project(flappybird)
add_executable(${PROJECT_NAME} main.cpp window.cpp pipes.cpp bird.cpp)
enable_abcg(${PROJECT_NAME})