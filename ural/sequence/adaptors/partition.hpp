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
namespace experimental
{
    /** @brief Курсор последовательности вывода, записывающая значения в одну из
    двух базовых последовательностей, в зависимости от значения предиката.
    @tparam Ouput1 тип курсора для элементов, удовлетворяющих предикату
    @tparam Ouput2 тип курсора для элементов, не удовлетворяющих
    предикату
    @tparam Predicate тип предикат
    */
    template <class Output1, class Output2, class Predicate>
    class partition_cursor
     : public cursor_base<partition_cursor<Output1, Output2, Predicate>>
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
        @param out_true выходной курсор для "истинных" элементов
        @param out_false выходной курсор для "ложных" элементов
        @param pred предикат
        @post <tt> this->true_cursor() == out_true </tt>
        @post <tt> this->false_cursor() == out_false </tt>
        @post <tt> this->predicate() == pred </tt>
        */
        explicit partition_cursor(Output1 out_true, Output2 out_false,
                                    Predicate pred)
         : data_{Bases{std::move(out_true), std::move(out_false)},
                 std::move(pred)}
        {}

        // Однопроходый курсор
        /** @brief Проверка исчерпания курсора
        @return @b true, если курсор исчерпан, иначе --- @b false.
        */
        bool operator!() const
        {
            return !this->true_cursor() || !this->false_cursor();
        }

        /** @return <tt> *this </tt>
        */
        partition_cursor & operator*()
        {
            return *this;
        }

        /** @brief Запись элемента
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

        // Адаптор курсора
        //@{
        /** @brief Курсор, в который записываются элементы, удовлетворяющие
        предикату.
        @return Курсор, в который записываются элементы, удовлетворяющие
        предикату.
        */
        Output1 const & true_cursor() const &
        {
            return data_.first()[ural::_1];
        }

        Output1 && true_cursor() &&
        {
            return std::move(data_.first())[ural::_1];
        }
        //@}

        //@{
        /** @brief Курсор, в который записываются элементы, не удовлетворяющие
        предикату.
        @return Курсор, в который записываются элементы, не удовлетворяющие
        предикату.
        */
        Output2 const & false_cursor() const &
        {
            return data_.first()[ural::_2];
        }

        Output2 && false_cursor() &&
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

    /** @brief Создание курсора вывода, записывающего значения в один из двух
    базовых курсоров, в зависимости от значения предиката.
    @param out_true выходная последовательность для "истинных" элементов
    @param out_false выходная последовательность для "ложных" элементов
    @param pred предикат
    */
    template <class Output1, class Output2, class Predicate>
    auto make_partition_cursor(Output1 && out_true, Output2 && out_false,
                                 Predicate pred)
    -> partition_cursor<decltype(::ural::cursor_fwd<Output1>(out_true)),
                          decltype(::ural::cursor_fwd<Output2>(out_false)),
                          decltype(make_callable(std::move(pred)))>
    {
        typedef partition_cursor<decltype(::ural::cursor_fwd<Output1>(out_true)),
                          decltype(::ural::cursor_fwd<Output2>(out_false)),
                          decltype(make_callable(std::move(pred)))> Result;
        return Result(::ural::cursor_fwd<Output1>(out_true),
                      ::ural::cursor_fwd<Output2>(out_false),
                      make_callable(std::move(pred)));
    }
}
// namespace experimental
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_PARTITION_HPP_INCLUDED
