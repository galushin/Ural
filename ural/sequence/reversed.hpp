#ifndef REVERSED_HPP_INCLUDED
#define REVERSED_HPP_INCLUDED

/** @file ural/sequence/reversed.hpp
 @brief ������� ������������������, ���������� �������� �������� ������������������
 � �������� �������.
 @todo seq | reversed | reversed == seq
*/

namespace ural
{
    template <class BidirectionalSequence>
    class reverse_sequence
     : public sequence_base<reverse_sequence<BidirectionalSequence>>
    {
    public:
        // ����
        typedef typename BidirectionalSequence::reference reference;
        typedef typename BidirectionalSequence::value_type value_type;

        // ������������
        explicit reverse_sequence(BidirectionalSequence seq)
         : base_(std::move(seq))
        {}

        // ������������� ������������������
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

        // ������ �������������� ������������������
        reverse_sequence traversed_front() const
        {
            return reverse_sequence{this->base().traversed_back()};
        }

        // ������������ ������������������
        reference back() const
        {
            return base_.front();
        }

        void pop_back()
        {
            return base_.pop_front();
        }

        // ������� ������������������
        BidirectionalSequence const & base() const
        {
            return this->base_;
        }

    private:
        BidirectionalSequence base_;
    };

    struct reversed_helper{};

    template <class BidirectionalSequence>
    auto make_reverse_sequence(BidirectionalSequence && seq)
    -> reverse_sequence<decltype(sequence(std::forward<BidirectionalSequence>(seq)))>
    {
        typedef reverse_sequence<decltype(sequence(std::forward<BidirectionalSequence>(seq)))> Seq;
        return Seq{sequence(std::forward<BidirectionalSequence>(seq))};
    }

    template <class BidirectionalSequence>
    auto operator|(BidirectionalSequence && seq, reversed_helper)
    -> reverse_sequence<decltype(sequence(std::forward<BidirectionalSequence>(seq)))>
    {
        return ural::make_reverse_sequence(std::forward<BidirectionalSequence>(seq));
    }

    constexpr auto reversed = reversed_helper{};
}
// namespace ural

#endif
// REVERSED_HPP_INCLUDED
