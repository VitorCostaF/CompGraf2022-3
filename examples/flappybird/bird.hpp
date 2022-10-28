#ifndef BIRD_HPP_
#define BIRD_HPP_

#include "abcgOpenGL.hpp"

#include "gamedata.hpp"

class Bird {
public:
  void create(GLuint program);
  void paint(GameData const &gameData);
  void destroy();
  void update(GameData const &gameData, float deltaTime);
  bool checkCeilColision();

  glm::vec4 m_color{1};
  float m_rotation{};
  float m_scale{0.125f};
  float gravity{-0.0005f};
  float impulse{0.0005f};
  glm::vec2 m_translation{};
  glm::vec2 m_velocity{};

  abcg::Timer m_trailBlinkTimer;
  abcg::Timer m_bulletCoolDownTimer;

  bool buttonReleased{true};

private:
  GLuint m_program{};
  GLint m_translationLoc{};
  GLint m_colorLoc{};
  GLint m_scaleLoc{};
  GLint m_rotationLoc{};

  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};
};
#endif