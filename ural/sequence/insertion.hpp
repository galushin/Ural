#ifndef Z_URAL_SEQUENCE_INSERTION_HPP_INCLUDED
#define Z_URAL_SEQUENCE_INSERTION_HPP_INCLUDED

/** @file ural/sequence/insertion.hpp
 @brief Последовательность на основе итераторов-вставок
*/

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

        decltype(*std::declval<OutputIterator>()) operator*()
        {
            return *(this->iter_);
        }

        /** @brief Переход к следующему элементу
        @return <tt> *this </tt>
        */
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
