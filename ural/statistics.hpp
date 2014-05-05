#ifndef Z_URAL_STATISTICS_HPP_INCLUDED
#define Z_URAL_STATISTICS_HPP_INCLUDED

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

/** @file ural/statistics.hpp
 @brief Средства математической статистики
*/

#include <ural/math.hpp>
#include <ural/algorithm.hpp>
#include <ural/sequence/make.hpp>
#include <ural/meta/hierarchy.hpp>
#include <ural/meta/list.hpp>
#include <ural/defs.hpp>

#include <stdexcept>

namespace ural
{
    template <class... Ts>
    struct are_integral
     : meta::all_of<typename meta::make_list<Ts...>::type, std::is_integral>
    {};

    template <class T, class N, class Enabler = void>
    struct average_type
     : declare_type<decltype(std::declval<T>() / std::declval<N>())>
    {};

    template <class T, class N>
    struct average_type<T, N, typename std::enable_if<are_integral<T, N>::value>::type>
     : declare_type<double>
    {};

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

    private:
        value_type value_;
    };

    /** @brief Создание вероятности на основе значения
    @param p значение
    @return <tt> probability<RealType>{std::move(p)} </tt>
    @throw То же, что <tt> probability<RealType>{std::move(p)} </tt>
    */
    template <class RealType>
    probability<RealType>
    make_probability(RealType p)
    {
        return probability<RealType>{std::move(p)};
    }

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

    template <class Char, class Tr, class T, class P>
    std::basic_ostream<Char, Tr> &
    operator<<(std::basic_ostream<Char, Tr> & os, probability<T, P> const & x)
    {
        return os << x.value();
    }

namespace statistics
{
    template <class... Ts>
    struct tags_list
    {};

namespace tags
{
    struct variance_tag;
    struct range_tag;

    constexpr auto variance = tags_list<variance_tag>{};
    constexpr auto range = tags_list<range_tag>{};

    template <class... Ts1, class... Ts2>
    tags_list<Ts1..., Ts2...>
    operator|(tags_list<Ts1...>, tags_list<Ts2...>)
    {
        return {};
    }

}
// namespace tags
}
// namespace statistics
    template <class T, class Tag>
    class descriptive;

    // @todo Разбить на более мелкие аккумуляторы
    template <class T>
    class descriptive<T, statistics::tags::variance_tag>
    {
    public:
        // Типы
        typedef size_t count_type;
        typedef T value_type;
        typedef typename average_type<T, count_type>::type mean_type;

        // Конструкторы
        descriptive()
         : data_{count_type{0}, mean_type{0}, mean_type{0}}
        {}

        descriptive(T const & x)
         : data_{count_type{1}, mean_type{x}, square(x)}
        {}

        // Обновление
        descriptive & operator()(T const & x)
        {
            using ural::square;
            data_[ural::_1] += 1;
            data_[ural::_2] += (x - this->mean()) / this->count();
            data_[ural::_3] += (square(x) - data_[ural::_3]) / this->count();

            return *this;
        }

        // Свойства
        count_type const & count() const
        {
            return data_[ural::_1];
        }

        mean_type const & mean() const
        {
            return data_[ural::_2];
        }

        mean_type variance() const
        {
            using ural::square;
            return data_[ural::_3] - square(this->mean());
        }

        mean_type standard_deviation() const
        {
            using std::sqrt;
            return sqrt(this->variance());
        }

    private:
        ural::tuple<count_type, mean_type, mean_type> data_;
    };

    // @todo Разбить на более мелкие аккумуляторы
    template <class T>
    class descriptive<T, statistics::tags::range_tag>
    {
    public:
        // Типы
        typedef T value_type;

        // Конструкторы
        descriptive()
         : data_{-std::numeric_limits<T>::infinity(),
                 std::numeric_limits<T>::infinity()}
        {}

        descriptive(T const & x)
         : data_{x, x}
        {}

        // Обновление
        descriptive & operator()(T const & x)
        {
            if(x < this->min())
            {
                data_[ural::_1] = x;
            }
            else if(x > this->max())
            {
                data_[ural::_2] = x;
            }
            return *this;
        }

        // Свойства
        value_type const & min URAL_PREVENT_MACRO_SUBSTITUTION () const
        {
            return data_[ural::_1];
        }

        value_type const & max URAL_PREVENT_MACRO_SUBSTITUTION () const
        {
            return data_[ural::_2];
        }

        value_type range () const
        {
            return this->max() - this->min();
        }

    private:
        ural::tuple<value_type, value_type> data_;
    };

    template <class T, class Tags>
    class descriptives;

    /** @todo Убедиться, что нет повторяющихся тэгов
    */
    template <class T, class... Tags>
    class descriptives<T, statistics::tags_list<Tags...>>
     : public meta::inherit_from<typename meta::make_list<descriptive<T, Tags>...>::type>
    {
        typedef typename meta::make_list<descriptive<T, Tags>...>::type Units;

        typedef meta::inherit_from<typename meta::make_list<descriptive<T, Tags>...>::type>
            Base;

        template <class U>
        T const & tagged_return(T const & x)
        {
            return x;
        }

    public:
        descriptives() = default;

        explicit descriptives(T const & init_value)
         : Base{tagged_return<Tags>(init_value)...}
        {}

        descriptives & operator()(T const & x)
        {
            // @todo Без "рекурсии"
            this->update_chain(x, ural::_1);
            return *this;
        }

    private:
        void update_chain(T const &, ural::placeholder<sizeof...(Tags)>)
        {
            return;
        }

        template <size_t Index>
        void update_chain(T const & x, placeholder<Index>)
        {
            typedef typename meta::at<Units, Index>::type Unit;
            static_cast<Unit&>(*this)(x);
            return this->update_chain(x, placeholder<Index+1>{});
        }
    };

    template <class Input, class Tags>
    auto describe(Input && in, Tags)
    -> descriptives<typename decltype(sequence(in))::value_type, Tags>
    {
        typedef typename decltype(sequence(in))::value_type Value;
        typedef descriptives<Value, Tags> Result;

        using ural::sequence;
        auto seq = sequence(in);

        if(!seq)
        {
            return Result{};
        }

        Result r{*seq};
        ++ seq;

        return ural::for_each(seq, std::move(r));
    }
}
// namespace ural

#endif
// Z_URAL_STATISTICS_HPP_INCLUDED
