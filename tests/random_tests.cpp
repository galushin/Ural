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
#include <ural/random.hpp>

#include <boost/test/unit_test.hpp>

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
    typedef std::discrete_distribution<> D;
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

// Генерация значений
BOOST_AUTO_TEST_CASE(discrete_distribution_eval_test)
{
    typedef ural::discrete_distribution<> D;
    typedef std::minstd_rand G;
    {
        G g;
        D d;
        const int N = 100;
        std::vector<D::result_type> u(d.max()+1);
        for (int i = 0; i < N; ++i)
        {
            D::result_type v = d(g);
            BOOST_CHECK(d.min() <= v && v <= d.max());
            u[v]++;
        }
        std::vector<double> prob = d.probabilities();
        for (int i = 0; i <= d.max(); ++i)
            BOOST_CHECK_EQUAL((double)u[i]/N, prob[i]);
    }
    {\
        G g;
        std::vector<double> const p0 = {.3};
        D d(p0.begin(), p0.end());
        const int N = 100;
        std::vector<D::result_type> u(d.max()+1);
        for (int i = 0; i < N; ++i)
        {
            D::result_type v = d(g);
            BOOST_CHECK(d.min() <= v && v <= d.max());
            u[v]++;
        }
        std::vector<double> prob = d.probabilities();
        for (int i = 0; i <= d.max(); ++i)
            BOOST_CHECK_EQUAL((double)u[i]/N, prob[i]);
    }
    {
        G g;
        std::vector<double> const p0 = {.75, .25};
        D d(p0.begin(), p0.end());
        const int N = 1000000;
        std::vector<D::result_type> u(d.max()+1);
        for (int i = 0; i < N; ++i)
        {
            D::result_type v = d(g);
            BOOST_CHECK(d.min() <= v && v <= d.max());
            u[v]++;
        }
        std::vector<double> prob = d.probabilities();
        for (int i = 0; i <= d.max(); ++i)
            BOOST_CHECK(std::abs((double)u[i]/N - prob[i]) / prob[i] < 0.001);
    }
    {
        G g;
        std::vector<double> const p0 = {0, 1};
        D d(p0.begin(), p0.end());
        const int N = 1000000;
        std::vector<D::result_type> u(d.max()+1);
        for (int i = 0; i < N; ++i)
        {
            D::result_type v = d(g);
            BOOST_CHECK(d.min() <= v && v <= d.max());
            u[v]++;
        }
        std::vector<double> prob = d.probabilities();
        BOOST_CHECK((double)u[0]/N == prob[0]);
        BOOST_CHECK((double)u[1]/N == prob[1]);
    }
    {
        G g;
        std::vector<double> const p0 = {1, 0};
        D d(p0.begin(), p0.end());
        const int N = 1000000;
        std::vector<D::result_type> u(d.max()+1);
        for (int i = 0; i < N; ++i)
        {
            D::result_type v = d(g);
            BOOST_CHECK(d.min() <= v && v <= d.max());
            u[v]++;
        }
        std::vector<double> prob = d.probabilities();
        BOOST_CHECK((double)u[0]/N == prob[0]);
        BOOST_CHECK((double)u[1]/N == prob[1]);
    }
    {
        G g;
        std::vector<double> const p0 = {.3, .1, .6};
        D d(p0.begin(), p0.end());
        const int N = 10000000;
        std::vector<D::result_type> u(d.max()+1);
        for (int i = 0; i < N; ++i)
        {
            D::result_type v = d(g);
            BOOST_CHECK(d.min() <= v && v <= d.max());
            u[v]++;
        }
        std::vector<double> prob = d.probabilities();
        for (int i = 0; i <= d.max(); ++i)
            BOOST_CHECK(std::abs((double)u[i]/N - prob[i]) / prob[i] < 0.001);
    }
    {
        G g;
        std::vector<double> const p0 = {0, 25, 75};
        D d(p0.begin(), p0.end());
        const int N = 1000000;
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
                BOOST_CHECK(std::abs((double)u[i]/N - prob[i]) / prob[i] < 0.001);
            else
                BOOST_CHECK(u[i] == 0);
    }
    {
        G g;
        std::vector<double> const p0 = {25, 0, 75};
        D d(p0.begin(), p0.end());
        const int N = 1000000;
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
                BOOST_CHECK(std::abs((double)u[i]/N - prob[i]) / prob[i] < 0.001);
            else
                BOOST_CHECK(u[i] == 0);
    }
    {
        G g;
        std::vector<double> const p0 = {25, 75, 0};
        D d(p0.begin(), p0.end());
        const int N = 1000000;
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
                BOOST_CHECK(std::abs((double)u[i]/N - prob[i]) / prob[i] < 0.001);
            else
                BOOST_CHECK(u[i] == 0);
    }
    {
        G g;
        std::vector<double> const p0 = {0, 0, 1};
        D d(p0.begin(), p0.end());
        const int N = 100;
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
                BOOST_CHECK(std::abs((double)u[i]/N - prob[i]) / prob[i] < 0.001);
            else
                BOOST_CHECK(u[i] == 0);
    }
    {
        G g;
        std::vector<double> const p0 = {0, 1, 0};
        D d(p0.begin(), p0.end());
        const int N = 100;
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
                BOOST_CHECK(std::abs((double)u[i]/N - prob[i]) / prob[i] < 0.001);
            else
                BOOST_CHECK(u[i] == 0);
    }
    {
        G g;
        std::vector<double> const p0 = {1, 0, 0};
        D d(p0.begin(), p0.end());
        const int N = 100;
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
                BOOST_CHECK(std::abs((double)u[i]/N - prob[i]) / prob[i] < 0.001);
            else
                BOOST_CHECK(u[i] == 0);
    }
    {
        G g;
        std::vector<double> const p0 = {33, 0, 0, 67};
        D d(p0.begin(), p0.end());
        const int N = 1000000;
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
                BOOST_CHECK(std::abs((double)u[i]/N - prob[i]) / prob[i] < 0.001);
            else
                BOOST_CHECK(u[i] == 0);
    }
}
