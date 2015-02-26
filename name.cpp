#include "util.h"

#include <random>
#include <functional>
#include <string>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <limits.h>

template <typename Random>
auto randomString(std::string::size_type max_size, Random& r) ->
  std::string {

  const static char printable[] = 
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.";

  const static auto randomChar = [&r] { 

    return *choose(std::begin(printable), std::end(printable), r); 
  };

  if (max_size == 0) return "";
  if (max_size == 1) return {randomChar()};

  std::uniform_int_distribution<std::string::size_type> sizeDist(0, max_size);

  std::string str(sizeDist(r), '\0');

  std::generate(std::begin(str), std::end(str), randomChar);

  return str;
}

template <typename Random>
auto randomFilename(Random& r) ->
  std::string {

#ifdef NAME_MAX
  return randomFilename(NAME_MAX, r);
#else
  // 20 is totally arbitrary.
  return randomFilename(20, r);
#endif
}

template <typename Random>
auto randomFilename(std::string::size_type maxSize, Random& r) ->
  std::string {

  // This is basically just for fun... although it does make the grep-rm
  // strategy much harder to enact.
  const static auto exts = {"", ".txt", ".mp3", ".cpp", ".a", ".out"};

  const std::string ext = *choose(std::begin(exts), std::end(exts), r);

  return randomString(maxSize - ext.length(), r) + ext;
}
