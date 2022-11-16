#include "window.hpp"
#include "core.h"

#include <unordered_map>

// Explicit specialization of std::hash for Vertex
template <> struct std::hash<Vertex> {
  size_t operator()(Vertex const &vertex) const noexcept {
    auto const h1{std::hash<glm::vec3>()(vertex.position)};
    return h1;
  }
};

void Window::onEvent(SDL_Event const &event) {
  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w) {
      m_dollySpeed = 1.0f;
      ball.verticalSpeed = -1.0f;
    }
    if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s) {
      ball.verticalSpeed = 1.0f;
      m_dollySpeed = -1.0f;
    }
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a) {
      ball.horizontalSpeed = -1.0f;
      m_panSpeed = -1.0f;
    }
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d) {
      ball.horizontalSpeed = 1.0f;
      m_panSpeed = 1.0f;
    }
    if (event.key.keysym.sym == SDLK_q)
      m_truckSpeed = -1.0f;
    if (event.key.keysym.sym == SDLK_e)
      m_truckSpeed = 1.0f;
  }
  if (event.type == SDL_KEYUP) {
    if ((event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w) &&
        m_dollySpeed > 0) {
      ball.verticalSpeed = 0.0f;
      m_dollySpeed = 0.0f;
    }
    if ((event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s) &&
        m_dollySpeed < 0) {
      ball.verticalSpeed = 0.0f;
      m_dollySpeed = 0.0f;
    }
    if ((event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a) &&
        m_panSpeed < 0) {
      ball.horizontalSpeed = 0.0f;
      m_panSpeed = 0.0f;
    }
    if ((event.key.keysym.sym == SDLK_RIGHT ||
         event.key.keysym.sym == SDLK_d) &&
        m_panSpeed > 0) {
      ball.horizontalSpeed = 0.0f;
      m_panSpeed = 0.0f;
    }
    if (event.key.keysym.sym == SDLK_q && m_truckSpeed < 0)
      m_truckSpeed = 0.0f;
    if (event.key.keysym.sym == SDLK_e && m_truckSpeed > 0)
      m_truckSpeed = 0.0f;
  }
}

void Window::onCreate() {
  auto const &assetsPath{abcg::Application::getAssetsPath()};

  abcg::glClearColor(0, 0, 0, 1);

  // Enable depth buffering
  abcg::glEnable(GL_DEPTH_TEST);

  // Create program
  m_program =
      abcg::createOpenGLProgram({{.source = assetsPath + "lookat.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + "lookat.frag",
                                  .stage = abcg::ShaderStage::Fragment}});

  m_ground.create(m_program);

  m_model.loadObj(assetsPath + "geosphere.obj", &ball.m_vertices,
                  &ball.m_indices, &ball.m_VBO, &ball.m_EBO);

  m_model.setupVAO(m_program, &ball.m_VBO, &ball.m_EBO, &ball.m_VAO);

  m_model.loadObj(assetsPath + "rectangle.obj", &wall.m_vertices,
                  &wall.m_indices, &wall.m_VBO, &wall.m_EBO);

  m_model.setupVAO(m_program, &wall.m_VBO, &wall.m_EBO, &wall.m_VAO);

  // fmt::print("{}", ball.m_vertices.at(0).position.y);

  // Get location of uniform variables
  m_viewMatrixLocation = abcg::glGetUniformLocation(m_program, "viewMatrix");
  m_projMatrixLocation = abcg::glGetUniformLocation(m_program, "projMatrix");
  m_modelMatrixLocation = abcg::glGetUniformLocation(m_program, "modelMatrix");
  m_colorLocation = abcg::glGetUniformLocation(m_program, "color");
}

void Window::onPaint() {
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

  glm::mat4 model{1.0f};
  model = glm::translate(model, ball.ballPosition);
  model = glm::scale(model, glm::vec3(0.1f));

  abcg::glUniformMatrix4fv(m_modelMatrixLocation, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(m_colorLocation, 1.0f, 1.0f, 1.0f, 1.0f);
  m_model.render(&ball.m_indices, &ball.m_VAO);

  // South wall
  model = glm::mat4(1.0);
  model = glm::translate(model, glm::vec3{0.0f, 0.2f, 0.925f});

  abcg::glUniformMatrix4fv(m_modelMatrixLocation, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(m_colorLocation, 1.0f, 0.8f, 0.0f, 1.0f);

  m_model.render(&wall.m_indices, &wall.m_VAO);

  // North wall
  model = glm::mat4(1.0);
  model = glm::translate(model, glm::vec3{0.0f, 0.2f, -0.925f});

  abcg::glUniformMatrix4fv(m_modelMatrixLocation, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(m_colorLocation, 1.0f, 0.8f, 0.0f, 1.0f);

  m_model.render(&wall.m_indices, &wall.m_VAO);

  // East Wall
  model = glm::mat4(1.0);
  model = glm::translate(model, glm::vec3{0.925f, 0.2f, 0.0f});
  model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));

  abcg::glUniformMatrix4fv(m_modelMatrixLocation, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(m_colorLocation, 1.0f, 0.8f, 0.0f, 1.0f);

  m_model.render(&wall.m_indices, &wall.m_VAO);

  // West Wall
  model = glm::mat4(1.0);
  model = glm::translate(model, glm::vec3{-0.925f, 0.2f, 0.0f});
  model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));

  abcg::glUniformMatrix4fv(m_modelMatrixLocation, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(m_colorLocation, 1.0f, 0.8f, 0.0f, 1.0f);

  m_model.render(&wall.m_indices, &wall.m_VAO);

  // Draw ground
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

  // Update LookAt camera
  m_camera.dolly(m_dollySpeed * deltaTime, ball.ballPosition);
  m_camera.truck(m_truckSpeed * deltaTime, ball.ballPosition);
  m_camera.pan(m_panSpeed * deltaTime, ball.ballPosition);
  ball.update(deltaTime);
}