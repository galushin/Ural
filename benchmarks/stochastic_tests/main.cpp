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

template <class Random_engine, class Container>
map<Container, size_t>
frequences_of_shuffle(Random_engine & rnd, Container const & s0, size_t iterations)
{
    std::map<std::string, size_t> counter;

    for(int n = iterations; n > 0; -- n)
    {
        auto s = s0;
        ural::shuffle(s, rnd);
        counter[s] += 1;
    }

    return counter;
}

void check_shuffle_uniformity(size_t iterations)
{
    typedef std::mt19937 Random_engine;
    Random_engine rnd(std::time(0));

    std::string const s0("abcd");
    auto const counter = frequences_of_shuffle(rnd, s0, iterations);

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
}

void check_shuffle_independency(size_t iterations)
{
    typedef std::mt19937 Random_engine;
    Random_engine rnd(std::time(0));

    std::string const s1("abcd");
    std::string const s2
        = [s1, &rnd]() { auto s = s1; std::shuffle(s.begin(), s.end(), rnd); return s; }();

    assert(ural::is_permutation(s1, s2));

    auto const counter_1 = frequences_of_shuffle(rnd, s1, iterations);
    auto const counter_2 = frequences_of_shuffle(rnd, s2, iterations);

    assert(counter_1.size() == factorial(s1.size()));
    assert(counter_2.size() == factorial(s1.size()));

    // Вычисляем статистику хи-квадрат
    double z = 0.0;

    for(auto const & p1 : counter_1)
    {
        auto const f1 = double(p1.second);
        auto const f2 = counter_2.at(p1.first);

        z += ural::square(double(f1) / iterations - double(f2) / iterations) / (f1 + f2);
    }

    z *= counter_1.size();
    z *= counter_2.size();

    // Выводим вероятность такого значения
    boost::math::chi_squared_distribution<double> chi_2(counter_1.size() - 1);

    auto const p = cdf(chi_2, z);

    cout << p << endl;

    assert(p < 1e-6);
}

int main(int argc, char const * argv[])
{
    auto iterations = 1'000;
    if (argc > 1) iterations = std::atoi(argv[1]);

    check_shuffle_uniformity(iterations);
    check_shuffle_independency(iterations);

    return 0;
}
