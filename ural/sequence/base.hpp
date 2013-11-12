#ifndef Z_URAL_SEQUENCE_BASE_HPP_INCLUDED
#define Z_URAL_SEQUENCE_BASE_HPP_INCLUDED

/** @file ural/sequence/base.hpp
 @brief Базовый класс для последовательностей (CRTP)
*/

namespace ural
{
    /** @brief Базовый класс для последовательностей (CRTP)
    @tparam Seq тип последовательности-наследника
    */
    template <class Seq>
    class sequence_base
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

    public:
    private:
    };
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_BASE_HPP_INCLUDED
