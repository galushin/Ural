#ifndef Z_URAL_SEQUENCE_PROGRESSION_HPP_INCLUDED
#define Z_URAL_SEQUENCE_PROGRESSION_HPP_INCLUDED

/** @file ural/sequence/progression.hpp
 @brief Прогрессии
 @todo Возможность задавать категорию обхода. Настройка структуры в зависимости
 от категории обхода
*/

namespace ural
{
    template <class Additive, class Plus = ural::plus<Additive>>
    class arithmetic_progression
     : public sequence_base<arithmetic_progression<Additive, Plus>>
    {
    public:
        // Типы
        typedef Additive const & reference;
        typedef Additive value_type;
        typedef size_t distance_type;
        typedef random_access_traversal_tag traversal_tag;

        // Конструкторы
        explicit arithmetic_progression(Additive first, Additive step)
         : init_first_{first}
         , first_{first}
         , step_{std::move(step)}
        {}

        // Свойства

        // Однопроходна последовательность
        bool operator!() const
        {
            return false;
        }

        reference front() const
        {
            return this->first_;
        }

        void pop_front()
        {
            this->first_ += this->step_;
        }

        // Прямая последовательность
        arithmetic_progression traversed_front() const
        {
            return arithmetic_progression(init_first_, step_);
        }

    private:
        Additive init_first_;
        Additive first_;
        Additive step_;
    };

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
