#ifndef GAMEDATA_HPP_
#define GAMEDATA_HPP_

#include <bitset>

enum class State { Playing, GameOver, Win, Restarted };

struct GameData {
  State m_state{State::Playing};
  std::bitset<5> m_input; // [fire, up, down, left, right]
};

#endif