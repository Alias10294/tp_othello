#ifndef TP_OTHELLO_INTELLIGENCE_EVALUATION_HPP
#define TP_OTHELLO_INTELLIGENCE_EVALUATION_HPP

#include <array>
#include <bit>
#include <cstdint>
#include "othello/utility.hpp"
#include "othello/board.hpp"
#include "othello/color.hpp"

namespace othello::intelligence
{
//! @brief The type used for evaluating a board's state.
using evaluation_value_type
= std::int32_t;

//! @brief An enumeration of all accounted kinds of heuristics.
enum class heuristic_kind
  : std::uint8_t
{
  position = 0,
  count    = 1,
  mobility = 2,
  mixed    = 3
};

/*!
 * @brief The type providing all evaluation functions based on the different heuristics.
 *
 * @tparam H The kind of heuristic to use.
 */
template<heuristic_kind H>
struct evaluation
{ };

template<>
struct evaluation<heuristic_kind::position>
{
  constexpr static
  std::array<evaluation_value_type, 64>
  tactical_value_matrix
  = { 100, -20,  10,   5,   5,  10, -20, 100,
      -20, -50,  -2,  -2,  -2,  -2, -50, -20,
       10,  -2,  -1,  -1,  -1,  -1,  -2,  10,
        5,  -2,  -1,  -1,  -1,  -1,  -2,   5,
        5,  -2,  -1,  -1,  -1,  -1,  -2,   5,
       10,  -2,  -1,  -1,  -1,  -1,  -2,  10,
      -20, -50,  -2,  -2,  -2,  -2, -50, -20,
      100, -20,  10,   5,   5,  10, -20, 100, };

public:
  constexpr static
  evaluation_value_type
  evaluate(board, color)
  noexcept;
};

constexpr
evaluation_value_type
evaluation<heuristic_kind::position>::evaluate(board const brd, color const clr)
noexcept
{
  bitboard const bb_b = brd.get<color::black>();
  bitboard const bb_w = brd.get<color::white>();

  bitboard const bb_p = (clr == color::black) ? bb_b : bb_w;
  bitboard const bb_o = (clr == color::black) ? bb_w : bb_b;

  auto score
  = [&](bitboard bb) constexpr -> evaluation_value_type
  {
    evaluation_value_type s{0};

    while (bb != bitboard{0})
    {
      bitboard const lsb = bb & (~bb + 1);
      s  += tactical_value_matrix[std::countr_zero(lsb)];
      bb ^= lsb;
    }
    return s;
  };

  return score(bb_p) - score(bb_o);
}

template<>
struct evaluation<heuristic_kind::count>
{
public:
  constexpr static
  evaluation_value_type
  evaluate(board, color)
  noexcept;
};

constexpr
evaluation_value_type
evaluation<heuristic_kind::count>::evaluate(board const brd, color const clr)
noexcept
{
  evaluation_value_type const count_b = brd.count<color::black>();
  evaluation_value_type const count_w = brd.count<color::white>();

  evaluation_value_type const count_p = (clr == color::black) ? count_b : count_w;
  evaluation_value_type const count_o = (clr == color::black) ? count_w : count_b;

  return count_p - count_o;
}

template<>
struct evaluation<heuristic_kind::mobility>
{
private:
  static constexpr
  evaluation_value_type
  corner_value
  = 100; // taken from the tactical value matrix above

public:
  constexpr static
  evaluation_value_type
  evaluate(board, color)
  noexcept;
};

constexpr
evaluation_value_type
evaluation<heuristic_kind::mobility>::evaluate(board const brd, color const clr)
noexcept
{
  evaluation_value_type const nb_mv_b = std::popcount(brd.moves<color::black>());
  evaluation_value_type const nb_mv_w = std::popcount(brd.moves<color::white>());

  evaluation_value_type const nb_mv_p = (clr == color::black) ? nb_mv_b : nb_mv_w;
  evaluation_value_type const nb_mv_o = (clr == color::black) ? nb_mv_w : nb_mv_b;


  auto constexpr corners = bitboard{0x8100000000000081};

  bitboard const bb_b = brd.get<color::black>();
  bitboard const bb_w = brd.get<color::white>();

  bitboard const bb_p = (clr == color::black) ? bb_b : bb_w;
  bitboard const bb_o = (clr == color::black) ? bb_w : bb_b;


  return (nb_mv_p - nb_mv_o) + (corner_value * (std::popcount(bb_p & corners) - std::popcount(bb_o & corners)));
}

template<>
struct evaluation<heuristic_kind::mixed>
{
private:
  constexpr static int position_threshold = 21;
  constexpr static int mobility_threshold = 42;

public:
  constexpr static
  evaluation_value_type
  evaluate(board, color)
  noexcept;
};

constexpr
evaluation_value_type
evaluation<heuristic_kind::mixed>
    ::evaluate(board const brd, color const clr)
noexcept
{
  int const turn = std::popcount(brd.get<board::view::occupied>());

  if (turn < position_threshold) return evaluation<heuristic_kind::position>::evaluate(brd, clr);
  if (turn < mobility_threshold) return evaluation<heuristic_kind::mobility>::evaluate(brd, clr);
  else                           return evaluation<heuristic_kind::count   >::evaluate(brd, clr);
}


template<heuristic_kind Heuristic>
constexpr
evaluation_value_type
evaluate(board, color)
noexcept;

template<heuristic_kind Heuristic>
constexpr
evaluation_value_type
evaluate(board const brd, color const clr)
noexcept
{
  return evaluation<Heuristic>::evaluate(brd, clr);
}


}

#endif // TP_OTHELLO_INTELLIGENCE_EVALUATION_HPP