#ifndef TP_OTHELLO_COLOR_HPP
#define TP_OTHELLO_COLOR_HPP

#include <cstddef>

namespace othello
{
//! @brief The representation of each player color on a Othello board.
enum class color
  : std::size_t
{
  black = 0,
  white = 1
};


template<color Clr>
inline constexpr
color
opposite_v
= (Clr == color::black) ? color::white : color::black;

[[nodiscard]] constexpr
color
opposite(color const clr)
noexcept
{
  return (clr == color::black) ? color::white : color::black;
}


}

#endif // TP_OTHELLO_COLOR_HPP