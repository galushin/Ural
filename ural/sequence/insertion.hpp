#ifndef Z_URAL_SEQUENCE_INSERTION_HPP_INCLUDED
#define Z_URAL_SEQUENCE_INSERTION_HPP_INCLUDED

namespace ural
{
    template <class OutputIterator>
    class output_iterator_sequence
    {
    public:
        explicit output_iterator_sequence(OutputIterator iter)
         : iter_(iter)
        {}

        bool operator!() const
        {
            return false;
        }

        OutputIterator operator*() const
        {
            return this->iter_;
        }

        output_iterator_sequence & operator++()
        {
            ++ iter_;
            return *this;
        }

    private:
        OutputIterator iter_;
    };
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_INSERTION_HPP_INCLUDED
