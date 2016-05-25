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
namespace experimental
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
    @tparam CursorTag категория курсора
    @tparam Step тип шага
    */
    template <class Additive,
              class Plus = use_default,
              class CursorTag = use_default,
              class Step = use_default>
    class arithmetic_progression_cursor
     : public cursor_base<arithmetic_progression_cursor<Additive, Plus, CursorTag, Step>,
                            FunctionType<experimental::defaulted_type_t<Plus, plus<>>>>
    {
        using Base = cursor_base<arithmetic_progression_cursor,
                                   FunctionType<experimental::defaulted_type_t<Plus, plus<>>>>;

        static_assert(!std::is_same<CursorTag, bidirectional_cursor_tag>::value,
                      "Infinite sequence can't be bidirectional");

    public:
        /** @brief Оператор "равно"
        @param x, y операнды
        @return @b true, если последовательности равны, иначе --- @b false.
        */
        friend bool operator==(arithmetic_progression_cursor const & x,
                               arithmetic_progression_cursor const & y)
        {
            return x.first_ == y.first_ && x.step_ == y.step_
                   && x.function() == y.function();
        }

        // Типы
        /// @brief Тип размера шага
        using step_type = experimental::defaulted_type_t<Step, Additive>;

        /// @brief Тип значения
        typedef Additive value_type;

        /// @brief Тип ссылки
        typedef value_type reference;

        /// @brief Тип расстояния
        using distance_type = std::ptrdiff_t;

        /** @brief Категория курсора
        @note Проблема в том, что при произвольном доступе возвращается
        вычисленное значение, а не ссылка.
        */
        using cursor_tag = experimental::defaulted_type_t<CursorTag, random_access_cursor_tag>;

        /// @brief Тип указателя
        typedef value_type const * pointer;

        /// @brief Тип операции
        using operation_type = FunctionType<experimental::defaulted_type_t<Plus, plus<>>>;

        // Конструкторы
        /** @brief Конструктор
        @param first Первый элемент
        @param step Шаг
        @post <tt> this->function() == operation_type{} </tt>
        @post <tt> **this == first </tt>
        @post <tt> this->step() == step </tt>
        */
        arithmetic_progression_cursor(Additive first, step_type step)
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
        arithmetic_progression_cursor(Additive first, step_type step, operation_type op)
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
        @return Передний элемент последовательности
        @pre <tt> bool(*this) != false </tt>
        */
        reference front() const
        {
            return ural::experimental::get(this->first_);
        }

        /** @brief Переход к следующему элементу последовательности
        @pre <tt> bool(*this) != false </tt>
        @return <tt> *this </tt>
        */
        void pop_front()
        {
            this->first_ = this->function()(std::move(ural::experimental::get(this->first_)),
                                            this->step_);
        }

        // Прямой курсор
        /** @brief Пройденная передняя часть прогрессии
        @return Пройденная передняя часть прогрессии
        */
        taken_exactly_cursor<arithmetic_progression_cursor, distance_type>
        traversed_front() const
        {
            distance_type n = (this->front() - this->first_.old_value()) / this->step();
            return arithmetic_progression_cursor(this->first_.old_value(), this->step())
                    | ::ural::experimental::taken_exactly(std::move(n));
        }

        /// @brief Отбрасывание пройденной части последовательности
        void shrink_front()
        {
            first_.commit();
        }

        /** @brief Полная последовательность (включая пройденные части)
        @return Полная последовательность
        */
        arithmetic_progression_cursor original() const
        {
            return arithmetic_progression_cursor(first_.old_value(), this->step_);
        }

        // Последовательность произвольного доступа (бесконечная)
        /** @brief Оператор доступа по индексу
        @param n номер элемента
        @pre <tt> n > 0 </tt>
        @return @c n-ый элемент
        */
        reference operator[](distance_type n) const
        {
            assert(0 <= n);

            return ural::power_accumulate_semigroup(this->front(),
                                                    value_type(this->step_),
                                                    n, this->function());
        }

        /** @brief Продвижение на заданное число шагов
        @param n число элементов, которые нужно пропустить
        @return <tt> *this </tt>
        @pre <tt> n > 0 </tt>
        */
        arithmetic_progression_cursor & operator+=(distance_type n)
        {
            first_ = (*this)[n];
            return *this;
        }

    private:
        using First_type
            = wrap_with_old_value_if_forward_t<cursor_tag, value_type>;

        First_type first_;
        step_type step_;
    };

    /** @brief Тип Функционального объекта для создания
    @c arithmetic_progression_cursor
    */
    struct make_arithmetic_progression_cursor_fn
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
        -> arithmetic_progression_cursor<Additive, decltype(make_callable(std::move(op)))>
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
        arithmetic_progression_cursor<Additive>
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
        arithmetic_progression_cursor<Multiplicative, ural::multiplies<>, use_default, Step>
        operator()(Multiplicative first, Step step) const
        {
            return {std::move(first), std::move(step)};
        }
    };

    namespace
    {
        /// @brief Функциональный объект для создания @c arithmetic_progression_cursor
        constexpr auto const & make_arithmetic_progression
            = odr_const<make_arithmetic_progression_cursor_fn>;

        /// @brief Функциональный объект для создания геометрической прогрессии
        constexpr auto const & make_geometric_progression
            = odr_const<make_geometric_progression_fn>;
    }
}
// namespace experimental
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_PROGRESSION_HPP_INCLUDED
