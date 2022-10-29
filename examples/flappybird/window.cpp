#include "window.hpp"
#include "core.h"
#include "range.hpp"
#include <SDL_keycode.h>

void Window::onCreate() {
  auto const assetsPath{abcg::Application::getAssetsPath()};

  // Load a new font
  auto const filename{assetsPath + "Inconsolata-Medium.ttf"};
  m_font = ImGui::GetIO().Fonts->AddFontFromFileTTF(filename.c_str(), 60.0f);
  if (m_font == nullptr) {
    throw abcg::RuntimeError("Cannot load font file");
  }

  // Create program to render the other objects
  m_objectsProgram =
      abcg::createOpenGLProgram({{.source = assetsPath + "objects.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + "objects.frag",
                                  .stage = abcg::ShaderStage::Fragment}});

  pipeProgram =
      abcg::createOpenGLProgram({{.source = assetsPath + "pipes.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + "pipes.frag",
                                  .stage = abcg::ShaderStage::Fragment}});

  abcg::glClearColor(0, 0, 0, 1);

#if !defined(__EMSCRIPTEN__)
  abcg::glEnable(GL_PROGRAM_POINT_SIZE);
#endif

  // Start pseudo-random number generator
  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());

  restart();
}

void Window::onPaint() {
  abcg::glClear(GL_COLOR_BUFFER_BIT);
  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  m_pipes.paint();
  m_bird.paint(m_gameData);
}

void Window::onPaintUI() {
  abcg::OpenGLWindow::onPaintUI();

  if (m_gameData.m_state == State::GameOver) {
    auto const size{ImVec2(350, 300)};
    auto const position{ImVec2((m_viewportSize.x - size.x) / 2.0f,
                               (m_viewportSize.y - size.y) / 2.0f)};
    ImGui::SetNextWindowPos(position);
    ImGui::SetNextWindowSize(size);
    ImGuiWindowFlags const flags{ImGuiWindowFlags_NoBackground |
                                 ImGuiWindowFlags_NoTitleBar |
                                 ImGuiWindowFlags_NoInputs};
    ImGui::Begin(" ", nullptr, flags);
    ImGui::PushFont(m_font);
    if (m_gameData.m_state == State::GameOver) {
      ImGui::Text("Game Over!");
      ImGui::Text("Press Enter");
    }

    ImGui::PopFont();
    ImGui::End();
  } else {
    auto const size{ImVec2(350, 300)};
    auto const position{ImVec2((m_viewportSize.x) / 2.0f, -1)};
    ImGui::SetNextWindowPos(position);
    ImGui::SetNextWindowSize(size);
    ImGuiWindowFlags const flags{ImGuiWindowFlags_NoBackground |
                                 ImGuiWindowFlags_NoTitleBar |
                                 ImGuiWindowFlags_NoInputs};
    ImGui::Begin(" ", nullptr, flags);
    ImGui::PushFont(m_font);
    ImGui::Text("%d", points);

    ImGui::PopFont();
    ImGui::End();
  }
}

void Window::onResize(glm::ivec2 const &size) {
  m_viewportSize = size;

  abcg::glClear(GL_COLOR_BUFFER_BIT);
}

void Window::onDestroy() {
  abcg::glDeleteProgram(m_objectsProgram);
  abcg::glDeleteProgram(pipeProgram);

  m_pipes.destroy();
  m_bird.destroy();
}

void Window::onEvent(SDL_Event const &event) {
  // Keyboard events
  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w) {
      if (m_bird.buttonReleased) {
        m_gameData.m_input.set(gsl::narrow<size_t>(Input::Up));
      } else {
        m_gameData.m_input.reset(gsl::narrow<size_t>(Input::Up));
      }
    }
    if (event.key.keysym.sym == SDLK_RETURN) {
      if (m_gameData.m_state == State::GameOver) {
        m_gameData.m_state = State::Restarted;
      }
    }
  }

  if (event.type == SDL_KEYUP) {
    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w) {
      m_gameData.m_input.reset(gsl::narrow<size_t>(Input::Up));
      m_bird.buttonReleased = true;
    }
  }

  if (event.type == SDL_MOUSEMOTION) {
    glm::ivec2 mousePosition;
    SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

    glm::vec2 direction{mousePosition.x - m_viewportSize.x / 2,
                        -(mousePosition.y - m_viewportSize.y / 2)};

    m_bird.m_rotation = std::atan2(direction.y, direction.x) - M_PI_2;
  }
}

void Window::restart() {
  m_gameData.m_state = State::Playing;
  points = 0;

  m_bird.create(m_objectsProgram);
  m_pipes.create(pipeProgram);
}

void Window::onUpdate() {
  auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};

  // Wait 5 seconds before restarting
  if (m_gameData.m_state == State::GameOver) {
    return;
  }

  if (m_gameData.m_state == State::Restarted) {
    restart();
  }

  m_bird.update(m_gameData, deltaTime);
  m_pipes.update(deltaTime);
  checkColisions();
}

void Window::checkColisions() {
  if (m_gameData.m_state != State::Playing) {
    return;
  }

  for (auto i : iter::range(3)) {
    auto infPipe = m_pipes.infPipes.at(i);
    auto supPipe = m_pipes.supPipes.at(i);
    if (m_bird.m_translation.y < -0.9) {
      m_gameData.m_state = State::GameOver;
      return;
    }

    if (supPipe.basePoints.at(0).x + supPipe.m_translation.x - 0.1f < 0 &&
        supPipe.basePoints.at(1).x + supPipe.m_translation.x + 0.1f > 0) {
      if (supPipe.centralPoints.at(0).y - 0.1f > m_bird.m_translation.y &&
          infPipe.centralPoints.at(0).y + 0.1f < m_bird.m_translation.y) {
        points++;

      } else {
        m_gameData.m_state = State::GameOver;
      }
    }
  }
}