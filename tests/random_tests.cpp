/* Этот файл основан на тестах стандартной библиотеки LLVM, расположенных по
адресу:
llvm.org/svn/llvm-project/libcxx/trunk/test/numerics/rand/rand.dis/rand.dist.samp/rand.dist.samp.discrete/

==============================================================================
LLVM Release License
==============================================================================
University of Illinois/NCSA
Open Source License

Copyright (c) 2003-2013 University of Illinois at Urbana-Champaign.
All rights reserved.

Developed by:

    LLVM Team

    University of Illinois at Urbana-Champaign

    http://llvm.org

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal with
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimers.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimers in the
      documentation and/or other materials provided with the distribution.

    * Neither the names of the LLVM Team, University of Illinois at
      Urbana-Champaign, nor the names of its contributors may be used to
      endorse or promote products derived from this Software without specific
      prior written permission.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS WITH THE
SOFTWARE.

==============================================================================
Copyrights and Licenses for Third Party Software Distributed with LLVM:
==============================================================================
The LLVM software contains code written by third parties.  Such software will
have its own individual LICENSE.TXT file in the directory in which it appears.
This file will describe the copyrights, license, and restrictions which apply
to that code.

The disclaimer of warranty in the University of Illinois Open Source License
applies to all code in the LLVM Distribution, and nothing in any of the
other licenses gives permission to use the names of the LLVM Team or the
University of Illinois to endorse or promote products derived from this
Software.

The following pieces of software have additional or alternate copyrights,
licenses, and/or restrictions:

Program             Directory
-------             ---------
Autoconf            llvm/autoconf
                    llvm/projects/ModuleMaker/autoconf
                    llvm/projects/sample/autoconf
Google Test         llvm/utils/unittest/googletest
OpenBSD regex       llvm/lib/Support/{reg*, COPYRIGHT.regex}
pyyaml tests        llvm/test/YAMLParser/{*.data, LICENSE.TXT}
ARM contributions   llvm/lib/Target/ARM/LICENSE.TXT
md5 contributions   llvm/lib/Support/MD5.cpp llvm/include/llvm/Support/MD5.h
*/

#include "rnd.hpp"

#include <ural/algorithm.hpp>
#include <ural/statistics.hpp>
#include <ural/random.hpp>
#include <ural/math.hpp>

#include <boost/test/unit_test.hpp>
#include <boost/math/distributions/chi_squared.hpp>

#include <sstream>

// Типы
BOOST_AUTO_TEST_CASE(discrete_distribution_types_test)
{
    {
        typedef ural::discrete_distribution<> D;
        typedef D::result_type result_type;
        static_assert((std::is_same<result_type, int>::value), "");
    }
    {
        typedef ural::discrete_distribution<long> D;
        typedef D::result_type result_type;
        static_assert((std::is_same<result_type, long>::value), "");
    }
}

BOOST_AUTO_TEST_CASE(discrete_distribution_param_types_test)
{
    typedef ural::discrete_distribution<> D;
    typedef D::param_type param_type;
    typedef param_type::distribution_type distribution_type;
    static_assert((std::is_same<D, distribution_type>::value), "");
}

// Конструкторы
BOOST_AUTO_TEST_CASE(discrete_distribution_default_ctor_test)
{
    typedef ural::discrete_distribution<> D;
    D d;
    std::vector<double> p = d.probabilities();
    BOOST_CHECK_EQUAL(1U, p.size());
    BOOST_CHECK_EQUAL(1, p[0]);
}

BOOST_AUTO_TEST_CASE(discrete_distribution_copy_ctor)
{
    typedef ural::discrete_distribution<> D;
    std::vector<double> const p = {2, 4, 1, 8};
    D d1(p.begin(), p.end());
    D d2 = d1;
    BOOST_CHECK(d1 == d2);
}

BOOST_AUTO_TEST_CASE(discrete_distribution_param_default_ctor_test)
{
    typedef ural::discrete_distribution<> D;
    typedef D::param_type P;
    P pa = {1};
    std::vector<double> p = pa.probabilities();
    BOOST_CHECK_EQUAL(1U, p.size());
    BOOST_CHECK_EQUAL(1, p[0]);
}

namespace
{
    template <class Vector>
    Vector & normalize_weights(Vector & ws)
    {
        auto w_sum = ural::accumulate(ws, typename Vector::value_type{0});

        ural::for_each(ws, [=](typename Vector::reference x) { x /= w_sum;});

        return ws;
    }
}

BOOST_AUTO_TEST_CASE(discrete_distribution_iterator_ctor_test)
{
    typedef std::vector<double> Probabilities;
    std::vector<Probabilities> p0s =
    {
        {1},
        {10},
        {10, 30},
        {30, 10},
        {30, 0, 10},
        {0, 30, 10},
        {0, 0, 10}
    };

    typedef ural::discrete_distribution<> D;

    for(auto & p0 : p0s)
    {
        D d(p0.begin(), p0.end());
        auto p = d.probabilities();
        normalize_weights(p0);
        BOOST_CHECK_EQUAL_COLLECTIONS(p0.begin(), p0.end(), p.begin(), p.end());
    }
}

BOOST_AUTO_TEST_CASE(discrete_distribution_iter_ctor_random_test)
{
    std::vector<double> ws;

    std::uniform_real_distribution<double> d_w(0.0, 1.0);
    auto & rnd = ural_test::random_engine();
    auto const N = 10;

    for(auto n = N; n > 0; -- n)
    {
        ws.push_back(d_w(rnd));
    }

    ural::discrete_distribution<> d(ws.begin(), ws.end());

    auto p = d.probabilities();
    normalize_weights(ws);

    BOOST_CHECK_EQUAL(p.size(), ws.size());

    for(size_t i = 0; i < p.size(); ++ i)
    {
        BOOST_CHECK_CLOSE(ws[i], p[i], 1e-6);
    }
}

BOOST_AUTO_TEST_CASE(discrete_distribution_ctor_param_test)
{
    typedef ural::discrete_distribution<> D;
    typedef D::param_type P;
    std::vector<double> p0 = {10, 30};
    P pa(p0.begin(), p0.end());
    D d(pa);
    std::vector<double> p = d.probabilities();

    normalize_weights(p0);
    BOOST_CHECK_EQUAL_COLLECTIONS(p0.begin(), p0.end(), p.begin(), p.end());
}

BOOST_AUTO_TEST_CASE(discrete_distribution_func_ctor_test)
{
    auto const fw = +[](double x) { return x+1; };

    typedef ural::discrete_distribution<> D;
    {
        D d(0, 0, 1, fw);
        std::vector<double> p = d.probabilities();
        BOOST_CHECK_EQUAL(1U, p.size());
        BOOST_CHECK_EQUAL(1, p[0]);
    }
    {
        D d(1, 0, 1, fw);
        std::vector<double> p = d.probabilities();
        BOOST_CHECK_EQUAL(1U, p.size());
        BOOST_CHECK_EQUAL(1, p[0]);
    }
    {
        D d(2, 0.5, 1.5, fw);
        std::vector<double> p = d.probabilities();
        BOOST_CHECK_EQUAL(2U, p.size());
        BOOST_CHECK_EQUAL(p[0], .4375);
        BOOST_CHECK_EQUAL(p[1], .5625);
    }
    {
        D d(4, 0, 2, fw);
        std::vector<double> p = d.probabilities();
        BOOST_CHECK_EQUAL(4U, p.size());
        BOOST_CHECK_EQUAL(p[0], .15625);
        BOOST_CHECK_EQUAL(p[1], .21875);
        BOOST_CHECK_EQUAL(p[2], .28125);
    }
}

BOOST_AUTO_TEST_CASE(discrete_distribution_ctor_init_list_test)
{
    typedef ural::discrete_distribution<> D;
    {
        D d = {};
        std::vector<double> p = d.probabilities();
        BOOST_CHECK_EQUAL(p.size(), 1U);
        BOOST_CHECK_EQUAL(p[0], 1);
    }
    {
        D d = {10};
        std::vector<double> p = d.probabilities();
        BOOST_CHECK_EQUAL(p.size(), 1U);
        BOOST_CHECK_EQUAL(p[0], 1);
    }
    {
        D d = {10, 30};
        std::vector<double> p = d.probabilities();
        BOOST_CHECK_EQUAL(p.size(), 2U);
        BOOST_CHECK_EQUAL(p[0], 0.25);
        BOOST_CHECK_EQUAL(p[1], 0.75);
    }
    {
        D d = {30, 10};
        std::vector<double> p = d.probabilities();
        BOOST_CHECK_EQUAL(p.size(), 2U);
        BOOST_CHECK_EQUAL(p[0], 0.75);
        BOOST_CHECK_EQUAL(p[1], 0.25);
    }
    {
        D d = {30, 0, 10};
        std::vector<double> p = d.probabilities();
        BOOST_CHECK_EQUAL(p.size(), 3U);
        BOOST_CHECK_EQUAL(p[0], 0.75);
        BOOST_CHECK_EQUAL(p[1], 0);
        BOOST_CHECK_EQUAL(p[2], 0.25);
    }
    {
        D d = {0, 30, 10};
        std::vector<double> p = d.probabilities();
        BOOST_CHECK_EQUAL(p.size(), 3U);
        BOOST_CHECK_EQUAL(p[0], 0);
        BOOST_CHECK_EQUAL(p[1], 0.75);
        BOOST_CHECK_EQUAL(p[2], 0.25);
    }
    {
        D d = {0, 0, 10};
        std::vector<double> p = d.probabilities();
        BOOST_CHECK_EQUAL(p.size(), 3U);
        BOOST_CHECK_EQUAL(p[0], 0);
        BOOST_CHECK_EQUAL(p[1], 0);
        BOOST_CHECK_EQUAL(p[2], 1);
    }
}

// Свойства
BOOST_AUTO_TEST_CASE(discrete_distribution_min_test)
{
    typedef ural::discrete_distribution<> D;
    double p0[] = {.3, .1, .6};
    D d(p0, p0+3);
    BOOST_CHECK_EQUAL(d.min(), 0);
}

BOOST_AUTO_TEST_CASE(discrete_distribution_max_test)
{
    typedef ural::discrete_distribution<> D;
    {
        double p0[] = {.3, .1, .6};
        D d(p0, p0+3);
        BOOST_CHECK_EQUAL(d.max(), 2);
    }
    {
        double p0[] = {.3, .1, .6, .2};
        D d(p0, p0+4);
        BOOST_CHECK_EQUAL(d.max(), 3);
    }
}

// Присваивание
BOOST_AUTO_TEST_CASE(discrete_distribution_assign_test)
{
    typedef ural::discrete_distribution<> D;
    std::vector<double> const p = {2, 4, 1, 8};
    D d1(p.begin(), p.end());
    D d2;
    BOOST_CHECK(d1 != d2);
    d2 = d1;
    BOOST_CHECK(d1 == d2);
}

// Проверка на равенство
BOOST_AUTO_TEST_CASE(discrete_distribution_equality)
{
    typedef ural::discrete_distribution<> D;
    {
        D d1;
        D d2;
        BOOST_CHECK(d1 == d2);
    }
    {
        double p0[] = {1};
        D d1(p0, p0+1);
        D d2;
        BOOST_CHECK(d1 == d2);
    }
    {
        double p0[] = {10, 30};
        D d1(p0, p0+2);
        D d2;
        BOOST_CHECK(d1 != d2);
    }
}

// Ввод/вывод
BOOST_AUTO_TEST_CASE(discrete_distribution_io_test)
{
    typedef ural::discrete_distribution<> D;
    double p0[] = {.3, .1, .6};
    D d1(p0, p0+3);
    std::ostringstream os;
    os << d1;
    std::istringstream is(os.str());
    D d2;
    is >> d2;
    BOOST_CHECK(d1 == d2);
}

namespace
{
    template <class Distribution>
    void test_discrete_distribution_exact(Distribution d, size_t N)
    {
        typedef std::minstd_rand G;
        G g;

        std::vector<typename Distribution::result_type> u(d.max()+1);

        for (size_t i = 0; i < N; ++i)
        {
            typename Distribution::result_type v = d(g);
            BOOST_CHECK(d.min() <= v && v <= d.max());
            u[v]++;
        }

        std::vector<double> prob = d.probabilities();

        for (auto i : ural::numbers(0, d.max() + 1))
            BOOST_CHECK_EQUAL((double)u[i]/N, prob[i]);
    }

    template <class InputSequence, class Probabilities>
    ural::probability<>
    pearson_test(InputSequence && in, Probabilities const & prob)
    {
        std::vector<typename InputSequence::value_type> u(prob.size());
        size_t N = 0;

        for(auto s = sequence(in); !!s; ++ s)
        {
            auto v = *s;

            assert(0U <= v && v < prob.size());

            ++ u[v];
            ++ N;
        }

        double chi_square = {0.0};

        for(size_t i = 0; i < u.size(); ++i)
        {
            if(prob[i] != 0)
            {
                chi_square += ural::square((double)u[i]/N - prob[i]) / prob[i];
            }
            else
            {
                assert(0 == u[i]);
            }
        }

        chi_square *= N;

        boost::math::chi_squared_distribution<double> d_teor{double(N)-1};

        return ural::probability<>{cdf(d_teor, chi_square)};
    }

    template <class Weights>
    void test_discrete_distribution_approx(Weights && ws, size_t N, double eps)
    {
        typedef ural::discrete_distribution<> D;
        typedef std::minstd_rand G;

        ural::probability<> alpha(eps);

        G g;
        D d(ws.begin(), ws.end());

        std::vector<double> prob = d.probabilities();

        auto gen = std::bind(std::move(d), std::ref(g));
        auto seq = ural::make_generator_sequence(std::move(gen)) | ural::taken(N);

        auto p = pearson_test(std::move(seq), prob);

        std::vector<D::result_type> u(d.max()+1);

        BOOST_CHECK_LE(p, alpha);
    }
}

// Генерация значений
BOOST_AUTO_TEST_CASE(discrete_distribution_eval_test)
{
    typedef ural::discrete_distribution<> D;
    // typedef std::minstd_rand G;

    test_discrete_distribution_exact(D{}, 100);
    test_discrete_distribution_exact(D{.3}, 100);

    auto const N = 100U;
    const double eps = 1e-6;
    typedef std::vector<double> Weights;
    test_discrete_distribution_approx(Weights{.75, .25}, N, eps);

    test_discrete_distribution_exact(D{0, 1}, N);
    test_discrete_distribution_exact(D{1, 0}, N);

    test_discrete_distribution_approx(Weights{.3, .1, .6}, N, eps);
    test_discrete_distribution_approx(Weights{0, 25, 75}, N, eps);
    test_discrete_distribution_approx(Weights{25, 0, 75}, N, eps);
    test_discrete_distribution_approx(Weights{25, 75, 0}, N, eps);
    test_discrete_distribution_exact(D{0, 0, 1}, N);
    test_discrete_distribution_exact(D{0, 1, 0}, N);
    test_discrete_distribution_exact(D{1, 0, 0}, N);
    test_discrete_distribution_approx(Weights{33, 0, 0, 67}, N, eps);
}

BOOST_AUTO_TEST_CASE(discrete_distribution_param_eval_test)
{
    // @todo Унифицировать с тестом d(g)
    typedef ural::discrete_distribution<> D;
    typedef D::param_type P;
    typedef std::minstd_rand G;
    G g;
    D d;
    double p0[] = {.3, .1, .6};
    P p(p0, p0+3);
    const int N = 1000000;
    std::vector<D::result_type> u(3);
    for (int i = 0; i < N; ++i)
    {
        D::result_type v = d(g, p);
        BOOST_CHECK(0 <= v && v <= 2);
        u[v]++;
    }
    std::vector<double> prob = p.probabilities();
    for (int i = 0; i <= 2; ++i)
        BOOST_CHECK(std::abs((double)u[i]/N - prob[i]) / prob[i] < 0.003);
}

// Параметр
BOOST_AUTO_TEST_CASE(discrete_distribution_get_param_test)
{
    typedef ural::discrete_distribution<> D;
    typedef D::param_type P;
    double p0[] = {.3, .1, .6};
    P p(p0, p0+3);
    D d(p);
    BOOST_CHECK(d.param() == p);
}

BOOST_AUTO_TEST_CASE(discrete_distribution_set_param_test)
{
    typedef ural::discrete_distribution<> D;
    typedef D::param_type P;
    std::vector<double> d0 = {.3, .1, .6};
    P p(d0.begin(), d0.end());
    D d;
    d.param(p);
    BOOST_CHECK(d.param() == p);
}

BOOST_AUTO_TEST_CASE(discrete_distribution_param_assign_test)
{
    typedef ural::discrete_distribution<> D;
    typedef D::param_type param_type;
    double d0[] = {.3, .1, .6};
    param_type p0(d0, d0+3);
    param_type p;
    p = p0;
    BOOST_CHECK(p == p0);
}

BOOST_AUTO_TEST_CASE(discrete_distribution_param_copy_test)
{
    typedef ural::discrete_distribution<> D;
    typedef D::param_type param_type;
    double d0[] = {.3, .1, .6};
    param_type p0(d0, d0+3);
    param_type p = p0;
    BOOST_CHECK(p == p0);
}

BOOST_AUTO_TEST_CASE(discrete_distribution_param_ctor_init)
{
    typedef ural::discrete_distribution<> D;

    {
        typedef D::param_type P;
        P pa = {};
        std::vector<double> p = pa.probabilities();
        BOOST_CHECK(p.size() == 1);
        BOOST_CHECK(p[0] == 1);
    }
    {
        typedef D::param_type P;
        P pa = {10};
        std::vector<double> p = pa.probabilities();
        BOOST_CHECK(p.size() == 1);
        BOOST_CHECK(p[0] == 1);
    }
    {
        typedef D::param_type P;
        P pa = {10, 30};
        std::vector<double> p = pa.probabilities();
        BOOST_CHECK(p.size() == 2);
        BOOST_CHECK(p[0] == 0.25);
        BOOST_CHECK(p[1] == 0.75);
    }
    {
        typedef D::param_type P;
        P pa = {30, 10};
        std::vector<double> p = pa.probabilities();
        BOOST_CHECK(p.size() == 2);
        BOOST_CHECK(p[0] == 0.75);
        BOOST_CHECK(p[1] == 0.25);
    }
    {
        typedef D::param_type P;
        P pa = {30, 0, 10};
        std::vector<double> p = pa.probabilities();
        BOOST_CHECK(p.size() == 3);
        BOOST_CHECK(p[0] == 0.75);
        BOOST_CHECK(p[1] == 0);
        BOOST_CHECK(p[2] == 0.25);
    }
    {
        typedef D::param_type P;
        P pa = {0, 30, 10};
        std::vector<double> p = pa.probabilities();
        BOOST_CHECK(p.size() == 3);
        BOOST_CHECK(p[0] == 0);
        BOOST_CHECK(p[1] == 0.75);
        BOOST_CHECK(p[2] == 0.25);
    }
    {
        typedef D::param_type P;
        P pa = {0, 0, 10};
        std::vector<double> p = pa.probabilities();
        BOOST_CHECK(p.size() == 3);
        BOOST_CHECK(p[0] == 0);
        BOOST_CHECK(p[1] == 0);
        BOOST_CHECK(p[2] == 1);
    }
}

BOOST_AUTO_TEST_CASE(discrete_distribution_param_ctor_func)
{
    typedef ural::discrete_distribution<> D;
    auto fw = +[](double x) { return x+1; };

    {
        typedef D::param_type P;
        P pa(0, 0, 1, fw);
        std::vector<double> p = pa.probabilities();
        BOOST_CHECK(p.size() == 1);
        BOOST_CHECK(p[0] == 1);
    }
    {
        typedef D::param_type P;
        P pa(1, 0, 1, fw);
        std::vector<double> p = pa.probabilities();
        BOOST_CHECK(p.size() == 1);
        BOOST_CHECK(p[0] == 1);
    }
    {
        typedef D::param_type P;
        P pa(2, 0.5, 1.5, fw);
        std::vector<double> p = pa.probabilities();
        BOOST_CHECK(p.size() == 2);
        BOOST_CHECK(p[0] == .4375);
        BOOST_CHECK(p[1] == .5625);
    }
    {
        typedef D::param_type P;
        P pa(4, 0, 2, fw);
        std::vector<double> p = pa.probabilities();
        BOOST_CHECK(p.size() == 4);
        BOOST_CHECK(p[0] == .15625);
        BOOST_CHECK(p[1] == .21875);
        BOOST_CHECK(p[2] == .28125);
    }
}

BOOST_AUTO_TEST_CASE(discrete_distribution_param_ctor_iter_test)
{
    typedef ural::discrete_distribution<> D;
    {
        typedef D::param_type P;
        double p0[] = {1};
        P pa(p0, p0);
        std::vector<double> p = pa.probabilities();
        BOOST_CHECK(p.size() == 1);
        BOOST_CHECK(p[0] == 1);
    }
    {
        typedef D::param_type P;
        double p0[] = {10};
        P pa(p0, p0+1);
        std::vector<double> p = pa.probabilities();
        BOOST_CHECK(p.size() == 1);
        BOOST_CHECK(p[0] == 1);
    }
    {
        typedef D::param_type P;
        double p0[] = {10, 30};
        P pa(p0, p0+2);
        std::vector<double> p = pa.probabilities();
        BOOST_CHECK(p.size() == 2);
        BOOST_CHECK(p[0] == 0.25);
        BOOST_CHECK(p[1] == 0.75);
    }
    {
        typedef D::param_type P;
        double p0[] = {30, 10};
        P pa(p0, p0+2);
        std::vector<double> p = pa.probabilities();
        BOOST_CHECK(p.size() == 2);
        BOOST_CHECK(p[0] == 0.75);
        BOOST_CHECK(p[1] == 0.25);
    }
    {
        typedef D::param_type P;
        double p0[] = {30, 0, 10};
        P pa(p0, p0+3);
        std::vector<double> p = pa.probabilities();
        BOOST_CHECK(p.size() == 3);
        BOOST_CHECK(p[0] == 0.75);
        BOOST_CHECK(p[1] == 0);
        BOOST_CHECK(p[2] == 0.25);
    }
    {
        typedef D::param_type P;
        double p0[] = {0, 30, 10};
        P pa(p0, p0+3);
        std::vector<double> p = pa.probabilities();
        BOOST_CHECK(p.size() == 3);
        BOOST_CHECK(p[0] == 0);
        BOOST_CHECK(p[1] == 0.75);
        BOOST_CHECK(p[2] == 0.25);
    }
    {
        typedef D::param_type P;
        double p0[] = {0, 0, 10};
        P pa(p0, p0+3);
        std::vector<double> p = pa.probabilities();
        BOOST_CHECK(p.size() == 3);
        BOOST_CHECK(p[0] == 0);
        BOOST_CHECK(p[1] == 0);
        BOOST_CHECK(p[2] == 1);
    }
}


BOOST_AUTO_TEST_CASE(discrete_distribution_param_eq_test)
{
    typedef ural::discrete_distribution<> D;
    typedef D::param_type param_type;
    std::vector<double> p0 = {30, 10};
    {
        param_type p1(p0.begin(), p0.end());
        param_type p2(p0.begin(), p0.end());
        BOOST_CHECK(p1 == p2);
    }
    {
        param_type p1(p0.begin(), p0.end());
        param_type p2;
        BOOST_CHECK(p1 != p2);
    }
}

// Удовлетворяют ли существующие типы концепции
BOOST_AUTO_TEST_CASE(URNG_concept_test)
{
    using namespace ural::concepts;
    BOOST_CONCEPT_ASSERT((Uniform_random_number_generator<std::mt19937>));
    BOOST_CONCEPT_ASSERT((Uniform_random_number_generator<ural::c_rand_engine>));
    BOOST_CONCEPT_ASSERT((Uniform_random_number_generator<ural::archetypes::URNG_archetype>));
}

BOOST_AUTO_TEST_CASE(discrete_distribution_concept_check)
{
    using namespace ural::concepts;

    typedef ural::discrete_distribution<int> D;

    BOOST_CONCEPT_ASSERT((RandomDistribution<D>));
    BOOST_CONCEPT_ASSERT((RandomDistribution<ural::iid_adaptor<D>>));
    BOOST_CONCEPT_ASSERT((RandomDistribution<ural::multivariate_normal_distribution<>>));
}

BOOST_AUTO_TEST_CASE(multivariate_normal_equality_test)
{
    ural::multivariate_normal_distribution<> const d1{2};

    typename decltype(d1)::result_type mu{d1.dim(), 1};
    typename decltype(d1)::result_type mu_2{d1.dim()+1, 1};

    ural::multivariate_normal_distribution<> const d2{mu};
    ural::multivariate_normal_distribution<> const d3{mu_2};

    BOOST_CHECK_CLOSE(norm_2(d2.mean() - mu), 0, 1e-6);
    BOOST_CHECK_CLOSE(norm_2(d3.mean() - mu_2), 0, 1e-6);

    BOOST_CHECK(d1 == d1);
    BOOST_CHECK(d2 == d2);
    BOOST_CHECK(d3 == d3);

    BOOST_CHECK(d1 != d2);
    BOOST_CHECK(d1 != d3);
    BOOST_CHECK(d2 != d3);
}

BOOST_AUTO_TEST_CASE(iid_adaptor_default_ctor_test)
{
    typedef std::bernoulli_distribution D;

    ural::iid_adaptor<D> const d0{};

    BOOST_CHECK_EQUAL(1U, d0.count());
    BOOST_CHECK(D{} == d0.base());
}

BOOST_AUTO_TEST_CASE(iid_adaptor_equality_test)
{
    typedef std::bernoulli_distribution D;

    ural::iid_adaptor<D> const d0{};
    ural::iid_adaptor<D> const d1{1, D{0.5}};
    ural::iid_adaptor<D> const d2{1, D{0.25}};
    ural::iid_adaptor<D> const d3{3, D{0.25}};

    BOOST_CHECK(d0 == d0);
    BOOST_CHECK(d0 == d1);

    BOOST_CHECK(d0 != d2);
    BOOST_CHECK(d1 != d2);
    BOOST_CHECK(d2 != d3);
}
