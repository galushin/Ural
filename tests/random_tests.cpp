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

// @todo Устранить (Уменьшить дублирование)
#include <ural/random.hpp>

#include <boost/test/unit_test.hpp>

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
    BOOST_CHECK_EQUAL(1, p.size());
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
    BOOST_CHECK_EQUAL(1, p.size());
    BOOST_CHECK_EQUAL(1, p[0]);
}

BOOST_AUTO_TEST_CASE(discrete_distribution_iterator_ctor_test)
{
    typedef ural::discrete_distribution<> D;
    {
        std::vector<double> p0 = {1};
        D d(p0.begin(), p0.end());
        std::vector<double> p = d.probabilities();
        BOOST_CHECK_EQUAL(p.size(), 1);
        BOOST_CHECK_EQUAL(p[0], 1);
    }
    {
        std::vector<double> p0 = {10};
        D d(p0.begin(), p0.end());
        std::vector<double> p = d.probabilities();
        BOOST_CHECK_EQUAL(p.size(), 1);
        BOOST_CHECK_EQUAL(p[0], 1);
    }
    {
        std::vector<double> p0 = {10, 30};
        D d(p0.begin(), p0.end());
        std::vector<double> p = d.probabilities();
        BOOST_CHECK_EQUAL(p.size(), 2);
        BOOST_CHECK_EQUAL(p[0], 0.25);
        BOOST_CHECK_EQUAL(p[1], 0.75);
    }
    {
        std::vector<double> p0 = {30, 10};
        D d(p0.begin(), p0.end());
        std::vector<double> p = d.probabilities();
        BOOST_CHECK_EQUAL(p.size(), 2);
        BOOST_CHECK_EQUAL(p[0], 0.75);
        BOOST_CHECK_EQUAL(p[1], 0.25);
    }
    {
        std::vector<double> p0 = {30, 0, 10};
        D d(p0.begin(), p0.end());
        std::vector<double> p = d.probabilities();
        BOOST_CHECK_EQUAL(p.size(), 3);
        BOOST_CHECK_EQUAL(p[0], 0.75);
        BOOST_CHECK_EQUAL(p[1], 0);
        BOOST_CHECK_EQUAL(p[2], 0.25);
    }
    {
        std::vector<double> p0 = {0, 30, 10};
        D d(p0.begin(), p0.end());
        std::vector<double> p = d.probabilities();
        BOOST_CHECK_EQUAL(p.size(), 3);
        BOOST_CHECK_EQUAL(p[0], 0);
        BOOST_CHECK_EQUAL(p[1], 0.75);
        BOOST_CHECK_EQUAL(p[2], 0.25);
    }
    {
        std::vector<double> p0 = {0, 0, 10};
        D d(p0.begin(), p0.end());
        std::vector<double> p = d.probabilities();
        BOOST_CHECK_EQUAL(p.size(), 3);
        BOOST_CHECK_EQUAL(p[0], 0);
        BOOST_CHECK_EQUAL(p[1], 0);
        BOOST_CHECK_EQUAL(p[2], 1);
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
    BOOST_CHECK_EQUAL(p.size(), 2);
    BOOST_CHECK_EQUAL(p[0], 0.25);
    BOOST_CHECK_EQUAL(p[1], 0.75);
}

BOOST_AUTO_TEST_CASE(discrete_distribution_func_ctor_test)
{
    auto const fw = +[](double x) { return x+1; };

    typedef ural::discrete_distribution<> D;
    {
        D d(0, 0, 1, fw);
        std::vector<double> p = d.probabilities();
        BOOST_CHECK_EQUAL(1, p.size());
        BOOST_CHECK_EQUAL(1, p[0]);
    }
    {
        D d(1, 0, 1, fw);
        std::vector<double> p = d.probabilities();
        BOOST_CHECK_EQUAL(1, p.size());
        BOOST_CHECK_EQUAL(1, p[0]);
    }
    {
        D d(2, 0.5, 1.5, fw);
        std::vector<double> p = d.probabilities();
        BOOST_CHECK_EQUAL(2, p.size());
        BOOST_CHECK_EQUAL(p[0], .4375);
        BOOST_CHECK_EQUAL(p[1], .5625);
    }
    {
        D d(4, 0, 2, fw);
        std::vector<double> p = d.probabilities();
        BOOST_CHECK_EQUAL(4, p.size());
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
        BOOST_CHECK_EQUAL(p.size(), 1);
        BOOST_CHECK_EQUAL(p[0], 1);
    }
    {
        D d = {10};
        std::vector<double> p = d.probabilities();
        BOOST_CHECK_EQUAL(p.size(), 1);
        BOOST_CHECK_EQUAL(p[0], 1);
    }
    {
        D d = {10, 30};
        std::vector<double> p = d.probabilities();
        BOOST_CHECK_EQUAL(p.size(), 2);
        BOOST_CHECK_EQUAL(p[0], 0.25);
        BOOST_CHECK_EQUAL(p[1], 0.75);
    }
    {
        D d = {30, 10};
        std::vector<double> p = d.probabilities();
        BOOST_CHECK_EQUAL(p.size(), 2);
        BOOST_CHECK_EQUAL(p[0], 0.75);
        BOOST_CHECK_EQUAL(p[1], 0.25);
    }
    {
        D d = {30, 0, 10};
        std::vector<double> p = d.probabilities();
        BOOST_CHECK_EQUAL(p.size(), 3);
        BOOST_CHECK_EQUAL(p[0], 0.75);
        BOOST_CHECK_EQUAL(p[1], 0);
        BOOST_CHECK_EQUAL(p[2], 0.25);
    }
    {
        D d = {0, 30, 10};
        std::vector<double> p = d.probabilities();
        BOOST_CHECK_EQUAL(p.size(), 3);
        BOOST_CHECK_EQUAL(p[0], 0);
        BOOST_CHECK_EQUAL(p[1], 0.75);
        BOOST_CHECK_EQUAL(p[2], 0.25);
    }
    {
        D d = {0, 0, 10};
        std::vector<double> p = d.probabilities();
        BOOST_CHECK_EQUAL(p.size(), 3);
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

// Генерация значений
namespace
{
    template <class Distribution>
    void test_discrete_distribution_exact(Distribution d, size_t N)
    {
        typedef std::minstd_rand G;
        G g;

        std::vector<typename Distribution::result_type> u(d.max()+1);

        for (int i = 0; i < N; ++i)
        {
            typename Distribution::result_type v = d(g);
            BOOST_CHECK(d.min() <= v && v <= d.max());
            u[v]++;
        }

        std::vector<double> prob = d.probabilities();
        for (int i = 0; i <= d.max(); ++i)
            BOOST_CHECK_EQUAL((double)u[i]/N, prob[i]);
    }

    template <class Weights>
    void test_discrete_distribution_approx(Weights && ws, size_t N, double eps)
    {
        typedef ural::discrete_distribution<> D;
        typedef std::minstd_rand G;

        G g;
        D d(ws.begin(), ws.end());

        std::vector<D::result_type> u(d.max()+1);
        for (int i = 0; i < N; ++i)
        {
            D::result_type v = d(g);
            BOOST_CHECK(d.min() <= v && v <= d.max());
            u[v]++;
        }
        std::vector<double> prob = d.probabilities();

        for (int i = 0; i <= d.max(); ++i)
            if (prob[i] != 0)
                BOOST_CHECK_CLOSE((double)u[i]/N, prob[i], eps * 100);
            else
                BOOST_CHECK(u[i] == 0);
    }
}

BOOST_AUTO_TEST_CASE(discrete_distribution_eval_test)
{
    typedef ural::discrete_distribution<> D;
    typedef std::minstd_rand G;

    test_discrete_distribution_exact(D{}, 100);
    test_discrete_distribution_exact(D{.3}, 100);

    const int N = 100000;
    const double eps = 0.01;
    typedef std::vector<double> Weights;
    test_discrete_distribution_approx(Weights{.75, .25}, N, eps);

    test_discrete_distribution_exact(D{0, 1}, N);
    test_discrete_distribution_exact(D{1, 0}, N);

    test_discrete_distribution_approx(Weights{.3, .1, .6}, N, eps);
    test_discrete_distribution_approx(Weights{0, 25, 75}, N, eps);
    test_discrete_distribution_approx(Weights{25, 0, 75}, N, eps);
    test_discrete_distribution_approx(Weights{25, 75, 0}, N, eps);
    test_discrete_distribution_approx(Weights{0, 0, 1}, N, eps);
    test_discrete_distribution_approx(Weights{0, 1, 0}, N, eps);
    test_discrete_distribution_approx(Weights{1, 0, 0}, N, eps);
    test_discrete_distribution_approx(Weights{33, 0, 0, 67}, N, eps);
}

BOOST_AUTO_TEST_CASE(discrete_distribution_param_eval_test)
{
    typedef ural::discrete_distribution<> D;
    typedef D::param_type P;
    typedef std::minstd_rand G;
    G g;
    D d;
    double p0[] = {.3, .1, .6};
    P p(p0, p0+3);
    const int N = 10000000;
    std::vector<D::result_type> u(3);
    for (int i = 0; i < N; ++i)
    {
        D::result_type v = d(g, p);
        BOOST_CHECK(0 <= v && v <= 2);
        u[v]++;
    }
    std::vector<double> prob = p.probabilities();
    for (int i = 0; i <= 2; ++i)
        BOOST_CHECK(std::abs((double)u[i]/N - prob[i]) / prob[i] < 0.001);
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
