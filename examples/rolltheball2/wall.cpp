#include "wall.hpp"
#include <glm/fwd.hpp>

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

  wallColorLocation = abcg::glGetUniformLocation(wallProgram, "color");

  m_model.loadDiffuseTexture(assetsPath + "wall.png", &diffuseTexture);
}

void Wall::update(glm::vec4 lightColorParam, glm::vec3 LightPosParam) {
  lightPos = glm::vec4(LightPosParam, 0);
  Is = lightColorParam;
}

void Wall::paint(glm::mat4 viewMatrix, glm::mat4 projMatrix, Model m_model) {

  abcg::glUseProgram(wallProgram);

  // Localização das matrizes
  auto const viewMatrixLoc{
      abcg::glGetUniformLocation(wallProgram, "viewMatrix")};
  auto const projMatrixLoc{
      abcg::glGetUniformLocation(wallProgram, "projMatrix")};
  auto const modelMatrixLoc{
      abcg::glGetUniformLocation(wallProgram, "modelMatrix")};
  auto const normalMatrixLoc{
      abcg::glGetUniformLocation(wallProgram, "normalMatrix")};

  auto const lightLoc{abcg::glGetUniformLocation(wallProgram, "lightPos")};

  // Localização das propriedades de iluminação do sol
  auto const shininessLoc{abcg::glGetUniformLocation(wallProgram, "shininess")};
  auto const IaLoc{abcg::glGetUniformLocation(wallProgram, "Ia")};
  auto const IdLoc{abcg::glGetUniformLocation(wallProgram, "Id")};
  auto const IsLoc{abcg::glGetUniformLocation(wallProgram, "Is")};
  auto const KaLoc{abcg::glGetUniformLocation(wallProgram, "Ka")};
  auto const KdLoc{abcg::glGetUniformLocation(wallProgram, "Kd")};
  auto const KsLoc{abcg::glGetUniformLocation(wallProgram, "Ks")};

  // Bind das propriedades
  abcg::glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &projMatrix[0][0]);

  // Propriedades da luz
  abcg::glUniform4fv(lightLoc, 1, &lightPos.x);
  abcg::glUniform4fv(IaLoc, 1, &Ia.x);
  abcg::glUniform4fv(IdLoc, 1, &Id.x);
  abcg::glUniform4fv(IsLoc, 1, &Is.x);
  abcg::glUniform4fv(KaLoc, 1, &Ka.x);
  abcg::glUniform4fv(KdLoc, 1, &Kd.x);
  abcg::glUniform4fv(KsLoc, 1, &Ks.x);
  abcg::glUniform1f(shininessLoc, shininess);

  // Desenho da parede sul. Notemos que a posição foi escolhida para alinhamento
  // dos cantos (encontro das paredes)
  glm::mat4 model{1.0f};
  model = glm::translate(model, glm::vec3{0.0f, 0.2f, 0.96f});

  auto modelViewMatrix{glm::mat3(viewMatrix * model)};
  auto normalMatrix{glm::inverseTranspose(modelViewMatrix)};
  abcg::glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);

  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(wallColorLocation, 1.0f, 0.8f, 0.0f, 1.0f);

  m_model.renderTexture(&m_indices, &m_VAO, diffuseTexture);

  // Desenho da parede norte. A posição z tem a mesma lógica da anterior, porém
  // refletida em relação a origem
  model = glm::mat4(1.0);
  model = glm::translate(model, glm::vec3{0.0f, 0.2f, -0.96f});

  modelViewMatrix = glm::mat3(viewMatrix * model);
  normalMatrix = glm::inverseTranspose(modelViewMatrix);
  abcg::glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);

  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(wallColorLocation, 1.0f, 0.8f, 0.0f, 1.0f);

  m_model.renderTexture(&m_indices, &m_VAO, diffuseTexture);

  // Desenho da parede leste. A lógica da coordenada z das paredes anteriores
  // foi aplicada na coordenada x dessa parede e também rodamos em -90.0 graus
  // para formar a parede lateral.
  model = glm::mat4(1.0);
  model = glm::translate(model, glm::vec3{0.96f, 0.2f, 0.0f});
  model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));

  modelViewMatrix = glm::mat3(viewMatrix * model);
  normalMatrix = glm::inverseTranspose(modelViewMatrix);
  abcg::glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);

  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(wallColorLocation, 1.0f, 0.8f, 0.0f, 1.0f);

  m_model.renderTexture(&m_indices, &m_VAO, diffuseTexture);

  // Desenho da parede oeste. Mesma lógica da parede anterior para a coordenada
  // x, porém refletida pela origem.
  model = glm::mat4(1.0);
  model = glm::translate(model, glm::vec3{-0.96f, 0.2f, 0.0f});
  model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));

  modelViewMatrix = glm::mat3(viewMatrix * model);
  normalMatrix = glm::inverseTranspose(modelViewMatrix);
  abcg::glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);

  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(wallColorLocation, 1.0f, 0.8f, 0.0f, 1.0f);

  // Renderização feita pela classe Model
  m_model.renderTexture(&m_indices, &m_VAO, diffuseTexture);
  abcg::glUseProgram(0);
}