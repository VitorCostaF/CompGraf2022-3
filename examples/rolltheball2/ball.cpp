#include "ball.hpp"
#include <glm/fwd.hpp>

void Ball::create(Model m_model, const std::string assetsPath) {

  // Criação do program para a bola
  ballProgram = abcg::createOpenGLProgram(
      {{.source = assetsPath + "ball.vert", .stage = abcg::ShaderStage::Vertex},
       {.source = assetsPath + "ball.frag",
        .stage = abcg::ShaderStage::Fragment}});

  // Carregamos os índices e vértices para a bola a partir do sphere.obj
  m_model.loadObj(assetsPath + "sphere.obj", &m_vertices, &m_indices, &m_VBO,
                  &m_EBO);

  // Inicializamos os buffers para a bola
  m_model.setupVAO(ballProgram, &m_VBO, &m_EBO, &m_VAO);

  // Iniciamos as localizações das variáveis uniformes
  ballViewMatrixLocation =
      abcg::glGetUniformLocation(ballProgram, "viewMatrix");
  ballProjMatrixLocation =
      abcg::glGetUniformLocation(ballProgram, "projMatrix");
  ballModelMatrixLocation =
      abcg::glGetUniformLocation(ballProgram, "modelMatrix");
  ballColorLocation = abcg::glGetUniformLocation(ballProgram, "color");
}

void Ball::update(float deltaTime) {
  // Aqui atualizamos a posição da bolinha de acordo com verticalSpeed e
  // horizontalSpeed. Essas variáveis podem ser -1, 0 ou 1, dependendo da seta
  // que apertarmos. Usamos deltaTime para uma variação por segundo.
  ballPosition.z += verticalSpeed * deltaTime;
  ballPosition.x += horizontalSpeed * deltaTime;
  // Aqui checamos se está ocorendo movimento para poder atualizar o ângulo e
  // atualizar o vetor de rotação. Isso evita que passemos o eixo de rotação
  // como sendo (0,0,0), o que fazia a bola sumir. Aqui basicamente montamos o
  // vetor de rotação invertendo o valor vertical e orizontal com as coordenadas
  // x e z. Por exemplo, se nos movimentamos na vertical, eixo z, precizamos
  // rodar em x. Alem disso, quando forem ambos eixos de movimento teremos uma
  // "soma" dos eixos: se somamos o eixo vertical com o orizontal teremos um
  // eixo na diagonal (45 graus) e a bolinha rodará para o lado certo.
  if (verticalSpeed != 0 || horizontalSpeed != 0) {
    angle = glm::wrapAngle(angle + glm::radians(angularSpeed) * deltaTime);
    rotationAxis = glm::vec3{verticalSpeed, 0, horizontalSpeed};
  }
}

void Ball::paint(glm::mat4 viewMatrix, glm::mat4 projMatrix, Model m_model) {

  // Ativação do program e bind das matrizes de projeção e view
  abcg::glUseProgram(ballProgram);

  abcg::glUniformMatrix4fv(ballViewMatrixLocation, 1, GL_FALSE,
                           &viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(ballProjMatrixLocation, 1, GL_FALSE,
                           &projMatrix[0][0]);

  // Aqui transladamos a bola para sua posição e aplicamos a escala, bem como a
  // rodamos se pelo ângulo de rotação que depende de para onde estamos
  // movimentando. Esse rotationAxis é inicializado com (1,0,0) apenas para não
  // ser (0,0,0) e o objeto começar apagado. Mesmo com início (1,0,0), a bolinha
  // não rodará pois o ângulo de rotação é zero.
  glm::mat4 model{1.0f};
  model = glm::translate(model, ballPosition);
  model = glm::scale(model, glm::vec3(ballScale));
  model = glm::rotate(model, angle, rotationAxis);

  // Vínculo da matriz e da cor bem como renderização dos pontos.
  abcg::glUniformMatrix4fv(ballModelMatrixLocation, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(ballColorLocation, 1.0f, 1.0f, 1.0f, 1.0f);
  m_model.render(&m_indices, &m_VAO);

  abcg::glUseProgram(0);
}