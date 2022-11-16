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

void Camera::dolly(float speed, glm::vec3 newPositionAt) {
  // Compute forward vector (view direction)
  auto const forward{glm::normalize(m_at - m_eye)};

  // Move eye and center forward (speed > 0) or backward (speed < 0)
  // m_eye = newPositionAt;
  m_at = newPositionAt;

  computeViewMatrix();
}

void Camera::truck(float speed, glm::vec3 newPositionAt) {
  // Compute forward vector (view direction)
  auto const forward{glm::normalize(m_at - m_eye)};
  // Compute vector to the left
  auto const left{glm::cross(m_up, forward)};

  // Move eye and center to the left (speed < 0) or to the right (speed > 0)
  m_at = newPositionAt;

  computeViewMatrix();
}

void Camera::pan(float speed, glm::vec3 newPositionAt) {
  glm::mat4 transform{1.0f};

  // // Rotate camera around its local y axis
  // transform = glm::translate(transform, m_eye);
  // transform = glm::rotate(transform, -speed, m_up);
  // transform = glm::translate(transform, -m_eye);

  // m_at = transform * glm::vec4(m_at, 1.0f);
  m_at = newPositionAt;

  computeViewMatrix();
}