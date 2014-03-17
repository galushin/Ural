#include "rnd.hpp"

#include <ctime>

Random_engine & random_engine()
{
    static Random_engine rnd(std::time(0));
    return rnd;
}
