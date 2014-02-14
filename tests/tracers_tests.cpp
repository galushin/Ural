#include <boost/test/unit_test.hpp>

#include <ural/utility/tracers.hpp>

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

