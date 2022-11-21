#include "ball.hpp"
#include <glm/fwd.hpp>

void Ball::create(Model m_model, GLuint m_program,
                  const std::string assetsPath) {
  // Carregamos os índices e vértices para a bola a partir do sphere.obj
  m_model.loadObj(assetsPath, &m_vertices, &m_indices, &m_VBO, &m_EBO);

  // Inicializamos os buffers para a bola
  m_model.setupVAO(m_program, &m_VBO, &m_EBO, &m_VAO);
}

void Ball::update(float deltaTime) {
  // Aqui atualizamos a posição da bolinha de acordo com verticalSpeed e
  // horizontalSpeed. Essas variáveis podem ser -1, 0 ou 1, dependendo da seta
  // que apertarmos. Usamos deltaTime para uma variação por segundo.
  ballPosition.z += verticalSpeed * deltaTime;
  ballPosition.x += horizontalSpeed * deltaTime;
  if (verticalSpeed != 0 || horizontalSpeed != 0) {
    angle = glm::wrapAngle(angle + glm::radians(angularSpeed) * deltaTime);
    rotationAxis = glm::vec3{verticalSpeed, 0, horizontalSpeed};
  }
}

void Ball::paint(GLint colorLocation, GLint modelMatrixLocation,
                 Model m_model) {
  // Aqui transladamos a bola para sua posição e aplicamos a escala.
  glm::mat4 model{1.0f};
  model = glm::translate(model, ballPosition);
  model = glm::scale(model, glm::vec3(ballScale));
  if (rotationAxis != glm::vec3{0, 0, 0}) {
    model = glm::rotate(model, angle, rotationAxis);
  }

  // Vínculo da matriz e da cor bem como renderização dos pontos.
  abcg::glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(colorLocation, 1.0f, 1.0f, 1.0f, 1.0f);
  m_model.render(&m_indices, &m_VAO);
}