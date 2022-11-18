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

  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;

  float boxScale{0.05f};

  void update(float deltaTime);
};

#endif