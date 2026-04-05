#ifndef TP_OTHELLO_INTELLIGENCE_ALGORITHM_HPP
#define TP_OTHELLO_INTELLIGENCE_ALGORITHM_HPP

#include <algorithm>
#include <cstdint>
#include <limits>
#include <vector>

#include "othello/board.hpp"
#include "othello/color.hpp"
#include "othello/utility.hpp"
#include "evaluation.hpp"

namespace othello::intelligence
{
enum class search_kind
  : std::uint8_t
{
  minimax    = 0,
  alpha_beta = 1,
  negamax    = 2
};

struct algorithm
{
  template<heuristic_kind Heuristic>
  class node
  {
  public:
    static constexpr
    heuristic_kind
    heuristic
    = Heuristic;

  private:
    board m_board;
    color m_color;
    color m_max;

  public:
    constexpr node(node const &) = default;
    constexpr node(node &&)      = default;

    constexpr
    node(board, color, color)
    noexcept;

    constexpr
    ~node()
    = default;

    constexpr node &operator=(node const &) = default;
    constexpr node &operator=(node &&)      = default;

    constexpr
    void
    swap(node &)
    noexcept;


    [[nodiscard]] constexpr
    bool
    min() const
    noexcept;

    [[nodiscard]] constexpr
    bool
    max() const
    noexcept;

    [[nodiscard]] constexpr
    bool
    terminal() const
    noexcept;

    [[nodiscard]] constexpr
    evaluation_value_type
    evaluate() const
    noexcept;

    [[nodiscard]] constexpr
    evaluation_value_type
    evaluate_as_current() const
    noexcept;

    [[nodiscard]] constexpr
    evaluation_value_type
    evaluate_as_opposite() const
    noexcept;

    constexpr
    std::vector<node> &
    extend(std::vector<node> &) const;


    friend constexpr
    void
    swap(node &lhs, node &rhs)
    noexcept
    {
      lhs.swap(rhs);
    }

    [[nodiscard]] friend constexpr
    bool
    operator==(node const &, node const &)
    = default;
  };

private:
  template<
      heuristic_kind Heuristic,
      std::uint8_t   Depth>
  static constexpr
  evaluation_value_type
  m_max(node<Heuristic> const &);

  template<
      heuristic_kind Heuristic,
      std::uint8_t   Depth>
  static constexpr
  evaluation_value_type
  m_min(node<Heuristic> const &);

  template<
      heuristic_kind Heuristic,
      std::uint8_t   Depth>
  static constexpr
  evaluation_value_type
  m_negamax(
      node<Heuristic> const &,
      evaluation_value_type,
      evaluation_value_type);

public:
  template<
      heuristic_kind Heuristic,
      std::uint8_t   Depth>
  static constexpr
  evaluation_value_type
  minimax(node<Heuristic> const &);

  template<
      heuristic_kind Heuristic,
      std::uint8_t   Depth>
  static constexpr
  evaluation_value_type
  alpha_beta(
      node<Heuristic> const &,
      evaluation_value_type = -std::numeric_limits<evaluation_value_type>::max(),
      evaluation_value_type =  std::numeric_limits<evaluation_value_type>::max());

  template<
      heuristic_kind Heuristic,
      std::uint8_t   Depth>
  static constexpr
  evaluation_value_type
  negamax(
      node<Heuristic> const &,
      evaluation_value_type = -std::numeric_limits<evaluation_value_type>::max(),
      evaluation_value_type =  std::numeric_limits<evaluation_value_type>::max());

  template<
      search_kind    Search,
      heuristic_kind Heuristic,
      std::uint8_t   Depth>
  static constexpr
  evaluation_value_type
  search(
      node<Heuristic> const &,
      evaluation_value_type = -std::numeric_limits<evaluation_value_type>::max(),
      evaluation_value_type =  std::numeric_limits<evaluation_value_type>::max());

  template<
      heuristic_kind Heuristic,
      std::uint8_t   Depth>
  static constexpr
  evaluation_value_type
  minimax(board, color);

  template<
      heuristic_kind Heuristic,
      std::uint8_t   Depth>
  static constexpr
  evaluation_value_type
  alpha_beta(
      board,
      color,
      evaluation_value_type = -std::numeric_limits<evaluation_value_type>::max(),
      evaluation_value_type =  std::numeric_limits<evaluation_value_type>::max());

  template<
      heuristic_kind Heuristic,
      std::uint8_t   Depth>
  static constexpr
  evaluation_value_type
  negamax(
      board,
      color,
      evaluation_value_type = -std::numeric_limits<evaluation_value_type>::max(),
      evaluation_value_type =  std::numeric_limits<evaluation_value_type>::max());

  template<
      search_kind    Search,
      heuristic_kind Heuristic,
      std::uint8_t   Depth>
  static constexpr
  evaluation_value_type
  search(
      board,
      color,
      evaluation_value_type = -std::numeric_limits<evaluation_value_type>::max(),
      evaluation_value_type =  std::numeric_limits<evaluation_value_type>::max());
};


template<heuristic_kind Heuristic>
constexpr
algorithm::node<Heuristic>::node(
    board const brd,
    color const clr,
    color const max_player)
noexcept
  : m_board{brd},
    m_color{clr},
    m_max  {max_player}
{ }

template<heuristic_kind Heuristic>
constexpr
void
algorithm::node<Heuristic>::swap(node &other)
noexcept
{
  using std::swap;

  swap(m_board, other.m_board);
  swap(m_color, other.m_color);
  swap(m_max  , other.m_max  );
}

template<heuristic_kind Heuristic>
constexpr
bool
algorithm::node<Heuristic>::min() const
noexcept
{
  return m_color != m_max;
}

template<heuristic_kind Heuristic>
constexpr
bool
algorithm::node<Heuristic>::max() const
noexcept
{
  return m_color == m_max;
}

template<heuristic_kind Heuristic>
constexpr
bool
algorithm::node<Heuristic>::terminal() const
noexcept
{
  return !m_board.moves<color::black>()
      && !m_board.moves<color::white>();
}

template<heuristic_kind Heuristic>
constexpr
evaluation_value_type
algorithm::node<Heuristic>::evaluate() const
noexcept
{
  return intelligence::evaluate<Heuristic>(m_board, m_max);
}

template<heuristic_kind Heuristic>
constexpr
evaluation_value_type
algorithm::node<Heuristic>::evaluate_as_current() const
noexcept
{
  return intelligence::evaluate<Heuristic>(m_board, m_color);
}

template<heuristic_kind Heuristic>
constexpr
evaluation_value_type
algorithm::node<Heuristic>::evaluate_as_opposite() const
noexcept
{
  return intelligence::evaluate<Heuristic>(m_board, opposite(m_color));
}

template<heuristic_kind Heuristic>
constexpr
std::vector<typename algorithm::node<Heuristic>> &
algorithm::node<Heuristic>::extend(std::vector<node> &out) const
{
  color const opp = opposite(m_color);
  bitboard    mvs = m_board.moves(m_color);

  if (mvs == bitboard{0})
  {
    if (!terminal())
      out.emplace_back(m_board, opp, m_max);

    return out;
  }

  while (mvs != bitboard{0})
  {
    bitboard const mv = mvs & (~mvs + 1);
    board          brd{m_board};

    brd.play(m_color, mv);
    out.emplace_back(brd, opp, m_max);

    mvs ^= mv;
  }

  return out;
}

template<
    heuristic_kind Heuristic,
    std::uint8_t   Depth>
constexpr
evaluation_value_type
algorithm::m_max(node<Heuristic> const &n)
{
  if constexpr (Depth == 0)
    return n.evaluate();
  else
  {
    if (n.terminal())
      return n.evaluate();

    evaluation_value_type        v = std::numeric_limits<evaluation_value_type>::lowest();
    std::vector<node<Heuristic>> children;

    n.extend(children);

    std::ranges::sort(
        children,
        [](node<Heuristic> const &lhs, node<Heuristic> const &rhs)
        {
          return lhs.evaluate() > rhs.evaluate();
        });

    for (auto const &child : children)
      v = std::max(v, m_min<Heuristic, Depth - 1>(child));

    return v;
  }
}

template<
    heuristic_kind Heuristic,
    std::uint8_t   Depth>
constexpr
evaluation_value_type
algorithm::m_min(node<Heuristic> const &n)
{
  if constexpr (Depth == 0)
    return n.evaluate();
  else
  {
    if (n.terminal())
      return n.evaluate();

    evaluation_value_type        v = std::numeric_limits<evaluation_value_type>::max();
    std::vector<node<Heuristic>> children;

    n.extend(children);

    std::ranges::sort(
        children,
        [](node<Heuristic> const &lhs, node<Heuristic> const &rhs)
        {
          return lhs.evaluate() < rhs.evaluate();
        });

    for (auto const &child : children)
      v = std::min(v, m_max<Heuristic, Depth - 1>(child));

    return v;
  }
}

template<
    heuristic_kind Heuristic,
    std::uint8_t   Depth>
constexpr
evaluation_value_type
algorithm::m_negamax(
    node<Heuristic> const &n,
    evaluation_value_type  a,
    evaluation_value_type  b)
{
  if constexpr (Depth == 0)
    return n.evaluate_as_current();
  else
  {
    if (n.terminal())
      return n.evaluate_as_current();

    evaluation_value_type        v = std::numeric_limits<evaluation_value_type>::lowest();
    std::vector<node<Heuristic>> children;

    n.extend(children);

    std::ranges::sort(
        children,
        [](node<Heuristic> const &lhs, node<Heuristic> const &rhs)
        {
          return lhs.evaluate_as_current() < rhs.evaluate_as_current();
        });

    for (auto const &child : children)
    {
      evaluation_value_type const s
      = -m_negamax<Heuristic, Depth - 1>(child, -b, -a);

      v = std::max(v, s);
      a = std::max(a, s);

      if (a >= b)
        break;
    }

    return v;
  }
}

template<
    heuristic_kind Heuristic,
    std::uint8_t   Depth>
constexpr
evaluation_value_type
algorithm::minimax(node<Heuristic> const &n)
{
  return n.max()
       ? m_max<Heuristic, Depth>(n)
       : m_min<Heuristic, Depth>(n);
}

template<
    heuristic_kind Heuristic,
    std::uint8_t   Depth>
constexpr
evaluation_value_type
algorithm::alpha_beta(
    node<Heuristic> const &n,
    evaluation_value_type  a,
    evaluation_value_type  b)
{
  if constexpr (Depth == 0)
    return n.evaluate();
  else
  {
    if (n.terminal())
      return n.evaluate();

    std::vector<node<Heuristic>> children;
    n.extend(children);

    if (n.max())
    {
      std::ranges::sort(
          children,
          [](node<Heuristic> const &lhs, node<Heuristic> const &rhs)
          {
            return lhs.evaluate() > rhs.evaluate();
          });

      evaluation_value_type v = std::numeric_limits<evaluation_value_type>::lowest();

      for (auto const &child : children)
      {
        v = std::max(v, alpha_beta<Heuristic, Depth - 1>(child, a, b));
        a = std::max(a, v);

        if (a >= b)
          break;
      }

      return v;
    }
    else
    {
      std::ranges::sort(
          children,
          [](node<Heuristic> const &lhs, node<Heuristic> const &rhs)
          {
            return lhs.evaluate() < rhs.evaluate();
          });

      evaluation_value_type v = std::numeric_limits<evaluation_value_type>::max();

      for (auto const &child : children)
      {
        v = std::min(v, alpha_beta<Heuristic, Depth - 1>(child, a, b));
        b = std::min(b, v);

        if (a >= b)
          break;
      }

      return v;
    }
  }
}

template<
    heuristic_kind Heuristic,
    std::uint8_t   Depth>
constexpr
evaluation_value_type
algorithm::negamax(
    node<Heuristic> const      &n,
    evaluation_value_type const a,
    evaluation_value_type const b)
{
  evaluation_value_type const v = m_negamax<Heuristic, Depth>(n, a, b);
  return n.max() ? v : -v;
}

template<
    search_kind    Search,
    heuristic_kind Heuristic,
    std::uint8_t   Depth>
constexpr
evaluation_value_type
algorithm::search(
    node<Heuristic> const      &n,
    evaluation_value_type const a,
    evaluation_value_type const b)
{
  if      constexpr (Search == search_kind::minimax   ) return minimax   <Heuristic, Depth>(n);
  else if constexpr (Search == search_kind::alpha_beta) return alpha_beta<Heuristic, Depth>(n, a, b);
  else                                                  return negamax   <Heuristic, Depth>(n, a, b);
}

template<
    heuristic_kind Heuristic,
    std::uint8_t   Depth>
constexpr
evaluation_value_type
algorithm::minimax(board const brd, color const clr)
{
  return minimax<Heuristic, Depth>(node<Heuristic>(brd, clr, clr));
}

template<
    heuristic_kind Heuristic,
    std::uint8_t   Depth>
constexpr
evaluation_value_type
algorithm::alpha_beta(board const brd,
                      color const clr,
                      evaluation_value_type const a,
                      evaluation_value_type const b)
{
  return alpha_beta<Heuristic, Depth>(node<Heuristic>(brd, clr, clr), a, b);
}

template<
    heuristic_kind Heuristic,
    std::uint8_t   Depth>
constexpr
evaluation_value_type
algorithm::negamax(board const brd,
                   color const clr,
                   evaluation_value_type const a,
                   evaluation_value_type const b)
{
  return negamax<Heuristic, Depth>(node<Heuristic>(brd, clr, clr), a, b);
}

template<
    search_kind    Search,
    heuristic_kind Heuristic,
    std::uint8_t   Depth>
constexpr
evaluation_value_type
algorithm::search(board const brd,
                  color const clr,
                  evaluation_value_type const a,
                  evaluation_value_type const b)
{
  return search<Search, Heuristic, Depth>(node<Heuristic>(brd, clr, clr), a, b);
}


}

#endif // TP_OTHELLO_INTELLIGENCE_ALGORITHM_HPP