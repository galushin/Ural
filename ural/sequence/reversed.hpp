#ifndef REVERSED_HPP_INCLUDED
#define REVERSED_HPP_INCLUDED

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

/** @file ural/sequence/reversed.hpp
 @brief Адаптер последовательности, проходящий элементы исходной последовательности
 в обратном порядке.
*/

namespace ural
{
    template <class BidirectionalSequence>
    class reverse_sequence
     : public sequence_base<reverse_sequence<BidirectionalSequence>>
    {
    public:
        // Типы
        typedef typename BidirectionalSequence::reference reference;
        typedef typename BidirectionalSequence::value_type value_type;
        typedef typename BidirectionalSequence::distance_type distance_type;
        typedef typename BidirectionalSequence::traversal_tag traversal_tag;

        // Конструкторы
        explicit reverse_sequence(BidirectionalSequence seq)
         : base_(std::move(seq))
        {}

        // Однопроходная последовательность
        bool operator!() const
        {
            return !this->base();
        }

        reference front() const
        {
            return base_.back();
        }

        void pop_front()
        {
            return base_.pop_back();
        }

        // Прямая многопроходная последовательность
        reverse_sequence traversed_front() const
        {
            return reverse_sequence{this->base().traversed_back()};
        }

        // Двусторонняя последовательность
        reference back() const
        {
            return base_.front();
        }

        void pop_back()
        {
            return base_.pop_front();
        }

        // Последовательность произвольного доступа
        reverse_sequence & operator+=(distance_type n)
        {
            base_.pop_back(n);
            return *this;
        }

        // Адаптор последовательности
        BidirectionalSequence const & base() const
        {
            return this->base_;
        }

    private:
        BidirectionalSequence base_;
    };

    template <class Sequence>
    bool operator==(reverse_sequence<Sequence> const & x,
                    reverse_sequence<Sequence> const & y);

    struct reversed_helper{};

    template <class BidirectionalSequence>
    auto make_reverse_sequence(BidirectionalSequence && seq)
    -> reverse_sequence<decltype(sequence(std::forward<BidirectionalSequence>(seq)))>
    {
        typedef reverse_sequence<decltype(sequence(std::forward<BidirectionalSequence>(seq)))> Seq;
        return Seq{sequence(std::forward<BidirectionalSequence>(seq))};
    }

    /**
    @todo Оптимизация
    */
    template <class Sequence>
    Sequence make_reverse_sequence(reverse_sequence<Sequence> seq)
    {
        return seq.base();
    }

    template <class BidirectionalSequence>
    auto operator|(BidirectionalSequence && seq, reversed_helper)
    -> decltype(make_reverse_sequence(std::forward<BidirectionalSequence>(seq)))
    {
        return make_reverse_sequence(std::forward<BidirectionalSequence>(seq));
    }

    constexpr auto reversed = reversed_helper{};

    template <class Iterator>
    auto make_iterator_sequence(std::reverse_iterator<Iterator> first,
                                std::reverse_iterator<Iterator> last)
    -> reverse_sequence<decltype(make_iterator_sequence(first.base(), last.base()))>
    {
        return make_reverse_sequence(make_iterator_sequence(first.base(),
                                                            last.base()));
    }
}
// namespace ural

#endif
// REVERSED_HPP_INCLUDED
