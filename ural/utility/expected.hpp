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
 @brief Обёртка для значения или исключения, которое помешало вычислению этого
 значения.
 @todo Поддержка типов с перемещением
*/

#include <ural/defs.hpp>

namespace ural
{
    /** @brief Обёртка для значения или исключения, которое помешало вычислению
    этого значения.
    @tparam T тип значения
    */
    template <class T>
    class expected
    {
        expected()
        {}

    public:
        // Типы
        /// @brief Тип значения
        typedef T value_type;

        // Создание, присваивание и уничтожение
        /** @brief Коструктор
        @param init_value значение
        @post <tt> this->has_value() == true </tt>
        @post <tt> this->value() == init_value </tt>
        */
        explicit expected(T init_value)
         : has_value_(true)
        {
            new(&value_)T(std::move(init_value));
        }

        /** @brief Конструктор копий
        @param x копируемый объект
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

        /// @brief Деструктор
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
        /** @brief Создание @c expected на основе исключения
        @param e исключение
        @return @c expected, содержащий исключение @c e
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
        //@}

        /** @brief Создание @c expected на основе активного в данный момент
        исключения
        @return <tt> from_exception(std::current_exception()) </tt>
        */
        static expected from_exception()
        {
            return expected::from_exception(std::current_exception());
        }

        // Свойства
        /** @brief Содержит ли значение
        @return @b true, если объект содержит значение, иначе --- @b false.
        */
        bool has_value() const
        {
            return this->has_value_;
        }

        //@{
        /** @brief Доступ к значению
        @return Заданное значение, если <tt> this->has_value() </tt>.
        @throw Заданное исключение.
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

        template <class U>
        T value_or(U && value) const &
        {
            return this->has_value() ? this->value_ : std::forward<U>(value);
        }

        /**
        @tparam Exception тип исключения
        @return Если данный объект содержит исключение типа @c Exception, то
        возвращает константный указатель на него, иначе --- nullptr,
        */
        template <class Exception>
        Exception const *
        get_exception() const
        {
            try
            {
                this->value();
            }
            catch(Exception const & e)
            {
                return &e;
            }
            catch(...)
            {}

            return nullptr;
        }

        // @todo future

        // Специализированные алгоритмы
        /** @brief Обмен содержимым объектов
        @param x объект, содержимое которого будет обменен с содержимым данного
        объекта.
        */
        void swap(expected & x)
        {
            if(this->has_value_)
            {
                if(x.has_value_)
                {
                    using std::swap;
                    swap(this->value_, x.value_);
                }
                else
                {
                    std::swap(this->has_value_, x.has_value_);

                    auto val = std::move(this->value_);

                    this->value_.~T();
                    new(&ex_) std::exception_ptr{std::move(x.ex_)};

                    using std::exception_ptr;
                    x.ex_.~exception_ptr();
                    new(&x.value_) T{std::move(val)};
                }
            }
            else
            {
                if(x.has_value_)
                {
                    x.swap(*this);
                }
                else
                {
                    std::swap(this->ex_, x.ex_);
                }
            }
        }

    private:
        bool has_value_;
        union
        {
            T value_;
            std::exception_ptr ex_;
        };
    };

    //@{
    /** @brief Оператор "равно"
    @param x объект
    @param a значение
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

    /** @brief Создание @c expected
    @param value значение
    @return <tt> expected<T>(value) </tt>
    */
    template <class T>
    expected<T> make_expected(T value)
    {
        return expected<T>(value);
    }

    /** @brief Создание expected на основе функциональног объекта
    @param f функциональный объект
    @param args аргументы
    */
    template <class F, class... Args>
    auto expected_from_call(F f, Args &&... args)
    -> expected<decltype(f(std::forward<Args>(args)...))>
    {
        typedef expected<decltype(f(std::forward<Args>(args)...))> Expected;
        try
        {
            return Expected(f(std::forward<Args>(args)...));
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
