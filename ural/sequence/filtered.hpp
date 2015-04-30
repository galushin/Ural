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

#include <ural/sequence/remove.hpp>
#include <ural/algorithm/details/algo_base.hpp>

#include <boost/compressed_pair.hpp>

namespace ural
{
    /** @brief Последовательность элементов базовой последовательности,
    удовлетворяющих заданному предикату.
    @tparam Sequence входная последовательность
    @tparam Predicate унарный предикат
    @todo Можно ли описать только отличия от @c remove_if_sequence?
    */
    template <class Sequence, class Predicate>
    class filter_sequence
     : public sequence_base<filter_sequence<Sequence, Predicate>>
    {
        friend bool operator==(filter_sequence const & x,
                               filter_sequence const & y)
        {
            return x.impl_ == y.impl_;
        }

        typedef ural::not_function<Predicate> NegatedPredicate;

        typedef ural::remove_if_sequence<Sequence, NegatedPredicate>
            Impl;

    public:
        // Типы
        /// @brief Тип ссылки
        typedef typename Impl::reference reference;

        /// @brief Тип значения
        typedef typename Impl::value_type value_type;

        /// @brief Тип указателя
        typedef typename Impl::pointer pointer;

        /// @brief Тип расстояния
        typedef typename Impl::distance_type distance_type;

        /// @brief Категория обхода
        typedef typename std::common_type<typename Impl::traversal_tag,
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
         : impl_{std::move(seq), ural::not_fn(std::move(pred))}
        {}

        // Однопроходная последовательность
        /** @brief Проверка того, что последовательность исчерпана
        @return @b true, если последовательность исчерпана, иначе --- @b false
        */
        bool operator!() const
        {
            return !impl_;
        }

        /** @brief Первый элемент последовательности
        @return Первый элемент последовательности
        */
        reference front() const
        {
            return impl_.front();
        }

        /// @brief Переход к следующему элементу
        void pop_front()
        {
            return impl_.pop_front();
        }

        // Адаптор последовательности
        /** @brief Предикат
        @return Используемый предикат
        */
        Predicate const & predicate() const
        {
            return impl_.predicate().target();
        }

        /** @brief Базовая последовательность
        @return Базовая последовательность
        */
        Sequence const & base() const
        {
            return impl_.base();
        }

    private:
        Impl impl_;
    };

    /** @brief Функция создания @c filter_sequence
    @param seq исходная последовательность
    @param pred унарный предикат
    @return <tt> Seq(::ural::sequence_fwd<Sequence>(seq), make_callable(std::move(pred))) </tt>,
    где @c Seq --- это экземпляр @c filter_sequence с подходящими шаблонными
    параметрами
    */
    template <class Sequence, class Predicate>
    auto make_filter_sequence(Sequence && seq, Predicate pred)
    -> filter_sequence<decltype(::ural::sequence_fwd<Sequence>(seq)),
                       decltype(make_callable(std::move(pred)))>
    {
        typedef decltype(::ural::sequence_fwd<Sequence>(seq)) Seq;
        typedef decltype(make_callable(std::move(pred))) Pred;
        typedef filter_sequence<Seq, Pred> Result;

        return Result(::ural::sequence_fwd<Sequence>(seq),
                      make_callable(std::move(pred)));
    }

    /** @brief Тип вспомогательного объекта, предназначенного для создания
    @c filter_sequence в конвейерном стиле
    @tparam Predicate тип унарного предиката
    */
    template <class Predicate>
    struct filtered_helper
    {
        /// @brief Тип унарного предиката
        Predicate predicate;
    };

    /** @brief Создание @c filter_sequence в конвейерном стиле
    @param seq последовательность
    @param helper объект, содержащий информацию об используемом предикате
    @return <tt> make_filter_sequence(std::forward<Sequence>(seq),
                                      std::move(helper.predicate)) </tt>
    */
    template <class Sequence, class Predicate>
    auto operator|(Sequence && seq, filtered_helper<Predicate> helper)
    -> decltype(make_filter_sequence(std::forward<Sequence>(seq),
                                     helper.predicate))
    {
        return make_filter_sequence(std::forward<Sequence>(seq),
                                    std::move(helper.predicate));
    }

    /** @brief Создание вспомогательного объекта, предназначенного для создания
    @c filter_sequence в конвейерном стиле
    @param pred унарный предикат
    @return {make_callable(std::move(pred))}
    */
    template <class Predicate>
    auto filtered(Predicate pred)
    -> filtered_helper<decltype(make_callable(std::move(pred)))>
    {
        typedef filtered_helper<decltype(make_callable(std::move(pred)))> Helper;
        return Helper{make_callable(std::move(pred))};
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_FILTERED_HPP_INCLUDED
