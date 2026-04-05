#ifndef TP_OTHELLO_BOARD_HPP
#define TP_OTHELLO_BOARD_HPP

#include <array>
#include <bit>
#include <cstddef>
#include <ranges>
#include <utility>
#include <vector>
#include "color.hpp"
#include "utility.hpp"

namespace othello
{
/*!
 * @brief A simple representation of a Othello board.
 *
 * @details Encodes the state of the board using two bitboards (i.e. one 64-bit integer for each player), ensuring optimal
 *   memory usage and performance.
 */
class board;

class board
{
public:
  using size_type       = std::size_t;
  using coordinate_type = char const (&)[3];

  enum class view
    : size_type
  {
    black    = 0,
    white    = 1,
    occupied = 2,
    empty    = 3
  };

private:
  struct bb_utility
  {
    constexpr static bitboard bb_file_a = 0x0101010101010101;
    constexpr static bitboard bb_file_h = 0x8080808080808080;

    constexpr static auto bb_e  = [](bitboard const bb) constexpr -> bitboard { return (bb & ~bb_file_h) << 1; };
    constexpr static auto bb_w  = [](bitboard const bb) constexpr -> bitboard { return (bb & ~bb_file_a) >> 1; };
    constexpr static auto bb_n  = [](bitboard const bb) constexpr -> bitboard { return bb << 8; };
    constexpr static auto bb_s  = [](bitboard const bb) constexpr -> bitboard { return bb >> 8; };
    constexpr static auto bb_ne = [](bitboard const bb) constexpr -> bitboard { return (bb & ~bb_file_h) << 9; };
    constexpr static auto bb_nw = [](bitboard const bb) constexpr -> bitboard { return (bb & ~bb_file_a) << 7; };
    constexpr static auto bb_se = [](bitboard const bb) constexpr -> bitboard { return (bb & ~bb_file_h) >> 7; };
    constexpr static auto bb_sw = [](bitboard const bb) constexpr -> bitboard { return (bb & ~bb_file_a) >> 9; };
  };

private:
  std::array<bitboard, 2>
  m_bbs;

private:
  template<color> [[nodiscard]] constexpr bitboard player  () const noexcept;
  template<color> [[nodiscard]] constexpr bitboard opponent() const noexcept;

public:
  constexpr
  board()
  noexcept;

  constexpr board(board const &) = default;
  constexpr board(board &&)      = default;

  constexpr
  ~board()
  = default;

  constexpr board &operator=(board const &) = default;
  constexpr board &operator=(board &&)      = default;

  constexpr
  void
  swap(board &)
  noexcept;


  template<view> [[nodiscard]] constexpr bitboard get  () const noexcept;
  template<view> [[nodiscard]] constexpr int      count() const noexcept;
  template<view> [[nodiscard]] constexpr bool is(coordinate_type) const noexcept;
  template<view> [[nodiscard]] constexpr bool is(size_type      ) const noexcept;

  [[nodiscard]] constexpr bitboard get  (view) const noexcept;
  [[nodiscard]] constexpr int      count(view) const noexcept;
  [[nodiscard]] constexpr bool is(view, coordinate_type) const noexcept;
  [[nodiscard]] constexpr bool is(view, size_type      ) const noexcept;


  template<color> [[nodiscard]] constexpr bitboard get  () const noexcept;
  template<color> [[nodiscard]] constexpr int      count() const noexcept;
  template<color> [[nodiscard]] constexpr bool is(coordinate_type) const noexcept;
  template<color> [[nodiscard]] constexpr bool is(size_type      ) const noexcept;

  [[nodiscard]] constexpr bitboard get  (color) const noexcept;
  [[nodiscard]] constexpr int      count(color) const noexcept;
  [[nodiscard]] constexpr bool is(color, coordinate_type) const noexcept;
  [[nodiscard]] constexpr bool is(color, size_type      ) const noexcept;


  template<color> [[nodiscard]] constexpr bitboard                moves()                         const noexcept;
  template<color>               constexpr std::vector<size_type> &moves(std::vector<size_type> &) const;

  [[nodiscard]] constexpr bitboard                moves(color)                           const noexcept;
  constexpr               std::vector<size_type> &moves(color, std::vector<size_type> &) const;


  template<color> [[nodiscard]] constexpr bitboard                flips(bitboard)                           const noexcept;
  template<color>               constexpr std::vector<size_type> &flips(bitboard, std::vector<size_type> &) const;
  template<color> [[nodiscard]] constexpr bitboard                flips(coordinate_type)                           const noexcept;
  template<color>               constexpr std::vector<size_type> &flips(coordinate_type, std::vector<size_type> &) const;

  [[nodiscard]] constexpr bitboard                flips(color, bitboard)                           const noexcept;
  constexpr               std::vector<size_type> &flips(color, bitboard, std::vector<size_type> &) const;
  [[nodiscard]] constexpr bitboard                flips(color, coordinate_type)                           const noexcept;
  constexpr               std::vector<size_type> &flips(color, coordinate_type, std::vector<size_type> &) const;


  template<color> [[nodiscard]] constexpr bool can_play(bitboard)        const noexcept;
  template<color> [[nodiscard]] constexpr bool can_play(coordinate_type) const noexcept;

  [[nodiscard]] constexpr bool can_play(color, bitboard)        const noexcept;
  [[nodiscard]] constexpr bool can_play(color, coordinate_type) const noexcept;


  template<color> constexpr bool play    (bitboard)        noexcept;
  template<color> constexpr bool play    (coordinate_type) noexcept;
  template<color> constexpr bool try_play(bitboard)        noexcept;
  template<color> constexpr bool try_play(coordinate_type) noexcept;

  constexpr bool play    (color, bitboard)        noexcept;
  constexpr bool play    (color, coordinate_type) noexcept;
  constexpr bool try_play(color, bitboard)        noexcept;
  constexpr bool try_play(color, coordinate_type) noexcept;


  friend constexpr
  void
  swap(board &lhs, board &rhs)
  noexcept
  {
    lhs.swap(rhs);
  }

  [[nodiscard]]
  friend constexpr
  bool
  operator==(board const &, board const &)
  = default;
};


template <color Clr>
constexpr
bitboard
board::player() const
noexcept
{
  if constexpr (Clr == color::black) return get<view::black>();
  if constexpr (Clr == color::white) return get<view::white>();

  return bitboard{0}; // should not be reachable
}

template <color Clr>
constexpr
bitboard
board::opponent() const
noexcept
{
  if constexpr (Clr == color::black) return get<view::white>();
  if constexpr (Clr == color::white) return get<view::black>();

  return bitboard{0}; // should not be reachable
}

constexpr
board::board()
noexcept
  : m_bbs{to_bitboard("d5") | to_bitboard("e4"), to_bitboard("d4") | to_bitboard("e5")}
{ }

constexpr
void
board::swap(board &other)
noexcept
{
  using std::swap;

  swap(m_bbs, other.m_bbs);
}

template<board::view Vw>
constexpr
bitboard
board::get() const
noexcept
{
  if constexpr (Vw == view::black) return m_bbs[0];
  if constexpr (Vw == view::white) return m_bbs[1];

  if constexpr (Vw == view::occupied) return m_bbs[0] | m_bbs[1];
  if constexpr (Vw == view::empty   ) return ~get<view::occupied>();

  return bitboard{0}; // should not be reachable
}

template<board::view Vw>
constexpr
int
board::count() const
noexcept
{
  return std::popcount(get<Vw>());
}

template<board::view Vw>
constexpr
bool
board::is(coordinate_type co) const
noexcept
{
  return (get<Vw>() & to_bitboard(co)) != 0;
}

template<board::view Vw>
constexpr
bool
board::is(size_type const idx) const
noexcept
{
  return (get<Vw>() & to_bitboard(idx)) != 0;
}

constexpr
bitboard
board::get(view const vw) const
noexcept
{
  switch (vw)
  {
  case view::black   : return get<view::black   >();
  case view::white   : return get<view::white   >();
  case view::occupied: return get<view::occupied>();
  case view::empty   : return get<view::empty   >();
  }
  return bitboard{0}; // should not be reachable
}

constexpr
int
board::count(view const vw) const
noexcept
{
  return std::popcount(get(vw));
}

constexpr
bool
board::is(view const vw, coordinate_type co) const
noexcept
{
  return (get(vw) & to_bitboard(co)) != bitboard{0};
}

constexpr
bool
board::is(view const vw, size_type const idx) const
noexcept
{
  return (get(vw) & to_bitboard(idx)) != bitboard{0};
}

template<color Clr>
constexpr
bitboard
board::get() const
noexcept
{
  if constexpr (Clr == color::black) return m_bbs[0];
  if constexpr (Clr == color::white) return m_bbs[1];

  return bitboard{0}; // should not be reachable
}

template<color Clr>
constexpr
int
board::count() const
noexcept
{
  return std::popcount(get<Clr>());
}

template<color Clr>
constexpr
bool
board::is(coordinate_type co) const
noexcept
{
  return (get<Clr>() & to_bitboard(co)) != 0;
}

template<color Clr>
constexpr
bool
board::is(size_type const idx) const
noexcept
{
  return (get<Clr>() & to_bitboard(idx)) != 0;
}

constexpr
bitboard
board::get(color const clr) const
noexcept
{
  switch (clr)
  {
  case color::black : return get<color::black>();
  case color::white : return get<color::white>();
  }
  return bitboard{0}; // should not be reachable
}

constexpr
int
board::count(color const clr) const
noexcept
{
  return std::popcount(get(clr));
}

constexpr
bool
board::is(color const clr, coordinate_type co) const
noexcept
{
  return (get(clr) & to_bitboard(co)) != bitboard{0};
}

constexpr
bool
board::is(color const clr, size_type const idx) const
noexcept
{
  return (get(clr) & to_bitboard(idx)) != bitboard{0};
}

template<color Clr>
constexpr
bitboard
board::moves() const
noexcept
{
  bitboard const bb_player   = player  <Clr>();
  bitboard const bb_opponent = opponent<Clr>();
  bitboard const bb_empty    = get<view::empty>();

  auto moves_for
  = [](bitboard const bb_p, bitboard const bb_o, bitboard const bb_e, auto dir) constexpr -> bitboard
  {
    bitboard bb{dir(bb_p) & bb_o};

    bb  |= dir(bb) & bb_o;
    bb  |= dir(bb) & bb_o;
    bb  |= dir(bb) & bb_o;
    bb  |= dir(bb) & bb_o;
    bb  |= dir(bb) & bb_o;
    return dir(bb) & bb_e;
  };

  bitboard bb{0};

  bb |= moves_for(bb_player, bb_opponent, bb_empty, bb_utility::bb_e );
  bb |= moves_for(bb_player, bb_opponent, bb_empty, bb_utility::bb_w );
  bb |= moves_for(bb_player, bb_opponent, bb_empty, bb_utility::bb_n );
  bb |= moves_for(bb_player, bb_opponent, bb_empty, bb_utility::bb_s );
  bb |= moves_for(bb_player, bb_opponent, bb_empty, bb_utility::bb_ne);
  bb |= moves_for(bb_player, bb_opponent, bb_empty, bb_utility::bb_nw);
  bb |= moves_for(bb_player, bb_opponent, bb_empty, bb_utility::bb_se);
  bb |= moves_for(bb_player, bb_opponent, bb_empty, bb_utility::bb_sw);
  return bb;
}

template<color Clr>
constexpr
std::vector<typename board::size_type> &
board::moves(std::vector<size_type> &out) const
{
  bitboard bb = moves<Clr>();

  // we presume that the vector is already empty
  while (bb != bitboard{0})
  {
    bitboard const lsb = bb & (~bb + 1);
    out.emplace_back(std::countr_zero(lsb));
    bb ^= lsb;
  }
  return out;
}

constexpr
bitboard
board::moves(color const clr) const
noexcept
{
  switch (clr)
  {
  case color::black: return moves<color::black>();
  case color::white: return moves<color::white>();
  }
  return bitboard{0}; // should not be reachable
}

constexpr
std::vector<typename board::size_type> &
board::moves(color const clr, std::vector<size_type> &out) const
{
  switch (clr)
  {
  case color::black: return moves<color::black>(out);
  case color::white: return moves<color::white>(out);
  }
  return out; // should not be reachable
}

template<color Clr>
constexpr
bitboard
board::flips(bitboard const move) const
noexcept
{
  bitboard const bb_player   = player<Clr>();
  bitboard const bb_opponent = opponent<Clr>();

  auto flips_for
  = [](bitboard const mv, bitboard const bb_p, bitboard const bb_o, auto dir) constexpr -> bitboard
  {
    bitboard bb = dir(mv) & bb_o;

    bb |= dir(bb) & bb_o;
    bb |= dir(bb) & bb_o;
    bb |= dir(bb) & bb_o;
    bb |= dir(bb) & bb_o;
    bb |= dir(bb) & bb_o;
    return (dir(bb) & bb_p) ? bb : bitboard{0};
  };

  bitboard bb{0};

  bb |= flips_for(move, bb_player, bb_opponent, bb_utility::bb_e );
  bb |= flips_for(move, bb_player, bb_opponent, bb_utility::bb_w );
  bb |= flips_for(move, bb_player, bb_opponent, bb_utility::bb_n );
  bb |= flips_for(move, bb_player, bb_opponent, bb_utility::bb_s );
  bb |= flips_for(move, bb_player, bb_opponent, bb_utility::bb_ne);
  bb |= flips_for(move, bb_player, bb_opponent, bb_utility::bb_nw);
  bb |= flips_for(move, bb_player, bb_opponent, bb_utility::bb_se);
  bb |= flips_for(move, bb_player, bb_opponent, bb_utility::bb_sw);
  return bb;
}

template<color Clr>
constexpr
std::vector<board::size_type> &
board::flips(bitboard const move, std::vector<size_type> &out) const
{
  bitboard bb = flips<Clr>(move);

  // we presume that the vector is already empty
  while (bb != bitboard{0})
  {
    bitboard const lsb = bb & (~bb + 1);
    out.emplace_back(std::countr_zero(lsb));
    bb ^= lsb;
  }
  return out;
}

template<color Clr>
constexpr
bitboard
board::flips(coordinate_type co) const
noexcept
{
  return flips<Clr>(to_bitboard(co));
}

template<color Clr>
constexpr
std::vector<board::size_type> &
board::flips(coordinate_type co, std::vector<size_type> &out) const
{
  return flips<Clr>(to_bitboard(co), out);
}

constexpr
bitboard
board::flips(color const clr, bitboard const move) const
noexcept
{
  switch (clr)
  {
  case color::black: return flips<color::black>(move);
  case color::white: return flips<color::white>(move);
  }
  return bitboard{0}; // should not be reachable
}

constexpr
std::vector<board::size_type> &
board::flips(color const clr, bitboard const move, std::vector<size_type> &out) const
{
  switch (clr)
  {
  case color::black: return flips<color::black>(move, out);
  case color::white: return flips<color::white>(move, out);
  }
  return out; // should not be reachable
}

constexpr
bitboard
board::flips(color const clr, coordinate_type co) const
noexcept
{
  return flips(clr, to_bitboard(co));
}

constexpr
std::vector<board::size_type> &
board::flips(color const clr, coordinate_type co, std::vector<size_type> &out) const
{
  return flips(clr, to_bitboard(co), out);
}

template<color Clr>
constexpr
bool
board::can_play(bitboard const move) const
noexcept
{
  return move
      && !(move & (move - 1))
      && !(move & get<view::occupied>())
      && flips<Clr>(move);
}

template<color Clr>
constexpr
bool
board::can_play(coordinate_type co) const
noexcept
{
  return can_play<Clr>(to_bitboard(co));
}

constexpr
bool
board::can_play(color const clr, bitboard const move) const
noexcept
{
  switch (clr)
  {
  case color::black: return can_play<color::black>(move);
  case color::white: return can_play<color::white>(move);
  }
  return false; // should not be reachable
}

constexpr
bool
board::can_play(color const clr, coordinate_type co) const
noexcept
{
  return can_play(clr, to_bitboard(co));
}

template<color Clr>
constexpr
bool
board::play(bitboard const move)
noexcept
{
  bitboard const
  f
  = flips<Clr>(move);

  if constexpr (Clr == color::black) { m_bbs[0] |= move | f; m_bbs[1] &= ~f; }
  if constexpr (Clr == color::white) { m_bbs[1] |= move | f; m_bbs[0] &= ~f; }
  return true;
}

template<color Clr>
constexpr
bool
board::play(coordinate_type co)
noexcept
{
  return play<Clr>(to_bitboard(co));
}

template<color Clr>
constexpr
bool
board::try_play(bitboard const move)
noexcept
{
  if (!can_play<Clr>(move))
    return false;

  return play<Clr>(move);
}

template<color Clr>
constexpr
bool
board::try_play(coordinate_type co)
noexcept
{
  return try_play<Clr>(to_bitboard(co));
}

constexpr
bool
board::play(color const clr, bitboard const move)
noexcept
{
  switch (clr)
  {
  case color::black: return play<color::black>(move);
  case color::white: return play<color::white>(move);
  }
  return false; // should not be reachable
}

constexpr
bool
board::play(color const clr, coordinate_type co)
noexcept
{
  return play(clr, to_bitboard(co));
}

constexpr
bool
board::try_play(color const clr, bitboard const move)
noexcept
{
  switch (clr)
  {
  case color::black: return try_play<color::black>(move);
  case color::white: return try_play<color::white>(move);
  }
  return false; // should not be reachable
}

constexpr
bool
board::try_play(color const clr, coordinate_type co)
noexcept
{
  return try_play(clr, to_bitboard(co));
}


}

#endif // TP_OTHELLO_BOARD_HPP