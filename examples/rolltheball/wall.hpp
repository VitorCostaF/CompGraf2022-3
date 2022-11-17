#ifndef WALL_HPP_
#define WALL_HPP_

#include "abcgOpenGL.hpp"

#include "model.hpp"

class Wall {
public:
  [[nodiscard]] int getNumTriangles() const {
    return gsl::narrow<int>(m_indices.size()) / 3;
  }

  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};

  glm::vec3 wallPosition{0.0f, 0.2f, 1.0f};

  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;
};

#endif