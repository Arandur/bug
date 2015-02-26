#pragma once

#include <vector>
#include <string>

/**
 * Returns those subdirectories of path that are able to be written to, read
 * from, and executed in.
 *
 * If path does not refer to a directory, an empty vector is returned.
 */
auto getSubdirs(const std::string& path) ->
  std::vector<std::string>;

/**
 * Returns true if the directory or file at the given path is readable,
 * writable, and executable.
 */
bool isRWX(const std::string& path);

/**
 * Returns the home directory of the current user.
 */
auto getHomeDir() ->
  std::string;
