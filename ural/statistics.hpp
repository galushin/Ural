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
    {
        typedef typename meta::make_list<Ts...>::type list;
    };

namespace tags
{
    template <class... Ts>
    struct declare_depend_on
    {
        typedef typename meta::make_list<Ts...>::type depends_on;
    };

    template <class T1, class T2>
    struct is_depend_on
     : std::integral_constant<bool, !std::is_same<typename meta::find<typename T1::depends_on, T2>::type, null_type>::value>
    {};

    struct count_tag    : declare_depend_on<>{};

    template <size_t N>
    struct raw_moment_tag   : declare_depend_on<count_tag>{};

    struct mean_tag     : declare_depend_on<count_tag, raw_moment_tag<1>>{};
    struct variance_tag : declare_depend_on<mean_tag, raw_moment_tag<2>>{};
    struct standard_deviation_tag : declare_depend_on<variance_tag>{};
    struct min_tag      : declare_depend_on<>{};
    struct max_tag      : declare_depend_on<>{};
    struct range_tag    : declare_depend_on<min_tag, max_tag>{};

    constexpr auto count = tags_list<count_tag>{};
    constexpr auto mean = tags_list<mean_tag>{};
    constexpr auto variance = tags_list<variance_tag>{};
    constexpr auto std_dev = tags_list<standard_deviation_tag>{};
    constexpr auto min = tags_list<min_tag>{};
    constexpr auto max = tags_list<max_tag>{};
    constexpr auto range = tags_list<range_tag>{};

    template <class... Ts1, class... Ts2>
    tags_list<Ts1..., Ts2...>
    operator|(tags_list<Ts1...>, tags_list<Ts2...>)
    {
        return {};
    }

    template <class List, class Out>
    struct expand_depend_on;

    template <class Out>
    struct expand_depend_on<null_type, Out>
     : declare_type<Out>
    {};

    template <class Head, class Tail, class Out>
    struct expand_depend_on<::ural::meta::list<Head, Tail>, Out>
    {
    private:
        typedef ::ural::meta::list<Head, Out> R1;
        typedef typename expand_depend_on<typename Head::depends_on, R1>::type
            R2;

    public:
        typedef typename expand_depend_on<Tail, R2>::type type;
    };

    template <class Tags>
    struct prepare
    {
        typedef typename expand_depend_on<typename Tags::list, null_type>::type
            WithDependencies;
        typedef typename meta::copy_without_duplicates<WithDependencies>::type
            UniqueTags;
        typedef typename meta::selection_sort<UniqueTags, statistics::tags::is_depend_on>::type
            type;
    };
}
// namespace tags
}
// namespace statistics
    template <class T, class Tag, class Base = empty_type>
    class descriptive;

    template <class T, class Base>
    class descriptive<T, statistics::tags::count_tag, Base>
     : public Base
    {
    public:
        // Типы
        typedef T value_type;
        typedef size_t count_type;

        descriptive()
         : Base{}
         , n_{0}
        {}

        descriptive(value_type const & x)
         : Base{x}
         , n_{1}
        {}

        count_type const & count() const
        {
            return n_;
        }

        descriptive & operator()(T const & x)
        {
            Base::operator()(x);
            ++ n_;
            return *this;
        }

    friend count_type const & at_tag(descriptive const & x,
                                     statistics::tags::count_tag)
    {
        return x.count();
    }

    protected:
        ~descriptive() = default;

    private:
        count_type n_;
    };

    template <class T, class Base, size_t N>
    class descriptive<T, statistics::tags::raw_moment_tag<N>, Base>
     : public Base
    {
        static_assert(N > 0, "zeroth moment is counter");
    public:
        typedef typename Base::count_type count_type;
        typedef typename average_type<T, count_type>::type moment_type;

        descriptive()
         : Base{}
         , value_(0)
        {}

        descriptive(T const & x)
         : Base{x}
         , value_(x)
        {}

        friend moment_type const &
        raw_moment(descriptive const & x, std::integral_constant<size_t, N>)
        {
            return x.value_;
        }

        descriptive & operator()(T const & x)
        {
            Base::operator()(x);
            value_ += (power(x) - value_) / this->count();
            return *this;
        }

    protected:
        ~descriptive() = default;

    private:
        static T power(T const & x)
        {
            return ural::natural_power(x, N);
        }

        moment_type value_;
    };

    template <class T, class Base>
    class descriptive<T, statistics::tags::mean_tag, Base>
     : public Base
    {
    public:
        typedef typename Base::count_type count_type;
        typedef typename Base::moment_type mean_type;

        descriptive()
         : Base{}
        {}

        descriptive(T const & x)
         : Base{x}
        {}

        mean_type const & mean() const
        {
            return raw_moment(*this, std::integral_constant<size_t, 1>{});
        }

    friend mean_type const & at_tag(descriptive const & x,
                                     statistics::tags::mean_tag)
    {
        return x.mean();
    }

        descriptive & operator()(T const & x)
        {
            Base::operator()(x);

            return *this;
        }

    protected:
        ~descriptive() = default;
    };

    // @todo Устранить дублирование с mean
    template <class T, class Base>
    class descriptive<T, statistics::tags::variance_tag, Base>
     : public Base
    {
    public:
        // Типы
        typedef typename Base::mean_type mean_type;

        // Конструкторы
        descriptive()
         : Base{}
        {}

        descriptive(T const & x)
         : Base{x}
        {}

        // Обновление
        descriptive & operator()(T const & x)
        {
            Base::operator()(x);

            return *this;
        }

        // Свойства
        friend mean_type at_tag(descriptive const & x,
                                statistics::tags::variance_tag)
        {
            return x.variance();
        }

        mean_type variance() const
        {
            using ural::square;
            return raw_moment(*this, std::integral_constant<size_t, 2>{})
                    - square(this->mean());
        }
    };

    template <class T, class Base>
    class descriptive<T, statistics::tags::standard_deviation_tag, Base>
     : public Base
    {
    public:
        // Типы
        typedef typename Base::mean_type mean_type;

        // Конструкторы
        descriptive()
         : Base{}
        {}

        descriptive(T const & x)
         : Base{x}
        {}

        // Обновление
        descriptive & operator()(T const & x)
        {
            Base::operator()(x);

            return *this;
        }

        // Свойства
        mean_type standard_deviation() const
        {
            using std::sqrt;
            return sqrt(this->variance());
        }

        friend mean_type at_tag(descriptive const & x,
                            statistics::tags::standard_deviation_tag)
        {
            return x.standard_deviation();
        }

    protected:
        ~descriptive() = default;
    };

    template <class T, class Base>
    class descriptive<T, statistics::tags::min_tag, Base>
     : public Base
    {
    public:
        typedef T value_type;

        descriptive()
         : Base{}
         , min_{std::numeric_limits<T>::infinity()}
        {}

        descriptive(T const & x)
         : Base{x}
         , min_{x}
        {}

        friend value_type const & at_tag(descriptive const & x,
                                         statistics::tags::min_tag)
        {
            return x.min();
        }

        value_type const & min URAL_PREVENT_MACRO_SUBSTITUTION () const
        {
            return min_;
        }

        descriptive & operator()(T const & x)
        {
            Base::operator()(x);
            if(x < min_)
            {
                min_ = x;
            }
            return *this;
        }

    protected:
        ~descriptive() = default;

    private:
        value_type min_;
    };

    template <class T, class Base>
    class descriptive<T, statistics::tags::max_tag, Base>
     : public Base
    {
    public:
        typedef T value_type;

        descriptive()
         : Base{}
         , max_{-std::numeric_limits<T>::infinity()}
        {}

        descriptive(T const & x)
         : Base{x}
         , max_{x}
        {}

        friend value_type const & at_tag(descriptive const & x,
                                         statistics::tags::max_tag)
        {
            return x.max();
        }

        value_type const & max URAL_PREVENT_MACRO_SUBSTITUTION () const
        {
            return max_;
        }

        descriptive & operator()(T const & x)
        {
            Base::operator()(x);

            if(max_ < x)
            {
                max_ = x;
            }

            return *this;
        }

    protected:
        ~descriptive() = default;

    private:
        value_type max_;
    };

    template <class T, class Base>
    class descriptive<T, statistics::tags::range_tag, Base>
     : public Base
    {
    public:
        // Типы
        typedef T value_type;

        // Конструкторы
        descriptive()
         : Base{}
        {}

        descriptive(T const & x)
         : Base{x}
        {}

        // Обновление
        descriptive & operator()(T const & x)
        {
            Base::operator()(x);
            return *this;
        }

        // Свойства
        friend value_type at_tag(descriptive const & x,
                                 statistics::tags::range_tag)
        {
            return x.range();
        }

        value_type range () const
        {
            return this->max() - this->min();
        }

    protected:
        ~descriptive() = default;
    };

    template <class T, class Tags>
    class descriptives;

    template <class T>
    class descriptives<T, null_type>
    {
    protected:
        ~descriptives() = default;

    public:
        descriptives() = default;

        descriptives(T const &)
        {};

        descriptives & operator()(T const &)
        {
            return *this;
        }
    };

    /** @todo Убедиться, что нет повторяющихся тэгов
    @note Одна из проблем с таким дизайном: можно забыть определить operator()
    @todo Оптимизировать min | max
    */
    template <class T, class Tags>
    class descriptives
     : public descriptive<T, typename Tags::head, descriptives<T, typename Tags::tail>>
    {
        typedef descriptive<T, typename Tags::head, descriptives<T, typename Tags::tail>>
            Base;
    // @todo protected
    public:
        descriptives() = default;

        explicit descriptives(T const & init_value)
         : Base{init_value}
        {}

        descriptives & operator()(T const & x)
        {
            Base::operator()(x);
            return *this;
        }
    };

    template <class T, class Tags>
    class descriptives_facade
     : public descriptives<T, typename statistics::tags::prepare<Tags>::type>
    {
        typedef typename statistics::tags::prepare<Tags>::type PreparedTags;
        typedef descriptives<T, PreparedTags> Base;

    public:
        typedef Tags tags_list;

        descriptives_facade() = default;

        descriptives_facade(T const & x)
         : Base{x}
        {}

        descriptives_facade & operator()(T const & x)
        {
            Base::operator()(x);
            return *this;
        }

        template <class Tag>
        // @todo Проверить, что тэг есть в списке
        auto operator[](statistics::tags_list<Tag>) const
        -> decltype(at_tag(*this, Tag{}))
        {
            return at_tag(*this, Tag{});
        }

    private:
    };

    template <class Input, class Tags>
    auto describe(Input && in, Tags)
    -> descriptives_facade<typename decltype(sequence(in))::value_type, Tags>
    {
        typedef typename decltype(sequence(in))::value_type Value;
        typedef descriptives_facade<Value, Tags> Result;

        using ural::sequence;
        auto seq = sequence(std::forward<Input>(in));

        if(!seq)
        {
            return Result{};
        }

        Result r{*seq};
        ++ seq;

        return ural::for_each(seq, std::move(r));
    }

    template <class Forward, class Output>
    void z_score(Forward && in, Output && out)
    {
        auto ds = ural::describe(std::forward<Forward>(in),
                                 ::ural::statistics::tags::std_dev);

        auto const m = ds.mean();
        auto const s = ds.standard_deviation();

        typedef typename decltype(ds)::value_type Value;

        auto f = [m, s](Value const & x) { return (x - m) / s; };

        ural::copy(ural::make_transform_sequence(std::move(f),
                                                 std::forward<Forward>(in)),
                   std::forward<Output>(out));
    }
}
// namespace ural

#endif
// Z_URAL_STATISTICS_HPP_INCLUDED
