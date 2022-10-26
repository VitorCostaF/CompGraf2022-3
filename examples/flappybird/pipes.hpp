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

    glm::vec4 m_color{1};
    bool m_hit{};
    float m_scale{1};
    float m_rotation{0};
    int pipeIndex{};
    glm::vec2 m_translation{0.1f};
    glm::vec2 m_velocity{-0.5f, 0.0f};

    std::array<glm::vec2, 2> basePoints{};
    std::array<glm::vec2, 2> centralPoints{};
  };

  float centralPointAux{0};
  std::array<Pipe, 3> supPipes{};
  std::array<Pipe, 3> infPipes{};
  std::array<glm::vec2, 4> floorPoints{
      glm::vec2{-1.1, -1.1f}, glm::vec2{1, -1.1f}, glm::vec2{-1.1f, -0.90},
      glm::vec2{1, -0.90}};

  bool pipePrint{true};

  Pipe makePipe(bool isSup, int index, float windowShift,
                glm::vec2 translation = {}, float scale = 1.0f);

  void makeFloor();

  float pipeWidth{0.2f};
  float pipeDistance{1.0f};
  float pipeGap{0.4f};

  glm::vec4 m_color{1};
  glm::vec2 m_translation{0.1f};

private:
  GLuint m_program{};
  GLint m_colorLoc{};
  GLint m_rotationLoc{};
  GLint m_translationLoc{};
  GLint m_scaleLoc{};

  GLuint m_VAO{};
  GLuint m_VBO{};

  std::default_random_engine m_randomEngine;
  std::uniform_real_distribution<float> m_randomDist{-1.0f, 1.0f};
};

#endif