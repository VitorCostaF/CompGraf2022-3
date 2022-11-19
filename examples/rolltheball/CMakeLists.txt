project(rolltheball)
add_executable(${PROJECT_NAME} main.cpp window.cpp camera.cpp ground.cpp model.cpp ball.cpp wall.cpp box.cpp)
enable_abcg(${PROJECT_NAME})