#ifndef Z_URAL_SEQUENCE_PROGRESSION_HPP_INCLUDED
#define Z_URAL_SEQUENCE_PROGRESSION_HPP_INCLUDED

/** @file ural/sequence/progression.hpp
 @brief Прогрессии
 @todo Настройка структуры в зависимости от категории обхода
*/

namespace ural
{
    /** @brief Арифметическая прогрессия
    @tparam Additive тип значений
    @tparam Plus операция, используемая в качестве сложения
    @todo Настройка структуры в зависимости от категории обхода
    */
    template <class Additive, class Plus = use_default,
              class Traversal = random_access_traversal_tag>
    class arithmetic_progression
     : public sequence_base<arithmetic_progression<Additive, Plus, Traversal>>
     , private default_helper<Plus, plus<>>::type
    {
        typedef sequence_base<arithmetic_progression<Additive, Plus, Traversal>>
            Base;

    public:
        // Типы
        /// @brief Тип значения
        typedef Additive value_type;

        /// @brief Тип ссылки
        typedef value_type const & reference;

        /// @brief Тип расстояния
        typedef size_t distance_type;

        /** @note Проблема в том, что при произвольном доступе возвращается
        вычисленное значение, а не ссылка.
        */
        typedef Traversal traversal_tag;

        /// @brief Тип указателя
        typedef value_type const * pointer;

        /// @brief Тип операции
        typedef typename default_helper<Plus, plus<>>::type operation_type;

        // Конструкторы
        /** @brief Конструктор
        @param first Первый элемент
        @param step Шаг
        @post <tt> this->functor() == operation_type{} </tt>
        @post <tt> **this == first </tt>
        @post <tt> this->step() == step </tt>
        */
        arithmetic_progression(Additive first, Additive step)
         : operation_type{}
         , first_{std::move(first)}
         , step_{std::move(step)}
        {}

        /** @brief Конструктор
        @param first Первый элемент
        @param step Шаг
        @post <tt> **this == first </tt>
        @post <tt> this->functor() == op </tt>
        */
        arithmetic_progression(Additive first, Additive step, operation_type op)
         : operation_type(std::move(op))
         , first_(std::move(first))
         , step_(std::move(step))
        {}

        // Свойства
        /** @brief Функциональный объект, используемый как операция
        @return Функциональный объект, используемый как операция
        */
        operation_type const & functor() const
        {
            return static_cast<operation_type const &>(*this);
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
            this->first_ = this->functor()(std::move(ural::get(this->first_)),
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

    template <class T, class F, class Tr>
    bool operator==(arithmetic_progression<T, F, Tr> const & x,
                    arithmetic_progression<T, F, Tr> const & y);

    template <class Additive, class Plus>
    auto make_arithmetic_progression(Additive first, Additive step, Plus op)
    -> arithmetic_progression<Additive, decltype(make_functor(std::move(op)))>
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
