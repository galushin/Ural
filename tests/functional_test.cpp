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

#include <ural/math/rational.hpp>
#include <ural/math.hpp>
#include <ural/utility/tracers.hpp>
#include <ural/functional.hpp>
#include <ural/memory.hpp>
#include <ural/functional/memoize.hpp>

#include "rnd.hpp"

#include <boost/test/unit_test.hpp>

#include <boost/mpl/list.hpp>
namespace
{
    typedef boost::mpl::list<ural::less<int>,
                             ural::less<int, void>,
                             ural::less<void, int>,
                             ural::less<>> Less_functions;

    typedef boost::mpl::list<ural::greater<int>,
                             ural::greater<int, void>,
                             ural::greater<void, int>,
                             ural::greater<>> Greater_functions;

    typedef boost::mpl::list<ural::not_equal_to<int>,
                         ural::not_equal_to<>,
                         ural::not_equal_to<int, void>,
                         ural::not_equal_to<void, int>> Neq_functions;

    typedef boost::mpl::list<ural::logical_not<bool>, ural::logical_not<>>
        Not_functions;

    typedef boost::mpl::list<ural::logical_and<bool>,
                         ural::logical_and<>,
                         ural::logical_and<bool, void>,
                         ural::logical_and<void, bool>> And_functions;

    typedef boost::mpl::list<ural::logical_or<bool>,
                         ural::logical_or<>,
                         ural::logical_or<bool, void>,
                         ural::logical_or<void, bool>> Or_functions;

    typedef boost::mpl::list<ural::bit_and<int>,
                         ural::bit_and<int, void>,
                         ural::bit_and<void, int>,
                         ural::bit_and<void, void>> Bit_and_functions;

    typedef boost::mpl::list<ural::bit_or<int>,
                         ural::bit_or<int, void>,
                         ural::bit_or<void, int>,
                         ural::bit_or<void, void>> Bit_or_functions;

    typedef boost::mpl::list<ural::bit_xor<int>,
                         ural::bit_xor<int, void>,
                         ural::bit_xor<void, int>,
                         ural::bit_xor<void, void>> Bit_xor_functions;

    typedef boost::mpl::list<ural::logical_implication<bool>,
                         ural::logical_implication<bool, void>,
                         ural::logical_implication<void, bool>,
                         ural::logical_implication<void, void>>
        Implication_functions;

    // Арифметические операторы
    typedef boost::mpl::list<ural::plus<int>,
                             ural::plus<>,
                             ural::plus<int, void>,
                             ural::plus<void, int>> Plus_functions;

    typedef boost::mpl::list<ural::modulus<int>,
                             ural::modulus<>,
                             ural::modulus<int, void>,
                             ural::modulus<void, int>> Modulus_functions;
}

using ::ural::ValueType;

BOOST_AUTO_TEST_CASE(memoize_function_test)
{
    struct Tag;

    typedef double(Signature)(double);

    typedef ural::abs_fn Target;

    ural::callable_tracer<Target, Tag> f_tracer(ural::abs());

    static_assert(std::is_empty<decltype(f_tracer)>::value, "Must be empty!");

    auto f = ural::memoize<Signature>(ural::ref = f_tracer);

    auto const y1 = f(-1);

    BOOST_CHECK_EQUAL(1, y1);
    BOOST_CHECK_EQUAL(1U, f_tracer.calls());

    auto const y2 = f(2);

    BOOST_CHECK_EQUAL(2, y2);
    BOOST_CHECK_EQUAL(2U, f_tracer.calls());

    auto const y3 = f(-1);

    BOOST_CHECK_EQUAL(1, y3);
    BOOST_CHECK_EQUAL(2U, f_tracer.calls());

    f.clear_cache();

    auto const y4 = f(-1);

    BOOST_CHECK_EQUAL(1, y4);
    BOOST_CHECK_EQUAL(3U, f_tracer.calls());
}

BOOST_AUTO_TEST_CASE(memoize_function_equality)
{
    typedef double(Signature)(double);
    auto const f1 = ural::memoize<Signature>((Signature*)(std::abs));
    auto const f2 = f1;

    auto const v1 = f1(-42);
    BOOST_CHECK(v1 == 42);

    BOOST_CHECK_EQUAL(1U, f1.cache_size());
    BOOST_CHECK_EQUAL(0U, f2.cache_size());
    BOOST_CHECK(f1 == f2);
}

BOOST_AUTO_TEST_CASE(function_tracer_test)
{
    typedef double(*Target)(double);

    typedef ural::callable_tracer<Target> Function;

    // Явное преобразование нужно, так как std::abs --- перегруженная функция
    auto f = ural::make_function_tracer(Target(std::abs));

    static_assert(std::is_same<decltype(f), Function>::value, "");

    BOOST_CHECK_EQUAL(0U, Function::calls());

    f(1.0);

    BOOST_CHECK_EQUAL(1U, Function::calls());

    f(-1.0);

    BOOST_CHECK_EQUAL(2U, Function::calls());

    Function::reset_calls();

    BOOST_CHECK_EQUAL(0U, Function::calls());
}

BOOST_AUTO_TEST_CASE(replace_if_function_different_pred_inequality_test)
{
    auto p1 = +[](int const & x) { return ural::is_even(x); };
    auto p2 = +[](int const & x) { return ural::is_odd(x); };

    auto const f1 = ural::make_replace_if_function(p1, 0);
    auto const f2 = ural::make_replace_if_function(p2, 0);

    BOOST_CHECK(f1 != f2);
}

BOOST_AUTO_TEST_CASE(replace_if_function_test)
{
    typedef int Type;

    constexpr auto const pred = ural::is_even_fn{};
    constexpr auto const new_value = Type(-7);

    constexpr auto const f = ural::make_replace_if_function(pred, new_value);
    constexpr auto const f1 = ural::make_replace_if_function(pred, new_value + 1);

    BOOST_CHECK(f1 != f);

    static_assert(pred == f.predicate(), "");
    static_assert(new_value == f.new_value(), "");

    constexpr auto const v1 = Type(2);
    constexpr auto const v2 = Type(3);

    static_assert(v1 != new_value, "");
    static_assert(pred(v1) == true, "");
    static_assert(f(v1) == new_value, "");

    static_assert(v2 != new_value, "");
    static_assert(pred(v2) == false, "");
    static_assert(f(v2) == v2, "");

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(replace_function_test)
{
    auto const old_value = ::ural::rational<int>(-1);
    auto const new_value = 1;
    auto const other_value = 42;

    BOOST_CHECK(other_value != old_value);
    BOOST_CHECK(other_value != new_value);

    ural::replace_function<::ural::rational<int>, int> const
        f{old_value, new_value};

    BOOST_CHECK_EQUAL(old_value, f.old_value());
    BOOST_CHECK_EQUAL(new_value, f.new_value());

    BOOST_CHECK_EQUAL(old_value.denominator(), 1);

    BOOST_CHECK_EQUAL(new_value, f(old_value.numerator()));
    BOOST_CHECK_EQUAL(new_value, f(new_value));
    BOOST_CHECK_EQUAL(other_value, f(other_value));
}

BOOST_AUTO_TEST_CASE(replace_function_test_custom_predicate)
{
    struct No_equal_op
    {
        int value;
    };

    No_equal_op const old_value = {-1};
    No_equal_op const new_value = {1};
    No_equal_op const other_value = {42};

    auto const eq = +[](No_equal_op const & x, No_equal_op const & y)
                    { return x.value == y.value; };

    BOOST_CHECK(!eq(other_value, old_value));
    BOOST_CHECK(!eq(other_value, new_value));

    ural::replace_function<decltype(old_value), decltype(new_value), decltype(eq)> const
        f{old_value, new_value, eq};

    BOOST_CHECK(eq(old_value, f.old_value()));
    BOOST_CHECK(eq(new_value, f.new_value()));

    BOOST_CHECK(eq == f.predicate());
    BOOST_CHECK_EQUAL(eq, f.predicate().target());

    BOOST_CHECK(eq(new_value, f(old_value)));
    BOOST_CHECK(eq(new_value, f(new_value)));
    BOOST_CHECK(eq(other_value, f(other_value)));
}

BOOST_AUTO_TEST_CASE(compare_by_test)
{
    auto tr = [](int a) { return std::abs(a); };
    auto const cmp1 = [=](int a, int b) {return tr(a) < tr(b);};
    auto const cmp2 = ural::compare_by(tr);

    BOOST_CHECK(cmp2 == cmp2);
    BOOST_CHECK(!(cmp2 != cmp2));

    for(int a = -10; a <= 10; ++ a)
    for(int b = -10; b <= 10; ++ b)
    {
        BOOST_CHECK_EQUAL(cmp1(a, b), cmp2(a, b));
    }
}

BOOST_AUTO_TEST_CASE(compare_by_consexpr)
{
    auto constexpr cmp = ural::compare_by(ural::square);

    static_assert(std::is_empty<decltype(cmp)>::value, "Must be empty class");

    static_assert(cmp(15, 11) == false, "");
    static_assert(cmp(-15, 11) == false, "");
    static_assert(cmp(15, -11) == false, "");
    static_assert(cmp(-15, -11) == false, "");

    static_assert(cmp(15, 17) == true, "");
    static_assert(cmp(-15, 17) == true, "");
    static_assert(cmp(15, -17) == true, "");
    static_assert(cmp(-15, -17) == true, "");

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(equals_by_test)
{
    typedef std::pair<int, int> Pair;

    auto eq = ural::equal_by(&Pair::first);

    Pair p11{1, 1};
    Pair p21{2, 1};
    Pair p12{1, 2};
    Pair p22{2, 2};

    BOOST_CHECK(eq(p11, p12) == true);
    BOOST_CHECK(eq(p11, p21) == false);

    BOOST_CHECK(eq(p21, p22) == true);
    BOOST_CHECK(eq(p12, p22) == false);
}

BOOST_AUTO_TEST_CASE(compare_by_test_custom_compare)
{
    auto tr = [](int a) { return std::abs(a); };
    auto cmp_base = ural::greater<>{};

    auto cmp1 = [=](int a, int b) {return cmp_base(tr(a), tr(b));};
    auto cmp2 = ural::compare_by(tr, cmp_base);

    BOOST_CHECK(cmp2 == cmp2);
    BOOST_CHECK(!(cmp2 != cmp2));

    for(int a = -10; a <= 10; ++ a)
    for(int b = -10; b <= 10; ++ b)
    {
        BOOST_CHECK_EQUAL(cmp1(a, b), cmp2(a, b));
    }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(less_test, Function, Less_functions)
{
    constexpr Function cmp {};

    static_assert(cmp == cmp, "");
    static_assert(!(cmp != cmp), "");

    static_assert(true == cmp(1, 2), "");
    static_assert(false == cmp(2, 1), "");
    static_assert(false == cmp(2, 2), "");
    static_assert(false == cmp(1, 1), "");

    BOOST_CHECK_EQUAL(true, cmp(1, 2));
    BOOST_CHECK_EQUAL(false, cmp(2, 1));
    BOOST_CHECK_EQUAL(false, cmp(2, 2));
    BOOST_CHECK_EQUAL(false, cmp(1, 1));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(greater_test, Function, Greater_functions)
{
    constexpr Function cmp {};

    static_assert(cmp == cmp, "");
    static_assert(!(cmp != cmp), "");

    static_assert(false == cmp(1, 2), "");
    static_assert(true == cmp(2, 1), "");
    static_assert(false == cmp(2, 2), "");
    static_assert(false == cmp(1, 1), "");

    BOOST_CHECK_EQUAL(false, cmp(1, 2));
    BOOST_CHECK_EQUAL(true, cmp(2, 1));
    BOOST_CHECK_EQUAL(false, cmp(2, 2));
    BOOST_CHECK_EQUAL(false, cmp(1, 1));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(not_equal_to_test, Function, Neq_functions)
{
    constexpr Function neq {};

    static_assert(neq == neq, "");
    static_assert(!(neq != neq), "");

    static_assert(true == neq(1, 2), "");
    static_assert(true == neq(2, 1), "");
    static_assert(false == neq(2, 2), "");
    static_assert(false == neq(1, 1), "");

    BOOST_CHECK_EQUAL(true, neq(1, 2));
    BOOST_CHECK_EQUAL(true, neq(2, 1));
    BOOST_CHECK_EQUAL(false, neq(2, 2));
    BOOST_CHECK_EQUAL(false, neq(1, 1));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(logical_not_test, Function, Not_functions)
{
    constexpr Function not_ {};

    static_assert(not_ == not_, "");
    static_assert(!(not_ != not_), "");

    static_assert(true == not_(false), "");
    static_assert(false == not_(true), "");

    BOOST_CHECK_EQUAL(true, not_(false));
    BOOST_CHECK_EQUAL(false, not_(true));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(logical_and_test, Function, And_functions)
{
    constexpr Function and_ {};

    static_assert(and_ == and_, "");
    static_assert(!(and_ != and_), "");

    static_assert(false == and_(false, false), "");
    static_assert(false == and_(true, false), "");
    static_assert(false == and_(false, true), "");
    static_assert(true == and_(true, true), "");

    BOOST_CHECK_EQUAL(false, and_(false, false));
    BOOST_CHECK_EQUAL(false, and_(false, true));
    BOOST_CHECK_EQUAL(false, and_(true, false));
    BOOST_CHECK_EQUAL(true, and_(true, true));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(logical_or_test, Function, Or_functions)
{
    constexpr Function or_ {};

    static_assert(or_ == or_, "");
    static_assert(!(or_ != or_), "");

    static_assert(false == or_(false, false), "");
    static_assert(true == or_(true, false), "");
    static_assert(true == or_(false, true), "");
    static_assert(true == or_(true, true), "");

    BOOST_CHECK_EQUAL(false, or_(false, false));
    BOOST_CHECK_EQUAL(true, or_(false, true));
    BOOST_CHECK_EQUAL(true, or_(true, false));
    BOOST_CHECK_EQUAL(true, or_(true, true));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(bit_and_test, Function, Bit_and_functions)
{
    constexpr auto f = Function{};

    std::uniform_int_distribution<int> d(-100, 100);

    auto const x = d(ural_test::random_engine());
    auto const y = d(ural_test::random_engine());

    BOOST_CHECK_EQUAL((x & y), f(x, y));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(bit_or_test, Function, Bit_or_functions)
{
    constexpr auto f = Function{};

    std::uniform_int_distribution<int> d(-100, 100);

    auto const x = d(ural_test::random_engine());
    auto const y = d(ural_test::random_engine());

    BOOST_CHECK_EQUAL((x | y), f(x, y));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(bit_xor_test, Function, Bit_xor_functions)
{
    constexpr auto f = Function{};

    std::uniform_int_distribution<int> d(-100, 100);

    auto const x = d(ural_test::random_engine());
    auto const y = d(ural_test::random_engine());

    BOOST_CHECK_EQUAL((x ^ y), f(x, y));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(logic_implication_test, Function, Implication_functions)
{
    constexpr auto f = Function{};

    static_assert(f == f, "");
    static_assert(!(f != f), "");

    static_assert(true == f(false, false), "");
    static_assert(false == f(true, false), "");
    static_assert(true == f(false, true), "");
    static_assert(true == f(true, true), "");

    BOOST_CHECK_EQUAL(true, f(false, false));
    BOOST_CHECK_EQUAL(false, f(true, false));
    BOOST_CHECK_EQUAL(true, f(false, true));
    BOOST_CHECK_EQUAL(true, f(true, true));
}

BOOST_AUTO_TEST_CASE(negate_test)
{
    const auto value = 42;

    constexpr auto f = ural::negate<decltype(value)>{};

    constexpr auto r = f(value);

    BOOST_CHECK_EQUAL(-value, r);
}

BOOST_AUTO_TEST_CASE(negate_test_auto)
{
    const auto value = 42;

    constexpr auto f = ural::negate<>{};

    constexpr auto r = f(value);

    BOOST_CHECK_EQUAL(-value, r);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(modulus_test, Modulus, Modulus_functions)
{
    constexpr Modulus mod {};

    auto const n = 10;

    constexpr auto r = mod(2 * n, n);

    static_assert(r == 0, "");

    for(auto x = -2 * n; x < 2 * n; ++ x)
    {
        BOOST_CHECK_EQUAL(x % n, mod(x, n));
    }
}

BOOST_AUTO_TEST_CASE(bit_not_test)
{
    typedef int Type;

    ural::bit_not<Type> constexpr f{};
    ural::bit_not<> constexpr fa{};

    static_assert(std::is_same<Type, decltype(f)::result_type>::value, "");
    static_assert(std::is_same<Type, decltype(f)::argument_type>::value, "");

    static_assert(f(0) == ~0, "");
    static_assert(f(1) == ~1, "");

    static_assert(fa(0) == ~0, "");
    static_assert(fa(1) == ~1, "");

    std::uniform_int_distribution<int> d(-100, 100);

    auto const value = d(ural_test::random_engine());

    BOOST_CHECK_EQUAL(~value, f(value));
    BOOST_CHECK_EQUAL(~value, fa(value));
}

BOOST_AUTO_TEST_CASE(make_function_for_member_var_test)
{
    typedef std::pair<int, std::string> Type;

    Type x{42, "abc"};
    Type const x_c = x;
    Type volatile x_v = x;
    Type volatile const x_cv = x;

    auto p_x = &x;
    auto p_x_c = &x_c;
    auto p_x_v = &x_v;
    auto p_x_cv = &x_cv;

    auto r = std::ref(x);
    auto r_c = std::ref(x_c);
    auto r_v = std::ref(x_v);
    auto r_cv = std::ref(x_cv);

    auto f = ural::make_callable(&Type::first);

    BOOST_CHECK_EQUAL(&Type::first, f.target());

    typedef decltype(f(x)) R;
    typedef decltype(f(x_c)) R_c;
    typedef decltype(f(x_v)) R_v;
    typedef decltype(f(x_cv)) R_cv;

    static_assert(std::is_same<int &, R>::value, "");
    static_assert(std::is_same<int const &, R_c>::value, "");
    static_assert(std::is_same<int volatile &, R_v>::value, "");
    static_assert(std::is_same<int const volatile &, R_cv>::value, "");

    static_assert(std::is_same<int &, decltype(f(p_x))>::value, "");
    static_assert(std::is_same<int const &, decltype(f(p_x_c))>::value, "");
    static_assert(std::is_same<int volatile &, decltype(f(p_x_v))>::value, "");
    static_assert(std::is_same<int const volatile &, decltype(f(p_x_cv))>::value, "");

    static_assert(std::is_same<int &, decltype(f(r))>::value, "");
    static_assert(std::is_same<int const &, decltype(f(r_c))>::value, "");
    static_assert(std::is_same<int volatile &, decltype(f(r_v))>::value, "");
    static_assert(std::is_same<int const volatile &, decltype(f(r_cv))>::value, "");

    BOOST_CHECK_EQUAL(x.first, f(x));
    BOOST_CHECK_EQUAL(x.first, f(x_c));
    BOOST_CHECK_EQUAL(x.first, f(x_v));
    BOOST_CHECK_EQUAL(x.first, f(x_cv));

    BOOST_CHECK_EQUAL(x.first, f(r));
    BOOST_CHECK_EQUAL(x.first, f(r_c));
    BOOST_CHECK_EQUAL(x.first, f(r_v));
    BOOST_CHECK_EQUAL(x.first, f(r_cv));

    BOOST_CHECK_EQUAL(x.first, f(p_x));
    BOOST_CHECK_EQUAL(x.first, f(p_x_c));
    BOOST_CHECK_EQUAL(x.first, f(p_x_v));
    BOOST_CHECK_EQUAL(x.first, f(p_x_cv));
}

BOOST_AUTO_TEST_CASE(make_function_for_member_var_test_smart_ptr)
{
    typedef std::pair<int, std::string> Type;
    Type x{42, "abc"};

    auto p = ural::make_unique<Type>(x);
    auto p_c = ural::make_unique<Type const>(x);
    auto p_v = ural::make_unique<Type volatile>(x);
    auto p_cv = ural::make_unique<Type const volatile>(x);

    auto f = ural::make_callable(&Type::first);

    BOOST_CHECK_EQUAL(&Type::first, f.target());

    static_assert(std::is_same<int &, decltype(f(p))>::value, "");
    static_assert(std::is_same<int const &, decltype(f(p_c))>::value, "");
    static_assert(std::is_same<int volatile &, decltype(f(p_v))>::value, "");
    static_assert(std::is_same<int const volatile &, decltype(f(p_cv))>::value, "");

    BOOST_CHECK_EQUAL(x.first, f(p));
    BOOST_CHECK_EQUAL(x.first, f(p_c));
    BOOST_CHECK_EQUAL(x.first, f(p_v));
    BOOST_CHECK_EQUAL(x.first, f(p_cv));
}

BOOST_AUTO_TEST_CASE(make_function_for_member_function_test)
{
    struct Inner
    {
    public:
        Inner(int x)
         : value{x}
        {}

        int value;

        int get_something() const
        {
            return value;
        }

        int get_something_threadsafe() const volatile
        {
            return value;
        }

        void do_something()
        {
            value = 0;
        }

        void do_something_threadsafe() volatile
        {
            value = 0;
        }
    };

    auto f = ural::make_callable(&Inner::do_something);
    auto f_v = ural::make_callable(&Inner::do_something_threadsafe);
    auto f_c = ural::make_callable(&Inner::get_something);
    auto f_cv = ural::make_callable(&Inner::get_something_threadsafe);

    BOOST_CHECK(f.target() == &Inner::do_something);
    BOOST_CHECK(f_v.target() == &Inner::do_something_threadsafe);
    BOOST_CHECK(f_c.target() == &Inner::get_something);
    BOOST_CHECK(f_cv.target() == &Inner::get_something_threadsafe);

    Inner x = {42};
    Inner const x_c = x;
    Inner volatile x_v = x;
    Inner const volatile x_cv = x;

    auto p = ural::make_unique<Inner>(42);
    auto p_c = ural::make_unique<Inner const>(42);
    auto p_v = ural::make_unique<Inner volatile>(42);
    auto p_cv = ural::make_unique<Inner const volatile>(42);

    auto r = std::ref(x);
    auto r_c = std::ref(x_c);
    auto r_v = std::ref(x_v);
    auto r_cv = std::ref(x_cv);

    BOOST_CHECK_EQUAL(x.value, f_c(x_c));
    BOOST_CHECK_EQUAL(x.value, f_cv(x_cv));
    BOOST_CHECK_EQUAL(x.value, f_c(p_c));
    BOOST_CHECK_EQUAL(x.value, f_cv(p_cv));
    BOOST_CHECK_EQUAL(x.value, f_c(r_c));
    BOOST_CHECK_EQUAL(x.value, f_cv(r_cv));

    f(x);
    f_v(x_v);
    f(p);
    f_v(p_v);
    f(r);
    f_v(r_v);

    BOOST_CHECK_EQUAL(0, x.value);
    BOOST_CHECK_EQUAL(0, x_v.value);
    BOOST_CHECK_EQUAL(0, p->value);
    BOOST_CHECK_EQUAL(0, p_v->value);
    BOOST_CHECK_EQUAL(0, r.get().value);
    BOOST_CHECK_EQUAL(0, r_v.get().value);
}

BOOST_AUTO_TEST_CASE(make_adjoin_function_test)
{
    auto f1 = [](int a) { return a != 0; };
    auto f2 = [](int a) { return a % 2; };

    auto f = ural::adjoin_functions(f1, f2);
    std::tuple<bool, int> x = f(5);

    BOOST_CHECK_EQUAL(true, std::get<0>(x));
    BOOST_CHECK_EQUAL(1, std::get<1>(x));
}

BOOST_AUTO_TEST_CASE(make_adjoint_function_constexpr_test)
{
    auto constexpr f1 = ural::divides<>{};
    auto constexpr f2 = ural::divides<int, void>{};
    auto constexpr f3 = ural::divides<void, int>{};
    auto constexpr f4 = ural::divides<int, int>{};

    auto constexpr tr = std::make_tuple(f1(5, 2), f2(5, 2), f3(5, 2), f4(5, 2));

    auto constexpr r1 = std::get<0>(tr);
    auto constexpr r2 = std::get<1>(tr);
    auto constexpr r3 = std::get<2>(tr);
    auto constexpr r4 = std::get<3>(tr);

    auto constexpr f = ural::adjoin_functions(f1, f2, f3, f4);

    static_assert(std::is_empty<decltype(f)>::value, "Must be empty class");

    constexpr std::tuple<int, int, int, int> x = f(5, 2);

    static_assert(std::get<0>(x) == 2, "");
    static_assert(std::get<1>(x) == 2, "");
    static_assert(std::get<2>(x) == 2, "");
    static_assert(std::get<3>(x) == 2, "");

    BOOST_CHECK_EQUAL(std::get<0>(x), 2);
    BOOST_CHECK_EQUAL(std::get<1>(x), 2);
    BOOST_CHECK_EQUAL(std::get<2>(x), 2);
    BOOST_CHECK_EQUAL(std::get<3>(x), 2);

    BOOST_CHECK_EQUAL(r1, std::get<0>(x));
    BOOST_CHECK_EQUAL(r2, std::get<1>(x));
    BOOST_CHECK_EQUAL(r3, std::get<2>(x));
    BOOST_CHECK_EQUAL(r4, std::get<3>(x));
}

BOOST_AUTO_TEST_CASE(value_function__test)
{
    typedef ural::value_function<int> Function;

    auto const n1 = 42;
    Function constexpr f1 = ural::make_value_function(n1);

    BOOST_CHECK_EQUAL(n1, f1());
    BOOST_CHECK_EQUAL(n1, f1(13));
    BOOST_CHECK_EQUAL(n1, f1(n1));
    BOOST_CHECK_EQUAL(n1, f1("abc", 13));
}

BOOST_AUTO_TEST_CASE(value_function_equality_test)
{
    typedef ural::value_function<int> Function;

    auto const n1 = 42;
    Function constexpr f1 = ural::make_value_function(n1);
    Function constexpr f2 = ural::make_value_function(13);

    auto constexpr r1 = f1();
    auto constexpr r2 = f2();

    static_assert(r1 != r2, "");

    static_assert(f1 == f1, "");
    static_assert(f2 == f2, "");
    static_assert(f2 != f1, "");
    static_assert(f1 != f2, "");

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(value_function_less_test)
{
    typedef ural::value_function<int> Function;

    auto const n42 = 42;
    Function constexpr f1 = ural::make_value_function(13);
    Function constexpr f2 = ural::make_value_function(n42);

    auto constexpr r1 = f1();
    auto constexpr r2 = f2();

    static_assert(r1 != r2, "");
    static_assert(r1 < r2, "");

    static_assert(f1 < f2, "");
    static_assert(f1 <= f2, "");
    static_assert(f2 > f1, "");
    static_assert(f2 >= f1, "");
    static_assert(!(f1 < f1), "");
    static_assert(!(f2 < f2), "");

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(replace_function_cref_test)
{
    auto const old_value = 13;
    auto const new_value = 42;

    auto const f = ural::make_replace_function(ural::cref(old_value),
                                               ural::cref = new_value);

    BOOST_CHECK(f == f);

    BOOST_CHECK_EQUAL(new_value, f(old_value));
    BOOST_CHECK_EQUAL(new_value, f(new_value));
}

BOOST_AUTO_TEST_CASE(replace_function_custom_predicate_test)
{
    auto constexpr old_value = 13;
    auto constexpr new_value = 42;

    auto constexpr f = ural::make_replace_function(old_value, new_value,
                                                   ural::greater_equal<>{});

    static_assert(12 == f(12), "");
    static_assert(new_value == f(13), "");
    static_assert(new_value == f(14), "");

    BOOST_CHECK(true);
}

namespace
{
    struct Equal
    {
    public:
        constexpr bool operator()(int x, int y)
        {
            return (x == y) && flag;
        }

        bool flag;

        friend constexpr bool operator==(Equal x, Equal y)
        {
            return x.flag == y.flag;
        }
    };
}

BOOST_AUTO_TEST_CASE(replace_function_equal_test)
{
    auto constexpr old_value = 13;
    auto constexpr new_value = 42;

    auto constexpr f1 = ural::make_replace_function(old_value, new_value, Equal{true});
    auto constexpr f2 = ural::make_replace_function(old_value+1, new_value, Equal{true});
    auto constexpr f3 = ural::make_replace_function(old_value, new_value+1, Equal{true});
    auto constexpr f4 = ural::make_replace_function(old_value, new_value, Equal{false});

    static_assert(f1 == f1, "");

    static_assert(f1 != f2, "");
    static_assert(f1 != f3, "");
    static_assert(f1 != f4, "");

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(min_element_accumulator_default_function_test)
{
    typedef ural::min_element_accumulator<int> Accumulator;
    Accumulator acc{42};

    static_assert(std::is_same<ural::less<>, Accumulator::compare_type>::value,
                  "Incorrect compare type");
    BOOST_CHECK_EQUAL(sizeof(ValueType<Accumulator>), sizeof(Accumulator));

    BOOST_CHECK_EQUAL(42, acc.result());
}

BOOST_AUTO_TEST_CASE(min_element_accumulator_move_ops_test)
{
    // Подготовка
    typedef std::string Element;
    typedef ural::min_element_accumulator<Element> Accumulator;

    std::vector<Element> xs = {"abc", "ab", "a", "abc"};

    auto const xs_old = xs;

    // Инициализация
    Accumulator acc(std::move(xs[0]));

    BOOST_CHECK(ural::empty(xs[0]));

    // Обновление - оператор ()
    BOOST_CHECK_LE(xs_old[1], acc.result());

    acc(std::move(xs[1]));

    BOOST_CHECK_EQUAL(xs_old[1], acc.result());
    BOOST_CHECK_NE(xs_old[1], xs[1]);

    // Обновление - update: true
    BOOST_CHECK_LE(xs_old[2], acc.result());

    bool const changed_1 = acc.update(std::move(xs[2]));

    BOOST_CHECK_EQUAL(xs_old[2], acc.result());
    BOOST_CHECK_NE(xs_old[2], xs[2]);
    BOOST_CHECK(changed_1);

    // Обновление - update: false
    bool const changed_2 = acc.update(std::move(xs[3]));
    BOOST_CHECK(!changed_2);
    BOOST_CHECK_EQUAL(xs_old[3], xs[3]);
}

namespace
{
    using Plus_assigns = boost::mpl::list<ural::plus_assign<int, int>,
                                          ural::plus_assign<int, void>,
                                          ural::plus_assign<void, int>,
                                          ural::plus_assign<>>;

    using Minus_assigns = boost::mpl::list<ural::minus_assign<int, int>,
                                           ural::minus_assign<int, void>,
                                           ural::minus_assign<void, int>,
                                           ural::minus_assign<>>;

    using Mult_assigns = boost::mpl::list<ural::multiplies_assign<int, int>,
                                          ural::multiplies_assign<int, void>,
                                          ural::multiplies_assign<void, int>,
                                          ural::multiplies_assign<>>;

    using Divides_assigns = boost::mpl::list<ural::divides_assign<int, int>,
                                             ural::divides_assign<int, void>,
                                             ural::divides_assign<void, int>,
                                             ural::divides_assign<>>;

    using Modulus_assigns = boost::mpl::list<ural::modulus_assign<int, int>,
                                             ural::modulus_assign<int, void>,
                                             ural::modulus_assign<void, int>,
                                             ural::modulus_assign<>>;
}
BOOST_AUTO_TEST_CASE_TEMPLATE(plus_assign_function_test, Function, Plus_assigns)
{
    auto x = 4;
    auto const y = 3;

    Function{}(x, y);

    BOOST_CHECK_EQUAL(7, x);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(minus_assign_function_test, Function, Minus_assigns)
{
    auto x = 4;
    auto const y = 3;

    Function{}(x, y);

    BOOST_CHECK_EQUAL(1, x);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(mult_assign_function_test, Function, Mult_assigns)
{
    auto x = 4;
    auto const y = 3;

    Function{}(x, y);

    BOOST_CHECK_EQUAL(12, x);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(divides_assign_function_test, Function, Divides_assigns)
{
    auto x = 18;
    auto const y = 7;

    Function{}(x, y);

    BOOST_CHECK_EQUAL(2, x);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(modulus_assign_function_test, Function, Modulus_assigns)
{
    auto x = 18;
    auto const y = 7;

    Function{}(x, y);

    BOOST_CHECK_EQUAL(4, x);
}

BOOST_AUTO_TEST_CASE(non_member_empty_for_c_array)
{
    int xs[] = {1, 2, 3, 4, 5};

    BOOST_CHECK(!ural::empty(xs));
}

BOOST_AUTO_TEST_CASE(ref_wrapper_test)
{
    typedef std::string T;
    T s("abc");
    T const & cs = s;

    // 1
    auto w_s = (ural::ref = s);

    BOOST_CHECK_EQUAL(&s, &w_s.get());
    static_assert(std::is_same<decltype(w_s), std::reference_wrapper<T>>::value, "");

    // 2
    auto w_s_2 = (ural::ref = w_s);

    BOOST_CHECK_EQUAL(&s, &w_s_2.get());
    static_assert(std::is_same<decltype(w_s_2), std::reference_wrapper<T>>::value, "");

    // 3
    auto w_cs = (ural::ref = cs);

    BOOST_CHECK_EQUAL(&s, &w_cs.get());
    static_assert(std::is_same<decltype(w_cs), std::reference_wrapper<T const>>::value, "");
}

BOOST_AUTO_TEST_CASE(cref_wrapper_test)
{
    typedef std::string T;
    T s("abc");
    T const & cs = s;

    // 1
    auto w_s = (ural::cref = s);

    BOOST_CHECK_EQUAL(&s, &w_s.get());
    static_assert(std::is_same<decltype(w_s), std::reference_wrapper<T const>>::value, "");

    // 2
    auto w_s_2 = (ural::cref = w_s);

    BOOST_CHECK_EQUAL(&s, &w_s_2.get());
    static_assert(std::is_same<decltype(w_s_2), std::reference_wrapper<T const>>::value, "");

    // 3
    auto w_cs = (ural::cref = cs);

    BOOST_CHECK_EQUAL(&s, &w_cs.get());
    static_assert(std::is_same<decltype(w_cs), std::reference_wrapper<T const>>::value, "");
}
