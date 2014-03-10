#ifndef Z_URAL_SEQUENCE_MOVED_HPP_INCLUDED
#define Z_URAL_SEQUENCE_MOVED_HPP_INCLUDED

/*  This file is part of Ural.

    Ural is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Ural is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Ural.  If not, see <http://www.gnu.org/licenses/>.
*/

/** @file ural/sequence/moved.hpp
 @brief Адаптор последовательности, преобразующий ссылки на элементы в
 rvalue-ссылки
 @todo Выразить через transform?
 @todo Больше тестов
*/

#include <ural/sequence/base.hpp>

namespace ural
{
    template <class Sequence>
    class move_sequence
     : public sequence_base<move_sequence<Sequence>>
    {
    public:
        // Типы
        typedef typename Sequence::value_type value_type;
        typedef value_type && reference;
        typedef typename Sequence::distance_type distance_type;
        typedef typename Sequence::traversal_tag traversal_tag;

        // Конструирование, копирование, присваивание
        explicit move_sequence(Sequence seq)
         : base_{std::move(seq)}
        {}

        // Однопроходная последовательность
        bool operator!() const
        {
            return !this->base();
        }

        reference front() const
        {
            return std::move(this->base().front());
        }

        void pop_front()
        {
            ++ base_;
        }

        // Двусторонняя последовательность
        reference back() const
        {
            return std::move(this->base().back());
        }

        void pop_back()
        {
            this->base_.pop_back();
        }

        // Адаптор последовательности
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

    template <class Iterator>
    auto make_iterator_sequence(std::move_iterator<Iterator> first,
                                std::move_iterator<Iterator> last)
    -> move_sequence<decltype(make_iterator_sequence(first.base(), last.base()))>
    {
        return make_move_sequence(make_iterator_sequence(first.base(),
                                                         last.base()));
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
