#include <random>
#include <iterator>

template <typename InputIterator, typename Random>
auto choose(InputIterator begin, InputIterator end, Random& r) ->
  InputIterator {

  auto length = std::distance(begin, end);

  if (length == 0) return end;
  if (length == 1) return begin;

  std::uniform_int_distribution<decltype(length)> dist(0, length - 1);

  std::advance(begin, dist(r));

  return begin;
}
