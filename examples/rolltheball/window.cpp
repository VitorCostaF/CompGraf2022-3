#include "window.hpp"
#include "core.h"

#include <glm/gtc/random.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <unordered_map>

// Explicit specialization of std::hash for Vertex
template <> struct std::hash<Vertex> {
  size_t operator()(Vertex const &vertex) const noexcept {
    auto const h1{std::hash<glm::vec3>()(vertex.position)};
    return h1;
  }
};

void Window::onEvent(SDL_Event const &event) {
  // Nessa função recebemos a entrada do teclado. Nesse primeiro if de tecla
  // apertada se apertamos para cima (ou w) ou para baixo (ou s) atribuimos a
  // velocidade vertical da bolinha para ela se mover para cima ou para baixo ao
  // longo do eixo z. Se apertamos para a direta (ou d) ou para a esquerda (ou
  // a) mexemos na velocidade horizontal da bolinha para se deslocar ao longo do
  // eixo x.
  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w) {
      ball.verticalSpeed = -1.0f;
    }
    if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s) {
      ball.verticalSpeed = 1.0f;
    }
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a) {
      ball.horizontalSpeed = -1.0f;
    }
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d) {
      ball.horizontalSpeed = 1.0f;
    }
  }
  // Nessa seguanda parte, ao soltarmos a tecla apertada, seja para cima ou para
  // baixo, esquerda ou direita, 'resetamos' (atribuimos zero) a velocidade
  // respectiva, vertical ou horizontal, mas sempre checando se já temos uma
  // velocidade naquele sentido
  if (event.type == SDL_KEYUP) {
    if ((event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w) &&
        ball.verticalSpeed < 0) {
      ball.verticalSpeed = 0.0f;
    }
    if ((event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s) &&
        ball.verticalSpeed > 0) {
      ball.verticalSpeed = 0.0f;
    }
    if ((event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a) &&
        ball.horizontalSpeed < 0) {
      ball.horizontalSpeed = 0.0f;
    }
    if ((event.key.keysym.sym == SDLK_RIGHT ||
         event.key.keysym.sym == SDLK_d) &&
        ball.horizontalSpeed > 0) {
      ball.horizontalSpeed = 0.0f;
    }
  }
}

void Window::onCreate() {
  // Nessa função carregamos os objetos seguindo o padrão das demais atividades.
  // Vale notar que a classe de carregamento do modelo foi mexida para a tornar
  // genérica de forma que passamos o objeto e essa classe carrega as
  // informações dos modelos para os atributos desse objeto. Por exemplo temos o
  // objeto ball, onde carregamos o modelo geosphere.obj e a classe model é
  // responsavel por preencher os vértices e índices do objeto ball, bem como os
  // objetos de renderização (EBO, VBO e VAO). Por fim, pegamos as posições das
  // matrizes e da cor.
  auto const &assetsPath{abcg::Application::getAssetsPath()};

  auto const seed{std::chrono::steady_clock::now().time_since_epoch().count()};
  m_randomEngine.seed(seed);

  // Colorimos o back ground de azul para simular o ceu
  abcg::glClearColor(0.2f, 0.9f, 1.0f, 1);

  // Habilitamos depth buffering
  abcg::glEnable(GL_DEPTH_TEST);

  // Criação do programa
  m_program =
      abcg::createOpenGLProgram({{.source = assetsPath + "lookat.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + "lookat.frag",
                                  .stage = abcg::ShaderStage::Fragment}});
  // Inicializamos os buffers para o chão
  m_ground.create(m_program);

  // Carregamos os índices e vértices para a bola a partir do sphere.obj
  m_model.loadObj(assetsPath + "sphere.obj", &ball.m_vertices, &ball.m_indices,
                  &ball.m_VBO, &ball.m_EBO);

  // Inicializamos os buffers para a bola
  m_model.setupVAO(m_program, &ball.m_VBO, &ball.m_EBO, &ball.m_VAO);

  m_model.loadObj(assetsPath + "rectangle.obj", &wall.m_vertices,
                  &wall.m_indices, &wall.m_VBO, &wall.m_EBO);

  m_model.setupVAO(m_program, &wall.m_VBO, &wall.m_EBO, &wall.m_VAO);
  int i = 0;
  while (i < qtdBoxes) {
    Box box;
    m_model.loadObj(assetsPath + "box.obj", &box.m_vertices, &box.m_indices,
                    &box.m_VBO, &box.m_EBO);

    m_model.setupVAO(m_program, &box.m_VBO, &box.m_EBO, &box.m_VAO);
    randomizeBox(box);
    if (!checkBoxesColision(box)) {
      boxes.emplace_back(box);
      i++;
    }
  }
  // Get location of uniform variables
  m_viewMatrixLocation = abcg::glGetUniformLocation(m_program, "viewMatrix");
  m_projMatrixLocation = abcg::glGetUniformLocation(m_program, "projMatrix");
  m_modelMatrixLocation = abcg::glGetUniformLocation(m_program, "modelMatrix");
  m_colorLocation = abcg::glGetUniformLocation(m_program, "color");
}

bool Window::checkBoxesColision(Box newBox) {
  // Aqui checamos se estamos sorteando uma box muito perto das outras ou perto
  // da bola. Usamos a boxScale e callScale, pois esses objetos foram
  // normalizados e então a escala representa o tamanho do objeto. Rigorosamente
  // falando deveriamos pegar a metade da escala para já evitar a colisão, porém
  // deixar um pouco mais distante ainda não há problema, é até melhor
  for (auto box : boxes) {
    auto const distance{glm::distance(box.boxPosition, newBox.boxPosition)};
    auto const ballDistance{
        glm::distance(newBox.boxPosition, ball.ballPosition)};
    if (distance < 2 * box.boxScale ||
        ballDistance < (box.boxScale + ball.ballScale)) {
      return true;
    }
  }
  return false;
}

void Window::randomizeBox(Box &box) {
  // Aqui vamos sortear aleatoriamente a posição das boxes, distPos, e o
  // movimento angular em torno do eixo de rotação, também sorteado
  // aleatoriamente.

  // Sorteamos a posição entre -0.9 e 0.9 para as boxes não seírem dos limites
  // do jogo.
  std::uniform_real_distribution<float> distPos(-0.9f, 0.9f);
  std::uniform_real_distribution<float> angularSpeed(90.0f, 360.0f);
  box.boxPosition =
      glm::vec3(distPos(m_randomEngine), 0.2f, distPos(m_randomEngine));

  box.angularSpeed = angularSpeed(m_randomEngine);
  // Random rotation axis
  box.rotationAxis = glm::sphericalRand(1.0f);
}

void Window::onPaint() {
  // Nessa função desenhamos os objetos da cena: bola, paredes e chão. As
  // paredes formam um quadrado que limita o movimento da bolinha.

  // Clear color buffer and depth buffer
  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  abcg::glUseProgram(m_program);

  // Set uniform variables for viewMatrix and projMatrix
  // These matrices are used for every scene object
  abcg::glUniformMatrix4fv(m_viewMatrixLocation, 1, GL_FALSE,
                           &m_camera.getViewMatrix()[0][0]);
  abcg::glUniformMatrix4fv(m_projMatrixLocation, 1, GL_FALSE,
                           &m_camera.getProjMatrix()[0][0]);

  // Abaixo desenhamos cada objeto da cena

  // Aqui desenhamos o objeto ball. Estamos transladando para a posição
  // ballPosition, que será atualizada conforme apertamos as setas. Também
  // estamos usando uma escala para diminuir a bola senão ela ficaria do tamanho
  // da tela devido a normalização, função standardize() da classe model.
  glm::mat4 model{1.0f};
  model = glm::translate(model, ball.ballPosition);
  model = glm::scale(model, glm::vec3(ball.ballScale));

  abcg::glUniformMatrix4fv(m_modelMatrixLocation, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(m_colorLocation, 1.0f, 1.0f, 1.0f, 1.0f);
  m_model.render(&ball.m_indices, &ball.m_VAO);

  // Desenho da parede sul. Notemos que a posição foi escolhida para alinhamento
  // dos cantos (encontro das paredes)
  model = glm::mat4(1.0);
  model = glm::translate(model, glm::vec3{0.0f, 0.2f, 0.96f});

  abcg::glUniformMatrix4fv(m_modelMatrixLocation, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(m_colorLocation, 1.0f, 0.8f, 0.0f, 1.0f);

  m_model.render(&wall.m_indices, &wall.m_VAO);

  // Desenho da parede norte. A posição z tem a mesma lógica da anterior, porém
  // refletida em relação a origem
  model = glm::mat4(1.0);
  model = glm::translate(model, glm::vec3{0.0f, 0.2f, -0.96f});

  abcg::glUniformMatrix4fv(m_modelMatrixLocation, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(m_colorLocation, 1.0f, 0.8f, 0.0f, 1.0f);

  m_model.render(&wall.m_indices, &wall.m_VAO);

  // Desenho da parede leste. A lógica da coordenada z das paredes anteriores
  // foi aplicada na coordenada x dessa parede
  model = glm::mat4(1.0);
  model = glm::translate(model, glm::vec3{0.96f, 0.2f, 0.0f});
  model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));

  abcg::glUniformMatrix4fv(m_modelMatrixLocation, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(m_colorLocation, 1.0f, 0.8f, 0.0f, 1.0f);

  m_model.render(&wall.m_indices, &wall.m_VAO);

  // Desenho da parede oeste. Mesma lógica da parede anterior para a coordenada
  // x, porém refletida pela origem.
  model = glm::mat4(1.0);
  model = glm::translate(model, glm::vec3{-0.96f, 0.2f, 0.0f});
  model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));

  abcg::glUniformMatrix4fv(m_modelMatrixLocation, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(m_colorLocation, 1.0f, 0.8f, 0.0f, 1.0f);

  m_model.render(&wall.m_indices, &wall.m_VAO);

  for (int i = 0; i < qtdBoxes; i++) {
    Box box = boxes.at(i);
    model = glm::mat4(1.0);
    model = glm::translate(model, box.boxPosition);
    model = glm::scale(model, glm::vec3(box.boxScale));
    model = glm::rotate(model, box.angle, box.rotationAxis);

    abcg::glUniformMatrix4fv(m_modelMatrixLocation, 1, GL_FALSE, &model[0][0]);
    abcg::glUniform4f(m_colorLocation, 1.0f, 1.0f, 1.0f, 1.0f);
    // model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));

    m_model.render(&box.m_indices, &box.m_VAO);
  }

  // Desenho chão
  m_ground.paint();

  abcg::glUseProgram(0);
}

void Window::onPaintUI() { abcg::OpenGLWindow::onPaintUI(); }

void Window::onResize(glm::ivec2 const &size) {
  m_viewportSize = size;
  m_camera.computeProjectionMatrix(size);
}

void Window::onDestroy() {
  m_ground.destroy();

  abcg::glDeleteProgram(m_program);
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}

void Window::onUpdate() {
  auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};

  checkWallColision();

  // Update LookAt camera
  m_camera.move(ball.ballPosition);

  // Update
  ball.update(deltaTime);

  for (auto &box : boxes) {
    box.angle =
        glm::wrapAngle(box.angle + glm::radians(box.angularSpeed) * deltaTime);
  }
}

void Window::checkWallColision() {
  if ((ball.ballPosition.x > 0.85f && ball.horizontalSpeed > 0) ||
      (ball.ballPosition.x < -0.85f && ball.horizontalSpeed < 0)) {
    ball.horizontalSpeed = 0.0f;
  }

  if ((ball.ballPosition.z > 0.85f && ball.verticalSpeed > 0) ||
      (ball.ballPosition.z < -0.85f && ball.verticalSpeed < 0)) {
    ball.verticalSpeed = 0.0f;
  }
}
