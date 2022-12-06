#ifndef SUN_HPP_
#define SUN_HPP_

#include "abcgOpenGL.hpp"

#include "model.hpp"

class Sun {
public:
  [[nodiscard]] int getNumTriangles() const {
    return gsl::narrow<int>(m_indices.size()) / 3;
  }

  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};

  glm::vec3 sunPosition{-6.0f, 0.0f, -6.5f};
  glm::vec4 sunColor{1.0f, 0.9764f, 0.0f, 1.0f};

  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;

  float sunScale{1.0f};

  void update(float deltaTime);
  void create(Model m_model, GLuint m_program, const std::string assetsPath);
  void paint(GLint colorLocation, GLint modelMatrixLocation, Model m_model);
};

#endif
