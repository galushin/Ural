#ifndef Z_URAL_SEQUENCE_INSERTION_HPP_INCLUDED
#define Z_URAL_SEQUENCE_INSERTION_HPP_INCLUDED

/** @file ural/sequence/insertion.hpp
 @brief Последовательности на основе итераторов-вставок и других выходных
 итераторов
*/

#include <ural/sequence/base.hpp>

namespace ural
{
    /** @brief Последовательности на основе итераторов-вставок и других выходных
    итераторов
    @tparam OutputIterator тип выходного итератора
    */
    template <class OutputIterator>
    class output_iterator_sequence
     : sequence_base<output_iterator_sequence<OutputIterator>>
    {
    public:
        /// @brief Тип возвращаемого значения для оператора *
        typedef decltype(*std::declval<OutputIterator>())  reference;

        /** @brief Конструктор
        @param iter итератор, на основе которого будет создана данная
        последовательность
        */
        explicit output_iterator_sequence(OutputIterator iter)
         : iter_(std::move(iter))
        {}

        /** @brief Проверка того, что последовательность исчерпана
        @return @b false
        */
        bool operator!() const
        {
            return false;
        }

        /** @brief Текущий элемент
        */
        reference operator*()
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
