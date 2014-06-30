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

#include <boost/test/unit_test.hpp>

#include <ural/flex_string.hpp>

#include <sstream>

template <class T>
class test_allocator
{
    typedef std::allocator<T> Base;
public:
    typedef typename Base::value_type value_type;
    typedef typename Base::size_type size_type;
    typedef typename Base::pointer pointer;

    test_allocator(int id = 0)
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

    int id() const
    {
        return this->id_;
    }

private:
    int id_;
    std::allocator<T> a_;
};

typedef ural::flex_string<char, ural::use_default, test_allocator<char>>
    String;

BOOST_AUTO_TEST_CASE(flex_string_default_ctor)
{
    String s;

    BOOST_CHECK_EQUAL(0U, s.size());
    BOOST_CHECK(nullptr != s.data());
    BOOST_CHECK_GE(s.capacity(), s.size());
}

BOOST_AUTO_TEST_CASE(flex_string_allocator_ctor)
{
    String::allocator_type a{42};

    String s{a};

    BOOST_CHECK_EQUAL(0U, s.size());
    BOOST_CHECK_GE(s.capacity(), s.size());
    BOOST_CHECK(nullptr != s.data());
    BOOST_CHECK_EQUAL(a.id(), s.get_allocator().id());
}

// @todo Конструктор копирования
// @todo Конструктор перемещения
// @todo Копирование фрагмента строки
// @todo Конструктор на основе указателя и количества элементов c-массива

BOOST_AUTO_TEST_CASE(flex_string_from_c_str)
{
    char const * cs = "Hello, world";
    String s(cs);

    typedef String::allocator_type Alloc;
    typedef String::traits_type Traits;

    BOOST_CHECK_EQUAL(Alloc{}.id(), s.get_allocator().id());

    BOOST_CHECK(cs != s.data());
    BOOST_CHECK(nullptr != s.data());

    BOOST_CHECK_EQUAL(Traits::length(cs), s.size());
    BOOST_CHECK_EQUAL(std::strlen(cs), s.size());
    BOOST_CHECK_GE(s.capacity(), s.size());

    BOOST_CHECK(std::strcmp(cs, s.c_str()) == 0);
}

BOOST_AUTO_TEST_CASE(flex_string_from_c_str_with_allocator)
{
    String::allocator_type a{42};
    char const * cs = "Hello, world";

    String const s{cs, a};

    typedef String::allocator_type Alloc;
    typedef String::traits_type Traits;

    BOOST_CHECK_EQUAL(a.id(), s.get_allocator().id());

    BOOST_CHECK(cs != s.data());
    BOOST_CHECK(nullptr != s.data());

    BOOST_CHECK_EQUAL(Traits::length(cs), s.size());
    BOOST_CHECK_EQUAL(std::strlen(cs), s.size());
    BOOST_CHECK_GE(s.capacity(), s.size());

    BOOST_CHECK(std::strcmp(cs, s.c_str()) == 0);
}

BOOST_AUTO_TEST_CASE(flex_string_from_n_char)
{
    auto const n = 13;
    auto const C = 'a';

    String const s(n, C);

    typedef String::allocator_type Alloc;
    typedef String::traits_type Traits;

    BOOST_CHECK_EQUAL(Alloc{}.id(), s.get_allocator().id());
    BOOST_CHECK(nullptr != s.data());
    BOOST_CHECK_EQUAL(n, s.size());
    BOOST_CHECK_GE(s.capacity(), s.size());

    for(size_t i = 0; i != n; ++ i)
    {
        BOOST_CHECK(Traits::eq(s[i], C));
    }
}

BOOST_AUTO_TEST_CASE(flex_string_from_n_char_and_allocator)
{
    String::allocator_type a{42};

    auto const n = 13;
    auto const C = 'a';

    String const s(n, C, a);

    typedef String::allocator_type Alloc;
    typedef String::traits_type Traits;

    BOOST_CHECK_EQUAL(a.id(), s.get_allocator().id());
    BOOST_CHECK(nullptr != s.data());
    BOOST_CHECK_EQUAL(n, s.size());
    BOOST_CHECK_GE(s.capacity(), s.size());

    for(size_t i = 0; i != n; ++ i)
    {
        BOOST_CHECK(Traits::eq(s[i], C));
    }
}

BOOST_AUTO_TEST_CASE(flex_string_from_iterators)
{
    std::string const src = "Stepanov";
    std::istringstream is(src);

    String const s{std::istream_iterator<char>(is),
                   std::istream_iterator<char>()};

    BOOST_CHECK_EQUAL(src.size(), s.size());
    BOOST_CHECK_EQUAL(src.c_str(), s.c_str());

     for(size_t i = 0; i != src.size(); ++ i)
    {
        BOOST_CHECK_EQUAL(src[i], s[i]);
    }
}

BOOST_AUTO_TEST_CASE(flex_string_from_iterators_and_allocator)
{
    String::allocator_type a{42};
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

// @todo Конструктор на основе списка инициализации
// @todo Конструктор на основе строки и распределителя памяти
// @todo Конструктор на основе временной строки и распределителя памяти

// @todo 21.4.3 Поддержка итераторов
// @todo 21.4.4 Ёмкость
// @todo 21.4.5 доступ к элементам
// @todo 21.4.6 модификаторы
// @todo 21.4.7 операции со строками
// @todo 21.4.8 вспомогательные функции

// 21.4.8.9 Потоковые операторы
// @todo Оператор ввода

BOOST_AUTO_TEST_CASE(flex_string_ostreaming)
{
    std::string const src = "Stepanov";

    String const s(src.begin(), src.end());

    std::ostringstream os;

    os << s;

    BOOST_CHECK_EQUAL(src, os.str());
}

// @todo get_line
