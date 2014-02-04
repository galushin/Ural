#include <boost/test/unit_test.hpp>

#include <ural/archetypes.hpp>

#include <ural/algorithm.hpp>

BOOST_AUTO_TEST_CASE(archetype_check)
{
    struct Type {};
    ural::archetypes::functor<bool(Type)> pred;
    ural::archetypes::input_sequence<Type> in1;

    ural::value_consumer<bool>() = ural::all_of(in1, pred);
    ural::value_consumer<bool>() = ural::none_of(in1, pred);
    ural::value_consumer<bool>() = ural::any_of(in1, pred);

    ural::archetypes::functor<void(Type &)> action;

    ural::value_consumer<decltype(action)>() = ural::for_each(in1, action);

    ural::value_consumer<decltype(in1)>() = ural::find(in1, Type{});

    BOOST_CHECK(true);
}
