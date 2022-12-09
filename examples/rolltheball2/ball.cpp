#include "ball.hpp"
#include "core.h"
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
  // ballColorLocation = abcg::glGetUniformLocation(ballProgram, "color");
}

void Ball::update(float deltaTime, Sun sun, Moon moon) {
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

  // Abaixo atualizamos as propriedades de iluminação de acordo com a cor do sol
  // para tentar representar o brilho e a cor

  // Aqui alteramos a cor das luzes incidentes de acordo com a luz do sol.
  // Mantemos as constantes k fixas, pois o objeto é o mesmo. Colocamos as luzes
  // especular e difusa como sendo a luz do sol pois ela é a predominante e
  // ficar com um efeito visual melhor dado que a luz do sol está do lado oposto
  // da camera.
  sunIs = sun.sunColor;
  sunId = sun.sunColor * 0.8f;

  moonIs = moon.moonColor * 0.5f;
  moonId = moon.moonColor * 0.5f;

  // A cor ambiente e atualizada com um fator da luz do sol e luz da lua. O sol
  // tem uma luz mais forte por isso mais representativo para a luz ambiente.
  Ia = sun.sunColor * 0.3f;

  // A luz do sol ilumina todo o ambiente em todas as direções e como movemos a
  // bolinha e também o sol se move calculamos a luz incidente do sol como se o
  // sol estivesse "olhando" para a bolinha
  sunLightDir = glm::vec4(ballPosition - sun.sunPosition, 1);

  // Usamos a mesma ideia da direção da luz do sol para a luz da lua
  moonLightDir = glm::vec4(ballPosition - moon.moonPosition, 1);

  // Aqui deixamos o brilho "maior" quando o sol está a pino. Isso por que a
  // posição x do sol é zero nesse momento e no nascer e se por do sol o brilho
  // é mais focalizado ou "menor".
  sunShininess = abs(sun.sunPosition.x);
  moonShininess = moon.moonShininess;
}

void Ball::paint(glm::mat4 viewMatrix, glm::mat4 projMatrix, Model m_model) {

  // Ativação do program e bind das matrizes de projeção e view
  abcg::glUseProgram(ballProgram);

  // Localização das matrizes
  auto const viewMatrixLoc{
      abcg::glGetUniformLocation(ballProgram, "viewMatrix")};
  auto const projMatrixLoc{
      abcg::glGetUniformLocation(ballProgram, "projMatrix")};
  auto const modelMatrixLoc{
      abcg::glGetUniformLocation(ballProgram, "modelMatrix")};
  auto const normalMatrixLoc{
      abcg::glGetUniformLocation(ballProgram, "normalMatrix")};
  // Localização da direção da luz
  auto const sunLightLoc{abcg::glGetUniformLocation(ballProgram, "sunLight")};
  auto const moonLightLoc{abcg::glGetUniformLocation(ballProgram, "moonLight")};

  // Localização das propriedades de iluminação do sol
  auto const sunShininessLoc{
      abcg::glGetUniformLocation(ballProgram, "sunShininess")};
  auto const IaLoc{abcg::glGetUniformLocation(ballProgram, "Ia")};
  auto const sunIdLoc{abcg::glGetUniformLocation(ballProgram, "sunId")};
  auto const sunIsLoc{abcg::glGetUniformLocation(ballProgram, "sunIs")};
  auto const KaLoc{abcg::glGetUniformLocation(ballProgram, "Ka")};
  auto const sunKdLoc{abcg::glGetUniformLocation(ballProgram, "sunKd")};
  auto const sunKsLoc{abcg::glGetUniformLocation(ballProgram, "sunKs")};

  // Localização das propriedades de iluminação da lua
  auto const moonShininessLoc{
      abcg::glGetUniformLocation(ballProgram, "moonShininess")};
  auto const moonIdLoc{abcg::glGetUniformLocation(ballProgram, "moonId")};
  auto const moonIsLoc{abcg::glGetUniformLocation(ballProgram, "moonIs")};
  auto const moonKdLoc{abcg::glGetUniformLocation(ballProgram, "moonKd")};
  auto const moonKsLoc{abcg::glGetUniformLocation(ballProgram, "moonKs")};

  // Localização das propriedades de atenuação
  auto const KcSunLoc{abcg::glGetUniformLocation(ballProgram, "KcSun")};
  auto const KlSunLoc{abcg::glGetUniformLocation(ballProgram, "KlSun")};
  auto const KqSunLoc{abcg::glGetUniformLocation(ballProgram, "KqSun")};

  // Localização das propriedades de atenuação
  auto const KcMoonLoc{abcg::glGetUniformLocation(ballProgram, "KcMoon")};
  auto const KlMoonLoc{abcg::glGetUniformLocation(ballProgram, "KlMoon")};
  auto const KqMoonLoc{abcg::glGetUniformLocation(ballProgram, "KqMoon")};

  // Bind das propriedades
  abcg::glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &projMatrix[0][0]);

  abcg::glUniform4fv(IaLoc, 1, &Ia.x);
  abcg::glUniform4fv(KaLoc, 1, &Ka.x);

  // Propriedades do sol
  abcg::glUniform4fv(sunLightLoc, 1, &sunLightDir.x);
  abcg::glUniform4fv(sunIdLoc, 1, &sunId.x);
  abcg::glUniform4fv(sunIsLoc, 1, &sunIs.x);

  abcg::glUniform4fv(sunKdLoc, 1, &sunKd.x);
  abcg::glUniform4fv(sunKsLoc, 1, &sunKs.x);
  abcg::glUniform1f(sunShininessLoc, sunShininess);

  // Propriedades da lua
  abcg::glUniform4fv(moonLightLoc, 1, &moonLightDir.x);
  abcg::glUniform4fv(moonIdLoc, 1, &moonId.x);
  abcg::glUniform4fv(moonIsLoc, 1, &moonIs.x);

  abcg::glUniform4fv(moonKdLoc, 1, &moonKd.x);
  abcg::glUniform4fv(moonKsLoc, 1, &moonKs.x);
  abcg::glUniform1f(moonShininessLoc, moonShininess);

  // Propriedades de distância sol
  abcg::glUniform1f(KcSunLoc, kcSun);
  abcg::glUniform1f(KlSunLoc, klSun);
  abcg::glUniform1f(KqSunLoc, kqSun);

  // Propriedades de distância lua
  abcg::glUniform1f(KcMoonLoc, kcMoon);
  abcg::glUniform1f(KlMoonLoc, klMoon);
  abcg::glUniform1f(KqMoonLoc, kqMoon);

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
  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &model[0][0]);
  // abcg::glUniform4f(ballColorLocation, 1.0f, 1.0f, 1.0f, 1.0f);

  auto const modelViewMatrix{glm::mat3(viewMatrix * model)};
  auto const normalMatrix{glm::inverseTranspose(modelViewMatrix)};
  abcg::glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);

  m_model.render(&m_indices, &m_VAO);

  abcg::glUseProgram(0);
}