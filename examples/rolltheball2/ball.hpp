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

  glm::vec3 ballPosition{0.0f, 0.2f, 0.0f};
  glm::vec3 rotationAxis{1.0f, 0.0f, 0.0f};

  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;

  float verticalSpeed{};
  float horizontalSpeed{};
  float ballScale{0.1f};
  float angle{};
  float angularSpeed{180.0f};

  void update(float deltaTime);
  void create(Model m_model, GLuint m_program, const std::string assetsPath);

  void paint(GLint colorLocation, GLint modelMatrixLocation, Model m_model);
};

#endif