#ifndef MOON_HPP_
#define MOON_HPP_

#include "abcgOpenGL.hpp"

#include "model.hpp"

class Moon {
public:
  [[nodiscard]] int getNumTriangles() const {
    return gsl::narrow<int>(m_indices.size()) / 3;
  }

  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};
  GLuint moonProgram{};

  GLint moonViewMatrixLocation{};
  GLint moonProjMatrixLocation{};
  GLint moonModelMatrixLocation{};
  GLint moonColorLocation{};

  glm::vec3 moonPosition{-0.9f, 1.0f, 1.0f};
  glm::vec4 moonColor{0.8f, 0.8f, 0.8f, 1.0f};

  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;

  float moonScale{0.4f};
  float moonShininess{20.0f};

  void update(float elapsedTime);
  void restart();
  void create(Model m_model, const std::string assetsPath);
  void paint(glm::mat4 viewMatrix, glm::mat4 projMatrix, Model m_model);
  void destroy();
};

#endif
