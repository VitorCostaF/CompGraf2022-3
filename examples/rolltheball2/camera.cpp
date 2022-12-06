#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

void Camera::computeProjectionMatrix(glm::vec2 const &size) {
  // Aqui computamos a perspectiva da camera
  m_projMatrix = glm::mat4(1.0f);
  auto const aspect{size.x / size.y};
  m_projMatrix = glm::perspective(glm::radians(50.0f), aspect, 0.1f, 10.0f);
}

void Camera::computeViewMatrix() {
  // Aqui basicamente computamos a view matrix usando a função lookAt. A m_at é
  // apontada para bolinha, então conforme essa se mexe a camera a acomanha,
  // porém não se desloca
  m_viewMatrix = glm::lookAt(m_eye, m_at, m_up);
}

void Camera::move(glm::vec3 newPositionAt) {
  // Atualizamos o ponto que a camera está olhando.
  m_at = newPositionAt;
  computeViewMatrix();
}