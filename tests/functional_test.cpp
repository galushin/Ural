#include <boost/test/unit_test.hpp>

#include <ural/utility/tracers.hpp>
#include <ural/functional.hpp>

BOOST_AUTO_TEST_CASE(functor_tracer_test)
{
    typedef ural::functor_tracer<double(*)(double)> Functor;
    Functor f(std::abs);

    BOOST_CHECK_EQUAL(0, Functor::calls());

    f(1.0);

    BOOST_CHECK_EQUAL(1, Functor::calls());

    f(-1.0);

    BOOST_CHECK_EQUAL(2, Functor::calls());

    Functor::reset_calls();

    BOOST_CHECK_EQUAL(0, Functor::calls());
}

BOOST_AUTO_TEST_CASE(replace_functor_test)
{
    auto const old_value = -1;
    auto const new_value = 1;
    auto const other_value = 42;

    BOOST_CHECK(other_value != old_value);
    BOOST_CHECK(other_value != new_value);

    ural::replace_functor<int> const f{old_value, new_value};

    BOOST_CHECK_EQUAL(old_value, f.old_value());
    BOOST_CHECK_EQUAL(new_value, f.new_value());

    BOOST_CHECK_EQUAL(new_value, f(old_value));
    BOOST_CHECK_EQUAL(new_value, f(new_value));
    BOOST_CHECK_EQUAL(other_value, f(other_value));
}

BOOST_AUTO_TEST_CASE(replace_functor_test_custom_predicate)
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

    ural::replace_functor<decltype(old_value), decltype(eq)> const
        f{old_value, new_value, eq};

    BOOST_CHECK(eq(old_value, f.old_value()));
    BOOST_CHECK(eq(new_value, f.new_value()));

    BOOST_CHECK_EQUAL(eq, f.predicate());

    BOOST_CHECK(eq(new_value, f(old_value)));
    BOOST_CHECK(eq(new_value, f(new_value)));
    BOOST_CHECK(eq(other_value, f(other_value)));
}

BOOST_AUTO_TEST_CASE(compare_by_test)
{
    auto tr = [](int a) { return std::abs(a); };
    auto cmp1 = [=](int a, int b) {return tr(a) < tr(b);};
    auto cmp2 = ural::compare_by(tr);

    for(int a = -10; a <= 10; ++ a)
    for(int b = -10; b <= 10; ++ b)
    {
        BOOST_CHECK_EQUAL(cmp1(a, b), cmp2(a, b));
    }
}

BOOST_AUTO_TEST_CASE(compare_by_test_custom_compare)
{
    auto tr = [](int a) { return std::abs(a); };
    auto cmp_base = ural::greater<>{};

    auto cmp1 = [=](int a, int b) {return cmp_base(tr(a), tr(b));};
    auto cmp2 = ural::compare_by(tr, cmp_base);

    for(int a = -10; a <= 10; ++ a)
    for(int b = -10; b <= 10; ++ b)
    {
        BOOST_CHECK_EQUAL(cmp1(a, b), cmp2(a, b));
    }
}

BOOST_AUTO_TEST_CASE(not_equal_to_test)
{
    // @todo Тесты с выводом типов аргументов и разными типами аргументов
    constexpr ural::not_equal_to<int> neq {};

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

// @todo Тест для tribool
BOOST_AUTO_TEST_CASE(logical_not_test_bool)
{
    constexpr ural::logical_not<bool> not_;

    static_assert(not_ == not_, "");
    static_assert(!(not_ != not_), "");

    static_assert(true == not_(false), "");
    static_assert(false == not_(true), "");

    BOOST_CHECK_EQUAL(true, not_(false));
    BOOST_CHECK_EQUAL(false, not_(true));
}

BOOST_AUTO_TEST_CASE(logical_not_test_auto)
{
    constexpr ural::logical_not<> not_;

    static_assert(not_ == not_, "");
    static_assert(!(not_ != not_), "");

    static_assert(true == not_(false), "");
    static_assert(false == not_(true), "");

    BOOST_CHECK_EQUAL(true, not_(false));
    BOOST_CHECK_EQUAL(false, not_(true));
}
