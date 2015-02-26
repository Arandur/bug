#include <string>
#include <vector>
#include <cstdint>
#include <memory>
#include <random>

class Task {

public:

  Task(const std::string&);
  virtual ~Task() = default;

  static std::unique_ptr<Task> newTask(const std::string&);

  virtual int run(int) = 0;

  void jump(const std::string&, int) const;


private:

  const   std::vector<char> prg;
  mutable std::default_random_engine g;

  std::string getNext() const;
};
