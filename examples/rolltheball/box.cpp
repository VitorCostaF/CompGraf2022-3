#include "box.hpp"

void Box::create(Model m_model, GLuint m_program,
                 const std::string assetsPath) {
  // Carregamos os índices e vértices para a bola a partir do sphere.obj
  m_model.loadObj(assetsPath, &m_vertices, &m_indices, &m_VBO, &m_EBO);

  // Inicializamos os buffers para a bola
  m_model.setupVAO(m_program, &m_VBO, &m_EBO, &m_VAO);
}

void Box::paint(GLint colorLocation, GLint modelMatrixLocation, Model m_model) {
  // Aqui transladamos a box para sua posição, aplicamos a escala e rodamos pelo
  // ângulo ao longo do eixo.
  glm::mat4 model{1.0f};
  model = glm::translate(model, boxPosition);
  model = glm::scale(model, glm::vec3(boxScale));
  model = glm::rotate(model, angle, rotationAxis);

  abcg::glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &model[0][0]);
  // A cor da box será um laranja.
  abcg::glUniform4f(colorLocation, 1.0f, 0.498f, 0.3137f, 1.0f);

  m_model.render(&m_indices, &m_VAO);
}