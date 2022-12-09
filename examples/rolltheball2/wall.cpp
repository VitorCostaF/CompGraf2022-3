#include "wall.hpp"

void Wall::create(Model m_model, const std::string assetsPath) {

  wallProgram = abcg::createOpenGLProgram(
      {{.source = assetsPath + "wall.vert", .stage = abcg::ShaderStage::Vertex},
       {.source = assetsPath + "wall.frag",
        .stage = abcg::ShaderStage::Fragment}});

  // Carregamos os índices e vértices para a bola a partir do sphere.obj
  m_model.loadObj(assetsPath + "rectangle.obj", &m_vertices, &m_indices, &m_VBO,
                  &m_EBO);

  // Inicializamos os buffers para a parede
  m_model.setupVAO(wallProgram, &m_VBO, &m_EBO, &m_VAO);

  wallViewMatrixLocation =
      abcg::glGetUniformLocation(wallProgram, "viewMatrix");
  wallProjMatrixLocation =
      abcg::glGetUniformLocation(wallProgram, "projMatrix");
  wallModelMatrixLocation =
      abcg::glGetUniformLocation(wallProgram, "modelMatrix");

  wallColorLocation = abcg::glGetUniformLocation(wallProgram, "color");
}

void Wall::paint(glm::mat4 viewMatrix, glm::mat4 projMatrix, Model m_model) {

  abcg::glUseProgram(wallProgram);

  // Bind das propriedades
  abcg::glUniformMatrix4fv(wallViewMatrixLocation, 1, GL_FALSE,
                           &viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(wallProjMatrixLocation, 1, GL_FALSE,
                           &projMatrix[0][0]);

  // Desenho da parede sul. Notemos que a posição foi escolhida para alinhamento
  // dos cantos (encontro das paredes)
  glm::mat4 model{1.0f};
  model = glm::translate(model, glm::vec3{0.0f, 0.2f, 0.96f});

  abcg::glUniformMatrix4fv(wallModelMatrixLocation, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(wallColorLocation, 1.0f, 0.8f, 0.0f, 1.0f);

  m_model.render(&m_indices, &m_VAO);

  // Desenho da parede norte. A posição z tem a mesma lógica da anterior, porém
  // refletida em relação a origem
  model = glm::mat4(1.0);
  model = glm::translate(model, glm::vec3{0.0f, 0.2f, -0.96f});

  abcg::glUniformMatrix4fv(wallModelMatrixLocation, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(wallColorLocation, 1.0f, 0.8f, 0.0f, 1.0f);

  m_model.render(&m_indices, &m_VAO);

  // Desenho da parede leste. A lógica da coordenada z das paredes anteriores
  // foi aplicada na coordenada x dessa parede e também rodamos em -90.0 graus
  // para formar a parede lateral.
  model = glm::mat4(1.0);
  model = glm::translate(model, glm::vec3{0.96f, 0.2f, 0.0f});
  model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));

  abcg::glUniformMatrix4fv(wallModelMatrixLocation, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(wallColorLocation, 1.0f, 0.8f, 0.0f, 1.0f);

  m_model.render(&m_indices, &m_VAO);

  // Desenho da parede oeste. Mesma lógica da parede anterior para a coordenada
  // x, porém refletida pela origem.
  model = glm::mat4(1.0);
  model = glm::translate(model, glm::vec3{-0.96f, 0.2f, 0.0f});
  model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));

  abcg::glUniformMatrix4fv(wallModelMatrixLocation, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(wallColorLocation, 1.0f, 0.8f, 0.0f, 1.0f);

  // Renderização feita pela classe Model
  m_model.render(&m_indices, &m_VAO);
  abcg::glUseProgram(0);
}