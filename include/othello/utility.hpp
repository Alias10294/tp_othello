#ifndef TP_OTHELLO_UTILITY_HPP
#define TP_OTHELLO_UTILITY_HPP

#include <cstdint>

namespace othello
{
using bitboard   = std::uint64_t;
using coordinate = char const (&)[3];

constexpr
auto
coordinate_array
= std::array<char[3], 64>(
  { "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8" });




constexpr
std::size_t
to_index(char const file, char const rank)
noexcept
{
  return static_cast<std::size_t>(file - 'a') + 8 * static_cast<std::size_t>(rank - '1');
}

constexpr
std::size_t
to_index(coordinate co)
noexcept
{
  return to_index(co[0], co[1]);
}


constexpr
bitboard
to_bitboard(char const file, char const rank)
noexcept
{
  return bitboard{1} << to_index(file, rank);
}

constexpr
bitboard
to_bitboard(coordinate co)
noexcept
{
  return bitboard{1} << to_index(co);
}

constexpr
bitboard
to_bitboard(std::size_t const idx)
noexcept
{
  return bitboard{1} << idx;
}

constexpr
coordinate
to_string(bitboard const bb)
noexcept
{
  std::size_t idx = 0;
  while ((bb >> idx) != bitboard{1})
    ++idx;

  return coordinate_array[idx];
}


}

#endif // TP_OTHELLO_UTILITY_HPP