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
#include <ural/operators.hpp>

#include <new>
#include <cassert>
#include <exception>
#include <typeinfo>
#include <utility>

namespace ural
{
    template <class E = std::exception_ptr>
    class unexpected
    {
    public:
        unexpected(E ex)
         : ex_{std::move(ex)}
        {}

        E move_out()
        {
            return std::move(ex_);
        }

        E const & get() const
        {
            return this->ex_;
        }

    private:
        E ex_;
    };

    template <class T>
    class expected;

    /** @brief Создание expected на основе функциональног объекта
    @param f функциональный объект
    @param args аргументы
    */
    template <class F, class... Args>
    auto expected_from_call(F f, Args &&... args) noexcept
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

    /** @brief Обёртка для значения или исключения, которое помешало вычислению
    этого значения.
    @tparam T тип значения
    */
    template <class T>
    class expected
    {
    public:
        // Типы
        /// @brief Тип значения
        typedef T value_type;

        typedef unexpected<std::exception_ptr> unexpected_type;

        // Создание, присваивание и уничтожение
        expected(unexpected<std::exception_ptr> ue)
         : has_value_{false}
        {
            new(&ex_) unexpected_type(std::move(ue.move_out()));
        }

        /** @brief Коструктор на основе значения
        @param init_value значение
        @post <tt> this->has_value() == true </tt>
        @post <tt> this->value() == init_value </tt>
        */
        expected(T const & init_value)
         : has_value_(true)
        {
            new(&value_) T(init_value);
        }

        /** @brief Коструктор на основе временного объекта
        @param x объект, содержимое которого будет передано создаваемогу объекту
        @post <tt> this->has_value() == true </tt>
        @post <tt> this->value() </tt> будет иметь значение, которое @c x имел
        до начала выполнения конструктора
        */
        expected(T && x)
         : has_value_(true)
        {
            new(&value_) T(std::move(x));
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
                new(&ex_) unexpected_type(x.ex_);
            }
        }

        /** @brief Конструктор с перемещением
        @param x объект, содержимое которого должно быть перемещено
        @post <tt> *this </tt> содержит значение, котороые @c x содержал до
        присваивания.
        */
        expected(expected && x)
         : has_value_(x.has_value_)
        {
            if(has_value_)
            {
                new(&value_) T(std::move(x.value_));
            }
            else
            {
                new(&ex_) unexpected_type(x.ex_.move_out());
            }
        }

        /** @brief Оператор копирующего присваивания
        @param x копируемый объект
        @post <tt> *this == x </tt>
        */
        expected & operator=(expected const & x)
        {
            if(x.has_value())
            {
                this->set_value(x.value_);
            }
            else
            {
                this->set_exception(x.ex_.get());
            }
            return *this;
        }

        /** @brief Оператор присваивания с перемещением
        @param x объект, содержимое которого должно быть перемещено
        @post <tt> *this </tt> содержит значение, котороые @c x содержал до
        присваивания.
        @return <tt> *this </tt>
        */
        expected & operator=(expected && x)
        {
            if(x.has_value())
            {
                this->set_value(std::move(x.value_));
            }
            else
            {
                this->set_exception(x.ex_.move_out());
            }
            return *this;
        }

        /// @brief Деструктор
        ~ expected()
        {
            if(this->has_value_)
            {
                value_.~T();
            }
            else
            {
                ex_.~unexpected_type();
            }
        }

        //@{
        /** @brief Создание @c expected на основе исключения
        @param e исключение
        @return @c expected, содержащий исключение @c e
        */
        static expected from_exception(std::exception_ptr const & e)
        {
            return {unexpected<std::exception_ptr>{e}};
        }

        template <class E>
        static expected from_exception(E const & e)
        {
            assert(typeid(E) == typeid(e));

            return expected::from_exception(std::make_exception_ptr<E>(e));
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
        @throw Заданное исключение, если <tt> this->has_value() == false </tt>.
        */
        T & value()
        {
            auto const & cr = *this;

            return const_cast<T&>(cr.value());
        }

        T const & value() const
        {
            if(this->has_value_)
            {
                return this->value_;
            }
            else
            {
                std::rethrow_exception(this->ex_.get());
            }
        }
        //@}

        /** @brief Доступ к хранимому значению с возвратом заданного в случае,
        если хранимого значения нет.
        @param value значение, которое должно быть возвращено, если
        <tt> *this </tt> не содержит значения
        @return Если <tt> this->has_value() </tt>, то возвращает
        <tt> this->value() </tt>, иначе --- <tt> std::forward<U>(value) </tt>.
        */
        template <class U>
        T value_or(U && value) const &
        {
            return this->has_value() ? this->value_ : std::forward<U>(value);
        }

        /** @brief Доступ к хранимому исключению
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

        // future
        /** @brief Задать исключение
        @param ex объект-исключение
        @post <tt> this->has_value() == false </tt>
        @post <tt> *this->get_exception() == ex </tt>
        */
        template <class Exception>
        void set_exception(Exception const & ex)
        {
            *this = from_exception(ex);
        }

        /** @brief Задать исключение
        @param p указатель, хранящий исключение
        @post <tt> this->has_value() == false </tt>
        */
        void set_exception(std::exception_ptr p)
        {
            if(this->has_value_)
            {
                this->value_.~T();
                new(&this->ex_) unexpected_type(std::move(p));
                has_value_ = false;
            }
            else
            {
                this->ex_ = std::move(p);
            }
        }

        /** @brief Установить значение
        @param value значение
        @post <tt> this->has_value() == true </tt>
        @post <tt> this->value() == value </tt>
        */
        void set_value(T value)
        {
            if(this->has_value_)
            {
                this->value_ = std::move(value);
            }
            else
            {
                this->ex_.~unexpected_type();
                new(&this->value_) T(std::move(value));
                has_value_ = true;
            }
        }

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
                    new(&ex_) unexpected_type{std::move(x.ex_)};

                    x.ex_.~unexpected_type();
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

        template <class F>
        auto fmap(F f) noexcept -> expected<decltype(f(std::declval<T>()))>
        {
            if(has_value_)
            {
                return ural::expected_from_call(f, this->value_);
            }
            else
            {
                return {ex_};
            }
        }

    private:
        bool has_value_;
        union
        {
            T value_;
            unexpected_type ex_;
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
}
//namespace ural

#endif
// Z_URAL_UTILITY_EXPECTED_HPP_INCLUDED
