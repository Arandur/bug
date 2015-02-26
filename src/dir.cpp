#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <pwd.h>
#include <stdlib.h>

#include <string>
#include <vector>

bool isRWX(const struct stat& info) {

  const auto dirOwner  = info.st_uid;
  const auto progOwner = getuid();

  if (dirOwner == progOwner) {

    if( S_IRUSR & info.st_mode and
        S_IWUSR & info.st_mode and
        S_IXUSR & info.st_mode)

      return true;
  }

  const auto dirGroup  = info.st_gid;
  const auto progGroup = getgid();

  if (dirGroup == progGroup) {

    if (S_IRGRP & info.st_mode and
        S_IWGRP & info.st_mode and
        S_IXGRP & info.st_mode)

      return true;
  }

  return  S_IROTH & info.st_mode and
          S_IWOTH & info.st_mode and
          S_IXOTH & info.st_mode;
}

bool isRWX(const std::string& path) {

  struct stat info;
  stat(path.c_str(), &info);

  return isRWX(info);
}

auto getSubdirs(const std::string& path) ->
  std::vector<std::string> {

  std::vector<std::string> subdirectories;

  DIR* startdir = opendir(path.c_str());

  if (startdir == nullptr) return {};

  struct dirent* dirent;
  const char* currpath;
  struct stat info;

  while ((dirent = readdir(startdir)) != nullptr) {

    currpath = dirent->d_name;

    if (strcmp(currpath, ".")  == 0 or
        strcmp(currpath, "..") == 0) continue;

    stat(currpath, &info);

    if (!S_ISDIR(info.st_mode)) continue;

    if (!isRWX(info)) continue;

    subdirectories.emplace_back(currpath);
  }

  return subdirectories;
}

auto getHomeDir() ->
  std::string {

  const char* home = getenv("HOME");

  if (home) return home;

  struct passwd* pw = getpwuid(getuid());

  return pw->pw_dir;
}
