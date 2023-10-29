
#include "container.h"


int main() {
  std::vector<int> elems = {1, 2, 3, 4, 5, 1, 2, 3, 19, 1111, 239};

  Container<int> container;
  container.AddAll(elems);
  container.Show();
}
