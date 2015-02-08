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

#include <ural/numeric/numbers_sequence.hpp>
#include <ural/flex_string.hpp>

#include <boost/test/unit_test.hpp>
#include <boost/mpl/list.hpp>

#include <string>
#include <sstream>

namespace
{
    template <class T>
    class test_allocator
    {
        typedef std::allocator<T> Base;
    public:
        typedef typename Base::value_type value_type;
        typedef typename Base::size_type size_type;
        typedef typename Base::difference_type difference_type;
        typedef typename Base::const_reference const_reference;
        typedef typename Base::reference reference;
        typedef typename Base::pointer pointer;
        typedef typename Base::const_pointer const_pointer;

        typedef std::true_type propagate_on_container_swap;

        explicit test_allocator(int id = 0)
         : id_{id}
        {}

        pointer allocate(size_type n)
        {
            return a_.allocate(n);
        }

        void deallocate(pointer p, size_type n)
        {
            return a_.deallocate(p, n);
        }

        template <class... Args>
        void construct(pointer p, Args && ... args)
        {
            a_.construct(p, std::forward<Args>(args)...);
        }

        void destroy(pointer p)
        {
            a_.destroy(p);
        }

        template <class U>
        struct rebind
        {
            typedef test_allocator<U> other;
        };

        int id() const
        {
            return this->id_;
        }

    private:
        int id_;
        std::allocator<T> a_;
    };

    typedef ural::flex_string<char, ural::use_default, test_allocator<char>,
                              ural::string_vector_storage<char, test_allocator<char>>>
        Vector_string;

    typedef ural::flex_string<char, ural::use_default, test_allocator<char>>
        String;

    typedef boost::mpl::list<String, Vector_string> Strings_list;
}

// Интеграция с std::string
BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_from_std_string, String, Strings_list)
{
    std::string const s{"Stepanov"};
    String const fs{s};

    BOOST_CHECK_EQUAL(s.c_str(), fs.c_str());
}

// 21.4.2 Конструкторы
BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_default_ctor, String, Strings_list)
{
    String s;

    BOOST_CHECK_EQUAL(0U, s.size());
    BOOST_CHECK(nullptr != s.data());
    BOOST_CHECK_GE(s.capacity(), s.size());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_allocator_ctor, String, Strings_list)
{
    typename String::allocator_type a{42};

    String s{a};

    BOOST_CHECK_EQUAL(0U, s.size());
    BOOST_CHECK_GE(s.capacity(), s.size());
    BOOST_CHECK(nullptr != s.data());
    BOOST_CHECK_EQUAL(a.id(), s.get_allocator().id());
}

// Конструктор копирования
BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_copy_ctor, String, Strings_list)
{
    char const * cs = "Hello, world";
    String s1(cs);
    String const s2 = s1;

    typedef typename String::traits_type Traits;

    BOOST_CHECK_EQUAL(s1.size(), Traits::length(cs));
    BOOST_CHECK_EQUAL(s1.c_str(), s2.c_str());
    BOOST_CHECK_EQUAL(s1.c_str(), s2.c_str());

    s1[0] = 'W';

    BOOST_CHECK_EQUAL(s2.c_str(), cs);
}

// Копирование фрагмента строки
BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_ctor_from_pos, String, Strings_list)
{
    char const * cs = "Hello, world";

    std::string const s(cs);
    String const fs(cs);

    BOOST_CHECK_THROW(String(fs, fs.size() + 2), std::out_of_range);

    BOOST_CHECK_EQUAL(s.size(), fs.size());

    std::string const s1{s, 2};
    String const fs1(fs, 2);

    BOOST_CHECK_EQUAL(s1.size(), fs1.size());
    BOOST_CHECK_EQUAL(s1.c_str(), fs1.c_str());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_ctor_from_pos_npos, String, Strings_list)
{
    char const * cs = "Hello, world";

    std::string const s(cs);
    String const fs(cs);

    std::string const s1{s, 2, 2};
    String const fs1(fs, 2, 2);

    BOOST_CHECK_EQUAL(s1.c_str(), fs1.c_str());

    std::string const s2{s, 2, s.size()};
    String const fs2(fs, 2, fs.size());

    BOOST_CHECK_EQUAL(s2.c_str(), fs2.c_str());

    typename String::allocator_type a{42};
    String fsa(fs, 2, 2, a);

    BOOST_CHECK_EQUAL(s1.c_str(), fsa.c_str());
    BOOST_CHECK_EQUAL(a.id(), fsa.get_allocator().id());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_from_c_str_n, String, Strings_list)
{
    char const * cs = "Hello, world";
    String const fs(cs, 4);
    std::string const s(cs, 4);

    typedef typename String::allocator_type Alloc;

    BOOST_CHECK_EQUAL(s.c_str(), fs.c_str());
    BOOST_CHECK_EQUAL(Alloc{}.id(), fs.get_allocator().id());

    Alloc a{42};

    String const fsa(cs, 4, a);

    BOOST_CHECK_EQUAL(s.c_str(), fsa.c_str());
    BOOST_CHECK_EQUAL(a.id(), fsa.get_allocator().id());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_from_c_str, String, Strings_list)
{
    char const * cs = "Hello, world";
    String s(cs);

    typedef typename String::allocator_type Alloc;
    typedef typename String::traits_type Traits;

    BOOST_CHECK_EQUAL(Alloc{}.id(), s.get_allocator().id());

    BOOST_CHECK(cs != s.data());
    BOOST_CHECK(nullptr != s.data());

    BOOST_CHECK_EQUAL(Traits::length(cs), s.size());
    BOOST_CHECK_EQUAL(std::strlen(cs), s.size());
    BOOST_CHECK_GE(s.capacity(), s.size());

    BOOST_CHECK(std::strcmp(cs, s.c_str()) == 0);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_from_c_str_with_allocator, String, Strings_list)
{
    typedef typename String::allocator_type Alloc;
    typedef typename String::traits_type Traits;

    Alloc a{42};
    char const * cs = "Hello, world";

    String const s{cs, a};

    BOOST_CHECK_EQUAL(a.id(), s.get_allocator().id());

    BOOST_CHECK(cs != s.data());
    BOOST_CHECK(nullptr != s.data());

    BOOST_CHECK_EQUAL(Traits::length(cs), s.size());
    BOOST_CHECK_EQUAL(std::strlen(cs), s.size());
    BOOST_CHECK_GE(s.capacity(), s.size());

    BOOST_CHECK(std::strcmp(cs, s.c_str()) == 0);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_from_n_char, String, Strings_list)
{
    auto const n = 13U;
    auto const C = 'a';

    String const s(n, C);
    std::string const s0(n, C);

    typedef typename String::allocator_type Alloc;
    typedef typename String::traits_type Traits;

    BOOST_CHECK_EQUAL(Alloc{}.id(), s.get_allocator().id());
    BOOST_CHECK(nullptr != s.data());
    BOOST_CHECK_EQUAL(n, s.size());
    BOOST_CHECK_EQUAL(n, s0.size());
    BOOST_CHECK_GE(s.capacity(), s.size());

    BOOST_CHECK_EQUAL_COLLECTIONS(s0.begin(), s0.end(), s.begin(), s.end());

    for(auto i : ural::numbers(0, n))
    {
        BOOST_CHECK_EQUAL(C, s[i]);
        BOOST_CHECK(Traits::eq(C, s[i]));
    }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_from_n_char_and_allocator, String, Strings_list)
{
    typename String::allocator_type a{42};

    auto const n = 13U;
    auto const C = 'a';

    String const s(n, C, a);

    typedef typename String::traits_type Traits;

    BOOST_CHECK_EQUAL(a.id(), s.get_allocator().id());
    BOOST_CHECK(nullptr != s.data());
    BOOST_CHECK_EQUAL(n, s.size());
    BOOST_CHECK_GE(s.capacity(), s.size());

    for(auto i : ural::numbers(0, s.size()))
    {
        BOOST_CHECK(Traits::eq(s[i], C));
    }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_from_iterators, String, Strings_list)
{
    std::string const src = "Stepanov";
    std::istringstream is(src);

    String const s{std::istream_iterator<char>(is),
                   std::istream_iterator<char>()};

    BOOST_CHECK_EQUAL(src.size(), s.size());
    BOOST_CHECK_EQUAL(src.c_str(), s.c_str());

    BOOST_CHECK_EQUAL_COLLECTIONS(src.begin(), src.end(),
                                  s.begin(), s.end());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_from_iterators_and_allocator, String, Strings_list)
{
    typename String::allocator_type a{42};
    std::string const src = "Stepanov";
    std::istringstream is(src);

    String const s{std::istream_iterator<char>(is),
                   std::istream_iterator<char>(), a};

    BOOST_CHECK_EQUAL(a.id(), s.get_allocator().id());
    BOOST_CHECK_EQUAL(src.size(), s.size());
    BOOST_CHECK_EQUAL(src.c_str(), s.c_str());

    for(size_t i = 0; i != src.size(); ++ i)
    {
        BOOST_CHECK_EQUAL(src[i], s[i]);
    }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_from_init_list, String, Strings_list)
{
    String const fs{'a', 'B', 'c'};
    std::string const s{'a', 'B', 'c'};

    BOOST_CHECK_EQUAL(s.c_str(), fs.c_str());

    typename String::allocator_type a{42};

    String const fsa{{'a', 'B', 'c'}, a};
    BOOST_CHECK_EQUAL(fsa.c_str(), s);
    BOOST_CHECK_EQUAL(a.id(), fsa.get_allocator().id());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_from_flex_string_and_allocator, String, Strings_list)
{
    typename String::allocator_type a{42};
    String const s1{"Step"};

    String const s2{s1, a};

    BOOST_CHECK_EQUAL(a.id(), s2.get_allocator().id());
    BOOST_CHECK_EQUAL(s1, s2);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_move_ctor, String, Strings_list)
{
    String s{"Alpha"};
    String const s_old = s;

    String s1(std::move(s));

    BOOST_CHECK_EQUAL(s1, s_old);
    BOOST_CHECK(s.empty());
}

// @todo Конструктор на основе временной строки и распределителя памяти

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_operator_assign, String, Strings_list)
{
    String const s{"Step"};
    String s0{"Alpha"};

    s0 = s;

    BOOST_CHECK_EQUAL_COLLECTIONS(s.begin(), s.end(),
                                  s0.begin(), s0.end());
    BOOST_CHECK_EQUAL(s, s0);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_operator_move_assign, String, Strings_list)
{
    String s{"Step"};
    String s0{"Alpha"};
    String const s0_copy = s0;

    auto * data_old = s0.data();

    s = std::move(s0);

    BOOST_CHECK_EQUAL(s, s0_copy);
    BOOST_CHECK_EQUAL(data_old, s.data());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_operator_assign_c_str, String, Strings_list)
{
    char const * s= "Step";
    String s0{"Alpha"};

    s0 = s;

    BOOST_CHECK_EQUAL(s, s0.c_str());
}

// 21.4.3 Поддержка итераторов
BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_mutable_iterators, String, Strings_list)
{
    std::string const src{"Stepanov"};

    String s(src.size(), 'x');

    BOOST_CHECK_EQUAL(s.size(), src.size());

    BOOST_CHECK(!ural::equal(src, s));

    typedef typename String::iterator Iterator;

    Iterator const res = std::copy(src.begin(), src.end(), s.begin());

    BOOST_CHECK_EQUAL_COLLECTIONS(src.begin(), src.end(), s.begin(), s.end());
    BOOST_CHECK(res == s.end());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_const_iterators, String, Strings_list)
{
    std::string const src{"Stepanov"};

    String const s(src.begin(), src.end());

    BOOST_CHECK_EQUAL_COLLECTIONS(src.begin(), src.end(), s.begin(), s.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(src.begin(), src.end(), s.cbegin(), s.cend());

    typedef typename String::const_iterator Iterator;

    Iterator cb = s.begin();
    Iterator ce = s.end();

    BOOST_CHECK(cb == s.cbegin());
    BOOST_CHECK(ce == s.end());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_reverse_iterators, String, Strings_list)
{
    String s1 = "Stepanov";

    typename String::const_reverse_iterator crb = s1.crbegin();
    typename String::const_reverse_iterator cre = s1.crend();

    String const s2{crb, cre};

    typename String::reverse_iterator rb = s1.rbegin();
    typename String::reverse_iterator re = s1.rend();

    std::reverse(rb, re);

    BOOST_CHECK_EQUAL(s1, s2);
}

// 21.4.4 Ёмкость
BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_size_and_lenght, String, Strings_list)
{
    String const s = "Stepanov";

    BOOST_CHECK_EQUAL(std::strlen(s.c_str()), s.size());
    BOOST_CHECK_EQUAL(s.size(), s.length());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_max_size, String, Strings_list)
{
    String s;

    std::allocator<typename String::value_type> a;

    BOOST_CHECK_GE(s.max_size(), 0U);
    BOOST_CHECK_GE(a.max_size(), s.max_size());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_resize_less, String, Strings_list)
{
    String s = "Stepanov";

    auto const n = 4U;

    s.resize(n, 'a');

    BOOST_CHECK_EQUAL(n, s.size());

    BOOST_CHECK_EQUAL("Step", s.c_str());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_resize_equal, String, Strings_list)
{
    String s = "Stepanov";
    String const s_old = s;

    s.resize(s.size(), 'a');

    BOOST_CHECK_EQUAL(s_old.size(), s.size());
    BOOST_CHECK_EQUAL(s_old.c_str(), s.c_str());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_resize_greater, String, Strings_list)
{
    String s = "Stepanov";
    String const s_old = s;

    auto const filler = 'x';
    s.resize(2 * s.size(), filler);

    BOOST_CHECK_EQUAL(2*s_old.size(), s.size());

    for(auto i : ural::numbers(0, s_old.size()))
    {
        BOOST_CHECK_EQUAL(s_old[i], s[i]);
    }

    for(auto i : ural::numbers(s_old.size(), s.size()))
    {
        BOOST_CHECK_EQUAL(filler, s[i]);
    }

    BOOST_CHECK_EQUAL(char{}, s.data()[s.size()]);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_resize_less_default, String, Strings_list)
{
    String s = "Stepanov";

    s.resize(4);

    BOOST_CHECK_EQUAL(4U, s.size());

    BOOST_CHECK_EQUAL("Step", s.c_str());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_resize_equal_default, String, Strings_list)
{
    String s = "Stepanov";
    String const s_old = s;

    s.resize(s.size());

    BOOST_CHECK_EQUAL(s_old.size(), s.size());
    BOOST_CHECK_EQUAL(s_old.c_str(), s.c_str());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_resize_greater_default, String, Strings_list)
{
    String s = "Stepanov";
    String const s_old = s;

    s.resize(2 * s.size());

    BOOST_CHECK_EQUAL(2*s_old.size(), s.size());

    for(size_t i = 0; i != s_old.size(); ++ i)
    {
        BOOST_CHECK_EQUAL(s_old[i], s[i]);
    }

    char const filler = 0;
    for(size_t i = s_old.size(); i != s.size(); ++ i)
    {
        BOOST_CHECK_EQUAL(filler, s[i]);
    }

    BOOST_CHECK_EQUAL(char{}, s.data()[s.size()]);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_shrink_to_fit, String, Strings_list)
{
    auto const n = 100U;

    String fs(n, 'a');

    fs.resize(fs.size() / 2);

    BOOST_CHECK_GE(fs.capacity(), fs.size());
    BOOST_CHECK_GE(fs.capacity(), n);

    auto const fs_old = fs;

    fs.shrink_to_fit();

    BOOST_CHECK_EQUAL(fs_old, fs);
    BOOST_CHECK_EQUAL(fs.capacity(), fs.size());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_reserve_shrink_capacity, String, Strings_list)
{
    auto const n = 100U;

    String fs(n, 'h');

    fs.resize(n / 4);

    BOOST_CHECK_GE(fs.capacity(), n);

    fs.reserve(n / 2);

    BOOST_CHECK_EQUAL(fs.capacity(), n / 2);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_reserve, String, Strings_list)
{
    String fs;
    std::string s;

    auto const n = 10U;
    fs.reserve(n);

    BOOST_CHECK_GE(fs.capacity(), n);

    const void * const old_data = fs.data();

    for(; fs.size() < n;)
    {
        char c = 'a';
        fs.push_back(c);
        s.push_back(c);

        BOOST_CHECK_EQUAL(fs, s.c_str());

        const void * const new_data = fs.data();
        BOOST_CHECK_EQUAL(old_data, new_data);
    }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_clear, String, Strings_list)
{
    String s = "Stepanov";
    s.clear();

    static_assert(noexcept(&String::clear), "");

    BOOST_CHECK_EQUAL("", s.c_str());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_empty, String, Strings_list)
{
    String const s0 {};
    String const s1 = {"Stepanov"};

    BOOST_CHECK(s0.empty());
    BOOST_CHECK(!s1.empty());
}

// 21.4.5 доступ к элементам
BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_index_access_operator, String, Strings_list)
{
    const char * cs = "Stepanov";

    String const s(cs);

    for(size_t i = 0; i <= s.size(); ++ i)
    {
        BOOST_CHECK_EQUAL(cs[i], s[i]);
    }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_at_const, String, Strings_list)
{
    String const src = "Stepanov";

    for(size_t i = 0; i != src.size(); ++ i)
    {
        BOOST_CHECK_EQUAL(src[i], src.at(i));
    }

    BOOST_CHECK_THROW(src.at(src.size()), std::out_of_range);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_at, String, Strings_list)
{
    String src = "Stepanov";

    for(size_t i = 0; i != src.size(); ++ i)
    {
        char const value = 'Q';
        src.at(i) = value;
        BOOST_CHECK_EQUAL(value, src[i]);
    }

    BOOST_CHECK_THROW(src.at(src.size() + 2), std::out_of_range);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_front, String, Strings_list)
{
    String s = "stepanov";

    String const & cr = s;

    BOOST_CHECK_EQUAL(s[0], cr.front());

    auto const value = 'S';
    s.front() = value;
    BOOST_CHECK_EQUAL(cr.front(), value);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_back, String, Strings_list)
{
    String s = "Stepanov";

    String const & cr = s;

    BOOST_CHECK_EQUAL(s[s.size() - 1], cr.back());

    auto const value = 'V';
    s.back() = value;
    BOOST_CHECK_EQUAL(cr.back(), value);
}

// @todo 21.4.6 модификаторы
BOOST_AUTO_TEST_CASE_TEMPLATE(felx_string_append_string, String, Strings_list)
{
    std::string s1 = "Step";
    std::string const s2 = "anov";

    String fs1{s1.c_str()};
    String const fs2{s2.c_str()};

    s1 += s2;
    fs1 += fs2;

    BOOST_CHECK_EQUAL(s1.c_str(), fs1.c_str());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(felx_string_append_c_string, String, Strings_list)
{
    std::string s1 = "Step";
    char const * s2 = "anov";

    String fs1{s1.c_str()};

    s1 += s2;
    fs1 += s2;

    BOOST_CHECK_EQUAL(s1.c_str(), fs1.c_str());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(felx_string_append_char, String, Strings_list)
{
    std::string s1 = "Step";
    char const c = 's';

    String fs1{s1.c_str()};

    s1 += c;
    fs1 += c;

    BOOST_CHECK_EQUAL(s1.c_str(), fs1.c_str());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_append_init_list, String, Strings_list)
{
    std::string s1 = "Step";
    String fs1 {s1.c_str()};

    s1 += {'a', 'n', 'o', 'v'};
    fs1 += {'a', 'n', 'o', 'v'};

    BOOST_CHECK_EQUAL(s1.c_str(), fs1.c_str());
}

// 21.4.6.2 Append
BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_append_sunstring, String, Strings_list)
{
    std::string s1 = "Step";
    std::string const s2 = "Ivanovich";

    String fs1{s1.c_str()};
    String const fs2{s2.c_str()};

    s1.append(s2, 2, 4);
    fs1.append(fs2, 2, 4);

    BOOST_CHECK_EQUAL(s1.c_str(), fs1.c_str());

    BOOST_CHECK_THROW(fs1.append(fs2, fs2.size() + 2, 4), std::out_of_range);
}

// 21.4.6.3 assign
BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_assign_string, String, Strings_list)
{
    std::string s1 = "Ivanovich";
    std::string const s2 = "Step";

    String fs1(s1.c_str());
    String const fs2(s2.c_str());

    s1.assign(s2);
    fs1.assign(fs2);

    BOOST_CHECK_EQUAL(s1.c_str(), fs1.c_str());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_assign_rvalue_string, String, Strings_list)
{
    String s1 = "Ivanovich";
    String s2 = "Step";

    String const s1_old = s1;

    s2.assign(std::move(s1));

    BOOST_CHECK_EQUAL(s2.c_str(), s1_old.c_str());

    static_assert(noexcept(s2.assign(std::move(s1))), "");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_assign_c_string, String, Strings_list)
{
    std::string const s = "Stepanov";

    String fs;
    fs.assign(s.c_str());

    BOOST_CHECK_EQUAL(s.c_str(), fs.c_str());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_assign_init_list, String, Strings_list)
{
    std::string s1 = "Step";
    String fs1 {s1.c_str()};

    s1.assign({'a', 'n', 'o', 'v'});
    fs1.assign({'a', 'n', 'o', 'v'});

    BOOST_CHECK_EQUAL(s1.c_str(), fs1.c_str());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_assign_n_chars, String, Strings_list)
{
    std::string s1 = "Step";
    String fs1 {s1.c_str()};

    s1.assign(7, 'a');
    fs1.assign(7, 'a');

    BOOST_CHECK_EQUAL(s1.c_str(), fs1.c_str());
}

// 21.4.6.4 insert
BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_insert_string, String, Strings_list)
{
    std::string s1 = "Stov";
    std::string const s2 = "epan";

    String fs1(s1.c_str());
    String const fs2(s2.c_str());

    s1.insert(2, s2);
    fs1.insert(2, fs2);

    BOOST_CHECK_EQUAL(s1.c_str(), fs1);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_insert_substring, String, Strings_list)
{
    std::string s = "Stov";
    std::string const s2 = "Depande";

    String fs(s.c_str());
    String fs2(s2.c_str());

    s.insert(2, s2, 1, 4);
    fs.insert(2, fs2, 1, 4);

    BOOST_CHECK_EQUAL(s.c_str(), fs);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_insert_c_string, String, Strings_list)
{
    std::string s = "Stov";
    char const * cs = "epan";

    String fs(s.c_str());

    s.insert(2, cs);
    fs.insert(2, cs);

    BOOST_CHECK_EQUAL(s.c_str(), fs);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_insert_char_at_iterator, String, Strings_list)
{
    std::string s = "ABC";
    String fs{s.c_str()};

    s.insert(s.begin() + 2, 'z');
    fs.insert(fs.cbegin() + 2, 'z');

    BOOST_CHECK_EQUAL(s.c_str(), fs);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_insert_n_chars_at_iterator, String, Strings_list)
{
    std::string s = "ABC";
    String fs{s.c_str()};

    s.insert(s.begin() + 2, 5, 'z');
    fs.insert(fs.cbegin() + 2, 5, 'z');

    BOOST_CHECK_EQUAL(s.c_str(), fs);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_insert_init_list_at_iterator, String, Strings_list)
{
    std::string s = "ABC";
    String fs{s.c_str()};

    s.insert(s.begin() + 2, {'a', 'n', 'o', 'v'});
    fs.insert(fs.cbegin() + 2, {'a', 'n', 'o', 'v'});

    BOOST_CHECK_EQUAL(s.c_str(), fs);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_insert_range, String, Strings_list)
{
    std::istringstream is_1("ABC");
    std::istringstream is_2("ABC");

    String fs{"Memory"};
    std::string s{fs.c_str()};

    typedef std::istream_iterator<char> Iterator;

    s.insert(s.begin() + 2, Iterator(is_1), Iterator{});
    fs.insert(fs.begin() + 2, Iterator(is_2), Iterator{});

    BOOST_CHECK_EQUAL(s.c_str(), fs);
}

// 21.4.6.5 erase
BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_erase_iterator_range, String, Strings_list)
{
    std::string s = "Stepanov";
    String fs{s.c_str()};

    s.erase(s.begin() + 2, s.end() - 1);
    fs.erase(fs.begin() + 2, fs.end() - 1);

    BOOST_CHECK_EQUAL(s.c_str(), fs);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_pop_back, String, Strings_list)
{
    std::string s = "ABC";
    String fs{s.c_str()};

    s.pop_back();
    fs.pop_back();

    BOOST_CHECK_EQUAL(s.c_str(), fs);
}

// replace
BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_replace_pos_n, String, Strings_list)
{
    // основано на http://www.cplusplus.com/reference/string/string/replace/
    std::string s{"this is a test string"};
    std::string const s2{"n example"};
    std::string const s3{"sample phrase"};

    String fs{s};
    String const fs2{s2};
    String const fs3{s3};

    s.replace(9, 5, s2);
    fs.replace(9, 5, fs2);

    BOOST_CHECK_EQUAL(s, fs.c_str());
    BOOST_CHECK_THROW(fs.replace(fs.size() + 1, 2, fs2), std::out_of_range);

    s.replace(19,6,s3,7,6);
    fs.replace(19,6,fs3,7,6);

    BOOST_CHECK_EQUAL(s, fs.c_str());
    BOOST_CHECK_THROW(fs.replace(fs.size() + 1, 2, fs3, 7, 6), std::out_of_range);
    BOOST_CHECK_THROW(fs.replace(19, 6, fs3, fs3.size() + 1, 6), std::out_of_range);

    s.replace(8,10,"just a");
    fs.replace(8,10,"just a");

    BOOST_CHECK_EQUAL(s, fs.c_str());
    BOOST_CHECK_THROW(fs.replace(fs.size() + 1,10,"just a"), std::out_of_range);

    s.replace(s.size(),1,3,'!');
    fs.replace(fs.size(),1,3,'!');

    BOOST_CHECK_EQUAL(s, fs.c_str());
    BOOST_CHECK_THROW(fs.replace(fs.size()+1,1,3,'!');, std::out_of_range);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_replace_iterators, String, Strings_list)
{
    // основано на http://www.cplusplus.com/reference/string/string/replace/
    std::string s{"this is a test string"};
    std::string const s2{"n example"};
    std::string const s3{"sample phrase"};

    String fs{s};
    String const fs2{s2};
    String const fs3{s3};

    s.replace(9, 5, s2);
    fs.replace(fs.begin() + 9, fs.begin() + 14, fs2);

    BOOST_CHECK_EQUAL(s, fs.c_str());
    BOOST_CHECK_THROW(fs.replace(fs.size() + 1, 2, fs2), std::out_of_range);
}

// 21.4.6.7 copy
BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_copy_from_0, String, Strings_list)
{
    String const fs{"Stepanov"};

    std::vector<typename String::value_type> s(5, 'x');

    auto const rlen = fs.copy(s.data(), s.size());

    BOOST_CHECK_EQUAL(rlen, s.size());

    auto const fs_sub = fs.substr(0, rlen);

    BOOST_CHECK_EQUAL(rlen, fs_sub.size());

    BOOST_CHECK_EQUAL_COLLECTIONS(fs_sub.cbegin(), fs_sub.cend(),
                                  s.cbegin(), s.cend());

    BOOST_CHECK_THROW(fs.copy(s.data(), s.size(), fs.size() + 1),
                      std::out_of_range);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_copy, String, Strings_list)
{
    String const fs{"Stepanov"};

    std::vector<typename String::value_type> s(5, 'x');

    auto const pos = 0;

    auto const rlen = fs.copy(s.data(), s.size(), pos);

    auto const fs_sub = fs.substr(pos, rlen);

    BOOST_CHECK_EQUAL_COLLECTIONS(fs_sub.cbegin(), fs_sub.cend(),
                                  s.cbegin(), s.cend());

    BOOST_CHECK_THROW(fs.copy(s.data(), s.size(), fs.size() + 1),
                      std::out_of_range);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_copy_to_end, String, Strings_list)
{
    String const fs{"Stepanov"};

    std::vector<typename String::value_type> s(fs.size() * 2, 'x');

    auto const pos = fs.size() - 1;

    auto const rlen = fs.copy(s.data(), fs.size(), pos);

    auto const fs_sub = fs.substr(pos, rlen);

    BOOST_CHECK_EQUAL_COLLECTIONS(fs_sub.cbegin(), fs_sub.cend(),
                                  s.cbegin(), s.cbegin() + rlen);
}

// 21.4.7 операции со строками

// @todo 21.4.7.1 Функции доступа

// @todo 21.4.7.2 find

// @todo 21.4.7.3
// @todo 21.4.7.4
// @todo 21.4.7.5
// @todo 21.4.7.6
// @todo 21.4.7.7

// 21.4.7.8 substr
BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_substr, String, Strings_list)
{
    std::string const s = "Stepanov";
    String const fs{s.c_str()};

    std::string const sub_s = s.substr(2, 2);
    String const sub_fs = fs.substr(2, 2);

    BOOST_CHECK_EQUAL(sub_s.c_str(), sub_fs.c_str());

    std::string const sub_s_2 = s.substr(2, s.size());
    String const sub_fs_2 = fs.substr(2, fs.size());

    BOOST_CHECK_EQUAL(sub_s_2.c_str(), sub_fs_2.c_str());

    BOOST_CHECK_THROW(fs.substr(fs.size() + 1), std::out_of_range);
}

// @todo 21.4.7.9 compare

// @todo 21.4.8 вспомогательные функции

// 21.4.8.1 Оператор +
BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_plus, String, Strings_list)
{
    String const fs1{"Paper"};
    String const fs2{"clip"};

    std::string const s1{fs1.c_str()};
    std::string const s2{fs2.c_str()};

    String const fs = fs1 + fs2;
    std::string const s = s1 + s2;

    BOOST_CHECK_EQUAL(s.c_str(), fs.c_str());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_plus_rvalue_first, String, Strings_list)
{
    String fs1{"Paper"};
    String fs2{"clip"};

    String const fs = fs1 + fs2;
    String const fs_moved = std::move(fs1) + fs2;

    BOOST_CHECK_EQUAL(fs, fs_moved);

    BOOST_CHECK(fs1.empty());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_plus_rvalue_second, String, Strings_list)
{
    String fs1{"Paper"};
    String fs2{"clip"};

    String const fs = fs1 + fs2;
    String const fs_moved = fs1 + std::move(fs2);

    BOOST_CHECK_EQUAL(fs, fs_moved);

    BOOST_CHECK(fs2.empty());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_plus_rvalue_both, String, Strings_list)
{
    String fs1{"Paper"};
    String fs2{"clip"};

    String const fs = fs1 + fs2;
    String const fs_moved = std::move(fs1) + std::move(fs2);

    BOOST_CHECK_EQUAL(fs, fs_moved);

    BOOST_CHECK(fs1.empty() || fs2.empty());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_plus_c_str_left, String, Strings_list)
{
    char const * s1 = "Paper";
    String const fs2{"clip"};
    std::string s2{fs2.c_str()};

    std::string const s = s1 + s2;
    String const fs = s1 + fs2;

    BOOST_CHECK_EQUAL(s.c_str(), fs.c_str());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_plus_c_str_left_rvalue, String, Strings_list)
{
    char const * s1 = "Paper";
    String fs2{"clip"};

    String const fs = s1 + fs2;
    String const fs_moved = s1 + std::move(fs2);

    BOOST_CHECK(fs2.empty());
    BOOST_CHECK_EQUAL(fs, fs_moved);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_plus_char_left, String, Strings_list)
{
    char const c = 'c';
    String const fs2{"lip"};
    std::string s2{fs2.c_str()};

    std::string const s = c + s2;
    String const fs = c + fs2;

    BOOST_CHECK_EQUAL(s.c_str(), fs.c_str());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_plus_char_left_rvalue, String, Strings_list)
{
    char const c = 'c';
    String fs2{"lip"};

    String const fs = c + fs2;
    String const fs_moved = c + std::move(fs2);

    BOOST_CHECK(fs2.empty());
    BOOST_CHECK_EQUAL(fs2, "");
    BOOST_CHECK_EQUAL(fs, fs_moved);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_plus_c_str_right, String, Strings_list)
{
    char const * s1 = "Paper";
    String const fs2{"clip"};
    std::string s2{fs2.c_str()};

    std::string const s = s2 + s1;
    String const fs = fs2 + s1;

    BOOST_CHECK_EQUAL(s.c_str(), fs.c_str());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_plus_c_str_right_rvalue, String, Strings_list)
{
    char const * s1 = "Paper";
    String fs2{"clip"};

    String const fs = fs2 + s1;
    String const fs_moved =  std::move(fs2) + s1;

    BOOST_CHECK(fs2.empty());
    BOOST_CHECK_EQUAL(fs, fs_moved);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_plus_char_right, String, Strings_list)
{
    char const c = 'c';
    String const fs2{"lip"};
    std::string s2{fs2.c_str()};

    std::string const s = s2 + c;
    String const fs = fs2 + c;

    BOOST_CHECK_EQUAL(s.c_str(), fs.c_str());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_plus_char_right_rvalue, String, Strings_list)
{
    char const c = 'c';
    String fs2{"lip"};

    String const fs = fs2 + c;
    String const fs_moved = std::move(fs2) + c;

    BOOST_CHECK(fs2.empty());
    BOOST_CHECK_EQUAL(fs, fs_moved);
}

// 21.4.8.2 Оператор ==
BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_equality, String, Strings_list)
{
    String const s1{"Paper"};
    String const s2{"Pair"};
    String const s3(s2);

    BOOST_CHECK(s1 == s1);
    BOOST_CHECK(s1 != s2);
    BOOST_CHECK(s1 != s3);

    BOOST_CHECK(s2 != s1);
    BOOST_CHECK(s2 == s2);
    BOOST_CHECK(s2 == s3);

    BOOST_CHECK(s3 != s1);
    BOOST_CHECK(s3 == s2);
    BOOST_CHECK(s3 == s3);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_equality_with_c_str, String, Strings_list)
{
    const char * cs1 = "Paper";
    const char * cs2 = "Pair";
    String const s{cs2};

    BOOST_CHECK(s != cs1);
    BOOST_CHECK(s == cs2);

    BOOST_CHECK(cs1 != s);
    BOOST_CHECK(cs2 == s);
}

// 21.4.8.4
BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_less, String, Strings_list)
{
    String const s1{"Paper"};
    String const s2{"Pair"};

    BOOST_CHECK_EQUAL(s1 < s2, false);
    BOOST_CHECK_EQUAL(s2 < s1, true);

    BOOST_CHECK_EQUAL(s1 < s2.c_str(), false);
    BOOST_CHECK_EQUAL(s2 < s1.c_str(), true);

    BOOST_CHECK_EQUAL(s1.c_str() < s2, false);
    BOOST_CHECK_EQUAL(s2.c_str() < s1, true);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_less_equal, String, Strings_list)
{
    String const s1{"Paper"};
    String const s2{"Pair"};

    BOOST_CHECK_EQUAL(s1 <= s2, false);
    BOOST_CHECK_EQUAL(s2 <= s1, true);

    BOOST_CHECK_EQUAL(s1 <= s1, true);
    BOOST_CHECK_EQUAL(s2 <= s2, true);

    BOOST_CHECK_EQUAL(s1 <= s2.c_str(), false);
    BOOST_CHECK_EQUAL(s2 <= s1.c_str(), true);

    BOOST_CHECK_EQUAL(s1.c_str() <= s2, false);
    BOOST_CHECK_EQUAL(s2.c_str() <= s1, true);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_greater, String, Strings_list)
{
    String const s1{"Paper"};
    String const s2{"Pair"};

    BOOST_CHECK_EQUAL(s1 > s2, true);
    BOOST_CHECK_EQUAL(s2 > s1, false);

    BOOST_CHECK_EQUAL(s1 > s2.c_str(), true);
    BOOST_CHECK_EQUAL(s2 > s1.c_str(), false);

    BOOST_CHECK_EQUAL(s1.c_str() > s2, true);
    BOOST_CHECK_EQUAL(s2.c_str() > s1, false);
}

// 21.4.8.8 swap
BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_swap, String, Strings_list)
{
    typedef typename String::allocator_type Alloc;

    String fs1("Paper", Alloc{13});
    String fs2("Pair", Alloc{42});

    auto fs1_old = fs1;
    auto fs2_old = fs2;

    ::ural::swap(fs1, fs2);

    BOOST_CHECK_EQUAL(fs1_old, fs2);
    BOOST_CHECK_EQUAL(fs2_old, fs1);

    BOOST_CHECK_EQUAL(fs1_old.get_allocator().id(), fs2.get_allocator().id());
    BOOST_CHECK_EQUAL(fs2_old.get_allocator().id(), fs1.get_allocator().id());
}

BOOST_AUTO_TEST_CASE(flex_string_swap_default_allocator)
{
    ural::flex_string<char> fs1("Paper");
    ural::flex_string<char> fs2("Pair");

    auto fs1_old = fs1;
    auto fs2_old = fs2;

    ::ural::swap(fs1, fs2);

    BOOST_CHECK_EQUAL(fs1_old, fs2);
    BOOST_CHECK_EQUAL(fs2_old, fs1);
}

// 21.4.8.9 Потоковые операторы
// @todo Больше тестов ввода
BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_istreaming, String, Strings_list)
{
    std::string const src{"Hello, world"};

    std::istringstream is(src);

    String reader{"Stepanov"};

    is >> reader;

    BOOST_CHECK_EQUAL(6U, reader.size());

    BOOST_CHECK_EQUAL_COLLECTIONS(src.begin(), src.begin() + reader.size(),
                                  reader.begin(), reader.end());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_ostreaming, String, Strings_list)
{
    std::string const src = "Stepanov";

    String const s(src.begin(), src.end());

    std::ostringstream os;

    os << s;

    BOOST_CHECK_EQUAL(src, os.str());
}

// @todo get_line

// Интеграция с std::string
BOOST_AUTO_TEST_CASE_TEMPLATE(flex_string_equals_std_string_test, String, Strings_list)
{
    std::string const s_std = "Stepanov";
    String const s_ural(s_std.begin(), s_std.end());

    BOOST_CHECK(s_std == s_ural);
    BOOST_CHECK(s_ural == s_std);

    auto const z = String{};

    BOOST_CHECK(s_std != z);
    BOOST_CHECK(z != s_std);
}
