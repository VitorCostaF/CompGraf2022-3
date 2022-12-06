#include "sun.hpp"

void Sun::create(Model m_model, GLuint m_program,
                 const std::string assetsPath) {
  // Carregamos os índices e vértices para a bola a partir do sphere.obj
  m_model.loadObj(assetsPath, &m_vertices, &m_indices, &m_VBO, &m_EBO);

  // Inicializamos os buffers para a bola
  m_model.setupVAO(m_program, &m_VBO, &m_EBO, &m_VAO);
}

void Sun::paint(GLint colorLocation, GLint modelMatrixLocation, Model m_model) {
  // Aqui transladamos a sun para sua posição e aplicamos a escala.
  glm::mat4 model{1.0f};
  model = glm::translate(model, sunPosition);
  model = glm::scale(model, glm::vec3(sunScale));

  abcg::glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &model[0][0]);
  // A cor da sun será um laranja.
  abcg::glUniform4fv(colorLocation, 1, &sunColor.x);

  // Usamos a classe model para renderizar o objeto
  m_model.render(&m_indices, &m_VAO);
}