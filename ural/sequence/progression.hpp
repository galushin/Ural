#ifndef Z_URAL_SEQUENCE_PROGRESSION_HPP_INCLUDED
#define Z_URAL_SEQUENCE_PROGRESSION_HPP_INCLUDED

/** @file ural/sequence/progression.hpp
 @brief Прогрессии
 @todo Возможность задавать категорию обхода. Настройка структуры в зависимости
 от категории обхода
*/

namespace ural
{
    /** @brief Арифметическая прогрессия
    @tparam Additive тип значений
    @tparam Plus операция, используемая в качестве сложения
    @todo Конструктор с функциональным объектом
    */
    template <class Additive, class Plus = ural::plus<Additive>>
    class arithmetic_progression
     : public sequence_base<arithmetic_progression<Additive, Plus>>
     , private Plus
    {
        typedef sequence_base<arithmetic_progression<Additive, Plus>> Base;

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
        typedef random_access_traversal_tag traversal_tag;

        /// @brief Тип операции
        typedef Plus operation_type;

        // Конструкторы
        /** @brief Конструктор
        @param first Первый элемент
        @param step Шаг
        @post <tt> this->operation() == Plus{} </tt>
        @post <tt> **this == first </tt>
        @post <tt> this->step() == step </tt>
        */
        explicit arithmetic_progression(Additive first, Additive step)
         : init_first_{first}
         , first_{first}
         , step_{std::move(step)}
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
            return this->first_;
        }

        /** @brief Переход к следующему элементу последовательности
        @pre <tt> bool(*this) != false </tt>
        @return <tt> *this </tt>
        */
        void pop_front()
        {
            this->first_ = this->functor()(this->first_, this->step_);
        }

        // Прямая последовательность
        /** @brief Пройденная передняя часть прогрессии
        @return Пройденная передняя часть прогрессии
        */
        arithmetic_progression traversed_front() const
        {
            return arithmetic_progression(init_first_, step_);
        }

    private:
        Additive init_first_;
        Additive first_;
        Additive step_;
    };

    /** @brief Создание последовательности, представляющей арифметическую
    прогрессию
    @param first Первый элемент
    @param step Шаг
    */
    template <class Additive>
    arithmetic_progression<Additive>
    make_arithmetic_progression(Additive first, Additive step)
    {
        return arithmetic_progression<Additive>{std::move(first), std::move(step)};
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_PROGRESSION_HPP_INCLUDED
