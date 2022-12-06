project(rolltheball2)
add_executable(${PROJECT_NAME} main.cpp window.cpp camera.cpp ground.cpp model.cpp ball.cpp wall.cpp box.cpp sun.cpp)
enable_abcg(${PROJECT_NAME})