#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include "abcgOpenGL.hpp"

#include "ball.hpp"
#include "box.hpp"
#include "camera.hpp"
#include "gamedata.hpp"
#include "ground.hpp"
#include "model.hpp"
#include "sun.hpp"
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

  GLuint sunProgram{};

  GLint m_viewMatrixLocation{};
  GLint m_projMatrixLocation{};
  GLint m_modelMatrixLocation{};
  GLint m_colorLocation{};

  GLint sunViewMatrixLocation{};
  GLint sunProjMatrixLocation{};
  GLint sunModelMatrixLocation{};
  GLint sunColorLocation{};

  ImFont *m_font{};

  Camera m_camera;
  GameData m_gameData;
  Ground m_ground;
  Model m_model;
  Ball ball;
  Wall wall;
  Sun sun;

  std::vector<Box> boxes;

  int qtdBoxes{20};

  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;

  float timeElapsed{};
  int maxTime{20};

  void loadModelFromFile(std::string_view path);
  void checkWallColision();
  void randomizeBox(Box *box);
  bool checkBoxValidPosition(Box newBox);
  void checkBoxColision();
  int checkActiveBoxes();
  void restart();
  const std::string assetsPath1;
};

#endif