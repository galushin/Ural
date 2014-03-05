#ifndef Z_URAL_SEQUENCE_INSERTION_HPP_INCLUDED
#define Z_URAL_SEQUENCE_INSERTION_HPP_INCLUDED

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

        typedef single_pass_traversal_tag traversal_tag;

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

    /** @brief Вспомогательный класс для создания
    <tt> std::back_insert_iterator </tt>
    */
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
