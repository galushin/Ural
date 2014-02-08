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

    BOOST_CHECK(cmp2 == cmp2);
    BOOST_CHECK(!(cmp2 != cmp2));

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

    BOOST_CHECK(cmp2 == cmp2);
    BOOST_CHECK(!(cmp2 != cmp2));

    for(int a = -10; a <= 10; ++ a)
    for(int b = -10; b <= 10; ++ b)
    {
        BOOST_CHECK_EQUAL(cmp1(a, b), cmp2(a, b));
    }
}

#include <boost/mpl/list.hpp>

typedef boost::mpl::list<ural::not_equal_to<int>,
                         ural::not_equal_to<>,
                         ural::not_equal_to<int, void>,
                         ural::not_equal_to<void, int>>
    Neq_functors;

BOOST_AUTO_TEST_CASE_TEMPLATE(not_equal_to_test, Functor, Neq_functors)
{
    constexpr Functor neq {};

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

// @todo ����� ��� tribool
typedef boost::mpl::list<ural::logical_not<bool>, ural::logical_not<>>
    Not_functors;

BOOST_AUTO_TEST_CASE_TEMPLATE(logical_not_test, Functor, Not_functors)
{
    constexpr Functor not_ {};

    static_assert(not_ == not_, "");
    static_assert(!(not_ != not_), "");

    static_assert(true == not_(false), "");
    static_assert(false == not_(true), "");

    BOOST_CHECK_EQUAL(true, not_(false));
    BOOST_CHECK_EQUAL(false, not_(true));
}

typedef boost::mpl::list<ural::logical_and<bool>,
                         ural::logical_and<>,
                         ural::logical_and<bool, void>,
                         ural::logical_and<void, bool>>
    And_functors;

BOOST_AUTO_TEST_CASE_TEMPLATE(logical_and_test, Functor, And_functors)
{
    constexpr Functor and_ {};

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

typedef boost::mpl::list<ural::logical_or<bool>,
                         ural::logical_or<>,
                         ural::logical_or<bool, void>,
                         ural::logical_or<void, bool>>
    Or_functors;

BOOST_AUTO_TEST_CASE_TEMPLATE(logical_or_test, Functor, Or_functors)
{
    constexpr Functor or_ {};

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

BOOST_AUTO_TEST_CASE(make_functor_for_member_var_test)
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

    auto f = ural::make_functor(&Type::first);

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

    // @todo ����� � ������ ����������� � reference_wrapper

    BOOST_CHECK_EQUAL(x.first, f(x));
    BOOST_CHECK_EQUAL(x.first, f(x_c));
    BOOST_CHECK_EQUAL(x.first, f(x_v));
    BOOST_CHECK_EQUAL(x.first, f(x_cv));

    BOOST_CHECK_EQUAL(x.first, f(p_x));
    BOOST_CHECK_EQUAL(x.first, f(p_x_c));
    BOOST_CHECK_EQUAL(x.first, f(p_x_v));
    BOOST_CHECK_EQUAL(x.first, f(p_x_cv));
}

BOOST_AUTO_TEST_CASE(make_functor_for_member_function_test)
{
    struct Inner
    {
    public:
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

    auto f = ural::make_functor(&Inner::do_something);
    auto f_v = ural::make_functor(&Inner::do_something_threadsafe);
    auto f_c = ural::make_functor(&Inner::get_something);
    auto f_cv = ural::make_functor(&Inner::get_something_threadsafe);

    Inner x = {42};
    Inner const x_c = x;
    Inner volatile x_v = x;
    Inner const volatile x_cv = x;

    BOOST_CHECK_EQUAL(x.value, f_c(x_c));
    BOOST_CHECK_EQUAL(x.value, f_cv(x_cv));

    f(x);
    f_v(x_v);

    BOOST_CHECK_EQUAL(0, x.value);
    BOOST_CHECK_EQUAL(0, x_v.value);

    // @todo ����� � (������) ����������� � reference_wrapper
}
