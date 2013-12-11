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

    auto const eq = [](No_equal_op const & x, No_equal_op const & y)
                    { return x.value == y.value; };

    BOOST_CHECK(!eq(other_value, old_value));
    BOOST_CHECK(!eq(other_value, new_value));

    ural::replace_functor<decltype(old_value), decltype(eq)> const
        f{old_value, new_value, eq};

    BOOST_CHECK(eq(old_value, f.old_value()));
    BOOST_CHECK(eq(new_value, f.new_value()));
    // @todo Проверить предикат

    BOOST_CHECK(eq(new_value, f(old_value)));
    BOOST_CHECK(eq(new_value, f(new_value)));
    BOOST_CHECK(eq(other_value, f(other_value)));
}
