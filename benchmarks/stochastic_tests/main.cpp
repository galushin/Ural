#include <ural/sequence/all.hpp>
#include <ural/algorithm.hpp>

#include <boost/math/distributions/chi_squared.hpp>

#include <map>
#include <string>
#include <ctime>
#include <random>
#include <iostream>

using namespace std;

template <class IntType>
IntType factorial(IntType n)
{
    auto result = IntType{1};

    for(; n > 1; -- n)
    {
        result *= n;
    }

    return result;
}

int main(int argc, char const * argv[])
{
    typedef std::mt19937 Random_engine;
    Random_engine rnd(std::time(0));

    std::map<std::string, size_t> counter;

    std::string const s0("abcd");
    auto s = s0;

    auto iterations = 10'000;
    if (argc > 1) iterations = std::atoi(argv[1]);

    for(int n = iterations; n > 0; -- n)
    {
        ural::shuffle(s, rnd);
        counter[s] += 1;
    }

    assert(counter.size() == factorial(s0.size()));

    auto const p_expected = 1.0 / counter.size();

    // Вычисляем статистику хи-квадрат
    double z = 0.0;

    for(auto const & p : counter)
    {
        z += ural::square(p.second / double(iterations) - p_expected) / p_expected;
    }

    z *= counter.size();

    // Выводим вероятность такого значения
    boost::math::chi_squared_distribution<double> chi_2(counter.size() - 1);

    auto const p = cdf(chi_2, z);

    cout << p << endl;

    assert(p < 1e-6);

    return 0;
}
