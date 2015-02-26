#pragma once

template <typename InputIterator, typename Random>
auto choose(InputIterator begin, InputIterator end, Random& r) ->
  InputIterator;

#include "util.cpp"
