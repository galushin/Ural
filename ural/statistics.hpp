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
 @todo Концепция "Описательная статистика"
 @todo Реализовать и использовать meta-алгоритм flatten
*/

/** @page StatisticsSystem Система статистического анализа на С++

Система компьюетерной алгебры и статистического анализа, встроенная в C++:
1. Библиотека аналитических, численных и статистических методов.
2. Интерпретируемый язык с тем же синтаксисом.
3. Графический интерфейс для этого языка.

Исследователь проводит анализ в интерактивном режиме, а затем полученную
программу копирует в исходный фал С++ и получает быструю независимую программу.

Для языка статистического анализа нужен, например, ввод данных из CSV, что
является аналогом функциональности <tt> std.csv </tt> языка программирования
@c D.

Статистичеcкий анализ опирается на линейную алгебру, классические алгоритмы и
другие разделы высшей математики. Было бы разумно, чтобы пользователь имел такой
же доступ к базовой математической и алгоритмической функциональности как и сама
система статистического анализа.
*/

#include <ural/meta/algo.hpp>
#include <ural/numeric/numbers_sequence.hpp>
#include <ural/math.hpp>
#include <ural/algorithm.hpp>
#include <ural/sequence/transform.hpp>
#include <ural/sequence/make.hpp>
#include <ural/defs.hpp>

#include <boost/numeric/ublas/symmetric.hpp>
#include <boost/math/distributions/chi_squared.hpp>
#include <boost/math/distributions/normal.hpp>

#include <stdexcept>

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

namespace statistics
{
    /** @brief Список тэгов
    @tparam Ts тэги
    @todo можно ли его самого считать контейнером типов?
    */
    template <class... Ts>
    struct tags_list
    {
        /// @brief Список типов-тэгов
        typedef ::ural::typelist<Ts...> list;
    };

    /** @brief Конкатенация списков тэгов
    @tparam Ts1 тэги первого списка
    @tparam Ts2 тэги второго списка
    */
    template <class... Ts1, class... Ts2>
    constexpr tags_list<Ts1..., Ts2...>
    operator|(tags_list<Ts1...>, tags_list<Ts2...>)
    {
        return {};
    }

namespace tags
{
    /** @brief Вспомогательный базовый класс, определеяющий список тэгов, от
    которых зависит данный.
    */
    template <class... Ts>
    struct declare_depend_on
    {
        /// @brief Список зависимостей
        typedef ::ural::typelist<Ts...> depends_on;
    };

    /** @brief Класс-характеристика для определения, зависит ли тэг @c T1 от
    тэга @c T2.
    @tparam T1 первый тэг
    @tparam T1 второй тэг
    */
    template <class T1, class T2>
    struct is_depend_on
     : std::integral_constant<bool, !std::is_same<typename meta::find<typename T1::depends_on, T2>::type, null_type>::value>
    {};

    // Тэги-типы
    /// @brief Тип-тэг описательной статистики "Количество элементов"
    struct count_tag    : declare_depend_on<>{};

    /** @brief Тип-тэг описательной статистики "Начальный момент"
    @tparam N порядок момента
    */
    template <size_t N>
    struct raw_moment_tag   : declare_depend_on<count_tag>{};

    /// @brief Тип-тэг описательной статистики "Математическое ожидание"
    struct mean_tag : declare_depend_on<raw_moment_tag<1>>{};

    /// @brief Тип-тэг описательной статистики "Дисперсия"
    struct variance_tag : declare_depend_on<mean_tag>{};

    /// @brief Тип-тэг описательной статистики "Среднеквадратическое уклонение"
    struct standard_deviation_tag : declare_depend_on<variance_tag>{};

    /// @brief Тип-тэг описательной статистики "Наименьшее значение"
    struct min_tag      : declare_depend_on<>{};

    /// @brief Тип-тэг описательной статистики "Наибольшее значение"
    struct max_tag      : declare_depend_on<>{};

    /// @brief Тип-тэг описательной статистики "Размах"
    struct range_tag    : declare_depend_on<min_tag, max_tag>{};

    // Тэги-объекты
    constexpr auto count = tags_list<count_tag>{};
    constexpr auto mean = tags_list<mean_tag>{};
    constexpr auto variance = tags_list<variance_tag>{};
    constexpr auto std_dev = tags_list<standard_deviation_tag>{};
    constexpr auto min = tags_list<min_tag>{};
    constexpr auto max = tags_list<max_tag>{};
    constexpr auto range = tags_list<range_tag>{};

    /** @brief Расширение списка тэгов: добавление тэгов, от которых зависят
    исходные.
    @param List список тэгов
    @param Out хвост списка-результата
    */
    template <class List, class Out>
    struct expand_depend_on
    {
    private:
        typedef typename List::head Head;
        typedef typename List::tail Tail;

        typedef typename ::ural::meta::push_front<Out, Head>::type R1;
        typedef typename expand_depend_on<typename Head::depends_on, R1>::type
            R2;

    public:
        /// @brief Тип-результат
        typedef typename expand_depend_on<Tail, R2>::type type;
    };

     /** @brief Специализация для пустых списков типов
    @tparam Out накопитель
    */
    template <class Out>
    struct expand_depend_on<null_type, Out>
     : declare_type<Out>
    {};

    /** @brief Подготовка списка тэгов для данного
    @tparam Tags список тэгов
    */
    template <class Tags>
    struct prepare
    {
    private:
        /* Заменять на sort-unique нельзя, так как отношение is_depend_on
        слишком слабое. Если есть два независимых тэга A и B, то
        список <A, B, A> при сортировке не изменится и, следовательно,
        после unique будет содержать дубликаты
        */
        typedef ural::meta::template_to_applied<statistics::tags::is_depend_on>
            is_depend_on;

        typedef typename expand_depend_on<typename Tags::list, null_type>::type
            WithDependencies;
        typedef typename meta::copy_without_duplicates<WithDependencies>::type
            UniqueTags;
        typedef typename meta::selection_sort<UniqueTags, is_depend_on>::type
            Sorted;
    public:
        /// @brief Список всех необходимых тэгов, топологически отсортированные
        typedef typename meta::reverse_copy<Sorted>::type type;
    };
}
// namespace tags
}
// namespace statistics
    /** @brief Накопитель для описательных статистик
    @tparam T тип элементов выборки
    @tparam Tag тэг, описывающий накопительную статистику
    @tparam Base базовый класс
    */
    template <class T, class Tag, class Base = empty_type>
    class descriptive;

    /** @brief Накопитель для описательной статистики "Количество элементов"
    @tparam T тип элементов выборки
    @tparam Base базовый класс
    */
    template <class T, class Base>
    class descriptive<T, statistics::tags::count_tag, Base>
     : public Base
    {
    public:
        // Типы
        /// @brief Тип элементов выборки
        typedef T value_type;

        /// @brief Тип для хранения количества элементов
        typedef size_t count_type;

        // Конструкторы
        /** @brief Конструктор без параметров
        @post <tt> this->count() == 0 </tt>
        @post Базовый класс инициализируется с помощью конструктора без
        параметров
        */
        descriptive()
         : Base{}
         , n_{0}
        {}

        /** @brief Конструктор
        @param x первый элемент выборки
        @post <tt> this->count() == 1 </tt>
        @post Базовый класс инициализируется как <tt> Base{x} </tt>
        */
        explicit descriptive(value_type const & x)
         : Base{x}
         , n_{1}
        {}

        // Свойства
        /** @brief Количество элементов выборки
        @return Количество элементов выборки
        */
        count_type const & count() const
        {
            return n_;
        }

        // Обновление
        /** Обработка нового элемента. Сначала данный элемент передаётся
        базовому накопителю, а затем счётчик увеличивается на единичу
        @brief Обновление
        @param x новый элемент выборки
        @return <tt> *this </tt>
        */
        descriptive & operator()(T const & x)
        {
            Base::operator()(x);
            ++ n_;
            return *this;
        }

        // Свободные функции
        /** @brief Свободная функция доступа к накопленному значению
        @param x описательная статистика
        @return <tt> x.count() </tt>
        */
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

    /** @brief Описательная статистика "начальный момента порядка @c N"
    @tparam T тип элементов
    @tparam Base базовая описательная статистика
    @tparam N порядок момента
    @todo Выразить через более общую статистику: с произвольным преобразованием
    */
    template <class T, class Base, size_t N>
    class descriptive<T, statistics::tags::raw_moment_tag<N>, Base>
     : public Base
    {
        static_assert(N > 0, "Use counter instead");
    public:
        // Типы
        /// @brief Тип количества элементов
        typedef typename Base::count_type count_type;

        /// @brief Тип момента
        typedef typename average_type<T, count_type>::type moment_type;

        // Конструкторы
        /** @brief Конструктор без аргументов
        @post <tt> raw_moment(*this, integral_constant<size_t, N>{}) == 0 </tt>
        @post Базовая описательная статистика инициализируется конструктором
        без аргментов
        */
        descriptive()
         : Base{}
         , value_(0)
        {}

        /** @brief Конструктор
        @param x первый элемент выборки
        @post <tt> raw_moment(*this, integral_constant<size_t, N>{}) == 1 </tt>
        @post Базовый класс инициализируется как <tt> Base{x} </tt>
        */
        explicit descriptive(T const & x)
         : Base{x}
         , value_(x)
        {}

        // Обновление
        /** Обработка нового элемента. Сначала данный элемент передаётся
        базовому накопителю, а затем выборочный момент пересчитывается с учётом
        нового значения.
        @brief Обновление
        @param x новый элемент выборки
        @return <tt> *this </tt>
        */
        descriptive & operator()(T const & x)
        {
            Base::operator()(x);
            value_ += (power(x) - value_) / this->count();
            return *this;
        }

        // Свойства
        /** @brief Значение начального момента
        @param x объект-накопитель
        @return Значение начального момента
        @todo Перегрузка с placeholder?
        */
        friend moment_type const &
        raw_moment(descriptive const & x, std::integral_constant<size_t, N>)
        {
            return x.value_;
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

    /** @brief Описательная статистика "выборочное среднее"
    @tparam T тип элементов
    @tparam Base базовая описательная статистика
    @todo Можно ли обойтись без этого класса вообще?
    */
    template <class T, class Base>
    class descriptive<T, statistics::tags::mean_tag, Base>
     : public Base
    {
    public:
        /// @brief Тип для представления выборочного среднего
        typedef typename Base::moment_type mean_type;

        /// @brief Конструкторы как в базовом классе
        using Base::Base;

        /** @brief Значение выборочного среднего
        @return Значение выборочного среднего накопленного к настоящему моменту
        */
        mean_type const & mean() const
        {
            return raw_moment(*this, std::integral_constant<size_t, 1>{});
        }

        /// @brief Обновление -- как в базовом классе
        using Base::operator();

        /** @brief Значение выборочного среднего
        @param d объект-накопитель описательной статистики
        @return <tt> d.mean() </tt>
        */
        friend mean_type const & at_tag(descriptive const & d,
                                        statistics::tags::mean_tag)
        {
            return d.mean();
        }

    protected:
        ~descriptive() = default;
    };

    /** @brief Описательная статистика "дисперсия"
    @tparam T тип элементов
    @tparam Base базовая описательная статистика
    @todo Несмещённая дисперсия?
    @todo Тип возвращаемого значения
    */
    template <class T, class Base>
    class descriptive<T, statistics::tags::variance_tag, Base>
     : public Base
    {
    public:
        // Типы
        /// @brief Тип для представления математического ожидания
        typedef typename Base::mean_type mean_type;

        // Конструкторы
        /** @brief Конструктор без параметров
        @post Инициализирует базовый класс конструктором без параметров
        @post <tt> this->variance() == 0 </tt>
        */
        descriptive()
         : Base{}
         , sq_(0)
        {}

        /** @brief Конструктор без параметров
        @param x первый элемент выборки
        @post Инициализирует базовый класс c @c x в качестве параметра
        @post <tt> this->variance() == 0 </tt>
        */
        explicit descriptive(T const & x)
         : Base{x}
         , sq_(0)
        {}

        // Обновление
        /** @brief Обновление
        @param x новый элемент выборки
        @return <tt> *this </tt>
        */
        descriptive & operator()(T const & x)
        {
            auto old_m = this->mean();

            Base::operator()(x);

            sq_ += (x - old_m) * (x - this->mean());

            return *this;
        }

        // Свойства
        /** @brief Текущее значение дисперсии
        @param x объект-накопитель описательной статистики
        @return <tt> x.variance() </tt>
        */
        friend mean_type at_tag(descriptive const & x,
                                statistics::tags::variance_tag)
        {
            return x.variance();
        }

        /** @brief Текущее значение дисперсии
        @return Текущее значение дисперсии
        */
        mean_type variance() const
        {
            return sq_ / this->count();
        }

    private:
        mean_type sq_;
    };

    /** @brief Описательная статистика "среднеквадратическое отклонение"
    @tparam T тип элементов
    @tparam Base базовая описательная статистика
    @todo Обобщённый накопитель с преобразованием, выразить через него же момент
    */
    template <class T, class Base>
    class descriptive<T, statistics::tags::standard_deviation_tag, Base>
     : public Base
    {
    public:
        // Типы
        typedef typename Base::mean_type mean_type;

        // Конструкторы
        using Base::Base;

        // Обновление
        using Base::operator();

        // Свойства
        /** @brief Значение среднеквадратического уклонения
        @return <tt> sqrt(this->variance()) </tt>
        */
        mean_type standard_deviation() const
        {
            using std::sqrt;
            return sqrt(this->variance());
        }

        /** @brief Значение среднеквадратического уклонения
        @param x объект-накопитель описательной статистики
        @return <tt> x.standard_deviation() </tt>
        */
        friend mean_type at_tag(descriptive const & x,
                            statistics::tags::standard_deviation_tag)
        {
            return x.standard_deviation();
        }

    protected:
        ~descriptive() = default;
    };

    /** @brief Описательная статистика "наименьшее значение"
    @tparam T тип элементов
    @tparam Base базовая описательная статистика
    */
    template <class T, class Base>
    class descriptive<T, statistics::tags::min_tag, Base>
     : public Base
    {
    public:
        /// @brief Тип значения
        typedef T value_type;

        /** @brief Конструктор без параметров
        @post Инициализирует базовый класс конструктором без аргументов
        @post <tt> this->min() == std::numeric_limits<T>::infinity() </tt>
        */
        descriptive()
         : Base{}
         , min_{std::numeric_limits<T>::infinity()}
        {}

        /** @brief Конструктор
        @param x первый элемент выборки
        @post Инициализирует базовый класс c @c x в качестве параметра
        @post <tt> this->min() == x </tt>
        */
        explicit descriptive(T const & x)
         : Base{x}
         , min_{x}
        {}

        /** @brief Значение, накопленное к данному моменту
        @c param x объект-накопитель для описательной статистики
        @return <tt> x.min() </tt>
        */
        friend value_type const & at_tag(descriptive const & x,
                                         statistics::tags::min_tag)
        {
            return x.min();
        }

        /** @brief Значение, накопленное к данному моменту
        @return Наименьшее из значений выборки, обработаннх к данному моменту
        */
        value_type const & min URAL_PREVENT_MACRO_SUBSTITUTION () const
        {
            return min_;
        }

        /** Передаёт базовому накопителю @c x, а затем заменяет наименьшее
        значение, накопленное к настоящему моменту, на @c x, если @c x меньше
        этого значения.
        @brief Обновление
        @param x новый элемент выборки
        @return <tt> *this </tt>
        */
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

    /** @brief Описательная статистика "наибольшее значение"
    @tparam T тип элементов
    @tparam Base базовая описательная статистика
    */
    template <class T, class Base>
    class descriptive<T, statistics::tags::max_tag, Base>
     : public Base
    {
    public:
        /// @brief Тип значения
        typedef T value_type;

        /** @brief Конструктор без параметров
        @post Инициализирует базовый класс конструктором без аргументов
        @post <tt> this->max() == -std::numeric_limits<T>::infinity() </tt>
        */
        descriptive()
         : Base{}
         , max_{-std::numeric_limits<T>::infinity()}
        {}

        /** @brief Конструктор
        @param x первый элемент выборки
        @post Инициализирует базовый класс c @c x в качестве параметра
        @post <tt> this->max() == x </tt>
        */
        explicit descriptive(T const & x)
         : Base{x}
         , max_{x}
        {}

        /** @brief Текущее значение описательной статистики
        @param x объект-накопитель описательной статистики
        @return <tt> x.max() </tt>
        */
        friend value_type const & at_tag(descriptive const & x,
                                         statistics::tags::max_tag)
        {
            return x.max();
        }

        value_type const & max URAL_PREVENT_MACRO_SUBSTITUTION () const
        {
            return max_;
        }

        /** Передаёт @c x базовому накопителю, а затем заменяет наибольшее
        значение, встреченное до сих пор, на @c x, если
        <tt> this->max() < x </tt>
        @brief Обновление значения
        @param x новый элемент выборки
        @return <tt> *this </tt>
        */
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

    /** @brief Описательная статистика "размах"
    @tparam T тип элементов
    @tparam Base базовая описательная статистика
    @todo Добавить требования к базовому классу: max и min
    */
    template <class T, class Base>
    class descriptive<T, statistics::tags::range_tag, Base>
     : public Base
    {
    public:
        /// @brief Тип значения
        typedef T value_type;

        /// @brief Конструкторы
        using Base::Base;

        /// @brief Обновление
        using Base::operator();

        // Свойства
        /** @brief Доступ к значению
        @param x объект-накопитель для описательной статистики
        @return <tt> x.range() </tt>
        */
        friend value_type at_tag(descriptive const & x,
                                 statistics::tags::range_tag)
        {
            return x.range();
        }

        /** @brief Значение размаха выборки
        @return <tt> this->max() - this->min() </tt>
        */
        value_type range () const
        {
            return this->max() - this->min();
        }

    protected:
        ~descriptive() = default;
    };

    /** @brief Класс "набор описательных статистик"
    @tparam T тип элементов выборки
    @tparam Tags список тэгов описательных статистик
    */
    template <class T, class Tags>
    class descriptives;

    /** @brief Специализаци для пустого списка тэгов
    @tparam T тип элементов выборки
    @todo Конструктор с одним аргументом
    */
    template <class T>
    class descriptives<T, null_type>
    {
    protected:
        ~descriptives() = default;

    public:
        /// @brief Конструктор
        descriptives() = default;

        /// @brief Конструктор
        explicit descriptives(T const &)
        {};

        /** @brief Обновление
        @return <tt> *this </tt>
        */
        descriptives & operator()(T const &)
        {
            return *this;
        }
    };

    /** @brief Описательные статистики
    @tparam T тип элементов
    @tparam Tags список тэгов
    @note Одна из проблем с таким дизайном: можно забыть определить operator()
    @note Дбулирование тэгов не проверяется, чтобы не увиличивать время
    компиляции. @c descriptives_facade устраняет дубликаты из списка, прежде,
    чем передать его @c descriptives
    @todo Оптимизировать min | max
    */
    template <class T, class Tags>
    class descriptives
     : public descriptive<T, typename Tags::head, descriptives<T, typename Tags::tail>>
    {
        typedef descriptive<T, typename Tags::head, descriptives<T, typename Tags::tail>>
            Base;
    public:
        /// @brief Конструктор без аргументов
        descriptives() = default;

        /** @brief Конструктор с параметром
        @param init_value первый элемент выборки
        */
        explicit descriptives(T const & init_value)
         : Base{init_value}
        {}

        /** @brief Обновление элемента выборки
        @param x новое значение
        @pre Объект создан с помощью конструктора с параметром или ему было
        присвоено значение
        @return <tt> *this </tt>
        */
        descriptives & operator()(T const & x)
        {
            Base::operator()(x);
            return *this;
        }
    };

    /** @brief Фасад для описательных статистик
    @tparam T тип элементов
    @tparam Tags список тэгов
    */
    template <class T, class Tags>
    class descriptives_facade
     : public descriptives<T, typename statistics::tags::prepare<Tags>::type>
    {
        typedef typename statistics::tags::prepare<Tags>::type PreparedTags;
        typedef descriptives<T, PreparedTags> Base;

    public:
        // Типы
        /// @brief Список тэгов
        typedef Tags tags_list;

        // Конструкторы
        /** @brief Конструктор без аргументов. Используется для описания пустой
        последовательности.
        */
        descriptives_facade() = default;

        /** @brief Конструктор
        @param x значение
        */
        explicit descriptives_facade(T const & x)
         : Base{x}
        {}

        // Обновление
        /** @brief Обновление статистик
        @param x новое значение
        @return <tt> *this </tt>
        */
        descriptives_facade & operator()(T const & x)
        {
            Base::operator()(x);
            return *this;
        }

        // Свойства
        /** @brief Доступ к свойствам по тэгу
        @tparam Tag тип-тэг
        @pre Тип @c Tag должен содержаться в списке типов @c tag_list
        @return <tt> at_tag(*this, Tag{}) </tt>
        */
        template <class Tag>
        auto operator[](statistics::tags_list<Tag>) const
        -> decltype(at_tag(*this, Tag{}))
        {
            static_assert(!std::is_same<ural::meta::find<PreparedTags, Tag>, null_type>::value,
                          "Unsupported tag");
            return at_tag(*this, Tag{});
        }
    };

    /** @brief Алгоритм сбора описательных статистик
    @tparam Tags список тэгов описательных статистик
    @param in входная последовательность
    @return Объект, содержащий описательные статистики
    */
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

    /** @brief Стандартизация выборки
    @param in входная последовательность
    @param out выходная последовательность
    @pre Дисперсия @c in должна быть не равной нулю.
    */
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

    /** @brief Накопитель для вычисления ковариационной матрицы
    @tparam Vector тип вектора (элемент выборки)
    @todo Унифицировать с вычислением дисперсии
    */
    template <class Vector>
    class covariance_matrix_accumulator
    {
    public:
        /// @brief Тип элементов
        typedef typename Vector::value_type element_type;

        /// @brief Тип среднего
        typedef Vector mean_type;

        /// @brief Тип коваирационной матрицы
        typedef boost::numeric::ublas::symmetric_matrix<element_type>
            covariance_matrix_type;

        /** @brief Конструктор
        @param dim размерность вектора
        */
        explicit covariance_matrix_accumulator(typename Vector::size_type dim)
         : n_{0}
         , m_(dim)
         , cov_(dim)
        {}

        /** @brief Обновление накопленных значений
        @param x новое значение
        @return <tt> *this </tt>
        */
        covariance_matrix_accumulator &
        operator()(Vector const & x)
        {
            assert(x.size() == m_.size());

            ++ n_;

            auto d1 = (x - m_);

            m_ += d1 / n_;

            auto d2 = (x - m_);

            for(auto i : numbers(0, cov_.size1()))
            for(auto j : numbers(0, i+1))
            {
                cov_(i, j) += d1(i) * d2(j);
            }

            return *this;
        }

        /** @brief Вектор средних
        @return Вектор средних, накопленный к данному моменту
        */
        mean_type const & mean() const
        {
            return m_;
        }

        //@{
        /** @brief Ковариационная матрица
        @return Выборочная ковариационная матрица, накопленная к данному моменту
        */
        covariance_matrix_type covariance_matrix() const
        {
            return cov_ / (n_ > 1 ? (n_ - 1) : 1);
        }

        covariance_matrix_type cov() const
        {
            return this->covariance_matrix();
        }
        //@}

    private:
        size_t n_;
        mean_type m_;
        covariance_matrix_type cov_;
    };

    /** @brief Проверка гипотезы о дисперсии
    @param s ожидаемое значение дисперсии
    @param s_sample выборочная (несмещённая) дисперсия
    @param n размер выборки
    @pre <tt> s > 0 </tt>
    @pre <tt> s_sample > 0 </tt>
    */
    template <class RealType>
    probability<>
    variance_hypothesis_test(RealType const & s_sample, RealType const & s,
                            size_t n)
    {
        assert(s > 0);
        assert(s_sample > 0);

        boost::math::chi_squared_distribution<> distr(n-1);

        RealType const chi_sq = s_sample / s * distr.degrees_of_freedom();

        return probability<>{cdf(distr, chi_sq)};
    }

    /** @brief Проверка гипотезы о мат. ожидании при известной дисперсии
    @param m_sample выборочное среднее
    @param m теоретическое значение математического ожидания
    @param s2 теоретическое значение дисперсии
    @param n размер выборки
    @pre <tt> s2 > 0 </tt>
    */
    template <class RealType>
    probability<>
    mean_hypothesis_test_known_variance(RealType const & m_sample,
                                        RealType const & m, RealType const & s2,
                                        size_t n)
    {
        assert(s2 > 0);

        using std::sqrt;
        auto const z = (m_sample - m) / sqrt(s2 / n);
        boost::math::normal_distribution<> distr{};
        return probability<>{cdf(distr, z)};
    }
}
// namespace ural

#endif
// Z_URAL_STATISTICS_HPP_INCLUDED
