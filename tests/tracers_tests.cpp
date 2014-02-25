#include <boost/test/unit_test.hpp>

#include <ural/utility/tracers.hpp>

BOOST_AUTO_TEST_CASE(regular_tracer_copy_ctor_test)
{
    typedef ural::regular_tracer<int> Type;

    Type x {0};

    auto const old_copy_ctor_count = Type::copy_ctor_count();
    auto const old_move_ctor_count = Type::move_ctor_count();
    auto const old_ctor_count = Type::constructed_objects();

    Type x1{x};

    BOOST_CHECK_EQUAL(old_copy_ctor_count + 1, Type::copy_ctor_count());
    BOOST_CHECK_EQUAL(old_ctor_count + 1, Type::constructed_objects());

    Type x2{std::move(x1)};

    BOOST_CHECK_EQUAL(old_move_ctor_count + 1, Type::move_ctor_count());
    BOOST_CHECK_EQUAL(old_ctor_count + 2, Type::constructed_objects());
}

BOOST_AUTO_TEST_CASE(regular_tracer_assign_test)
{
    typedef ural::regular_tracer<int> Type;

    Type x {0};
    Type y {42};

    auto const old_assign_count = Type::copy_assignments_count();
    auto const old_move_assign_count = Type::move_assignments_count();

    x = y;

    BOOST_CHECK_EQUAL(old_assign_count + 1, Type::copy_assignments_count());

    x = std::move(y);

    BOOST_CHECK_EQUAL(old_assign_count + 1, Type::copy_assignments_count());
    BOOST_CHECK_EQUAL(old_move_assign_count + 1, Type::move_assignments_count());
}

BOOST_AUTO_TEST_CASE(regular_tracer_equality_test)
{
    typedef ural::regular_tracer<int> Type;

    Type const x {0};
    Type const y {42};

    auto const old_eq_count = Type::equality_count();

    BOOST_CHECK_EQUAL(x.value() == y.value(), x == y);
    BOOST_CHECK_EQUAL(old_eq_count + 1, Type::equality_count());

    BOOST_CHECK_EQUAL(x.value() != y.value(), x != y);
    BOOST_CHECK_EQUAL(old_eq_count + 2, Type::equality_count());
}

BOOST_AUTO_TEST_CASE(regular_tracer_less_test)
{
    typedef ural::regular_tracer<int> Type;

    Type const x {0};
    Type const y {42};

    auto const old_less_count = Type::less_count();

    BOOST_CHECK_EQUAL(x.value() < y.value(), x < y);
    BOOST_CHECK_EQUAL(old_less_count + 1, Type::less_count());

    BOOST_CHECK_EQUAL(x.value() > y.value(), x > y);
    BOOST_CHECK_EQUAL(old_less_count + 2, Type::less_count());

    BOOST_CHECK_EQUAL(x.value() <= y.value(), x <= y);
    BOOST_CHECK_EQUAL(old_less_count + 3, Type::less_count());

    BOOST_CHECK_EQUAL(x.value() >= y.value(), x >= y);
    BOOST_CHECK_EQUAL(old_less_count + 4, Type::less_count());
}

