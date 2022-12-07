#ifndef BALL_HPP_
#define BALL_HPP_

#include "abcgOpenGL.hpp"

#include "model.hpp"
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

  glm::vec4 m_lightDir{0.0f, -1.0f, 0.0f, 0.0f};
  glm::vec4 m_Ia{1.0f};
  glm::vec4 m_Id{1.0f};
  glm::vec4 m_Is{1.0f};
  glm::vec4 m_Ka{0.1f, 0.1f, 0.1f, 1.0f};
  glm::vec4 m_Kd{0.7f, 0.7f, 0.7f, 1.0f};
  glm::vec4 m_Ks{1.0f};
  float m_shininess{25.0f};

  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;

  float verticalSpeed{};
  float horizontalSpeed{};
  float ballScale{0.1f};
  float angle{};
  float angularSpeed{180.0f};

  void update(float deltaTime, glm::vec4 sunColor);
  void create(Model m_model, const std::string assetsPath);

  void paint(glm::mat4 viewMatrix, glm::mat4 projMatrix, Model m_model);
};

#endif