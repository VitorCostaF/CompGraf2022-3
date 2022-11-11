#ifndef MODEL_HPP_
#define MODEL_HPP_

#include "abcgOpenGL.hpp"

struct Vertex {
  glm::vec3 position{};

  friend bool operator==(Vertex const &, Vertex const &) = default;
};

class Model {
public:
  void loadObj(std::string_view path, std::vector<Vertex> *m_vertices,
               std::vector<GLuint> *m_indices, bool standardize = true);
  void render(std::vector<GLuint> *m_indices, int numTriangles = -1) const;
  void setupVAO(GLuint program);
  void destroy() const;

private:
  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};

  void createBuffers(std::vector<Vertex> *m_vertices,
                     std::vector<GLuint> *m_indices);
  void standardize(std::vector<Vertex> *m_vertices);
};

#endif