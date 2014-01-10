#ifndef Z_URAL_SEQUENCE_MOVED_HPP_INCLUDED
#define Z_URAL_SEQUENCE_MOVED_HPP_INCLUDED

/** @file ural/sequence/moved.hpp
 @todo Выразить через transform?
 @todo Больше тестов
*/

#include <ural/sequence/base.hpp>

namespace ural
{
    template <class Sequence>
    class move_sequence
     : sequence_base<move_sequence<Sequence>>
    {
    public:
        // Типы
        typedef typename Sequence::value_type && reference;

        // Конструирование, копирование, присваивание
        explicit move_sequence(Sequence seq)
         : base_{std::move(seq)}
        {}

        // Однопроходная последовательность
        bool operator!() const
        {
            return !this->base();
        }

        reference operator*() const
        {
            return std::move(*this->base());
        }

        move_sequence & operator++()
        {
            ++ base_;
            return *this;
        }

        Sequence const & base() const
        {
            return this->base_;
        }

    private:
        Sequence base_;
    };

    template <class Sequence>
    auto make_move_sequence(Sequence && seq)
    -> move_sequence<decltype(sequence(std::forward<Sequence>(seq)))>
    {
        typedef move_sequence<decltype(sequence(std::forward<Sequence>(seq)))> Result;
        return Result{sequence(std::forward<Sequence>(seq))};
    }

    struct moved_helper{};

    template <class Sequence>
    auto operator|(Sequence && seq, moved_helper)
    -> decltype(make_move_sequence(std::forward<Sequence>(seq)))
    {
        return make_move_sequence(std::forward<Sequence>(seq));
    }

    auto constexpr moved = moved_helper{};
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_MOVED_HPP_INCLUDED
