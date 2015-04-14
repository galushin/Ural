/*  This file is part of Ural.

    Ural is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Ural is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Ural.  If not, see <http://www.gnu.org/licenses/>.
*/

// "Математическое" дискретное распределение
// @todo min и max
#include <ural/distributions/discrete.hpp>
#include <ural/math/rational.hpp>

#include <boost/mpl/list.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(math_discrete_distribution_default_ctor)
{
    typedef ural::distributions::discrete<int> Distribution;

    auto const d = Distribution{};

    BOOST_CHECK_EQUAL(d.min(), 0);
    BOOST_CHECK_EQUAL(d.max(), 0);

    auto const m = mean(d);
    BOOST_CHECK_EQUAL(m, 0.0);

    auto const s2 = variance(d);
    BOOST_CHECK_EQUAL(s2, 0.0);

    auto const s = standard_deviation(d);
    BOOST_CHECK_EQUAL(s, 0.0);

    BOOST_CHECK_EQUAL(cdf(d, -1), 0.0);
    BOOST_CHECK_EQUAL(cdf(d, 0), 1.0);
    BOOST_CHECK_EQUAL(cdf(d, 1), 1.0);

    auto const ps_0 = std::vector<double>{1.0};
    auto const ps   = d.probabilities();

    BOOST_CHECK_EQUAL_COLLECTIONS(ps.begin(), ps.end(),
                                  ps_0.begin(), ps_0.end());
}

BOOST_AUTO_TEST_CASE(math_discrete_distibution_from_empty_container)
{
    typedef ural::distributions::discrete<int> Distribution;

    auto const w = std::vector<double>{};

    auto const d = Distribution(w.begin(), w.end());

    BOOST_CHECK_EQUAL(d.min(), 0);
    BOOST_CHECK_EQUAL(ural::to_unsigned(d.max()), d.probabilities().size() - 1);

    auto const m = mean(d);
    BOOST_CHECK_EQUAL(m, 0.0);

    auto const s2 = variance(d);
    BOOST_CHECK_EQUAL(s2, 0.0);

    auto const s = standard_deviation(d);
    BOOST_CHECK_EQUAL(s, 0.0);

    BOOST_CHECK_EQUAL(cdf(d, -1), 0.0);
    BOOST_CHECK_EQUAL(cdf(d, 0), 1.0);
    BOOST_CHECK_EQUAL(cdf(d, 1), 1.0);

    auto const ps_0 = std::vector<double>{1.0};
    auto const ps   = d.probabilities();

    BOOST_CHECK_EQUAL_COLLECTIONS(ps.begin(), ps.end(),
                                  ps_0.begin(), ps_0.end());
}

namespace
{
    typedef boost::mpl::list<double, int> Weight_types;
}

BOOST_AUTO_TEST_CASE_TEMPLATE(math_discrete_distibution_from_container,
                              Weight, Weight_types)
{
    typedef ural::distributions::discrete<int, Weight> Distribution;

    std::vector<int> const w{4, 3, 2, 1};

    auto const d = Distribution(w.begin(), w.end());

    BOOST_CHECK_EQUAL(d.min(), 0);
    BOOST_CHECK_EQUAL(ural::to_unsigned(d.max()), d.probabilities().size() - 1);

    auto const m = mean(d);
    BOOST_CHECK_CLOSE(m, 1.0, 1e-10);

    auto const s2 = variance(d);
    BOOST_CHECK_CLOSE(s2, 1.0, 1e-10);

    auto const s = standard_deviation(d);
    BOOST_CHECK_CLOSE(s*s, s2, 1e-10);

    BOOST_CHECK_EQUAL(cdf(d, -0.5), 0);

    BOOST_CHECK_CLOSE(cdf(d, 0.0), 0.4, 1e-10);
    BOOST_CHECK_CLOSE(cdf(d, 0.5), 0.4, 1e-10);
    BOOST_CHECK_CLOSE(cdf(d, 1.0), 0.7, 1e-10);
    BOOST_CHECK_CLOSE(cdf(d, 1.5), 0.7, 1e-10);
    BOOST_CHECK_CLOSE(cdf(d, 2.0), 0.9, 1e-10);
    BOOST_CHECK_CLOSE(cdf(d, 2.5), 0.9, 1e-10);
    BOOST_CHECK_CLOSE(cdf(d, 3.0), 1.0, 1e-10);
    BOOST_CHECK_CLOSE(cdf(d, 3.5), 1.0, 1e-10);

    BOOST_CHECK_EQUAL(cdf(d, 4), 1.0);

    auto const ps_0 = std::vector<double>{0.4, 0.3, 0.2, 0.1};
    auto const ps   = d.probabilities();

    BOOST_CHECK_EQUAL(ps.size(), ps_0.size());

    for(auto i : ural::indices_of(ps))
    {
        BOOST_CHECK_CLOSE(ps[i], ps_0[i], 1e-10);
    }
}

BOOST_AUTO_TEST_CASE(math_discrete_distibution_from_container_rational_weight)
{
    typedef ural::rational<int> Weight;

    typedef ural::distributions::discrete<int, Weight> Distribution;

    std::vector<int> const w{4, 3, 2, 1};

    auto const d = Distribution(w.begin(), w.end());

    BOOST_CHECK_EQUAL(d.min(), 0);
    BOOST_CHECK_EQUAL(ural::to_unsigned(d.max()), d.probabilities().size() - 1);

    auto const m = mean(d);
    BOOST_CHECK_EQUAL(m, 1);

    auto const s2 = variance(d);
    BOOST_CHECK_EQUAL(s2, 1);

    BOOST_CHECK_EQUAL(cdf(d, -0.5), 0);

    BOOST_CHECK_EQUAL(cdf(d, 0), Weight(4, 10));
    BOOST_CHECK_EQUAL(cdf(d, 0.5), Weight(4, 10));
    BOOST_CHECK_EQUAL(cdf(d, 1), Weight(7, 10));
    BOOST_CHECK_EQUAL(cdf(d, 1.5), Weight(7, 10));
    BOOST_CHECK_EQUAL(cdf(d, 2), Weight(9, 10));
    BOOST_CHECK_EQUAL(cdf(d, 2.5), Weight(9, 10));
    BOOST_CHECK_EQUAL(cdf(d, 3), Weight(1));
    BOOST_CHECK_EQUAL(cdf(d, 3.5), Weight(1));

    BOOST_CHECK_EQUAL(cdf(d, 4), 1);

    auto const ps_0 = std::vector<Weight>{Weight(4, 10), Weight(3, 10),
                                          Weight(2, 10), Weight(1, 10)};
    auto const ps   = d.probabilities();

    BOOST_CHECK_EQUAL_COLLECTIONS(ps.begin(), ps.end(),
                                  ps_0.begin(), ps_0.end());
}

BOOST_AUTO_TEST_CASE(math_discrete_distibution_from_init_list)
{
    typedef ural::distributions::discrete<int> Distribution;

    auto const d = Distribution{4, 3, 2, 1};

    BOOST_CHECK_EQUAL(d.min(), 0);
    BOOST_CHECK_EQUAL(ural::to_unsigned(d.max()), d.probabilities().size() - 1);

    auto const m = mean(d);
    BOOST_CHECK_CLOSE(m, 1.0, 1e-10);

    auto const s2 = variance(d);
    BOOST_CHECK_CLOSE(s2, 1.0, 1e-10);

    auto const s = standard_deviation(d);
    BOOST_CHECK_CLOSE(s*s, s2, 1e-10);

    BOOST_CHECK_EQUAL(cdf(d, -0.5), 0);

    BOOST_CHECK_CLOSE(cdf(d, 0.0), 0.4, 1e-10);
    BOOST_CHECK_CLOSE(cdf(d, 0.5), 0.4, 1e-10);
    BOOST_CHECK_CLOSE(cdf(d, 1.0), 0.7, 1e-10);
    BOOST_CHECK_CLOSE(cdf(d, 1.5), 0.7, 1e-10);
    BOOST_CHECK_CLOSE(cdf(d, 2.0), 0.9, 1e-10);
    BOOST_CHECK_CLOSE(cdf(d, 2.5), 0.9, 1e-10);
    BOOST_CHECK_CLOSE(cdf(d, 3.0), 1.0, 1e-10);
    BOOST_CHECK_CLOSE(cdf(d, 3.5), 1.0, 1e-10);

    BOOST_CHECK_EQUAL(cdf(d, 4), 1.0);

    auto const ps_0 = std::vector<double>{0.4, 0.3, 0.2, 0.1};
    auto const ps   = d.probabilities();

    BOOST_CHECK_EQUAL(ps.size(), ps_0.size());

    for(auto i : ural::indices_of(ps))
    {
        BOOST_CHECK_CLOSE(ps[i], ps_0[i], 1e-10);
    }
}
