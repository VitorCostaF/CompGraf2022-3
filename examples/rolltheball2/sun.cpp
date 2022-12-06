#include "sun.hpp"
#include "core.h"

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

void Sun::update(float deltaTime) {
  // Nessa função movimentamos o sol, a ideia é fazer ele executar um movimento
  // de nascer e depois se por. Isse será simulado por uma função quadrática
  // para o desenho da parábola. A função será -1/50x^2 + 1.5, onde x é a
  // posição x do sol. Além disso, vamos decrementando a compente verde da cor
  // para passar de um amarelo (1,1,0) para um laranja (1,0.5,0)

  // Incrementamos a posição em x do sol
  sunPosition.x += deltaTime;

  // Aqui calculamos a nova posição y do sol após deslocar o sol um pouco para a
  // direita respeitando função quadrática -1/50x^2 + 1.5.
  sunPosition.y = -1.0 / 50.0 * pow(sunPosition.x, 2.0) + 1.5;

  // Aqui vamos decrementando a componente verde da cor para ir de amarelo para
  // o laranja enquanto o sol se move.
  sunColor.g -= deltaTime / 30;
  fmt::print("g {}\n", sunColor.g);
}

void Sun::restart() {
  sunPosition = glm::vec3{-8.66f, 0.0f, -7.0f};
  sunColor = glm::vec4{1.0f, 01.0f, 0.0f, 1.0f};
}
