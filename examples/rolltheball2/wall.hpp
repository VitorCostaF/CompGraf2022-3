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
  GLuint wallProgram{};

  GLuint diffuseTexture{};

  GLint wallViewMatrixLocation{};
  GLint wallProjMatrixLocation{};
  GLint wallModelMatrixLocation{};
  GLint wallColorLocation{};

  glm::vec4 lightDir{0.0f, 1.0f, 0.0f, 1.0f};
  glm::vec4 Ia{1.0f};
  glm::vec4 Ka{0.2f, 0.2f, 0.2f, 1.0f};
  glm::vec4 Id{1.0f};
  glm::vec4 Is{1.0f};
  glm::vec4 Kd{1.0f};
  glm::vec4 Ks{1.0f};
  float shininess{20.0f};

  glm::vec3 wallPosition{0.0f, 0.2f, 1.0f};

  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;

  void create(Model m_model, const std::string assetsPath);
  void paint(glm::mat4 viewMatrix, glm::mat4 projMatrix, Model m_model,
             glm::vec3 sunLightDir);
};

#endif