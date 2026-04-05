#ifndef TP_OTHELLO_GAME_HPP
#define TP_OTHELLO_GAME_HPP

#include <cctype>
#include <iostream>
#include <limits>
#include "othello/intelligence/algorithm.hpp"
#include "othello/intelligence/evaluation.hpp"
#include "othello/board.hpp"

namespace othello::game
{
using namespace othello::intelligence;

class game
{
private:
  board m_board;
  color m_current;

private:
  [[nodiscard]] constexpr
  static bool
  m_is_valid_input(char const file, char const rank)
  noexcept
  {
    return file >= 'a' && file <= 'h'
        && rank >= '1' && rank <= '8';
  }

  constexpr
  void
  m_show_board(unsigned padding = 0) const
  noexcept;

  constexpr
  void
  m_show_current() const
  noexcept;

  constexpr
  void
  m_show_moves() const
  noexcept;

  constexpr
  void
  m_show_score() const
  noexcept;

  template<
      search_kind    Search,
      heuristic_kind Heuristic,
      std::uint8_t   Depth>
  [[nodiscard]] constexpr
  bitboard
  m_best_move() const;

public:
  constexpr
  game()
  noexcept;

  constexpr explicit
  game(board const &, color = color::black)
  noexcept;

  constexpr game(game const &) = default;
  constexpr game(game &&)      = default;

  constexpr
  ~game()
  = default;

  constexpr game &operator=(game const &) = default;
  constexpr game &operator=(game &&)      = default;

  constexpr
  void
  swap(game &)
  noexcept;


  [[nodiscard]] constexpr
  board const &
  get_board() const
  noexcept
  {
    return m_board;
  }

  [[nodiscard]] constexpr
  color
  current() const
  noexcept
  {
    return m_current;
  }

  [[nodiscard]] constexpr
  bool
  ongoing() const
  noexcept;

  [[nodiscard]] constexpr
  bool
  ended() const
  noexcept;

  constexpr
  void
  show() const
  noexcept;

  constexpr
  void
  show_result() const
  noexcept;


  constexpr
  void
  play_turn();

  template<
      search_kind    Search,
      heuristic_kind Heuristic,
      std::uint8_t   Depth>
  constexpr
  void
  automatic_turn();

  template<
      search_kind    Search,
      heuristic_kind Heuristic,
      std::uint8_t   Depth>
  constexpr
  void
  play_vs_ai_turn(color human);


  constexpr
  void
  play_until_end();

  template<
      search_kind    Search,
      heuristic_kind Heuristic,
      std::uint8_t   Depth>
  constexpr
  void
  automatic_until_end();

  template<
      search_kind    BlackSearch,
      heuristic_kind BlackHeuristic,
      std::uint8_t   BlackDepth,
      search_kind    WhiteSearch,
      heuristic_kind WhiteHeuristic,
      std::uint8_t   WhiteDepth>
  constexpr
  void
  automatic_until_end();

  template<
      search_kind    Search,
      heuristic_kind Heuristic,
      std::uint8_t   Depth>
  constexpr
  void
  play_vs_ai_until_end(color human);


  friend constexpr
  void
  swap(game &lhs, game &rhs)
  noexcept
  {
    lhs.swap(rhs);
  }

  [[nodiscard]] friend constexpr
  bool
  operator==(game const &, game const &)
  = default;
};


constexpr
void
game::m_show_board(unsigned const padding) const
noexcept
{
  auto pad = [padding]
  {
    for (unsigned i = 0; i < padding; ++i)
      std::cout << ' ';
  };

  pad();
  std::cout << "  a b c d e f g h" << '\n';

  for (std::size_t rank = 0; rank < 8; ++rank)
  {
    pad();
    std::cout << (rank + 1);

    for (std::size_t file = 0; file < 8; ++file)
    {
      std::size_t const idx = file + rank * 8;

      std::cout << ' ';
      if      (m_board.is<color::black>(idx)) std::cout << 'B';
      else if (m_board.is<color::white>(idx)) std::cout << 'W';
      else                                    std::cout << '_';
    }

    std::cout << '\n';
  }

  std::cout << '\n';
}

constexpr
void
game::m_show_current() const
noexcept
{
  std::cout
      << (m_current == color::black ? "Black" : "White")
      << " to play"
      << '\n';
}

constexpr
void
game::m_show_moves() const
noexcept
{
  std::cout << "Available moves: ";

  bitboard bb = m_board.moves(m_current);

  if (bb == bitboard{0})
  {
    std::cout << "none\n";
    return;
  }

  bool first = true;

  while (bb != bitboard{0})
  {
    bitboard const lsb = bb & (~bb + 1);

    if (!first)
      std::cout << ", ";

    std::cout << to_string(lsb);
    first = false;
    bb ^= lsb;
  }

  std::cout << '\n';
}

constexpr
void
game::m_show_score() const
noexcept
{
  std::cout
      << "Score - Black: " << m_board.count<color::black>()
      << ", White: "       << m_board.count<color::white>()
      << '\n';
}

constexpr
game::game()
noexcept
  : m_board  {},
    m_current{color::black}
{ }

constexpr
game::game(board const &brd, color const clr)
noexcept
  : m_board  {brd},
    m_current{clr}
{ }

constexpr
void
game::swap(game &other)
noexcept
{
  using std::swap;

  swap(m_board  , other.m_board  );
  swap(m_current, other.m_current);
}

constexpr
bool
game::ongoing() const
noexcept
{
  return m_board.moves<color::black>() || m_board.moves<color::white>();
}

constexpr
bool
game::ended() const
noexcept
{
  return !ongoing();
}

constexpr
void
game::show() const
noexcept
{
  m_show_board();
  m_show_current();
  m_show_moves();
}

constexpr
void
game::show_result() const
noexcept
{
  m_show_board();
  m_show_score();

  int const black = m_board.count<color::black>();
  int const white = m_board.count<color::white>();

  if (black > white)
    std::cout << "Black wins\n";
  else if (white > black)
    std::cout << "White wins\n";
  else
    std::cout << "Draw\n";
}

constexpr
void
game::play_turn()
{
  // show();

  bitboard const mvs = m_board.moves(m_current);

  if (mvs == bitboard{0})
  {
    std::cout << "No available move, passing turn\n";
    m_current = opposite(m_current);
    return;
  }

  auto mv = bitboard{0};

  while (!(mvs & mv))
  {
    char file = '\0';
    char rank = '\0';

    std::cout << "Your move: ";
    std::cin >> file >> rank;

    if (!std::cin)
    {
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      std::cout << "Invalid input, please enter a move like d3\n";
      continue;
    }

    file = static_cast<char>(std::tolower(static_cast<unsigned char>(file)));

    if (!m_is_valid_input(file, rank))
    {
      std::cout << "Invalid coordinate, please enter a move between a1 and h8\n";
      continue;
    }

    mv = to_bitboard(file, rank);

    if (!(mvs & mv))
      std::cout << "Illegal move, please choose one of the available moves\n";
  }

  m_board.play(m_current, mv);
  m_current = opposite(m_current);
}

template<
    search_kind    Search,
    heuristic_kind Heuristic,
    std::uint8_t   Depth>
constexpr
bitboard
game::m_best_move() const
{
  bitboard              mvs = m_board.moves(m_current);
  auto                  bmv = bitboard{0};
  evaluation_value_type bv  = -std::numeric_limits<evaluation_value_type>::max();

  while (mvs != bitboard{0})
  {
    bitboard const mv = mvs & (~mvs + 1);

    board brd = m_board;
    brd.play(m_current, mv);

    evaluation_value_type v{0};

    if constexpr (Depth == 0)
      v = intelligence::evaluate<Heuristic>(brd, m_current);
    else
    {
      algorithm::node<Heuristic> const child{brd, opposite(m_current), m_current};
      v = algorithm::search<Search, Heuristic, Depth - 1>(child);
    }

    if (bmv == bitboard{0} || v > bv)
    {
      bv  = v;
      bmv = mv;
    }

    mvs ^= mv;
  }

  return bmv;
}

template<
    search_kind    Search,
    heuristic_kind Heuristic,
    std::uint8_t   Depth>
constexpr
void
game::automatic_turn()
{
  // show();

  bitboard const mvs = m_board.moves(m_current);

  if (mvs == bitboard{0})
  {
    // std::cout << "No available move, passing turn\n";
    m_current = opposite(m_current);
    return;
  }

  bitboard const mv = m_best_move<Search, Heuristic, Depth>();

  // std::cout << "Automatic move: " << to_string(mv) << '\n';

  m_board.play(m_current, mv);
  m_current = opposite(m_current);
}

template<
    search_kind    Search,
    heuristic_kind Heuristic,
    std::uint8_t   Depth>
constexpr
void
game::play_vs_ai_turn(color const human)
{
  if (m_current == human)
  {
    std::cout << "[Human turn]" << '\n';
    play_turn();
  }
  else
  {
    std::cout << "[AI turn]" << '\n';
    automatic_turn<Search, Heuristic, Depth>();
  }
}

constexpr
void
game::play_until_end()
{
  while (ongoing())
  {
    play_turn();
    std::cout << '\n';
  }

  show_result();
}

template<
    search_kind    Search,
    heuristic_kind Heuristic,
    std::uint8_t   Depth>
constexpr
void
game::automatic_until_end()
{
  while (ongoing())
  {
    automatic_turn<Search, Heuristic, Depth>();
    std::cout << '\n';
  }

  show_result();
}

template<
    search_kind    BlackSearch,
    heuristic_kind BlackHeuristic,
    std::uint8_t   BlackDepth,
    search_kind    WhiteSearch,
    heuristic_kind WhiteHeuristic,
    std::uint8_t   WhiteDepth>
constexpr
void
game::automatic_until_end()
{
  while (ongoing())
  {
    if (m_current == color::black)
      automatic_turn<BlackSearch, BlackHeuristic, BlackDepth>();
    else
      automatic_turn<WhiteSearch, WhiteHeuristic, WhiteDepth>();

    std::cout << '\n';
  }

  show_result();
}

template<
    search_kind    Search,
    heuristic_kind Heuristic,
    std::uint8_t   Depth>
constexpr
void
game::play_vs_ai_until_end(color const human)
{
  while (ongoing())
  {
    play_vs_ai_turn<Search, Heuristic, Depth>(human);
    std::cout << '\n';
  }

  show_result();
}

}

#endif // TP_OTHELLO_GAME_HPP