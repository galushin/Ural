#ifndef Z_URAL_SEQUENCE_PARTITION_HPP_INCLUDED
#define Z_URAL_SEQUENCE_PARTITION_HPP_INCLUDED

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

/** @file ural/sequence/partition.hpp
 @brief Последовательность вывода, записывающая значения в одну из двух базовых
 последовательностей, в зависимости от значения предиката
*/

#include <ural/concepts.hpp>
#include <ural/sequence/base.hpp>
#include <ural/sequence/make.hpp>

namespace ural
{
    /** @brief Последовательность вывода, записывающая значения в одну из двух
    базовых последовательностей, в зависимости от значения предиката
    @tparam Ouput1 тип последовательности для элементов, удовлетворяющих
    предикату
    @tparam Ouput2 тип последовательности для элементов, не удовлетворяющих
    предикату
    @tparam Predicate тип предикат
    */
    template <class Output1, class Output2, class Predicate>
    class partition_sequence
     : public sequence_base<partition_sequence<Output1, Output2, Predicate>>
    {
    public:
        // Типы
        /// @brief Категория курсора
        using cursor_tag = common_tag_t<forward_cursor_tag,
                                        typename Output1::cursor_tag,
                                        typename Output2::cursor_tag>;

        /// @brief Тип расстояния
        typedef CommonType<DifferenceType<Output1>, DifferenceType<Output2>>
            difference_type;

        // Конструкторы
        /** @brief Конструктор
        @param out_true выходная последовательность для "истинных" элементов
        @param out_false выходная последовательность для "ложных" элементов
        @param pred предикат
        @post <tt> this->true_sequence() == out_true </tt>
        @post <tt> this->false_sequence() == out_false </tt>
        @post <tt> this->predicate() == pred </tt>
        */
        explicit partition_sequence(Output1 out_true, Output2 out_false,
                                    Predicate pred)
         : data_{Bases{std::move(out_true), std::move(out_false)},
                 std::move(pred)}
        {}

        // Однопроходная последовательность
        /** @brief Проверка исчерпания последовательностей
        @return @b true, если последовательность исчерпана, иначе --- @b false.
        */
        bool operator!() const
        {
            return !this->true_sequence() || !this->false_sequence();
        }

        /** @return <tt> *this </tt>
        */
        partition_sequence & operator*()
        {
            return *this;
        }

        /** @brief Запись элемента в последовательность
        @param x новый элемент
        */
        template <class T>
        void operator=(T && x)
        {
            if(this->predicate()(x))
            {
                *data_.first()[ural::_1] = std::forward<T>(x);
                ++ data_.first()[ural::_1];
            }
            else
            {
                *data_.first()[ural::_2] = std::forward<T>(x);
                ++ data_.first()[ural::_2];
            }
        }

        /** @brief Переход к следующему элементу
        @pre <tt> !*this == false </tt>
        */
        void pop_front()
        {}

        // Адаптор последовательности
        //@{
        /** @brief Последовательность, в которую записываются элементы,
        удовлетворяющие предикату
        @return Последовательность, в которую записываются элементы,
        удовлетворяющие предикату.
        */
        Output1 const & true_sequence() const &
        {
            return data_.first()[ural::_1];
        }

        Output1 && true_sequence() &&
        {
            return std::move(data_.first())[ural::_1];
        }
        //@}

        //@{
        /** @brief Последовательность, в которую записываются элементы, не
        удовлетворяющие предикату
        @return Последовательность, в которую записываются элементы, не
        удовлетворяющие предикату.
        */
        Output2 const & false_sequence() const &
        {
            return data_.first()[ural::_2];
        }

        Output2 && false_sequence() &&
        {
            return std::move(data_.first())[ural::_2];
        }
        //@}

        /** @brief Используемый предикат
        @return Используемый предикат
        */
        Predicate const & predicate() const
        {
            return data_.second();
        }

    private:
        typedef ural::tuple<Output1, Output2> Bases;
        boost::compressed_pair<Bases, Predicate> data_;
    };

    /** @brief Создание последовательность вывода, записывающая значения в одну
    из двух базовых последовательностей, в зависимости от значения предиката.
    @param out_true выходная последовательность для "истинных" элементов
    @param out_false выходная последовательность для "ложных" элементов
    @param pred предикат
    */
    template <class Output1, class Output2, class Predicate>
    auto make_partition_sequence(Output1 && out_true, Output2 && out_false,
                                 Predicate pred)
    -> partition_sequence<decltype(::ural::sequence_fwd<Output1>(out_true)),
                          decltype(::ural::sequence_fwd<Output2>(out_false)),
                          decltype(make_callable(std::move(pred)))>
    {
        typedef partition_sequence<decltype(::ural::sequence_fwd<Output1>(out_true)),
                          decltype(::ural::sequence_fwd<Output2>(out_false)),
                          decltype(make_callable(std::move(pred)))> Result;
        return Result(::ural::sequence_fwd<Output1>(out_true),
                      ::ural::sequence_fwd<Output2>(out_false),
                      make_callable(std::move(pred)));
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_PARTITION_HPP_INCLUDED
