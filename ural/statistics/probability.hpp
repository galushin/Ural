#ifndef Z_URAL_PROBABILITY_HPP_INCLUDED
#define Z_URAL_PROBABILITY_HPP_INCLUDED

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

/** @brief ural/statistics/probability.hpp
 @brief Класс для представления вероятностей
*/

#include <stdexcept>
#include <istream>
#include <ostream>

namespace ural
{
    /** @brief Класс исключения "некорректное значение вероятности"
    */
    class bad_probability
     : public std::logic_error
    {
    public:
        /// @brief Конструктор
        bad_probability()
         : std::logic_error("bad probability")
        {}
    };

    /** @brief Стратегия проверок для класса вероятности, возбуждающая
    исключения в случае ошибок.
    @tparam RealType тип значения
    */
    template <class RealType>
    class throw_probability_policy
    {
    public:
        /// @brief Тип значения
        typedef RealType value_type;

        /** @brief Функция проверки значения
        @param value проверяемое значение
        @return Возвращает @c value
        @throw bad_probability, если <tt> value < 0 || value > 1 </tt>
        */
        constexpr static value_type enforce(value_type value)
        {
            return (value < 0 || value > 1) ?
                    throw bad_probability{} : std::move(value);
        }

    protected:
        ~throw_probability_policy() = default;
    };

    /** @brief Класс для представления вероятности
    @tparam RealType тип значения
    @tparam Policy тип стратегии проверок
    */
    template <class RealType = double,
              class Policy = throw_probability_policy<RealType>>
    class probability
    {
    public:
        /// @brief Тип значения
        typedef RealType value_type;

        /// @brief Тип ссылки
        typedef value_type const & const_reference;

        /// @brief Тип стратегии
        typedef Policy policy_type;

        /** @brief Конструктор без параметров
        @post <tt> this->value() == value_type{0} </tt>
        */
        constexpr probability() = default;

        /** @brief Конструктор
        @param value значение
        @pre <tt> 0 < value && value < 1 </tt>
        @post <tt> this->value() == value </tt>
        @throw Тоже, что <tt> Policy::enforce(std::move(value)) </tt>
        */
        explicit constexpr probability(value_type value)
         : value_{Policy::enforce(std::move(value))}
        {}

        /** @brief Оператор присваивания
        @param value значение
        @pre <tt> 0 < value && value < 1 </tt>
        @post <tt> this->value() == value </tt>
        @return <tt> *this </tt>
        @throw Тоже, что <tt> Policy::enforce(std::move(value)) </tt>
        */
        probability & operator=(value_type value)
        {
            this->value_ = Policy::enforce(std::move(value));

            return *this;
        }

        /** @brief Текущее значения
        @return Текущее значения
        */
        constexpr const_reference value() const
        {
            return this->value_;
        }

        /** @brief Оператор неявного преобразования
        @return <tt> this->value() </tt>
        */
        constexpr operator const_reference() const
        {
            return this->value();
        }

    private:
        value_type value_;
    };

    struct make_probability_fn
    {
    public:
        /** @brief Создание вероятности на основе значения
        @param p значение
        @return <tt> probability<RealType>{std::move(p)} </tt>
        @throw То же, что <tt> probability<RealType>{std::move(p)} </tt>
        */
        template <class RealType>
        constexpr probability<RealType>
        operator()(RealType p) const
        {
            return probability<RealType>{std::move(p)};
        }
    };
    auto constexpr make_probability = make_probability_fn{};

    /** @brief Оператор равно
    @param x левый операнд
    @param y правый операнд
    @return <tt> x.value() == y.value() </tt>
    */
    template <class T1, class P1, class T2, class P2>
    constexpr bool operator==(probability<T1, P1> const & x,
                              probability<T2, P2> const & y)
    {
        return x.value() == y.value();
    }

    /** @brief Оператор равно
    @param x левый операнд
    @param y правый операнд
    @return <tt> x.value() == y </tt>
    */
    template <class T1, class P1, class T2>
    constexpr bool operator==(probability<T1, P1> const & x, T2 const & y)
    {
        return x.value() == y;
    }

    /** @brief Оператор "равно"
    @param x левый операнд
    @param y правый операнд
    @return <tt> x == y.value() </tt>
    */
    template <class T1, class T2, class P2>
    constexpr bool operator==(T1 const & x, probability<T2, P2> const & y)
    {
        return x == y.value();
    }

    /** @brief Оператор "меньше"
    @param x левый операнд
    @param y правый операнд
    @return <tt> x.value() < y.value() </tt>
    */
    template <class T1, class P1, class T2, class P2>
    constexpr bool operator<(probability<T1, P1> const & x,
                             probability<T2, P2> const & y)
    {
        return x.value() < y.value();
    }

    /** @brief Вывод вероятности в поток
    @param os поток вывода
    @param x вероятность
    @return @c os
    */
    template <class Char, class Tr, class T, class P>
    std::basic_ostream<Char, Tr> &
    operator<<(std::basic_ostream<Char, Tr> & os, probability<T, P> const & x)
    {
        return os << x.value();
    }

    /** @brief Ввод вероятности из потока
    @param is поток ввода
    @param x объект, хранящий вероятность
    @return @c is
    @throw std::logic_error, если введённое значение не может быть
    интерпретировано как вероятность, то есть не является вещественным числом
    из интервала <tt> [0; 1] </tt>.
    */
    template <class Char, class Tr, class T, class P>
    std::basic_istream<Char, Tr> &
    operator>>(std::basic_istream<Char, Tr> & is, probability<T, P> & x)
    {
        T reader;
        is >> reader;

        try
        {
            x = reader;
        }
        catch(...)
        {
            is.setstate(::std::ios_base::failbit);
            throw;
        }

        return is;
    }
}
// namespace ural

#endif
// Z_URAL_PROBABILITY_HPP_INCLUDED
