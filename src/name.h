#pragma once

#include <string>

/**
 * Returns a random filename 
 */
template <typename Random>
auto randomFilename(Random& r) ->
  std::string;

/**
 * Returns a random filename that is no longer than maxSize
 */
template <typename Random>
auto randomFilename(std::string::size_type maxSize, Random& r) ->
  std::string;

#include "name.cpp"
