#ifndef Z_URAL_SEQUENCE_SEQUENCE_ITERATOR_HPP_INCLUDED
#define Z_URAL_SEQUENCE_SEQUENCE_ITERATOR_HPP_INCLUDED

#include <ural/optional.hpp>

namespace ural
{
    template <class Sequence>
    class sequence_iterator
    {
        friend bool operator==(sequence_iterator const & x,
                               sequence_iterator const & y)
        {
            assert(!y.impl_);
            return !x.impl_.value();
        }

    public:
        // Типы
        typedef typename Sequence::reference reference;

        // Конструктор
        sequence_iterator()
         : impl_{nullopt}
        {}

        sequence_iterator(Sequence s)
         : impl_{std::move(s)}
        {}

        // Итератор ввода
        reference operator*() const
        {
            assert(!!impl_);
            return **impl_;
        }

        sequence_iterator & operator++()
        {
            assert(!!impl_);
            ++*impl_;
            return *this;
        }

    private:
        ural::optional<Sequence> impl_;
    };
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_SEQUENCE_ITERATOR_HPP_INCLUDED
