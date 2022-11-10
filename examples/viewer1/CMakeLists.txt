project(viewer1)
add_executable(${PROJECT_NAME} main.cpp window.cpp model.cpp trackball.cpp)
enable_abcg(${PROJECT_NAME})