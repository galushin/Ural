#include "rnd.hpp"

#include <ctime>

namespace ural_test
{
    Random_engine & random_engine()
    {
        static Random_engine rnd(std::time(0));
        return rnd;
    }
}
// namespace ural_test
