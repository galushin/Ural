#include <ural/sequence/all.hpp>
#include <ural/algorithm.hpp>

#include <map>
#include <string>
#include <ctime>
#include <random>
#include <iostream>

using namespace std;

int main(int argc, char const * argv[])
{
    typedef std::mt19937 Random_engine;
    Random_engine rnd(std::time(0));

    std::map<std::string, size_t> counter;

    std::string s("abcd");

    auto iterations = 10'000'000;
    if (argc > 1) iterations = std::atoi(argv[1]);

    for(int n = iterations; n > 0; -- n)
    {
        ural::shuffle(s, rnd);
        counter[s] += 1;
    }

    auto const p_expected = double(iterations) / counter.size();

    for(auto const & p : counter)
    {
        std::cout << p.first << "\t" << p.second / p_expected << "\n";
    }

    auto mm = ural::minmax_element(counter | ural::experimental::map_values);

    std::cout << "\n"
              << mm[ural::_1].front() / p_expected << "\n"
              << mm[ural::_2].front() / p_expected << "\n";

    return 0;
}
