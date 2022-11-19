#include "wall.hpp"

void Wall::create(Model m_model, GLuint m_program,
                  const std::string assetsPath) {
  // Carregamos os índices e vértices para a bola a partir do sphere.obj
  m_model.loadObj(assetsPath, &m_vertices, &m_indices, &m_VBO, &m_EBO);

  // Inicializamos os buffers para a bola
  m_model.setupVAO(m_program, &m_VBO, &m_EBO, &m_VAO);
}