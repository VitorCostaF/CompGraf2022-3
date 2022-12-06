#include "ground.hpp"

void Ground::create(GLuint program) {
  // Pontos de um quadrado
  std::array<glm::vec3, 4> vertices{{{-0.5f, 0.0f, +0.5f},
                                     {-0.5f, 0.0f, -0.5f},
                                     {+0.5f, 0.0f, +0.5f},
                                     {+0.5f, 0.0f, -0.5f}}};

  // Geração do VBO com os vértices
  abcg::glGenBuffers(1, &m_VBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(),
                     GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Criação e vínculo do VAO
  abcg::glGenVertexArrays(1, &m_VAO);
  abcg::glBindVertexArray(m_VAO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  auto const positionAttribute{
      abcg::glGetAttribLocation(program, "inPosition")};
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);
  abcg::glBindVertexArray(0);

  // Save location of uniform variables
  m_modelMatrixLoc = abcg::glGetUniformLocation(program, "modelMatrix");
  m_colorLoc = abcg::glGetUniformLocation(program, "color");
}

void Ground::paint() {
  abcg::glBindVertexArray(m_VAO);

  // Desenhamos uma Grid com 2N+1 de comprimento (eixo z) e 2N+6 de largura
  // (eixo x). Esse aumento da largura foi necessário para não aparecer o fundo
  // azul nas laterais quando a bolinha vai para os cantos.
  auto const N{5};
  for (auto const z : iter::range(-N, N + 1)) {
    for (auto const x : iter::range(-N - 3, N + 4)) {
      // Transladamos o quadrado pelos parametros da iteração para formação da
      // malha quadrada
      glm::mat4 model{1.0f};
      model = glm::translate(model, glm::vec3(x, 0.0f, z));
      abcg::glUniformMatrix4fv(m_modelMatrixLoc, 1, GL_FALSE, &model[0][0]);
      // Os quadrado centrais pintamos de marrom para coincidir com a área útil
      // do jogo
      if (z >= -1 && z <= 1 && x >= -1 && x <= 1) {
        abcg::glUniform4f(m_colorLoc, 0.3176f, 0.2078f, 0.1333f, 1.0f);
      } else {
        // Os outros quadrados serão verdes (para simular grama)
        abcg::glUniform4f(m_colorLoc, 0.1333f, 0.5451f, 0.1333f, 1.0f);
      }

      abcg::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
  }

  abcg::glBindVertexArray(0);
}

void Ground::destroy() {
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}