#ifndef Z_URAL_TEST_RND_HPP_INCLUDED
#define Z_URAL_TEST_RND_HPP_INCLUDED

#include <random>

namespace ural_test
{
    typedef std::mt19937 Random_engine;

    Random_engine & random_engine();
}
// namespace ural_test
#endif
// Z_URAL_TEST_RND_HPP_INCLUDED
