#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
#include <algorithm>
#include <functional>
#include <iterator>
#include <utility>
#include <cstdlib>

const char myString[] = "foobar";

const char moveScript[] = "#! python

from sys import argv
from os import remove

binFile = '''%s'''

if __name__ == '__main__':
  toPath = argv[1]

  with open(toPath, 'wb') as toFile:
    toPath.write(binFile)

  # Remove self
  os.remove(argv[0])
";

std::string randomString(size_t size) {

  std::string newStr;
  newStr.reserve(size);

  std::default_random_engine generator(time(0));
  std::uniform_int_distribution<char> distribution(0, 0xFF);

  std::generate_n(std::back_inserter(newStr), size, 
                  std::bind(distribution, generator));

  return newStr;
}

std::string pyEscape(const std::string& binStr) {

  char escape[] = "\\xXX";
  std::string outStr;
  outStr.reserve(binStr.size() * sizeof(escape));

  for (const char& c : binStr) {
    
  }

  std::transform( std::begin(binStr), std::end(binStr),
                  std::back_inserter(outStr),
                  [escape = escape] (char c) {

                    snprintf(escape, sizeof(escape), "\\x%hhx", c);


                  })
}

int main(int argc, char* argv[]) {

  // Read in own binary
  std::ifstream inFile(argv[0], std::ios::in | std::ios::binary);
  std::stringstream ss;
  ss << inFile.rdbuf();
  std::string bin = ss.str();

  // Find string
  auto pos = bin.find(myString);

  std::cout << "Found string at position: " << pos << std::endl;

  std::string newStr = randomString(sizeof(myString));

  // Replace string
  bin.replace(pos, newStr.length(), newStr);

  std::cout << "Replaced string with \"" << newStr << "\"" << std::endl;

  /**
   * We can't write directly to the running binary, so we need a workaround...
   * we write out a script that will do the writing for us, and then execve to
   * that script.
   */
  int script_fd;
  const char* script_name;

  /**
   * We need the filename for execve(), so we're using the unsafe tmpnam()
   * despite all warnings and making it as safe as possible.
   */
  do {

    script_name = tmpnam();
    script_fd = open(script_name, O_CREAT | O_EXCL);
  } while (script_fd == -1);

  write(script_fd, moveScript, sizeof(moveScript));
  close(script_fd);

  execve(script_name, {script_name, argv[0]}, nullptr);
}
