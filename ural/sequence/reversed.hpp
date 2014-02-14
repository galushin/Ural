#ifndef REVERSED_HPP_INCLUDED
#define REVERSED_HPP_INCLUDED

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
        return ural::make_reverse_sequence(std::forward<BidirectionalSequence>(seq));
    }

    constexpr auto reversed = reversed_helper{};
}
// namespace ural

#endif
// REVERSED_HPP_INCLUDED
