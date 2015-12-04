#ifndef Z_URAL_SEQUENCE_PROGRESSION_HPP_INCLUDED
#define Z_URAL_SEQUENCE_PROGRESSION_HPP_INCLUDED

/** @file ural/sequence/progression.hpp
 @brief Прогрессии
*/

#include <ural/sequence/adaptors/taken_exactly.hpp>
#include <ural/math.hpp>
#include <ural/sequence/base.hpp>
#include <ural/defs.hpp>

namespace ural
{
    /// @brief Тип-тэг, обозначающий (абстрактную единицу)
    struct unit_t
    {
    public:
        template <class Number>
        constexpr operator Number() const
        {
            return Number(1);
        }
    };

    /** @brief Эквивалентно <tt> ++ x </tt>
    @param x число
    @return <tt> x </tt>
    */
    template <class Number>
    Number & operator+=(Number & x, unit_t)
    {
        return ++ x;
    }

    //@{
    /** @brief Оператор сложения числа с @c unit_t
    @param x число
    @return <tt> x + 1 </tt>
    */
    template <class Number>
    Number operator+(Number const & x, unit_t)
    {
        return x+1;
    }

    template <class Number>
    Number operator+(unit_t, Number const & x)
    {
        return x+1;
    }
    //@}


    //@{
    /** @brief Оператор умножения числа на @c unit_t
    @param x число
    @return <tt> x </tt>
    */
    template <class Number>
    Number const & operator*(Number const & x, unit_t)
    {
        return x;
    }

    template <class Number>
    Number const & operator*(unit_t, Number const & x)
    {
        return x;
    }
    //@}

    /** @brief Оператор деление числа для @c unit_t
    @param x число
    @return <tt> x </tt>
    */
    template <class Number>
    Number const & operator/(Number const & x, unit_t)
    {
        return x;
    }

    /** @brief Арифметическая прогрессия
    @tparam Additive тип значений
    @tparam Plus операция, используемая в качестве сложения
    @tparam Traversal категория обхода
    @tparam Step тип шага

    @todo Последовательность произвольного доступа. Здесь есть две проблемы:
    1. Необходимо по операции сложения построить операцию умножения
    2. Тип возвращаемого значения оператора [] и front не совпадает
    */
    template <class Additive,
              class Plus = use_default,
              class Traversal = use_default,
              class Step = use_default>
    class arithmetic_progression
     : public sequence_base<arithmetic_progression<Additive, Plus, Traversal, Step>,
                            typename default_helper<Plus, plus<>>::type>
    {
        using Base =
            sequence_base<arithmetic_progression<Additive, Plus, Traversal, Step>,
                          typename default_helper<Plus, plus<>>::type>;

        static_assert(!std::is_same<Traversal, bidirectional_traversal_tag>::value,
                      "Infinite sequence can't be bidirectional");

    public:
        /** @brief Оператор "равно"
        @param x, y операнды
        */
        friend bool operator==(arithmetic_progression const & x,
                               arithmetic_progression const & y)
        {
            return x.first_ == y.first_ && x.step_ == y.step_
                   && x.function() == y.function();
        }

        // Типы
        /// @brief Тип размера шага
        using step_type = typename default_helper<Step, Additive>::type;

        /// @brief Тип значения
        typedef Additive value_type;

        /// @brief Тип ссылки
        typedef value_type reference;

        /// @brief Тип расстояния
        using distance_type = std::ptrdiff_t;

        /** @note Проблема в том, что при произвольном доступе возвращается
        вычисленное значение, а не ссылка.
        */
        using traversal_tag
            = typename default_helper<Traversal, random_access_traversal_tag>::type;

        /// @brief Тип указателя
        typedef value_type const * pointer;

        /// @brief Тип операции
        typedef typename default_helper<Plus, plus<>>::type operation_type;

        // Конструкторы
        /** @brief Конструктор
        @param first Первый элемент
        @param step Шаг
        @post <tt> this->function() == operation_type{} </tt>
        @post <tt> **this == first </tt>
        @post <tt> this->step() == step </tt>
        */
        arithmetic_progression(Additive first, step_type step)
         : Base()
         , first_{std::move(first)}
         , step_{std::move(step)}
        {}

        /** @brief Конструктор
        @param first Первый элемент
        @param step Шаг
        @param op Операция, используемая для получения новых элементов
        @post <tt> **this == first </tt>
        @post <tt> this->step() == step </tt>
        @post <tt> this->function() == op </tt>
        */
        arithmetic_progression(Additive first, step_type step, operation_type op)
         : Base(std::move(op))
         , first_(std::move(first))
         , step_(std::move(step))
        {}

        // Свойства
        /** @brief Функциональный объект, используемый как операция
        @return Функциональный объект, используемый как операция
        */
        operation_type const & function() const
        {
            return Base::payload();
        }

        /** @brief Размер шага
        @return Размер шага
        */
        step_type const & step() const
        {
            return this->step_;
        }

        // Однопроходна последовательность
        /** @brief Проверка исчерпания последовательности
        @return @b true, если в последовательности больше нет элементов,
        иначе --- @b false.
        */
        bool operator!() const
        {
            return false;
        }

        /** @brief Доступ к текущему (переднему) элементу последовательности
        @return Ссылка на передний элемент последовательности
        @pre <tt> bool(*this) != false </tt>
        */
        reference front() const
        {
            return this->first_.value();
        }

        /** @brief Переход к следующему элементу последовательности
        @pre <tt> bool(*this) != false </tt>
        @return <tt> *this </tt>
        */
        void pop_front()
        {
            this->first_ = this->function()(std::move(ural::get(this->first_)),
                                           this->step_);
        }

        // Прямая последовательность
        /** @brief Пройденная передняя часть прогрессии
        @return Пройденная передняя часть прогрессии
        */
        taken_exactly_sequence<arithmetic_progression, distance_type>
        traversed_front() const
        {
            distance_type n = (this->front() - this->first_.old_value()) / this->step();
            return arithmetic_progression(this->first_.old_value(), this->step())
                    | ural::taken_exactly(std::move(n));
        }

        /// @brief Отбрасывание пройденной части последовательности
        void shrink_front()
        {
            first_.commit();
        }

        /** @brief Полная последовательность (включая пройденные части)
        @return Полная последовательность
        */
        arithmetic_progression original() const
        {
            return arithmetic_progression(first_.old_value(), this->step_);
        }

        // Последовательность произвольного доступа (бесконечная)
        reference operator[](distance_type n) const
        {
            assert(0 <= n);

            return ural::power_accumulate_semigroup(this->front(),
                                                    value_type(this->step_),
                                                    n, this->function());
        }

        arithmetic_progression & operator+=(distance_type n)
        {
            first_.value() += n * step_;
            return *this;
        }

    private:
        static auto constexpr is_forward
            = std::is_convertible<traversal_tag, forward_traversal_tag>::value;

        typedef typename std::conditional<is_forward,
                                          with_old_value<value_type>,
                                          value_type>::type
            First_type;

        First_type first_;
        step_type step_;
    };

    /** @brief Тип Функционального объекта для создания
    @c arithmetic_progression
    */
    struct make_arithmetic_progression_fn
    {
    public:
        /** @brief Создание последовательности, представляющей арифметическую
        прогрессию
        @param first Первый элемент
        @param step Шаг
        @param op операция, используемая в качестве сложения
        */
        template <class Additive, class Plus>
        auto operator()(Additive first, Additive step, Plus op) const
        -> arithmetic_progression<Additive, decltype(make_callable(std::move(op)))>
        {
            return {std::move(first), std::move(step), std::move(op)};
        }

        /** @brief Создание последовательности, представляющей арифметическую
        прогрессию
        @param first Первый элемент
        @param step Шаг
        @return <tt> {std::move(first), std::move(step)} </tt>
        */
        template <class Additive>
        arithmetic_progression<Additive>
        operator()(Additive first, Additive step) const
        {
            return {std::move(first), std::move(step)};
        }

    };

    /** @brief Тип функционального объекта для создания геометрической
    прогрессии
    */
    struct make_geometric_progression_fn
    {
    public:
        /** @brief Создание последовательности, представляющей геометрическую
        прогрессию
        @param first Первый элемент
        @param step Шаг
        @return <tt> {std::move(first), std::move(step)} </tt>
        */
        template <class Multiplicative, class Step>
        arithmetic_progression<Multiplicative, ural::multiplies<>, use_default, Step>
        operator()(Multiplicative first, Step step) const
        {
            return {std::move(first), std::move(step)};
        }
    };

    namespace
    {
        /// @brief Функциональный объект для создания @c arithmetic_progression
        constexpr auto const & make_arithmetic_progression
            = odr_const<make_arithmetic_progression_fn>;

        /// @brief Функциональный объект для создания геометрической прогрессии
        constexpr auto const & make_geometric_progression
            = odr_const<make_geometric_progression_fn>;
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_PROGRESSION_HPP_INCLUDED
