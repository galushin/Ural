#ifndef Z_URAL_SEQUENCE_UNIQUED_HPP_INCLUDED
#define Z_URAL_SEQUENCE_UNIQUED_HPP_INCLUDED

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

/** @file ural/sequence/uniqued.hpp
 @brief Последовательность неповторяющихся соседних элементов
*/

#include <ural/algorithm/details/algo_base.hpp>

namespace ural
{
    /** @brief Последовательность неповторяющихся соседних элементов базовой
    последовательности
    @tparam Forward Тип базовой последовательности
    @tparam BinaryPredicat бинарный предикат, определяющий, совпадают ли два
    последовательных элемента.
    */
    template <class Forward, class BinaryPredicate = ural::equal_to<> >
    class unique_sequence
     : public sequence_base<unique_sequence<Forward, BinaryPredicate>>
    {
    public:
        // Типы
        /// @brief Тип ссылки
        typedef typename Forward::reference reference;

        /// @brief Тип значения
        typedef typename Forward::value_type value_type;

        /// @brief Категория обхода
        typedef ural::forward_traversal_tag traversal_tag;

        // Конструкторы
        /** @brief Конструктор
        @param in входная последовательность
        @post <tt> this->base() == in </tt>
        @post <tt> this->predicate() == BinaryPredicate{} </tt>
        */
        explicit unique_sequence(Forward in)
         : data_(std::move(in))
        {}

        /** @brief Конструктор
        @param in входная последовательность
        @param pred используемый предикат
        @post <tt> this->base() == in </tt>
        @post <tt> this->predicate() == pred </tt>
        */
        explicit unique_sequence(Forward in, BinaryPredicate pred)
         : data_(std::move(in), std::move(pred))
        {}

        // Адаптор последовательности
        /** @brief Базовая последовательность
        @return Базовая последовательность
        */
        Forward const & base() const
        {
            return this->data_.first();
        }

        /** @brief Используемый предикат
        @return Используемый предикат
        */
        BinaryPredicate const & predicate() const
        {
            return this->data_.second();
        }

        // Однопроходная последовательность
        /** @brief Проверка исчерпания последовательностей
        @return @b true, если последовательность исчерпана, иначе --- @b false.
        */
        bool operator!() const
        {
            return !this->base();
        }

        /** @brief Текущий элемент последовательности
        @pre <tt> !*this == false </tt>
        @return Ссылка на текущий элемент последовательности
        */
        reference front() const
        {
            return *this->base();
        }

        /** @brief Переход к следующему элементу
        @pre <tt> !*this == false </tt>
        */
        void pop_front()
        {
            auto s = this->base();
            ++ s;

            data_.first() = ::ural::details::find(s, *this->base(),
                                                  not_fn(this->predicate()));
        }

    private:
        boost::compressed_pair<Forward, BinaryPredicate> data_;
    };

    template <class Forward, class BinaryPredicate>
    auto make_unique_sequence(Forward && in, BinaryPredicate pred)
    -> unique_sequence<decltype(sequence(std::forward<Forward>(in))),
                        decltype(make_functor(std::move(pred)))>
    {
        typedef unique_sequence<decltype(sequence(std::forward<Forward>(in))),
                        decltype(make_functor(std::move(pred)))> Seq;
        return Seq(sequence(std::forward<Forward>(in)), make_functor(std::move(pred)));
    }

    template <class Forward>
    auto make_unique_sequence(Forward && in)
    -> unique_sequence<decltype(sequence(std::forward<Forward>(in)))>
    {
        typedef unique_sequence<decltype(sequence(std::forward<Forward>(in)))>
            Result;

         return Result{sequence(std::forward<Forward>(in))};
    }

    struct uniqued_helper{};

    template <class Forward>
    auto operator|(Forward && in, uniqued_helper)
    -> unique_sequence<decltype(sequence(std::forward<Forward>(in)))>
    {
        return ::ural::make_unique_sequence(std::forward<Forward>(in));
    }

    constexpr auto uniqued = uniqued_helper{};
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_UNIQUED_HPP_INCLUDED
