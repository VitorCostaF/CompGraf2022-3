#include "pipes.hpp"
#include "core.h"
#include "range.hpp"

#include <array>
#include <glm/fwd.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <vector>

void Pipes::create(GLuint program) {
  destroy();

  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());

  m_program = program;

  // Get location of uniforms in the program
  m_colorLoc = abcg::glGetUniformLocation(m_program, "color");
  m_translationLoc = abcg::glGetUniformLocation(m_program, "translation");

  // Create Pipes

  for (auto i : iter::range(3)) {
    supPipes.at(i) = makePipe(true, i, 1);

    infPipes.at(i) = makePipe(false, i, 1);
  }

  makeFloor();
}

void Pipes::paint() {
  abcg::glUseProgram(m_program);

  for (auto const i : iter::range(3)) {
    Pipe supPipe = supPipes.at(i);
    Pipe infPipe = infPipes.at(i);

    for (auto const &pipe : {supPipe, infPipe}) {
      abcg::glBindVertexArray(pipe.m_VAO);

      abcg::glUniform4fv(m_colorLoc, 1, &pipe.m_color.r);
      abcg::glUniform2f(m_translationLoc, pipe.m_translation.x,
                        pipe.m_translation.y);

      abcg::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
      abcg::glBindVertexArray(0);
    }
  }
  abcg::glBindVertexArray(m_VAO);
  abcg::glUniform4fv(m_colorLoc, 1, &m_color.r);
  abcg::glUniform2f(m_translationLoc, m_translation.x, m_translation.y);
  abcg::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  abcg::glBindVertexArray(0);

  abcg::glUseProgram(0);
}

void Pipes::destroy() {
  for (auto &pipes : {supPipes, infPipes}) {
    for (auto &pipe : pipes) {
      abcg::glDeleteBuffers(1, &pipe.m_VBO);
      abcg::glDeleteVertexArrays(1, &pipe.m_VAO);
    }
  }
}

void Pipes::update(float deltaTime) {

  for (auto i : iter::range(3)) {
    auto &pipe = supPipes.at(i);
    pipe.m_translation += pipe.m_velocity * deltaTime;
    if (pipe.m_translation.x + pipe.basePoints.at(1).x < -1.0f) {
      supPipes.at(i) = makePipe(true, 1, 1 - pipeWidth);
    }
  }

  for (auto i : iter::range(3)) {
    auto &pipe = infPipes.at(i);
    pipe.m_translation += pipe.m_velocity * deltaTime;
    if (pipe.m_translation.x + pipe.basePoints.at(1).x < -1.0f) {
      infPipes.at(i) = makePipe(false, 1, 1 - pipeWidth);
    }
  }
}

void Pipes::makeFloor() {
  // Generate VBO
  abcg::glGenBuffers(1, &m_VBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  abcg::glBufferData(GL_ARRAY_BUFFER, floorPoints.size() * sizeof(glm::vec2),
                     floorPoints.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  auto const positionAttribute{
      abcg::glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  abcg::glGenVertexArrays(1, &m_VAO);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_VAO);

  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
}

Pipes::Pipe Pipes::makePipe(bool isSup, int index, float windowShift,
                            glm::vec2 translation, float scale) {
  Pipe pipe;

  auto &re{m_randomEngine}; // Shortcut

  // Randomly pick the number of sides
  std::uniform_int_distribution randomSides(6, 20);

  // Get a random color (actually, a grayscale)
  std::uniform_real_distribution randomIntensity(-1.0f + pipeGap + 0.1f, 0.9f);
  // pipe.m_color = glm::vec4(randomIntensity(re));

  pipe.m_color.a = 1.0f;
  pipe.m_scale = scale;
  pipe.m_translation = translation;
  pipe.pipeIndex = index;

  float firstXBasePoint = index * pipeDistance + windowShift;

  glm::vec2 firstBasePoint;
  glm::vec2 secondBasePoint;

  glm::vec2 firstCentralPoint;
  glm::vec2 secondCentralPoint;

  firstBasePoint = glm::vec2{firstXBasePoint, -0.8f};

  firstCentralPoint = glm::vec2{firstXBasePoint, centralPointAux - pipeGap};

  if (isSup) {
    centralPointAux = randomIntensity(re);
    firstBasePoint.y = 1;
    firstCentralPoint.y = centralPointAux;
  }

  secondCentralPoint = firstCentralPoint + glm::vec2{pipeWidth, 0};
  secondBasePoint = firstBasePoint + glm::vec2{pipeWidth, 0};

  pipe.basePoints = {firstBasePoint, secondBasePoint};
  pipe.centralPoints = {firstCentralPoint, secondCentralPoint};

  // Create geometry data
  std::vector<glm::vec2> positions{};

  positions.push_back(firstBasePoint);
  positions.push_back(secondBasePoint);
  positions.push_back(firstCentralPoint);
  positions.push_back(secondCentralPoint);

  // Generate VBO
  abcg::glGenBuffers(1, &pipe.m_VBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, pipe.m_VBO);
  abcg::glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
                     positions.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  auto const positionAttribute{
      abcg::glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  abcg::glGenVertexArrays(1, &pipe.m_VAO);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(pipe.m_VAO);

  abcg::glBindBuffer(GL_ARRAY_BUFFER, pipe.m_VBO);
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);

  return pipe;
}