#include "pipes.hpp"
#include "range.hpp"

#include <array>
#include <glm/gtx/fast_trigonometry.hpp>

void Pipes::create(GLuint program, int quantity) {
  destroy();

  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());

  m_program = program;

  // Get location of uniforms in the program
  m_colorLoc = abcg::glGetUniformLocation(m_program, "color");
  m_rotationLoc = abcg::glGetUniformLocation(m_program, "rotation");
  m_scaleLoc = abcg::glGetUniformLocation(m_program, "scale");
  m_translationLoc = abcg::glGetUniformLocation(m_program, "translation");

  // Create Pipes
  m_pipes.clear();
  m_pipes.resize(quantity);

  std::array<glm::vec2, 24> pipesPositions;

  for (auto i : iter::range(3)) {
    float firstXBasePointInf = i * 0.5f + 1;

    glm::vec2 firstBasePointInf{firstXBasePointInf, -0.8f};
    glm::vec2 secondBasePointInf = firstBasePointInf + glm::vec2{pipeWidth, 0};

    glm::vec2 firstBasePointSup{firstXBasePointInf, 1};
    glm::vec2 secondBasePointSup = firstBasePointInf + glm::vec2{pipeWidth, 0};

    infPipes.at(i).basePoints = {firstBasePointInf, secondBasePointInf};
    supPipes.at(i).basePoints = {firstBasePointSup, secondBasePointSup};
  }

  for (auto &pipe : m_pipes) {
    pipe = makePipe();

    // Make sure the pipe won't collide with the ship
    do {
      pipe.m_translation = {m_randomDist(m_randomEngine),
                            m_randomDist(m_randomEngine)};
    } while (glm::length(pipe.m_translation) < 0.5f);
  }
}

void Pipes::paint() {
  abcg::glUseProgram(m_program);

  for (auto const &pipe : m_pipes) {
    abcg::glBindVertexArray(pipe.m_VAO);

    abcg::glUniform4fv(m_colorLoc, 1, &pipe.m_color.r);
    abcg::glUniform1f(m_scaleLoc, pipe.m_scale);
    abcg::glUniform1f(m_rotationLoc, pipe.m_rotation);

    for (auto i : {-2, 0, 2}) {
      for (auto j : {-2, 0, 2}) {
        abcg::glUniform2f(m_translationLoc, pipe.m_translation.x + j,
                          pipe.m_translation.y + i);

        abcg::glDrawArrays(GL_TRIANGLE_FAN, 0, pipe.m_polygonSides + 2);
      }
    }

    abcg::glBindVertexArray(0);
  }

  abcg::glUseProgram(0);
}

void Pipes::destroy() {
  for (auto &pipe : m_pipes) {
    abcg::glDeleteBuffers(1, &pipe.m_VBO);
    abcg::glDeleteVertexArrays(1, &pipe.m_VAO);
  }
}

void Pipes::update(const Bird &ship, float deltaTime) {
  for (auto &pipe : m_pipes) {
    pipe.m_translation -= ship.m_velocity * deltaTime;
    pipe.m_rotation =
        glm::wrapAngle(pipe.m_rotation + pipe.m_angularVelocity * deltaTime);
    pipe.m_translation += pipe.m_velocity * deltaTime;

    // Wrap-around
    if (pipe.m_translation.x < -1.0f)
      pipe.m_translation.x += 2.0f;
    if (pipe.m_translation.x > +1.0f)
      pipe.m_translation.x -= 2.0f;
    if (pipe.m_translation.y < -1.0f)
      pipe.m_translation.y += 2.0f;
    if (pipe.m_translation.y > +1.0f)
      pipe.m_translation.y -= 2.0f;
  }
}

Pipes::Pipe Pipes::makePipe(glm::vec2 translation, float scale) {
  Pipe pipe;

  auto &re{m_randomEngine}; // Shortcut

  // Randomly pick the number of sides
  std::uniform_int_distribution randomSides(6, 20);
  pipe.m_polygonSides = randomSides(re);

  // Get a random color (actually, a grayscale)
  std::uniform_real_distribution randomIntensity(0.5f, 1.0f);
  pipe.m_color = glm::vec4(randomIntensity(re));

  pipe.m_color.a = 1.0f;
  pipe.m_rotation = 0.0f;
  pipe.m_scale = scale;
  pipe.m_translation = translation;

  // Get a random angular velocity
  pipe.m_angularVelocity = m_randomDist(re);

  // Get a random direction
  glm::vec2 const direction{m_randomDist(re), m_randomDist(re)};
  pipe.m_velocity = glm::normalize(direction) / 7.0f;

  // Create geometry data
  std::vector<glm::vec2> positions{{0, 0}};
  auto const step{M_PI * 2 / pipe.m_polygonSides};
  std::uniform_real_distribution randomRadius(0.8f, 1.0f);
  for (auto const angle : iter::range(0.0, M_PI * 2, step)) {
    auto const radius{randomRadius(re)};
    positions.emplace_back(radius * std::cos(angle), radius * std::sin(angle));
  }
  positions.push_back(positions.at(1));

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