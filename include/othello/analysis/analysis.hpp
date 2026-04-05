#ifndef TP_OTHELLO_ANALYSIS_HPP
#define TP_OTHELLO_ANALYSIS_HPP

#include <algorithm>
#include <array>
#include <chrono>
#include <cctype>
#include <cstdint>
#include <iostream>
#include <limits>
#include "othello/game/game.hpp"

namespace othello::analysis
{
namespace detail
{
using namespace othello::intelligence;

constexpr std::size_t   compared_heuristics_count = 3;
constexpr std::size_t   recorded_games_count      = 4;
constexpr std::uint32_t opening_move_count        = 0;
constexpr std::uint32_t middle_move_count         = 20;
constexpr std::uint32_t endgame_move_count        = 40;

struct heuristic_summary
{
  std::uint32_t games        = 0;
  std::uint32_t wins         = 0;
  std::uint32_t draws        = 0;
  std::uint32_t losses       = 0;
  std::int32_t  total_margin = 0;
  std::int32_t  best_margin  = std::numeric_limits<std::int32_t>::min();
  std::int32_t  worst_margin = std::numeric_limits<std::int32_t>::max();
};

struct starting_state
{
  char const *name;
  board       brd;
  color       current;
};

struct stage_definition
{
  char const    *name;
  starting_state start;
  std::uint32_t  target_occupied;
  bool           until_end;
};

struct recorded_game
{
  char const *name;
  char const *moves;
};

constexpr recorded_game recorded_games[recorded_games_count]
{
  {
    "WTHOR 1977 - Inoue Hiroshi vs Heiberg Thomas (34-30)",
    "f5d6c3f3f4d3c4g6f6e6c5c6d7d8e7g5e3d2g4h3f7b5h5g3b4b6c8b8c7e8f8g8h4h6e2d1c1a3a6a5e1f2g7c2a4h8a2b7f1b3b1b2h7a7a8a1g1g2h2h1"
  },
  {
    "WTHOR 1977 - Ilagan Rey vs Heiberg Thomas (33-31)",
    "f5d6c6f6e6b6f7g6h5f8e7f3d3f4e3c3g5e8a6c5b5c4g3h3c7g4d2c2e2e1f2h4h2h7b3a4a3a2b4f1c1d7d8c8d1b1h6h1g1g2a5g7a1a7a8b2b8b7g8h8"
  },
  {
    "WTHOR 1977 - Jacobs Carol vs Ilagan Rey (40-24)",
    "f5f6e6f4c3d6g5g4e3g6h5f3h4c6c5d3c4b6b5b3b4a6a5a4d7g3a2c2h3c7c1d8f7e7f8d2b8h7h2g7h8g8h6c8d1e2e1f1f2b1b2g2a7b7a3a1e8g1a8h1"
  },
  {
    "WTHOR 1977 - Woch Alan vs Ilagan Rey (37-27)",
    "f5f6e6f4f3d3c3c6d6c5e3c4c7b3b5b4a3a5g4e2a4a2f1c8f7f2d2d7e8d1e1g1e7c2b1h3h5g3g5b6d8f8g6h7g2h1h2c1a7a1b8h4h6a6g8a8h8g7b7b2"
  }
};

[[nodiscard]] constexpr
std::size_t
heuristic_index(heuristic_kind const h)
noexcept
{
  switch (h)
  {
  case heuristic_kind::position: return 0;
  case heuristic_kind::count   : return 1;
  case heuristic_kind::mobility: return 2;
  case heuristic_kind::mixed   : return compared_heuristics_count;
  }

  return compared_heuristics_count;
}

[[nodiscard]] constexpr
char const *
heuristic_name(heuristic_kind const h)
noexcept
{
  switch (h)
  {
  case heuristic_kind::position: return "position";
  case heuristic_kind::count   : return "count";
  case heuristic_kind::mobility: return "mobility";
  case heuristic_kind::mixed   : return "mixed";
  }

  return "unknown";
}

[[nodiscard]] constexpr
char const *
search_name(search_kind const s)
noexcept
{
  switch (s)
  {
  case search_kind::minimax   : return "minimax";
  case search_kind::alpha_beta: return "alpha-beta";
  case search_kind::negamax   : return "negamax";
  }

  return "unknown";
}

[[nodiscard]] constexpr
std::int32_t
final_margin(board const &brd)
noexcept
{
  return brd.count<color::black>() - brd.count<color::white>();
}

[[nodiscard]] constexpr
std::uint32_t
occupied_count(board const &brd)
noexcept
{
  return static_cast<std::uint32_t>(brd.count<board::view::occupied>());
}

[[nodiscard]] constexpr
starting_state
make_state_from_recorded_game(
    recorded_game const &game,
    char const          *state_name,
    std::uint32_t        move_count)
noexcept
{
  board brd{};
  color current = color::black;

  for (std::uint32_t i = 0; i < move_count; ++i)
  {
    std::size_t const offset = static_cast<std::size_t>(i) * 2;

    if (game.moves[offset] == '\0' || game.moves[offset + 1] == '\0')
      break;
    if (!brd.moves(current) && brd.moves(opposite(current)))
      current = opposite(current);

    brd.play(
        current,
        to_bitboard(
            static_cast<char>(std::tolower(static_cast<unsigned char>(game.moves[offset]))),
            game.moves[offset + 1]));
    current = opposite(current);
  }

  return starting_state{state_name, brd, current};
}

[[nodiscard]] constexpr
std::array<starting_state, recorded_games_count>
make_stage_states(char const *const stage_name, std::uint32_t const move_count)
noexcept
{
  return std::array<starting_state, recorded_games_count>{
      make_state_from_recorded_game(recorded_games[0], stage_name, move_count),
      make_state_from_recorded_game(recorded_games[1], stage_name, move_count),
      make_state_from_recorded_game(recorded_games[2], stage_name, move_count),
      make_state_from_recorded_game(recorded_games[3], stage_name, move_count)};
}

constexpr
void
update_summary(heuristic_summary &summary, std::int32_t const margin)
noexcept
{
  ++summary.games;

  if (margin > 0)
    ++summary.wins;
  else if (margin < 0)
    ++summary.losses;
  else
    ++summary.draws;

  summary.total_margin += margin;
  summary.best_margin   = std::max(summary.best_margin , margin);
  summary.worst_margin  = std::min(summary.worst_margin, margin);
}

constexpr
void
print_summary_line(
    heuristic_kind           const heuristic,
    heuristic_summary const &summary)
{
  std::cout << "  - " << heuristic_name(heuristic)
            << ": games ="       << summary.games
            << ", wins  ="       << summary.wins
            << ", draws ="       << summary.draws
            << ", losses="       << summary.losses
            << ", total margin=" << summary.total_margin;

  if (summary.games != 0)
    std::cout << ", average margin="
              << (static_cast<double>(summary.total_margin) / static_cast<double>(summary.games))
              << ", best =" << summary.best_margin
              << ", worst=" << summary.worst_margin;

  std::cout << '\n';
}

template<
    search_kind    Search,
    heuristic_kind BlackHeuristic,
    std::uint8_t   BlackDepth,
    heuristic_kind WhiteHeuristic,
    std::uint8_t   WhiteDepth>
[[nodiscard]] constexpr
std::int32_t
play_stage(
    board const       &brd,
    color const        current,
    std::uint32_t const target_occupied,
    bool const         until_end)
{
  game::game g{brd, current};
  std::int32_t const initial_margin = final_margin(brd);

  while (g.ongoing())
  {
    if (!until_end && occupied_count(g.get_board()) >= target_occupied)
      break;

    if (g.current() == color::black)
      g.automatic_turn<Search, BlackHeuristic, BlackDepth>();
    else
      g.automatic_turn<Search, WhiteHeuristic, WhiteDepth>();
  }

  return final_margin(g.get_board()) - initial_margin;
}

template<typename Function>
constexpr
void
for_each_compared_heuristic(Function &&f)
{
  f.template operator()<heuristic_kind::position>();
  f.template operator()<heuristic_kind::count>();
  f.template operator()<heuristic_kind::mobility>();
}

template<
    search_kind    SearchKind,
    std::uint8_t   Depth,
    heuristic_kind H1,
    heuristic_kind H2>
constexpr
void
analyze_pair(
    stage_definition const                                   &stage,
    std::array<heuristic_summary, compared_heuristics_count> &stage_summaries,
    std::array<heuristic_summary, compared_heuristics_count> &overall_summaries)
{
  if constexpr (static_cast<std::size_t>(H1) < static_cast<std::size_t>(H2))
  {
    std::int32_t const delta_black_h1
    = play_stage<SearchKind, H1, Depth, H2, Depth>(
        stage.start.brd,
        stage.start.current,
        stage.target_occupied,
        stage.until_end);

    std::int32_t const delta_black_h2
    = play_stage<SearchKind, H2, Depth, H1, Depth>(
        stage.start.brd,
        stage.start.current,
        stage.target_occupied,
        stage.until_end);

    std::int32_t const h1_as_black =  delta_black_h1;
    std::int32_t const h2_as_black =  delta_black_h2;
    std::int32_t const h1_as_white = -delta_black_h2;
    std::int32_t const h2_as_white = -delta_black_h1;
    std::int32_t const h1_total    = h1_as_black + h1_as_white;
    std::int32_t const h2_total    = h2_as_black + h2_as_white;

    update_summary(stage_summaries[heuristic_index(H1)], h1_as_black);
    update_summary(stage_summaries[heuristic_index(H1)], h1_as_white);
    update_summary(stage_summaries[heuristic_index(H2)], h2_as_black);
    update_summary(stage_summaries[heuristic_index(H2)], h2_as_white);

    update_summary(overall_summaries[heuristic_index(H1)], h1_as_black);
    update_summary(overall_summaries[heuristic_index(H1)], h1_as_white);
    update_summary(overall_summaries[heuristic_index(H2)], h2_as_black);
    update_summary(overall_summaries[heuristic_index(H2)], h2_as_white);

    std::cout << "  * " << heuristic_name(H1) << " vs " << heuristic_name(H2) << '\n';
    std::cout << "      - " << heuristic_name(H1) << " as black : " << h1_as_black << '\n';
    std::cout << "      - " << heuristic_name(H2) << " as black : " << h2_as_black << '\n';
    std::cout << "      - cumulative advantage for " << heuristic_name(H1)
              << " : " << h1_total << '\n';
    std::cout << "      - cumulative advantage for " << heuristic_name(H2)
              << " : " << h2_total << "\n\n";
  }
}

} // namespace detail

template<
    intelligence::heuristic_kind Heuristic,
    std::uint8_t                 Depth>
constexpr
void
compare_algorithms()
{
  using clock
  = std::chrono::steady_clock;


  game::game    g;
  std::uint16_t move = 1;

  std::chrono::microseconds tt_minimax    = std::chrono::microseconds::zero();
  std::chrono::microseconds tt_alpha_beta = std::chrono::microseconds::zero();
  std::chrono::microseconds tt_negamax    = std::chrono::microseconds::zero();

  std::chrono::microseconds min_minimax    = std::chrono::microseconds::max();
  std::chrono::microseconds min_alpha_beta = std::chrono::microseconds::max();
  std::chrono::microseconds min_negamax    = std::chrono::microseconds::max();

  std::chrono::microseconds max_minimax    = std::chrono::microseconds::min();
  std::chrono::microseconds max_alpha_beta = std::chrono::microseconds::min();
  std::chrono::microseconds max_negamax    = std::chrono::microseconds::min();

  while (g.ongoing())
  {
    std::chrono::time_point<clock> start;
    std::chrono::time_point<clock> finish;
    std::chrono::microseconds      duration;

    game::game g_minimax    = g;
    game::game g_alpha_beta = g;
    game::game g_negamax    = g;



    // start  = clock::now();
    // g_minimax.automatic_turn<intelligence::search_kind::minimax, Heuristic, Depth>();
    // finish = clock::now();
    //
    // duration    = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
    // tt_minimax += duration;
    // min_minimax = std::min(min_minimax, duration);
    // max_minimax = std::max(max_minimax, duration);
    // std::cout
    //     << "move " << move
    //     << ": time spent (minimax):    " << duration
    //     << "\n";
    //
    // start  = clock::now();
    // g_alpha_beta.automatic_turn<intelligence::search_kind::alpha_beta, Heuristic, Depth>();
    // finish = clock::now();
    //
    // duration       = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
    // tt_alpha_beta += duration;
    // min_alpha_beta = std::min(min_alpha_beta, duration);
    // max_alpha_beta = std::max(max_alpha_beta, duration);
    // std::cout
    //     << "move " << move
    //     << ": time spent (alpha-beta): " << duration
    //     << "\n";

    start  = clock::now();
    g_negamax.automatic_turn<intelligence::search_kind::negamax, Heuristic, Depth>();
    finish = clock::now();

    duration    = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
    tt_negamax += duration;
    min_negamax = std::min(min_negamax, duration);
    max_negamax = std::max(max_negamax, duration);
    std::cout
        << "move " << move
        << ": time spent (negamax):    " << duration
        << "\n";

    g.automatic_turn<intelligence::search_kind::negamax, Heuristic, Depth>();
    ++move;
    std::cout << "\n";
  }

  // std::cout << "total   time spent per move (minimax):    " << tt_minimax    << "\n";
  // std::cout << "total   time spent per move (alpha-beta): " << tt_alpha_beta << "\n";
  std::cout << "total   time spent per move (negamax):    " << tt_negamax    << "\n";

  // std::cout << "minimum time spent per move (minimax):    " << min_minimax    << "\n";
  // std::cout << "minimum time spent per move (alpha-beta): " << min_alpha_beta << "\n";
  std::cout << "minimum time spent per move (negamax):    " << min_negamax    << "\n";

  // std::cout << "maximum time spent per move (minimax):    " << max_minimax    << "\n";
  // std::cout << "maximum time spent per move (alpha-beta): " << max_alpha_beta << "\n";
  std::cout << "maximum time spent per move (negamax):    " << max_negamax    << "\n";

  std::cout << "\n";
}

template<
    intelligence::search_kind SearchKind,
    std::uint8_t              Depth>
constexpr
void
compare_heuristics()
{
  using namespace detail;

  auto const opening_states = make_stage_states("opening reference state"    , opening_move_count);
  auto const middle_states  = make_stage_states("middle-game reference state", middle_move_count);
  auto const endgame_states = make_stage_states("endgame reference state"    , endgame_move_count);

  std::array<heuristic_summary, compared_heuristics_count> overall_summaries;

  std::cout
      << "=== Heuristics comparison ===\n"
      << "search algorithm: " << search_name(SearchKind) << '\n'
      << "depth:            " << static_cast<unsigned>(Depth) << '\n'
      << "compared heuristics: position, count, mobility\n"
      << "excluded heuristic: mixed\n"
      << "reference games: " << recorded_games_count << " WTHOR 1977 tournament games\n"
      << "opening states extracted after about " << opening_move_count << " played moves\n"
      << "middle  states extracted after about " << middle_move_count  << " played moves\n"
      << "endgame states extracted after about " << endgame_move_count << " played moves\n\n";

  auto analyze_stage_collection
  = [&](char const * const stage_name, auto const &states, std::uint32_t const target_occupied, bool const until_end)
  {
    std::array<heuristic_summary, compared_heuristics_count> stage_summaries{};

    for (starting_state const &state : states)
    {
      stage_definition const stage{stage_name, state, target_occupied, until_end};

      std::cout
          << "--- Stage: " << stage.name << " ---\n"
          << "reference game:    " << stage.start.name << '\n'
          << "current player:    " << (stage.start.current == color::black ? "Black" : "White") << '\n'
          << "occupied squares:  " << occupied_count(stage.start.brd) << '\n';

      if (stage.until_end)
        std::cout << "target:            end of game\n\n";
      else
        std::cout << "target occupied:   " << stage.target_occupied << "\n\n";

      auto outer = [&]<heuristic_kind H1>()
      {
        auto inner = [&]<heuristic_kind H2>()
        {
          analyze_pair<SearchKind, Depth, H1, H2>(stage, stage_summaries, overall_summaries);
        };

        for_each_compared_heuristic(inner);
      };

      for_each_compared_heuristic(outer);
    }

    std::cout << "  Summary for stage " << stage_name << ":\n";

    auto print_stage_summary = [&]<heuristic_kind H>()
    {
      print_summary_line(H, stage_summaries[heuristic_index(H)]);
    };

    for_each_compared_heuristic(print_stage_summary);
    std::cout << '\n';
  };

  analyze_stage_collection("opening", opening_states, 4 + middle_move_count , false);
  analyze_stage_collection("middle" , middle_states , 4 + endgame_move_count, false);
  analyze_stage_collection("endgame", endgame_states, 64, true );

  std::cout << "=== Overall summary ===\n";

  auto print_overall_summary = [&]<heuristic_kind H>()
  {
    print_summary_line(H, overall_summaries[heuristic_index(H)]);
  };

  for_each_compared_heuristic(print_overall_summary);
  std::cout << '\n';
}

} // namespace othello::analysis

#endif // TP_OTHELLO_ANALYSIS_HPP
