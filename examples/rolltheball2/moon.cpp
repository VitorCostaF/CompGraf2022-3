#include "moon.hpp"
#include "core.h"

void Moon::create(Model m_model, const std::string assetsPath) {

  // Criação do program com o .vert e .frag específicos para a lua, pois ele não
  // será afetado pela iluminação, uma vez que ele seria a fonte de luz
  moonProgram = abcg::createOpenGLProgram(
      {{.source = assetsPath + "sun.vert", .stage = abcg::ShaderStage::Vertex},
       {.source = assetsPath + "sun.frag",
        .stage = abcg::ShaderStage::Fragment}});

  // Carregamos os índices e vértices para a lua a partir do sphere.obj
  m_model.loadObj(assetsPath + "sphere.obj", &m_vertices, &m_indices, &m_VBO,
                  &m_EBO);

  // Inicializamos os buffers para a lua
  m_model.setupVAO(moonProgram, &m_VBO, &m_EBO, &m_VAO);

  // Iniciamos as localizações das variáveis uniformes
  moonViewMatrixLocation =
      abcg::glGetUniformLocation(moonProgram, "viewMatrix");
  moonProjMatrixLocation =
      abcg::glGetUniformLocation(moonProgram, "projMatrix");
  moonModelMatrixLocation =
      abcg::glGetUniformLocation(moonProgram, "modelMatrix");
  moonColorLocation = abcg::glGetUniformLocation(moonProgram, "color");
}

void Moon::update(float elapsedTime) { moonShininess = 22 - elapsedTime; }

void Moon::paint(glm::mat4 viewMatrix, glm::mat4 projMatrix, Model m_model) {

  abcg::glUseProgram(moonProgram);

  abcg::glUniformMatrix4fv(moonViewMatrixLocation, 1, GL_FALSE,
                           &viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(moonProjMatrixLocation, 1, GL_FALSE,
                           &projMatrix[0][0]);

  // Aqui transladamos a moon para sua posição e aplicamos a escala.
  glm::mat4 model{1.0f};
  model = glm::translate(model, moonPosition);
  model = glm::scale(model, glm::vec3(moonScale));

  abcg::glUniformMatrix4fv(moonModelMatrixLocation, 1, GL_FALSE, &model[0][0]);
  // A cor da moon será um laranja.
  abcg::glUniform4fv(moonColorLocation, 1, &moonColor.x);

  // Usamos a classe model para renderizar o objeto
  m_model.render(&m_indices, &m_VAO);
  abcg::glUseProgram(0);
}

void Moon::destroy() {
  abcg::glDeleteProgram(moonProgram);
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}
