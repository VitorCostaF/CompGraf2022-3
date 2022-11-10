project(starfield)
add_executable(${PROJECT_NAME} main.cpp window.cpp camera.cpp ground.cpp model.cpp)
enable_abcg(${PROJECT_NAME})