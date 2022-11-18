#include "ball.hpp"

void Ball::update(float deltaTime) {
  ballPosition.z += verticalSpeed * deltaTime;
  ballPosition.x += horizontalSpeed * deltaTime;
}