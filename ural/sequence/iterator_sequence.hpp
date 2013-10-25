#ifndef Z_URAL_SEQUENCE_ITERATOR_SEQUENCE_HPP_INCLUDED
#define Z_URAL_SEQUENCE_ITERATOR_SEQUENCE_HPP_INCLUDED

#include <ural/tuple.hpp>

namespace ural
{
    class strict_sequence_policy
    {
    protected:
        ~strict_sequence_policy() = default;

    public:
        template <class Seq>
        static void assert_not_empty(Seq const & seq)
        {
            if(!seq)
            {
                throw std::logic_error("Sequence must be not empty");
            }
        }
    };

    template <class Iterator, class Policy = strict_sequence_policy>
    class iterator_sequence
    {
    public:
        typedef typename std::iterator_traits<Iterator>::reference reference;

        typedef Policy policy_type;

        explicit iterator_sequence(Iterator first, Iterator last)
         : iterators_{first, last}
        {}

        bool operator!() const
        {
            return this->front_() == this->stop_();
        }

        reference operator*() const
        {
            policy_type::assert_not_empty(*this);

            return *(this->front_());
        }

        iterator_sequence & operator++()
        {
            policy_type::assert_not_empty(*this);
            ++ this->front_();
            return *this;
        }

    private:
        Iterator & front_()
        {
            return iterators_[ural::_1];
        }

        Iterator const & front_() const
        {
            return iterators_[ural::_1];
        }

        Iterator & stop_()
        {
            return iterators_[ural::_2];
        }

        Iterator const & stop_() const
        {
            return iterators_[ural::_2];
        }

    private:
        ural::tuple<Iterator, Iterator> iterators_;
    };
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_ITERATOR_SEQUENCE_HPP_INCLUDED
