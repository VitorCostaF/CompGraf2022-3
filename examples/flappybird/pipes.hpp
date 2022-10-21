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
  void create(GLuint program, int quantity);
  void paint();
  void destroy();
  void update(const Bird &bird, float deltaTime);

  struct Pipe {
    GLuint m_VAO{};
    GLuint m_VBO{};

    float m_angularVelocity{};
    glm::vec4 m_color{1};
    bool m_hit{};
    int m_polygonSides{};
    float m_rotation{};
    float m_scale{};
    glm::vec2 m_translation{};
    glm::vec2 m_velocity{};

    std::array<glm::vec2, 2> basePoints{};
    std::array<glm::vec2, 2> centralPoints{};
  };

  std::list<Pipe> m_pipes;
  std::array<Pipe, 3> supPipes;
  std::array<Pipe, 3> infPipes;

  Pipe makePipe(glm::vec2 translation = {}, float scale = 0.25f);
  float pipeWidth{0.1f};

private:
  GLuint m_program{};
  GLint m_colorLoc{};
  GLint m_rotationLoc{};
  GLint m_translationLoc{};
  GLint m_scaleLoc{};

  std::default_random_engine m_randomEngine;
  std::uniform_real_distribution<float> m_randomDist{-1.0f, 1.0f};
};

#endif