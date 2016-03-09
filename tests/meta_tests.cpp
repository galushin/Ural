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

#include <ural/meta/list.hpp>
#include <ural/meta/algo.hpp>

#include <boost/test/unit_test.hpp>

namespace
{
    namespace ural_ex = ::ural::experimental;
    namespace ural_meta = ::ural_ex::meta;
}
// namespace

BOOST_AUTO_TEST_CASE(meta_unique_test)
{
    typedef ::ural_ex::typelist<int, int, char, int, char, char, double> Source;
    typedef ::ural_ex::typelist<int, char, int, char, double> Objective;
    using Result = ::ural::experimental::meta::unique<Source>::type;

    Objective obj = Result{};

    static_assert(std::is_same<decltype(obj), Result>::value, "");
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(meta_flatten_test)
{
    using ural_ex::typelist;

    typedef typelist<char, typelist<bool, typelist<void, int*>, double>> Source;
    typedef typelist<char, bool, void, int*, double> Objective;
    using Result = typename ural_meta::flatten<Source>::type;

    static_assert(std::is_same<Objective, Result>::value, "");
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(meta_remove_first_test)
{
    typedef ::ural_ex::typelist<int, int, char, int, char, char, double> Source;
    typedef typename ural_meta::remove_first<Source, char>::type Result;

    typedef ::ural_ex::typelist<int, int, int, char, char, double> Objective;

    static_assert(std::is_same<Objective, Result>::value, "");

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(meta_remove_all_test)
{
    typedef ::ural_ex::typelist<int, int, char, int, char, char, double> Source;
    using Result = typename ural_meta::remove_all<Source, char>::type;

    using Objective = ::ural_ex::typelist<int, int, int, double>;

    static_assert(std::is_same<Objective, Result>::value, "");

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(meta_count_test)
{
    typedef ::ural_ex::typelist<int, int, char, int, char, char, double> Source;
    typedef std::integral_constant<size_t, 3> Objective;
    typedef ural_meta::count<Source, int> Result;

    static_assert(Objective::value == Result::value, "");

    BOOST_CHECK_EQUAL(Objective::value, Result::value);
}

BOOST_AUTO_TEST_CASE(meta_replace_test)
{
    typedef ::ural_ex::typelist<int, char, int, char, void> Source;
    typedef typename ural_meta::replace<Source, char, double>::type Result;

    typedef ::ural_ex::typelist<int, double, int, double, void> Objective;

    static_assert(std::is_same<Objective, Result>::value, "");

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(meta_identity_test)
{
    using id = ural_meta::identity;

    static_assert(std::is_same<void, ural_meta::apply<id, void>::type>::value, "");
    static_assert(std::is_same<char, ural_meta::apply<id, char>::type>::value, "");
    static_assert(std::is_same<double, ural_meta::apply<id, double>::type>::value, "");
    static_assert(std::is_same<std::string, ural_meta::apply<id, std::string>::type>::value, "");

    BOOST_CHECK(true);
}

#include <ural/meta/map.hpp>

BOOST_AUTO_TEST_CASE(meta_cartesian_product_single_type_test)
{
    using ::ural_ex::typelist;

    typedef ::ural_ex::typelist<int, long, long long> Integers;

    typedef ::ural_ex::typelist<typelist<int>, typelist<long>,
                             typelist<long long>> Expected;

    typedef typename ural_meta::cartesian_product<Integers>::type
        Result;

    static_assert(std::is_same<Expected, Result>::value, "");
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(meta_cartesian_product_test)
{
    using ::ural_ex::typelist;

    typedef ::ural_ex::typelist<int, long, long long> Integers;
    typedef ::ural_ex::typelist<float, double> Doubles;

    typedef ::ural_ex::typelist<typelist<int, float>, typelist<int, double>,
                             typelist<long, float>, typelist<long, double>,
                             typelist<long long, float>, typelist<long long, double>> Expected;

    typedef typename ural_meta::cartesian_product<Integers, Doubles>::type
        Result;

    Expected e = Result();

    static_assert(std::is_same<decltype(e), Result>::value, "");
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(meta_cartesian_product_tree_arg_test)
{
    using ::ural_ex::typelist;

    typedef ::ural_ex::typelist<char, std::string> L1;
    typedef ::ural_ex::typelist<int, long> L2;
    typedef ::ural_ex::typelist<float, double> L3;

    typedef ::ural_ex::typelist<typelist<char, int, float>,
                             typelist<char, int, double>,
                             typelist<char, long, float>,
                             typelist<char, long, double>,
                             typelist<std::string, int, float>,
                             typelist<std::string, int, double>,
                             typelist<std::string, long, float>,
                             typelist<std::string, long, double>
                            > Expected;

    typedef typename ural_meta::cartesian_product<L1, L2, L3>::type
        Result;

    static_assert(std::is_same<Expected, Result>::value, "");
    BOOST_CHECK(true);
}

// Задачи из Category Theory for Programmers. Bartosz Milewski
// 1.2
BOOST_AUTO_TEST_CASE(meta_composed_test)
{
    typedef ural_meta::template_to_applied<std::add_lvalue_reference> F1;
    typedef ural_meta::template_to_applied<std::common_type> F2;

    typedef ural_meta::composed<F1, F2> F;

    typedef typename ural_meta::apply<F, int, double>::type Result;
    typedef double & Expected;

    static_assert(std::is_same<Result, Expected>::value, "");
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(meta_compose_test)
{
    typedef ural_meta::template_to_applied<std::add_lvalue_reference> F1;
    typedef ural_meta::template_to_applied<std::add_const> F2;
    typedef ural_meta::template_to_applied<std::common_type> F3;

    typedef typename ural_meta::compose::apply<F1, F2, F3>::type F;

    typedef typename ural_meta::apply<F, int, double>::type Result;
    typedef double const & Expected;

    static_assert(std::is_same<Result, Expected>::value, "");
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(meta_compose_respect_identity)
{
    typedef ural_meta::template_to_applied<std::add_const> F0;
    typedef ural_meta::identity Id;

    typedef ural_meta::compose::apply<F0, Id>::type Right;
    typedef ural_meta::compose::apply<Id, F0>::type Left;
    typedef ural_meta::compose::apply<Id, Id>::type Both;

    static_assert(std::is_same<F0, Right>::value, "");
    static_assert(std::is_same<F0, Left>::value, "");
    static_assert(std::is_same<Id, Both>::value, "");
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(meta_pop_front_or_not_empty_test)
{
    typedef void Default;

    typedef ::ural_ex::typelist<int, long, long long> Source;
    typedef ::ural_ex::typelist<long, long long> Expected;
    typedef typename ural_meta::pop_front_or<Source, Default>::type Result;

    static_assert(std::is_same<Result, Expected>::value, "");
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(meta_pop_front_or_empty_test)
{
    typedef void Default;

    typedef ::ural_ex::typelist<> Source;
    typedef typename ural_meta::pop_front_or<Source, Default>::type Result;

    static_assert(std::is_same<Result, Default>::value, "");
    BOOST_CHECK(true);
}
