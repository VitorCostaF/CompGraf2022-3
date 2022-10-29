#ifndef PIPES_HPP_
#define PIPES_HPP_

#include <array>
#include <list>
#include <random>

#include "abcgOpenGL.hpp"

#include "bird.hpp"
#include "gamedata.hpp"

class Pipes {
public:
  void create(GLuint program);
  void paint();
  void destroy();
  void update(float deltaTime);
  GLuint pipeProgram{};

  struct Pipe {
    GLuint m_VAO{};
    GLuint m_VBO{};

    glm::vec4 m_color{0.28, 0.42, 0.21, 1};
    bool m_hit{};
    float m_scale{1};
    float m_rotation{0};
    int pipeIndex{};
    bool pipePassed{false};
    glm::vec2 m_translation{0.1f};
    glm::vec2 m_velocity{-0.5f, 0.0f};

    std::array<glm::vec2, 2> basePoints{};
    std::array<glm::vec2, 2> centralPoints{};
  };

  float centralPointAux{0};
  std::array<Pipe, 3> supPipes{};
  std::array<Pipe, 3> infPipes{};

  std::array<glm::vec2, 4> floorPoints{
      glm::vec2{-1.0, -1.0f}, glm::vec2{1, -1.0f}, glm::vec2{-1.0f, -0.80},
      glm::vec2{1, -0.80}};

  std::array<glm::vec2, 4> ceilPoints{
      glm::vec2{-1.0f, 1.0f}, glm::vec2{1, 1.0f}, glm::vec2{-1.0f, 1.1f},
      glm::vec2{1, 1.1f}};

  bool pipePrint{true};

  Pipe makePipe(bool isSup, int index, float windowShift,
                glm::vec2 translation = {}, float scale = 1.0f);

  void makeFloor();
  void makeCeil();

  float pipeWidth{0.2f};
  float pipeDistance{1.0f};
  float pipeGap{0.6f};

  glm::vec4 m_color{0.45, 0.25, 0.19, 1};
  glm::vec2 m_translation{0};

private:
  GLuint m_program{};
  GLint m_colorLoc{};
  GLint m_rotationLoc{};
  GLint m_translationLoc{};
  GLint m_scaleLoc{};

  GLuint m_VAOFloor{};
  GLuint m_VBOFloor{};

  GLuint m_VAOCeil{};
  GLuint m_VBOCeil{};

  std::default_random_engine m_randomEngine;
  std::uniform_real_distribution<float> m_randomDist{-1.0f, 1.0f};
};

#endif