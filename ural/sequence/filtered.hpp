#ifndef Z_URAL_SEQUENCE_FILTERED_HPP_INCLUDED
#define Z_URAL_SEQUENCE_FILTERED_HPP_INCLUDED

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

/** @file ural/sequence/filtered.hpp
 @brief Последовательность элементов базовой последовательности, удовлетворяющих
 заданному предикату.
*/

#include <ural/algorithm/details/algo_base.hpp>

#include <boost/compressed_pair.hpp>

namespace ural
{
    /** @brief Последовательность элементов базовой последовательности,
    удовлетворяющих заданному предикату.
    @todo Усилить категорию обхода
    */
    template <class Sequence, class Predicate>
    class filter_sequence
     : public sequence_base<filter_sequence<Sequence, Predicate>>
    {
        friend bool operator==(filter_sequence const & x,
                               filter_sequence const & y)
        {
            return x.data_ == y.data_;
        }

    public:
        // Типы
        /// @brief Тип ссылки
        typedef typename Sequence::reference reference;

        /// @brief Тип значения
        typedef typename Sequence::value_type value_type;

        /// @brief Тип указателя
        typedef typename Sequence::pointer pointer;

        //// @brief Тип расстояния
        typedef typename Sequence::distance_type distance_type;

        /// @brief Категория обхода
        typedef typename std::common_type<typename Sequence::traversal_tag,
                                          forward_traversal_tag>::type
            traversal_tag;

        // Конструкторы
        /** @brief Конструктор
        @param seq базовая последовательность
        @param pred предикат
        @post <tt> this->base() == seq </tt>
        @post <tt> this->predicate() == pred </tt>
        */
        explicit filter_sequence(Sequence seq, Predicate pred)
         : data_{std::move(seq), std::move(pred)}
        {
            this->seek();
        }

        // Однопроходная последовательность
        /** @brief Проверка того, что последовательность исчерпана
        @return @b true, если последовательность исчерпана, иначе --- @b false
        */
        bool operator!() const
        {
            return !this->base();
        }

        /** @brief Первый элемент последовательности
        @return Первый элемент последовательности
        */
        reference front() const
        {
            return *data_[ural::_1];
        }

        /// @brief Переход к следующему элементу
        void pop_front()
        {
            ++ data_[ural::_1];
            this->seek();
        }

        // Адаптор последовательности
        /** @brief Предикат
        @return Используемый предикат
        */
        Predicate const & predicate() const
        {
            return data_[ural::_2];
        }

        /** @brief Базовая последовательность
        @return Базовая последовательность
        */
        Sequence const & base() const
        {
            return data_[ural::_1];
        }

    private:
        void seek()
        {
            data_[ural::_1]
                = ::ural::details::find_if(data_[ural::_1], this->predicate());
        }

    private:
        ural::tuple<Sequence, Predicate> data_;
    };

    template <class Sequence, class Predicate>
    auto make_filter_sequence(Sequence && seq, Predicate pred)
    -> filter_sequence<decltype(sequence(std::forward<Sequence>(seq))),
                       decltype(make_functor(std::move(pred)))>
    {
        typedef decltype(sequence(std::forward<Sequence>(seq))) Seq;
        typedef decltype(make_functor(std::move(pred))) Pred;
        typedef filter_sequence<Seq, Pred> Result;

        return Result{sequence(std::forward<Sequence>(seq)),
                      make_functor(std::move(pred))};
    }

    template <class Predicate>
    struct filtered_helper
    {
        Predicate predicate;
    };

    template <class Sequence, class Predicate>
    auto operator|(Sequence && seq, filtered_helper<Predicate> helper)
    -> decltype(make_filter_sequence(std::forward<Sequence>(seq),
                                     helper.predicate))
    {
        return make_filter_sequence(std::forward<Sequence>(seq),
                                    helper.predicate);
    }

    template <class Predicate>
    auto filtered(Predicate pred)
    -> filtered_helper<decltype(make_functor(std::move(pred)))>
    {
        typedef filtered_helper<decltype(make_functor(std::move(pred)))> Helper;
        return Helper{make_functor(std::move(pred))};
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_FILTERED_HPP_INCLUDED
