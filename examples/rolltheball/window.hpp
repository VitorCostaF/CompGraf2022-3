#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include "abcgOpenGL.hpp"

#include "ball.hpp"
#include "box.hpp"
#include "camera.hpp"
#include "ground.hpp"
#include "model.hpp"
#include "wall.hpp"
#include <random>
#include <vector>

class Window : public abcg::OpenGLWindow {
protected:
  void onEvent(SDL_Event const &event) override;
  void onCreate() override;
  void onPaint() override;
  void onPaintUI() override;
  void onResize(glm::ivec2 const &size) override;
  void onDestroy() override;
  void onUpdate() override;

private:
  std::default_random_engine m_randomEngine;
  glm::ivec2 m_viewportSize{};

  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};
  GLuint m_program{};

  GLint m_viewMatrixLocation{};
  GLint m_projMatrixLocation{};
  GLint m_modelMatrixLocation{};
  GLint m_colorLocation{};

  Camera m_camera;

  Ground m_ground;
  Model m_model;
  Ball ball;
  Wall wall;
  std::vector<Box> boxes;

  int qtdBoxes{20};

  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;

  void loadModelFromFile(std::string_view path);
  void checkWallColision();
  void randomizeBox(Box &box);
  bool checkBoxesColision(Box newBox);
};

#endif