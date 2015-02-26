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

  void jump(int) const;

protected:

  const   std::string prgName;
  mutable std::default_random_engine g;

private:

  const   std::vector<char> prg;

  std::string getNext() const;
};
