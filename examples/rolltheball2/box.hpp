#ifndef BOX_HPP_
#define BOX_HPP_

#include "abcgOpenGL.hpp"

#include "model.hpp"

class Box {
public:
  [[nodiscard]] int getNumTriangles() const {
    return gsl::narrow<int>(m_indices.size()) / 3;
  }

  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};

  glm::vec3 boxPosition{0.0f, 0.2f, 0.0f};
  glm::vec3 rotationAxis{};

  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;

  float boxScale{0.05f};
  float angle{};
  float angularSpeed{};
  bool colision{false};

  void update(float deltaTime);
  void create(Model m_model, GLuint m_program, const std::string assetsPath);
  void paint(GLint colorLocation, GLint modelMatrixLocation, Model m_model);
};

#endif
