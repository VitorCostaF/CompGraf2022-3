#ifndef BALL_HPP_
#define BALL_HPP_

#include "abcgOpenGL.hpp"

#include "model.hpp"
#include "moon.hpp"
#include "sun.hpp"
#include <glm/gtx/fast_trigonometry.hpp>

class Ball {
public:
  [[nodiscard]] int getNumTriangles() const {
    return gsl::narrow<int>(m_indices.size()) / 3;
  }

  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};
  GLuint ballProgram{};

  GLint ballViewMatrixLocation{};
  GLint ballProjMatrixLocation{};
  GLint ballModelMatrixLocation{};
  GLint ballColorLocation{};

  glm::vec3 ballPosition{0.0f, 0.2f, 0.0f};
  glm::vec3 rotationAxis{1.0f, 0.0f, 0.0f};

  glm::vec4 sunLightDir{0.0f, -1.0f, 0.0f, 1.0f};
  glm::vec4 Ia{1.0f};
  glm::vec4 Ka{0.2f, 0.2f, 0.2f, 1.0f};
  glm::vec4 sunId{1.0f};
  glm::vec4 sunIs{1.0f};
  glm::vec4 sunKd{1.0f};
  glm::vec4 sunKs{1.0f};
  float sunShininess{20.0f};

  glm::vec4 moonLightDir{0.0f, -1.0f, 0.0f, 1.0f};
  glm::vec4 moonId{1.0f};
  glm::vec4 moonIs{1.0f};
  glm::vec4 moonKd{0.7f};
  glm::vec4 moonKs{1.0f};
  float moonShininess{10.0f};

  float kcSun{0.01f};
  float klSun{0.01f};
  float kqSun{0.02f};

  float kcMoon{0.05f};
  float klMoon{0.04f};
  float kqMoon{0.08f};

  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;

  float verticalSpeed{};
  float horizontalSpeed{};
  float ballScale{0.1f};
  float angle{};
  float angularSpeed{180.0f};

  void update(float deltaTime, Sun sun, Moon moon);
  void create(Model m_model, const std::string assetsPath);

  void paint(glm::mat4 viewMatrix, glm::mat4 projMatrix, Model m_model);
};

#endif