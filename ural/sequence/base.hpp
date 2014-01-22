#ifndef Z_URAL_SEQUENCE_BASE_HPP_INCLUDED
#define Z_URAL_SEQUENCE_BASE_HPP_INCLUDED

/** @file ural/sequence/base.hpp
 @brief Базовый класс для последовательностей (CRTP)
*/

#include <ural/defs.hpp>

namespace ural
{
    /** @brief Базовый класс для последовательностей (CRTP)
    @tparam Seq тип последовательности-наследника
    */
    template <class Seq, class Base = ural::empty_type>
    class sequence_base
     : public Base
    {
        friend Seq sequence(Seq s)
        {
            return s;
        }

        friend Seq operator++(Seq & seq, int)
        {
            Seq seq_old = seq;
            ++ seq;
            return seq_old;
        }

    protected:
        /** @brief Конструктор
        @param args список аргументов для конструктора базового класса
        */
        template <class... Args>
        sequence_base(Args && ... args)
         : Base(std::forward<Args>(args)...)
        {}
    };

    template <class Sequence>
    Sequence shrink_front(Sequence s)
    {
        s.shrink_front();
        return s;
    }

    namespace details
    {
    }

    template <class Sequence>
    typename Sequence::distance_type
    size(Sequence const & s)
    {
        // @todo Диспетчеризация по категориям

        // @todo Через count_if(s, {return true;})?
        typename Sequence::distance_type n{0};

        for(auto in = s; !!in; ++ in)
        {
            ++ n;
        }

        return n;
    }

    template <class Sequence>
    void advance(Sequence & s, typename Sequence::distance_type n)
    {
        // @todo Диспетчеризация по категориям
        for(; n > 0; -- n)
        {
            ++ s;
        }
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_BASE_HPP_INCLUDED
