#include "window.hpp"
#include "core.h"
#include "imgui.h"
#include <array>
#include <stdio.h>
#include <string>

void Window::onCreate() {
  // Load font with bigger size for the X's and O's
  auto const filename{abcg::Application::getAssetsPath() +
                      "Inconsolata-Medium.ttf"};
  m_font = ImGui::GetIO().Fonts->AddFontFromFileTTF(filename.c_str(), 20.0f);
  if (m_font == nullptr) {
    throw abcg::RuntimeError{"Cannot load font file"};
  }

  startGame();
}

void Window::restartGame() {
  m_board.fill(' ');
  m_gameState = GameState::Play;
}

void Window::startGame() {
  levelsPassed.fill(false);
  m_board.fill(' ');
  level = 0;
  mark = true;
  m_gameState = GameState::Play;
}

std::string Window::getLabelColumn(int coluna) {
  if (level == 0) {
    return valoresColunas1.at(coluna);
  }
  return valoresColunas2.at(coluna);
}

std::string Window::getLabelLine(int linha) {
  if (level == 0) {
    return valoresLinhas1.at(linha);
  }
  return valoresLinhas2.at(linha);
}

char Window::getBoardChar(int offset) {
  if (level == 0) {
    return level1Board.at(offset);
  }
  return level2Board.at(offset);
}

void Window::setBoardChar(int offset, char c) {
  if (level == 0) {
    level1Board.at(offset) = c;
  }
  level2Board.at(offset) = c;
}

void Window::onPaintUI() {
  // Get size of application's window
  auto const appWindowWidth{gsl::narrow<float>(getWindowSettings().width)};
  auto const appWindowHeight{gsl::narrow<float>(getWindowSettings().height)};
  // "Tic-Tac-Toe" window
  {
    ImGui::SetNextWindowSize(ImVec2(appWindowWidth, appWindowHeight));
    ImGui::SetNextWindowPos(ImVec2(0, 0));

    auto const flags{ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoCollapse};
    ImGui::Begin("Nonogram", nullptr, flags);
    // Menu
    {
      bool restartSelected{};
      if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Game")) {
          ImGui::MenuItem("Restart", nullptr, &restartSelected);
          ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
      }
      if (restartSelected) {
        restartGame();
      }
    }
    // Static text showing current turn or win/draw messages
    {
      std::string text;
      switch (m_gameState) {
      case GameState::Play:
        text = fmt::format("Level {}", level + 1);
        break;
      case GameState::Win:
        text = "Congratulations!";
        break;
      }
      // Center text
      ImGui::SetCursorPosX(
          (appWindowWidth - ImGui::CalcTextSize(text.c_str()).x) / 2);
      ImGui::Text("%s", text.c_str());
      ImGui::Spacing();
    }
    ImGui::Spacing();
    // Game board
    {
      auto const levelSize = levelsSize.at(level);
      auto const gridHeight{appWindowHeight - 22 - 60 - ((levelSize + 1) * 10) -
                            60 - 60};
      auto const buttonHeight{gridHeight / (levelSize + 1)};
      // Use custom font
      ImGui::PushFont(m_font);
      if (ImGui::BeginTable("Game board {}", levelSize + 1)) {
        for (auto i : iter::range(levelSize + 1)) {
          ImGui::TableNextRow();
          for (auto j : iter::range(levelSize + 1)) {
            ImGui::TableSetColumnIndex(j);
            if (i == 0) {
              if (j != 0) {
                std::string text = fmt::format("{}\n", getLabelColumn(j - 1));
                ImGui::Text("%s", text.c_str());
              }
            } else {
              if (j == 0) {
                std::string text = fmt::format("{}", getLabelLine(i - 1));
                ImGui::Text("%s", text.c_str());
              } else {
                auto const offset{(i - 1) * levelSize + (j - 1)};
                // Button text is ch followed by an ID in the format ##ij
                auto buttonText{
                    fmt::format("{}##{}{}", m_board.at(offset), i, j)};
                if (ImGui::Button(buttonText.c_str(),
                                  ImVec2(-1, buttonHeight))) {
                  if (m_gameState == GameState::Play) {
                    m_board.at(offset) =
                        mark ? m_board.at(offset) == '0' ? ' ' : '1'
                        : m_board.at(offset) == '1' ? ' '
                                                    : '0';
                    checkEndCondition();
                  }
                }
              }
            }
          }
          ImGui::Spacing();
        }
        ImGui::EndTable();
      }
      ImGui::PopFont();
    }

    ImGui::Spacing();
    // "Restart game" button
    {
      if (ImGui::Button("Mark", ImVec2(100, 50))) {
        mark = true;
      }
      ImGui::SameLine();
      if (ImGui::Button("Unmark", ImVec2(100, 50))) {
        mark = false;
      }
    }
    ImGui::Spacing();
    {
      if (ImGui::Button("Restart game", ImVec2(-1, 50))) {
        restartGame();
        mark = true;
      }
    }

    ImGui::End();
  }
}

void Window::checkEndCondition() {

  auto allXsOrOs{[&](std::string_view str) {
    std::string concatenation;
    auto const levelSize = levelsSize.at(level);
    if (level == 0) {
      for (auto i : iter::range(levelSize)) {

        for (auto j : iter::range(levelSize)) {
          concatenation += level1Board.at(i * levelSize + j);
        }
      }
    } else {
      for (auto i : iter::range(levelSize)) {

        for (auto j : iter::range(levelSize)) {
          concatenation += level2Board.at(i * levelSize + j);
        }
      }
    }
    bool resultado = str.compare(concatenation);

    if (!resultado) {
      m_gameState = GameState::Win;
    }
  }};

  std::string concatenation;
  auto const levelSize = levelsSize.at(level);
  for (auto i : iter::range(levelSize)) {

    for (auto j : iter::range(levelSize)) {
      concatenation += m_board.at(i * levelSize + j);
    }
  }

  allXsOrOs(concatenation);
}