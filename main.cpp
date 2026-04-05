#include "othello/analysis/analysis.hpp"

int main()
{
  using namespace othello;
  using namespace othello::intelligence;
  using namespace othello::analysis;


  // // TEST: algorithm comparison
  // constexpr std::uint8_t depth = 5;
  //
  // othello::analysis::compare_algorithms<heuristic_kind::count   , depth>();
  // othello::analysis::compare_algorithms<heuristic_kind::mobility, depth>();
  // othello::analysis::compare_algorithms<heuristic_kind::position, depth>();
  // othello::analysis::compare_algorithms<heuristic_kind::mixed   , depth>();

  // // TEST: heuristic comparison
  // constexpr auto         search = search_kind::negamax;
  // constexpr std::uint8_t depth  = 5;
  //
  // othello::analysis::compare_heuristics<search, depth>();
}
