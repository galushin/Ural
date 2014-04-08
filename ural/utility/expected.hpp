#ifndef Z_URAL_UTILITY_EXPECTED_HPP_INCLUDED
#define Z_URAL_UTILITY_EXPECTED_HPP_INCLUDED

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

/** @file ural/utility/expected.hpp
 @brief ������ ��� �������� ��� ����������, ������� �������� ���������� �����
 ��������.
*/

#include <ural/defs.hpp>

namespace ural
{
    /** @brief ������ ��� �������� ��� ����������, ������� �������� ����������
    ����� ��������.
    @tparam T ��� ��������
    */
    template <class T>
    class expected
    {
        expected()
        {}

    public:
        // ����
        /// @brief ��� ��������
        typedef T value_type;

        // ��������, ������������ � �����������
        /** @brief ����������
        @param init_value ��������
        @post <tt> this->has_value() == true </tt>
        @post <tt> this->value() == init_value </tt>
        */
        explicit expected(T init_value)
         : has_value_(true)
        {
            new(&value_)T(std::move(init_value));
        }

        /** @brief ����������� �����
        @param x ���������� ������
        @post <tt> this->has_value() == x.has_value() </tt>
        */
        expected(expected const & x)
         : has_value_{x.has_value_}
        {
            if(has_value_)
            {
                new(&value_) T(x.value_);
            }
            else
            {
                new(&ex_) std::exception_ptr(x.ex_);
            }
        }

        expected(expected && x);

        expected & operator=(expected const & x);
        expected & operator=(expected && x);

        /// @brief ����������
        ~ expected()
        {
            if(this->value_)
            {
                value_.~T();
            }
            else
            {
                using std::exception_ptr;
                ex_.~exception_ptr();
            }
        }

        //@{
        /** @brief �������� @c expected �� ������ ����������
        @param e ����������
        @return @c expected, ���������� ���������� @c e
        */
        static expected from_exception(std::exception_ptr const & e)
        {
            expected result{};

            result.has_value_ = false;
            new(&result.ex_) std::exception_ptr(e);

            return result;
        }

        template <class E>
        static expected from_exception(E const & e)
        {
            assert(typeid(E) == typeid(e));

            return expected::from_exception(std::copy_exception(e));
        }

        static expected from_exception()
        {
            return expected::from_exception(std::current_exception());
        }
        //@}

        // ��������
        /** @brief �������� �� ��������
        @return @b true, ���� ������ �������� ��������, ����� --- @b false.
        */
        bool has_value() const
        {
            return this->has_value_;
        }

        //@{
        /** @brief ������ � ��������
        @return �������� ��������, ���� <tt> this->has_value() </tt>.
        @throw �������� ����������.
        */
        T & value()
        {
            if(this->has_value_)
            {
                return this->value_;
            }
            else
            {
                std::rethrow_exception(this->ex_);
            }
        }

        T const & value() const
        {
            if(this->has_value_)
            {
                return this->value_;
            }
            else
            {
                std::rethrow_exception(this->ex_);
            }
        }
        //@}

        // ������������������ ���������
        void swap(expected & x);

    private:
        bool has_value_;
        union
        {
            T value_;
            std::exception_ptr ex_;
        };
    };

    //@{
    /** @brief �������� "�����"
    @param x ������
    @param a ��������
    @return <tt> x.has_value() ? x.value() == a : false </tt>
    */
    template <class T>
    bool operator==(expected<T> const & x, T const & a)
    {
        return x.has_value() ? x.value() == a : false;
    }

    template <class T>
    bool operator==(T const & a, expected<T> const & x)
    {
        return x == a;
    }
    //@}

    /** @brief �������� @c expected
    @param value ��������
    @return <tt> expected<T>(value) </tt>
    */
    template <class T>
    expected<T> make_expected(T value)
    {
        return expected<T>(value);
    }

    template <class F>
    auto expected_from_call(F f) -> expected<decltype(f())>
    {
        typedef expected<decltype(f())> Expected;
        try
        {
            return Expected(f());
        }
        catch(...)
        {
            return Expected::from_exception();
        }
    }
}
//namespace ural

#endif
// Z_URAL_UTILITY_EXPECTED_HPP_INCLUDED
