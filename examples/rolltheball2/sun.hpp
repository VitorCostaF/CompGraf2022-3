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
  GLuint sunProgram{};

  GLint sunViewMatrixLocation{};
  GLint sunProjMatrixLocation{};
  GLint sunModelMatrixLocation{};
  GLint sunColorLocation{};

  glm::vec3 sunPosition{-6.0f, 0.0f, -6.5f};
  glm::vec4 sunColor{1.0f, 1.0f, 0.0f, 1.0f};

  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;

  float sunScale{2.0f};

  void update(float deltaTime);
  void restart();
  void create(Model m_model, const std::string assetsPath);
  void paint(glm::mat4 viewMatrix, glm::mat4 projMatrix, Model m_model);
};

#endif
