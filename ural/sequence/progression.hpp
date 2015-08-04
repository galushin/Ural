#ifndef Z_URAL_SEQUENCE_PROGRESSION_HPP_INCLUDED
#define Z_URAL_SEQUENCE_PROGRESSION_HPP_INCLUDED

/** @file ural/sequence/progression.hpp
 @brief Прогрессии
*/

#include <ural/sequence/base.hpp>
#include <ural/defs.hpp>

namespace ural
{
    /// @brief Тип-тэг, обозначающий (абстрактную единицу)
    struct unit_t
    {};

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

    @todo настройка типа шага

    @todo Последовательность произвольного доступа. Здесь есть две проблемы:
    1. Необходимо по операции сложения построить операцию умножения
    2. Тип возвращаемого значения оператора [] и front не совпадает
    */
    template <class Additive, class Plus = use_default,
              class Traversal = use_default>
    class arithmetic_progression
     : public sequence_base<arithmetic_progression<Additive, Plus, Traversal>,
                            typename default_helper<Plus, plus<>>::type>
    {
        using Base =
            sequence_base<arithmetic_progression<Additive, Plus, Traversal>,
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
        /// @brief Тип значения
        typedef Additive value_type;

        /// @brief Тип ссылки
        typedef value_type const & reference;

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
        arithmetic_progression(Additive first, Additive step)
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
        arithmetic_progression(Additive first, Additive step, operation_type op)
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
        arithmetic_progression traversed_front() const
        {
            return arithmetic_progression(first_.old_value(), step_);
        }

        /// @brief Отбрасывание пройденной части последовательности
        void shrink_front()
        {
            first_.commit();
        }

        /** @brief Полная последовательность (включая пройденные части)
        @return Полная последовательность
        */
        arithmetic_progression original() const;

        // Последовательность произвольного доступа (бесконечная)

    private:
        static auto constexpr is_forward
            = std::is_convertible<traversal_tag, forward_traversal_tag>::value;

        typedef typename std::conditional<is_forward,
                                          with_old_value<value_type>,
                                          value_type>::type
            First_type;

        First_type first_;
        Additive step_;
    };

    /** @brief Создание последовательности, представляющей арифметическую
    прогрессию
    @param first Первый элемент
    @param step Шаг
    @param op операция, используемая в качестве сложения
    */
    template <class Additive, class Plus>
    auto make_arithmetic_progression(Additive first, Additive step, Plus op)
    -> arithmetic_progression<Additive, decltype(make_callable(std::move(op)))>
    {
        return {std::move(first), std::move(step), std::move(op)};
    }

    /** @brief Создание последовательности, представляющей арифметическую
    прогрессию
    @param first Первый элемент
    @param step Шаг
    */
    template <class Additive>
    arithmetic_progression<Additive>
    make_arithmetic_progression(Additive first, Additive step)
    {
        return {std::move(first), std::move(step)};
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_PROGRESSION_HPP_INCLUDED
