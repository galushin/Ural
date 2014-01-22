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
     : public sequence_base<output_iterator_sequence<OutputIterator>>
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
        void pop_front()
        {
            ++ iter_;
        }

    private:
        OutputIterator iter_;
    };

    struct back_inserter_helper
    {
    public:
        template <class Container>
        constexpr std::back_insert_iterator<Container>
        operator()(Container & c) const
        {
            return std::back_inserter(c);
        }
    };

    constexpr auto back_inserter = back_inserter_helper{};

    template <class Container>
    std::back_insert_iterator<Container>
    operator|(Container & c, back_inserter_helper helper)
    {
        return helper(c);
    }

    struct front_inserter_helper
    {
    template <class Container>
        constexpr std::front_insert_iterator<Container>
        operator()(Container & c) const
        {
            return std::front_inserter(c);
        }
    };

    constexpr auto front_inserter = front_inserter_helper{};

    template <class Container>
    std::front_insert_iterator<Container>
    operator|(Container & c, front_inserter_helper helper)
    {
        return helper(c);
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_INSERTION_HPP_INCLUDED
