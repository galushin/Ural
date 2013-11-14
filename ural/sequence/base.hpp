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
        template <class... Args>
        sequence_base(Args && ... args)
         : Base(std::forward<Args>(args)...)
        {}
    };
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_BASE_HPP_INCLUDED
