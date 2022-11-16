#include "camera.hpp"

#include <glm/fwd.hpp>
#include <glm/gtc/matrix_transform.hpp>

void Camera::computeProjectionMatrix(glm::vec2 const &size) {
  m_projMatrix = glm::mat4(1.0f);
  auto const aspect{size.x / size.y};
  m_projMatrix = glm::perspective(glm::radians(50.0f), aspect, 0.1f, 10.0f);
}

void Camera::computeViewMatrix() {
  m_viewMatrix = glm::lookAt(m_eye, m_at, m_up);
}

void Camera::dolly(glm::vec3 newPositionAt) {
  m_at = newPositionAt;
  computeViewMatrix();
}

void Camera::truck(glm::vec3 newPositionAt) {
  m_at = newPositionAt;
  computeViewMatrix();
}

void Camera::pan(glm::vec3 newPositionAt) {
  m_at = newPositionAt;
  computeViewMatrix();
}