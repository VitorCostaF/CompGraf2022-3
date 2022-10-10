#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include "abcgOpenGL.hpp"
#include <array>

class Window : public abcg::OpenGLWindow {
protected:
  void onCreate() override;
  void onPaintUI() override;

private:
  static int const m_N{11}; // Board size is m_N x m_N
  static int const numberLevels{2};
  bool mark{true};

  enum class GameState { Play, Win };
  GameState m_gameState;

  // static constexpr std::array<int, numberLevels> levelsSize{10, 10, 10, 15,
  // 15};
  static constexpr std::array<int, numberLevels> levelsSize{5, 10};

  std::array<std::string, levelsSize.at(0)> valoresColunas1{"2", "4", "4", "4",
                                                            "2"};
  std::array<std::string, levelsSize.at(0)> valoresLinhas1{"1 1", "5", "5", "3",
                                                           "1"};

  std::array<std::string, levelsSize.at(1)> valoresColunas2{
      "1", "5\n1", "2\n1\n3", "1\n5",    "2\n7",
      "7", "2\n7", "1\n5\n1", "1\n1\n1", "3\n2"};
  std::array<std::string, levelsSize.at(1)> valoresLinhas2{
      "1 1 2", "1 1 1", "", "7", "1 6", "1 5 1", "1 7", "7", "5 1", "10"};

  std::array<char, levelsSize.at(0) * levelsSize.at(0)> level1Board{
      '0', '1', '0', '1', '0', //
      '1', '1', '1', '1', '1', //
      '1', '1', '1', '1', '1', //
      '0', '1', '1', '1', '0', //
      '0', '0', '1', '0', '0'};

  std::array<char, levelsSize.at(1) * levelsSize.at(1)> level2Board{
      '0', '0', '1', '0', '1', '0', '1', '1', '0', '0', //
      '0', '0', '1', '0', '1', '0', '1', '0', '0', '0', //
      '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', //
      '0', '1', '1', '1', '1', '1', '1', '1', '0', '0', //
      '0', '1', '0', '0', '1', '1', '1', '1', '1', '1', //
      '0', '1', '0', '1', '1', '1', '1', '1', '0', '1', //
      '0', '1', '0', '1', '1', '1', '1', '1', '1', '1', //
      '0', '1', '1', '1', '1', '1', '1', '1', '0', '0', //
      '0', '0', '1', '1', '1', '1', '1', '0', '0', '1', //
      '1', '1', '1', '1', '1', '1', '1', '1', '1', '1'};

  std::array<char, m_N * m_N> m_board{}; // '\0', 'X' or 'O'
  std::array<bool, numberLevels> levelsVisible{};

  int level;

  ImFont *m_font{};

  void checkEndCondition();
  void restartGame();
  void nextLevel();
  void startGame();
  std::string getLabelColumn(int coluna);
  std::string getLabelLine(int linha);
  char getBoardChar(int offset);
  void setBoardChar(int offset, char c);
  void selectLevel(int level);
};

#endif