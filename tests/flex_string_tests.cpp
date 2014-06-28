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
}

BOOST_AUTO_TEST_CASE(flex_string_allocator_ctor)
{
    String::allocator_type a{42};

    String s{a};

    BOOST_CHECK_EQUAL(0U, s.size());
    BOOST_CHECK(nullptr != s.data());
    BOOST_CHECK_EQUAL(a.id(), s.get_allocator().id());
}

// @todo Конструктор с распределителем памяти
// @todo Конструктор копирования
// @todo Конструктор перемещения
// @todo Копирование фрагмента строки
// @todo Конструктор на основе указателя и количества элементов c-массива
