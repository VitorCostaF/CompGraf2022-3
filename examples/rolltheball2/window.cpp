#include "window.hpp"
#include "core.h"

#include <glm/gtc/random.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <string>
#include <unordered_map>

// Explicit specialization of std::hash for Vertex
template <> struct std::hash<Vertex> {
  size_t operator()(Vertex const &vertex) const noexcept {
    auto const h1{std::hash<glm::vec3>()(vertex.position)};
    auto const h2{std::hash<glm::vec3>()(vertex.normal)};
    return abcg::hashCombine(h1, h2);
  }
};

void Window::onEvent(SDL_Event const &event) {
  // Nessa função recebemos a entrada do teclado. Nesse primeiro if de tecla
  // apertada se apertamos para cima (ou w) ou para baixo (ou s) atribuimos a
  // velocidade vertical da bolinha para ela se mover para cima ou para baixo ao
  // longo do eixo z. Se apertamos para a direta (ou d) ou para a esquerda (ou
  // a) mexemos na velocidade horizontal da bolinha para se deslocar ao longo do
  // eixo x. Também utilizamos o botão Enter para restartar o jogo.
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
    if (event.key.keysym.sym == SDLK_RETURN) {
      if (m_gameData.m_state == State::GameOver ||
          m_gameData.m_state == State::Win) {
        m_gameData.m_state = State::Restarted;
      }
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
  auto const filename{assetsPath + "Inconsolata-Medium.ttf"};
  m_font = ImGui::GetIO().Fonts->AddFontFromFileTTF(filename.c_str(), 60.0f);
  if (m_font == nullptr) {
    throw abcg::RuntimeError("Cannot load font file");
  }

  auto const seed{std::chrono::steady_clock::now().time_since_epoch().count()};
  m_randomEngine.seed(seed);

  // Colorimos o back ground de azul para simular o ceu
  abcg::glClearColor(0.2f, 0.9f, 1.0f, 1);

  // Habilitamos depth buffering
  abcg::glEnable(GL_DEPTH_TEST);

  // Criação do programa principal
  m_program = abcg::createOpenGLProgram(
      {{.source = assetsPath + "main.vert", .stage = abcg::ShaderStage::Vertex},
       {.source = assetsPath + "main.frag",
        .stage = abcg::ShaderStage::Fragment}});

  // Carregamos os indices, vertices e montamos o VAO, VBO e EBO para a bola,
  // bem como criamos o program
  ball.create(m_model, assetsPath);

  // Carregamos os indices, vertices e montamos o VAO, VBO e EBO para a parede
  wall.create(m_model, assetsPath);

  // Carregamos os indices, vertices e montamos o VAO, VBO e EBO para o sol, bem
  // como criamos o program
  sun.create(m_model, assetsPath);

  // Carregamos os indices, vertices e montamos o VAO, VBO e EBO para a lua, bem
  // como criamos o program
  moon.create(m_model, assetsPath);

  // Carregamos os indices, vertices e montamos o VAO, VBO e EBO para o chão,
  // bem como criamos o program
  m_ground.create(m_model, assetsPath);

  // Carregamos os indices, vertices e montamos o VAO, VBO e EBO para as
  // boxes e colocamos na lista de boxes
  for (int i = 0; i < qtdBoxes; i++) {
    Box box;
    box.create(m_model, m_program, assetsPath + "box.obj");
    boxes.emplace_back(box);
  }

  // Inicializamos as variáveis necessárias
  restart();
  // Get location of uniform variables
  m_viewMatrixLocation = abcg::glGetUniformLocation(m_program, "viewMatrix");
  m_projMatrixLocation = abcg::glGetUniformLocation(m_program, "projMatrix");
  m_modelMatrixLocation = abcg::glGetUniformLocation(m_program, "modelMatrix");
  m_colorLocation = abcg::glGetUniformLocation(m_program, "color");
}

bool Window::checkBoxValidPosition(Box newBox) {
  // Aqui checamos se estamos sorteando uma box muito perto das outras ou perto
  // da bola. Usamos a boxScale e callScale, pois esses objetos foram
  // normalizados e então a escala representa o raio do circulo com centro no
  // objeto. Porém, multiplicamos pelos fatores 4 e 2 para deixar uma distância
  // maior entre os objetos. Além disso checamos se não for zero, pois
  // reaproveitamos o vetor boxes no momento do restart então distância zero diz
  // que estamos comparando ele com ele mesmo.
  for (auto box : boxes) {
    auto const distance{glm::distance(box.boxPosition, newBox.boxPosition)};
    auto const ballDistance{
        glm::distance(newBox.boxPosition, ball.ballPosition)};
    if ((distance < 4 * box.boxScale ||
         ballDistance < 2 * (box.boxScale + ball.ballScale)) &&
        distance != 0) {
      return true;
    }
  }
  return false;
}

void Window::randomizeBox(Box *box) {
  // Aqui vamos sortear aleatoriamente a posição das boxes, distPos, e o
  // movimento angular em torno do eixo de rotação, também sorteado
  // aleatoriamente.

  // Sorteamos a posição entre -0.9 e 0.9 para as boxes não seírem dos limites
  // do jogo.
  std::uniform_real_distribution<float> distPos(-0.8f, 0.8f);
  // Sorteamos uma velocidade angular para a box girar em torno do eixo.
  std::uniform_real_distribution<float> angularSpeed(90.0f, 360.0f);
  box->boxPosition =
      glm::vec3(distPos(m_randomEngine), 0.2f, distPos(m_randomEngine));
  box->angularSpeed = angularSpeed(m_randomEngine);
  // Sorteamos também o angulo de rotação
  box->rotationAxis = glm::sphericalRand(1.0f);
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

  // Desenho das caixas
  for (int i = 0; i < (int)boxes.size(); i++) {
    Box box = boxes.at(i);
    if (!box.colision) {
      box.paint(m_colorLocation, m_modelMatrixLocation, m_model);
    }
  }

  abcg::glUseProgram(0);

  // Desenho das paredes
  wall.paint(m_camera.getViewMatrix(), m_camera.getProjMatrix(), m_model);

  // Desenho da bolinha
  ball.paint(m_camera.getViewMatrix(), m_camera.getProjMatrix(), m_model);

  // Desenho do sol
  sun.paint(m_camera.getViewMatrix(), m_camera.getProjMatrix(), m_model);

  moon.paint(m_camera.getViewMatrix(), m_camera.getProjMatrix(), m_model);

  // Desenho chão
  m_ground.paint(m_camera.getViewMatrix(), m_camera.getProjMatrix(), m_model);
}

void Window::onPaintUI() {
  abcg::OpenGLWindow::onPaintUI();
  // Aqui criamos uma nova telinhas para exibirmos os textos do jogo
  auto const size{ImVec2(350, 300)};
  ImVec2 position;

  // Jo o jogo não estiver rolando exibimos o texo te vitória ou derrota no meio
  // da tela
  if (m_gameData.m_state != State::Playing &&
      m_gameData.m_state != State::Restarted) {

    position = ImVec2((m_viewportSize.x - size.x) / 2.0f,
                      (m_viewportSize.y - size.y) / 2.0f);
  } else {
    // Se o jogo estiver rolando exibimos a contagem do tempo no topo da tela
    position = ImVec2((m_viewportSize.x) / 2.0f - 20.0f, 0);
  }

  // Acerto da criação da tela, sem backgroud, titlebar e inputs para apenas
  // exibir o texto
  ImGui::SetNextWindowPos(position);
  ImGui::SetNextWindowSize(size);
  ImGuiWindowFlags const flags{ImGuiWindowFlags_NoBackground |
                               ImGuiWindowFlags_NoTitleBar |
                               ImGuiWindowFlags_NoInputs};

  // Inicializamos a tela com a fonte
  ImGui::Begin(" ", nullptr, flags);
  ImGui::PushFont(m_font);
  // Se o jogo acabou exibimos o texto de vitória ou derrota e a dica de apertar
  // enter para continuar
  if (m_gameData.m_state != State::Playing &&
      m_gameData.m_state != State::Restarted) {

    if (m_gameData.m_state == State::GameOver) {
      ImGui::Text("Game Over!");
    } else {
      ImGui::Text("You Win!");
    }
    ImGui::Text("Press Enter");

  } else {
    // Se o jogo está rolando subtraímos o tempo máximo para pegar todos os
    // pontos pelo tempo passado para termos o tempo restante.
    int remainTime = maxTime - (int)timeElapsed;
    ImGui::Text("%d", remainTime < 0 ? 0 : remainTime);
  }
  ImGui::PopFont();
  ImGui::End();
}

void Window::onResize(glm::ivec2 const &size) {
  // Acertamos o viewPort e recomputamos a matriz de projeção
  m_viewportSize = size;
  m_camera.computeProjectionMatrix(size);
}

void Window::restart() {
  // Aqui inicializamos o estado do jogo, o tempo passado.
  m_gameData.m_state = State::Playing;
  timeElapsed = 0;
  int i = 0;
  // Para cada box sorteamos seus atributos (posição, eixo e velocidade de
  // rotação), verificamos se não está conflitando com outra box e acertamos a
  // colisão dessa box. Só contamos, avançamos o valor de i, quando conseguimos
  // uma posição válida.
  while (i < qtdBoxes) {
    Box *box = &boxes.at(i);
    randomizeBox(box);
    if (!checkBoxValidPosition(*box)) {
      box->colision = false;
      i++;
    }
  }
  // restart da posição e cor do sol
  sun.restart();
}

void Window::onDestroy() {
  // Liberação dos recursos
  m_ground.destroy();
  sun.destroy();
  wall.destroy();
  abcg::glDeleteProgram(m_program);
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}

void Window::onUpdate() {
  // Se o jogo acabou ainda assim permitimos que a bolinha se movimente.
  // Se foi apertado enter após o fim do jogo restarmos o jogo.
  if (m_gameData.m_state == State::Restarted) {
    restart();
    return;
  }
  auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};
  // Checagem da colisão com as paredes, limites do jogo.
  checkWallColision();

  // Atualizamos os parâmetros da bola.
  ball.update(deltaTime, sun, moon);
  // Acertamos o ponto que a câmera olha para que ela acompanhe a bola.
  m_camera.move(ball.ballPosition);

  for (auto &box : boxes) {
    // Rodamos as boxes por sua velocidade angular, por isso precisamos
    // atualização o ângulo.
    box.angle =
        glm::wrapAngle(box.angle + glm::radians(box.angularSpeed) * deltaTime);
  }

  // Se o jogo acabou não fazemos o resto
  if (m_gameData.m_state != State::Playing) {
    return;
  }

  // Somamos o tempo passado desde o início para usar depois
  timeElapsed += deltaTime;
  // Checamos se a bolinha atingiu alguma box.
  checkBoxColision();

  // Calculamos o tempo restante subtraindo o tempo máximo do tempo passado.
  int remainedTime = maxTime - (int)timeElapsed;

  // Checamos se ainda hã alguma box sem colisão. Se todas já foram "pegas"
  // então o jogador ganhou
  if (checkActiveBoxes() == 0) {
    m_gameData.m_state = State::Win;
    return;
  }

  // Se o tempo chegou a zero o jogador perdeu
  if (remainedTime <= 0) {
    m_gameData.m_state = State::GameOver;
  }

  sun.update(deltaTime);
  moon.update(timeElapsed);
  // Passamos a cor do sol e sua posição para dar as coordenadas da fonte de luz
  // mais forte, que é o sol
  wall.update(sun.sunColor, sun.sunPosition);

  m_ground.update(sun.sunColor, sun.sunPosition);
}

int Window::checkActiveBoxes() {
  int result = 0;
  // Para cada box checamos se não foi colidida, ou pega, pela bolinha e
  // contamos como ativa.
  for (auto box : boxes) {
    if (!box.colision) {
      result++;
    }
  }
  return result;
}

void Window::checkBoxColision() {
  // Função para saber se a bolinha colidiu com alguma box. Deixamos um fator
  // 0.5 para permitir uma sobreposição maior
  for (auto &box : boxes) {
    auto const ballDistance{glm::distance(box.boxPosition, ball.ballPosition)};

    if (ballDistance < (0.5f * box.boxScale + 0.5f * ball.ballScale)) {
      box.colision = true;
    }
  }
}

void Window::checkWallColision() {
  // Como as paredes estão fixas basicamente precisamos checar se a bolinha não
  // passou dos limites e zeramos a velocidade naquela direção para simular uma
  // batida.
  if ((ball.ballPosition.x > 0.85f && ball.horizontalSpeed > 0) ||
      (ball.ballPosition.x < -0.85f && ball.horizontalSpeed < 0)) {
    ball.horizontalSpeed = 0.0f;
  }

  if ((ball.ballPosition.z > 0.85f && ball.verticalSpeed > 0) ||
      (ball.ballPosition.z < -0.85f && ball.verticalSpeed < 0)) {
    ball.verticalSpeed = 0.0f;
  }
}
