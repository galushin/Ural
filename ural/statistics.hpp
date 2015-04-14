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
 @todo Накопительные статистики: поддержка временных значений
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

#include <ural/statistics/probability.hpp>

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

namespace ural
{
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
    struct range_tag    : declare_depend_on<>{};

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

        // @todo Выразить через более высокоуровневые конструкции
        typedef meta::contains<WithDependencies, tags::min_tag> C_min;
        typedef meta::contains<WithDependencies, tags::max_tag> C_max;
        typedef std::integral_constant<bool, C_min::value && C_max::value>
            C_min_and_max;

        struct is_min_or_max_tag
        {
            template <class T>
            struct apply
             : std::integral_constant<bool, std::is_same<T, tags::min_tag>::value || std::is_same<T, tags::max_tag>::value>
            {};
        };

        typedef meta::replace_if<WithDependencies, is_min_or_max_tag, tags::range_tag> R2;

        typedef typename std::conditional<C_min_and_max::value,
                                          R2,
                                          declare_type<WithDependencies>>::type::type NeededTags;

        typedef typename meta::copy_without_duplicates<NeededTags>::type
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
    @tparam Weight тип веса
    */
    template <class T, class Tag, class Base, class Weight>
    class descriptive;

    /** @brief Накопитель для описательной статистики "Количество элементов"
    @tparam T тип элементов выборки
    @tparam Base базовый класс
    @tparam Weight тип веса
    */
    template <class T, class Base, class Weight>
    class descriptive<T, statistics::tags::count_tag, Base, Weight>
     : public Base
    {
    public:
        // Типы
        /// @brief Тип элементов выборки
        typedef T value_type;

        /// @brief Тип для хранения количества элементов
        typedef size_t count_type;

        /// @brief Тип весов
        typedef Weight weight_type;

        // Конструкторы
        /** @brief Конструктор без параметров
        @post <tt> this->count() == 0 </tt>
        @post Базовый класс инициализируется с помощью конструктора без
        параметров
        */
        descriptive()
         : Base{}
         , n_{0}
         , w_sum_(0)
        {}

        /** @brief Конструктор
        @param x первый элемент выборки
        @param w вес первого элемента выборки
        @post <tt> this->count() == 1 </tt>
        @post Базовый класс инициализируется как <tt> Base{x} </tt>
        */
        explicit descriptive(value_type const & x, weight_type const & w)
         : Base(x, w)
         , n_{1}
         , w_sum_(w)
        {}

        // Свойства
        /** @brief Количество элементов выборки
        @return Количество элементов выборки
        */
        count_type const & count() const
        {
            return n_;
        }

        /** @brief Сумма весов
        @return Накопленная к настоящему моменту сумма весов
        */
        weight_type const & weight_sum() const
        {
            return this->w_sum_;
        }

        // Обновление
        /** Обработка нового элемента. Сначала данный элемент передаётся
        базовому накопителю, а затем счётчик увеличивается на единичу
        @brief Обновление
        @param x новый элемент выборки
        @param w вес нового элемента выборки
        @return <tt> *this </tt>
        */
        descriptive & operator()(T const & x, weight_type const & w)
        {
            Base::operator()(x, w);
            ++ n_;
            w_sum_ += w;
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

        // @todo покрыть тестами аналогичную функцию для weight_sum

    protected:
        ~descriptive() = default;

    private:
        count_type n_;
        weight_type w_sum_;
    };

    /** @brief Описательная статистика "начальный момента порядка @c N"
    @tparam T тип элементов
    @tparam Base базовая описательная статистика
    @tparam N порядок момента
    @tparam Weight тип веса
    @todo Выразить через более общую статистику: с произвольным преобразованием
    */
    template <class T, class Base, size_t N, class Weight>
    class descriptive<T, statistics::tags::raw_moment_tag<N>, Base, Weight>
     : public Base
    {
        static_assert(N > 0, "Use counter instead");
    public:
        // Типы
        /// @brief Тип количества элементов
        typedef typename Base::count_type count_type;

        /// @brief Тип весов
        typedef typename Base::weight_type weight_type;

        /// @brief Тип момента
        typedef typename average_type<T, weight_type>::type moment_type;

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
        @param w вес первого элемента выборки
        @post <tt> raw_moment(*this, integral_constant<size_t, N>{}) == 1 </tt>
        @post Базовый класс инициализируется как <tt> Base{x} </tt>
        */
        explicit descriptive(T const & x, weight_type const & w)
         : Base(x, w)
         , value_(x)
        {}

        // Обновление
        /** Обработка нового элемента. Сначала данный элемент передаётся
        базовому накопителю, а затем выборочный момент пересчитывается с учётом
        нового значения.
        @brief Обновление
        @param x новый элемент выборки
        @param w вес нового элемента выборки
        @return <tt> *this </tt>
        */
        descriptive & operator()(T const & x, weight_type const & w)
        {
            Base::operator()(x, w);
            value_ += (power(x) - value_) * w / this->weight_sum();
            return *this;
        }

        // Свойства
        template <class Placeholder>
        friend
        typename std::enable_if<std::is_placeholder<Placeholder>::value == N, moment_type const &>::type
        raw_moment(descriptive const & x, Placeholder)
        {
            return x.value_;
        }

        /** @brief Значение начального момента
        @param x объект-накопитель
        @return Значение начального момента
        */
        friend moment_type const &
        raw_moment(descriptive const & x, std::integral_constant<size_t, N>)
        {
            return raw_moment(x, ural::placeholder<N-1>{});
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
    @tparam Weight тип веса
    @todo Можно ли обойтись без этого класса вообще?
    */
    template <class T, class Base, class Weight>
    class descriptive<T, statistics::tags::mean_tag, Base, Weight>
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
    @tparam Weight тип веса
    @todo Несмещённая дисперсия?
    @todo Тип возвращаемого значения
    */
    template <class T, class Base, class Weight>
    class descriptive<T, statistics::tags::variance_tag, Base, Weight>
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
        @param w вес первого элемента выборки
        @post Инициализирует базовый класс c @c x в качестве параметра
        @post <tt> this->variance() == 0 </tt>
        */
        explicit descriptive(T const & x, Weight const & w)
         : Base(x, w)
         , sq_(0)
        {}

        // Обновление
        /** @brief Обновление
        @param x новый элемент выборки
        @param w вес нового элемента выборки
        @return <tt> *this </tt>
        */
        descriptive & operator()(T const & x, Weight const & w)
        {
            auto old_m = this->mean();

            Base::operator()(x, w);

            sq_ += (x - old_m) * (x - this->mean()) * w;

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
            return sq_ / this->weight_sum();
        }

    private:
        mean_type sq_;
    };

    /** @brief Описательная статистика "среднеквадратическое отклонение"
    @tparam T тип элементов
    @tparam Base базовая описательная статистика
    @tparam Weight тип веса
    @todo Обобщённый накопитель с преобразованием, выразить через него же момент
    */
    template <class T, class Base, class Weight>
    class descriptive<T, statistics::tags::standard_deviation_tag, Base, Weight>
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
    @tparam Weight тип веса
    */
    template <class T, class Base, class Weight>
    class descriptive<T, statistics::tags::min_tag, Base, Weight>
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
        @param w вес первого элемента выборки
        @post Инициализирует базовый класс c @c x в качестве параметра
        @post <tt> this->min() == x </tt>
        */
        explicit descriptive(T const & x, Weight const & w)
         : Base(x, w)
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
        @param w вес нового элемента выборки
        @return <tt> *this </tt>
        */
        descriptive & operator()(T const & x, Weight const & w)
        {
            Base::operator()(x, w);
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
    @tparam Weight тип веса
    */
    template <class T, class Base, class Weight>
    class descriptive<T, statistics::tags::max_tag, Base, Weight>
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
        @param w вес первого элемента выборки
        @post Инициализирует базовый класс c @c x в качестве параметра
        @post <tt> this->max() == x </tt>
        */
        explicit descriptive(T const & x, Weight const & w)
         : Base(x, w)
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
        @param w вес нового элемента выборки
        @return <tt> *this </tt>
        */
        descriptive & operator()(T const & x, Weight const & w)
        {
            Base::operator()(x, w);

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
    @tparam Weight тип веса
    */
    template <class T, class Base, class Weight>
    class descriptive<T, statistics::tags::range_tag, Base, Weight>
     : public Base
    {
    public:
        /// @brief Тип значения
        typedef T value_type;

        // Конструкторы
        /** @brief Конструктор без параметров
        @post Инициализирует базовый класс конструктором без аргументов
        @post <tt> this->max() == -std::numeric_limits<T>::infinity() </tt>
        @post <tt> this->min() == std::numeric_limits<T>::infinity() </tt>
        */
        descriptive()
         : Base()
         , min_(std::numeric_limits<T>::infinity())
         , max_(-std::numeric_limits<T>::infinity())
        {}

        /** @brief Конструктор
        @param x первый элемент выборки
        @param w вес первого элемента выборки
        @post <tt> this->max() == x </tt>
        @post <tt> this->min() == x </tt>
        */
        explicit descriptive(T const & x, Weight const & w)
         : Base(x, w)
         , min_(x)
         , max_(x)
        {}

        /** @brief Обновление значения
        @param x новый элемент выборки
        @param w вес нового элемента выборки
        @return <tt> *this </tt>
        */
        descriptive & operator()(T const & x, Weight const & w)
        {
            Base::operator()(x, w);

            assert(this->min_ != std::numeric_limits<T>::infinity());

            if(x < this->min())
            {
                min_ = x;
            }
            else if(x > this->max())
            {
                max_ = x;
            }
            return *this;
        }

        // Свойства
        /** @brief Наименьшее значение, накопленное к данному моменту
        @c param x объект-накопитель для описательной статистики
        @return <tt> x.min() </tt>
        */
        friend value_type const & at_tag(descriptive const & x,
                                         statistics::tags::min_tag)
        {
            return x.min();
        }

        /** @brief Наименьшее значение, накопленное к данному моменту
        @return Наименьшее из значений выборки, обработаннх к данному моменту
        */
        value_type const & min URAL_PREVENT_MACRO_SUBSTITUTION () const
        {
            return this->min_;
        }

        /** @brief Наибольшее значение значение описательной статистики
        @param x объект-накопитель описательной статистики
        @return <tt> x.max() </tt>
        */
        friend value_type const & at_tag(descriptive const & x,
                                         statistics::tags::max_tag)
        {
            return x.max();
        }

        /** @brief Наибольшее значение, накопленное к данному моменту
        @return Наибольшее из значений выборки, обработаннх к данному моменту
        */
        value_type const & max URAL_PREVENT_MACRO_SUBSTITUTION () const
        {
            return this->max_;
        }

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

    private:
        value_type min_;
        value_type max_;
    };

    /** @brief Класс "набор описательных статистик"
    @tparam T тип элементов выборки
    @tparam Tags список тэгов описательных статистик
    @tparam Weight тип веса
    */
    template <class T, class Tags, class Weight>
    class descriptives;

    /** @brief Специализаци для пустого списка тэгов
    @tparam T тип элементов выборки
    @tparam Weight тип веса
    */
    template <class T, class Weight>
    class descriptives<T, null_type, Weight>
    {
    protected:
        ~descriptives() = default;

    public:
        /// @brief Конструктор
        descriptives() = default;

        /// @brief Конструктор
        explicit descriptives(T const &, Weight const &)
        {};

        /** @brief Обновление
        @return <tt> *this </tt>
        */
        descriptives & operator()(T const &, Weight const &)
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
    */
    template <class T, class Tags, class Weight>
    class descriptives
     : public descriptive<T, typename Tags::head, descriptives<T, typename Tags::tail, Weight>, Weight>
    {
        typedef descriptive<T, typename Tags::head, descriptives<T, typename Tags::tail, Weight>, Weight>
            Base;
    public:
        /// @brief Конструктор без аргументов
        descriptives() = default;

        /** @brief Конструктор с параметром
        @param init_value первый элемент выборки
        @param w вес первого элемента выборки
        */
        explicit descriptives(T const & init_value, Weight const & w)
         : Base(init_value, w)
        {}

        /** @brief Обновление элемента выборки
        @param x новое значение
        @param w вес нового элемента выборки
        @pre Объект создан с помощью конструктора с параметром или ему было
        присвоено значение
        @return <tt> *this </tt>
        */
        descriptives & operator()(T const & x, Weight const & w)
        {
            Base::operator()(x, w);
            return *this;
        }
    };

    /** @brief Фасад для описательных статистик
    @tparam T тип элементов
    @tparam Tags список тэгов
    @tparam Weight тип весов
    */
    template <class T, class Tags, class Weight = std::size_t>
    class descriptives_facade
     : public descriptives<T, typename statistics::tags::prepare<Tags>::type, Weight>
    {
        typedef typename statistics::tags::prepare<Tags>::type PreparedTags;
        typedef descriptives<T, PreparedTags, Weight> Base;

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
         : descriptives_facade(x, Weight(1))
        {}

        /** @brief Конструктор
        @param x значение
        @param w вес первого элемента выборки
        */
        descriptives_facade(T const & x, Weight const & w)
         : Base(x, w)
        {}

        // Обновление
        /** @brief Обновление статистик
        @param x новое значение
        @return <tt> *this </tt>
        */
        descriptives_facade & operator()(T const & x)
        {
            return (*this)(x, Weight(1));
        }

        /** @brief Обновление статистик
        @param x новое значение
        @param w вес нового элемента выборки
        @return <tt> *this </tt>
        */
        descriptives_facade & operator()(T const & x, Weight const & w)
        {
            Base::operator()(x, w);
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

    /** @brief Алгоритм сбора описательных статистик
    @tparam Tags список тэгов описательных статистик
    @param in входная последовательность
    @param ws последовательность весов
    @return Объект, содержащий описательные статистики
    */
    template <class Input, class Tags, class Weights>
    auto describe(Input && in, Tags, Weights && ws)
    -> descriptives_facade<typename decltype(ural::sequence_fwd<Input>(in))::value_type,
                           Tags,
                           typename decltype(ural::sequence_fwd<Weights>(ws))::value_type>
    {
        // @todo как обрабатывать ситуацию разной длины последовательностей
        // данных и весов
        // @todo устранить дублирование
        typedef typename decltype(ural::sequence_fwd<Input>(in))::value_type Value;
        typedef typename decltype(ural::sequence_fwd<Weights>(ws))::value_type Weight_type;
        typedef descriptives_facade<Value, Tags, Weight_type> Result;

        using ural::sequence;
        auto in_seq = ural::sequence_fwd<Input>(in);
        auto ws_seq = ural::sequence_fwd<Weights>(ws);

        if(!in_seq || !ws_seq)
        {
            return Result{};
        }

        // @todo заменить на алгоритм
        Result r(*in_seq, *ws_seq);
        ++ in_seq;
        ++ ws_seq;

        for(; !!in_seq && !!ws_seq; ++ in_seq, ++ ws_seq)
        {
            r(*in_seq, *ws_seq);
        }

        return r;
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

        assert(s != 0);

        auto f = [&m, &s](Value const & x) { return (x - m) / s; };

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
