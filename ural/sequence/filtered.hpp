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
#include <ural/algorithm/core.hpp>

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
        typedef ValueType<Impl> value_type;

        /// @brief Тип указателя
        typedef typename Impl::pointer pointer;

        /// @brief Тип расстояния
        typedef DifferenceType<Impl> distance_type;

        /// @brief Категория обхода
        typedef CommonType<typename Impl::traversal_tag, forward_traversal_tag>
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

        // Прямая последовательность
        /** @brief Пройденная часть последовательности
        @return Пройденная часть последовательности
        */
        filter_sequence traversed_front() const
        {
            return filter_sequence(impl_.traversed_front().base(),
                                   this->predicate());
        }

        /** @brief Отбрасывание пройденной части последовательности
        @post <tt> !this->traversed_front() </tt>
        */
        void shrink_front()
        {
            return this->impl_.shrink_front();
        }

        /** @brief Полная последовательность (включая пройденные части)
        @return Полная последовательность
        */
        filter_sequence original() const;

        // Адаптор последовательности
        /** @brief Предикат
        @return Используемый предикат
        */
        Predicate const & predicate() const
        {
            return impl_.predicate().target();
        }

        //@{
        /** @brief Базовая последовательность
        @return Базовая последовательность
        */
        Sequence const & base() const &
        {
            return this->impl_.base();
        }

        Sequence && base() &&
        {
            return std::move(this->impl_).base();
        }
        //@}

    private:
        Impl impl_;
    };

    /// @brief Тип функционального объекта для создания @c filter_sequence
    class make_filter_sequence_fn
    {
    public:
        /** @brief Функция создания @c filter_sequence
        @param seq исходная последовательность
        @param pred унарный предикат
        @return <tt> Seq(::ural::sequence_fwd<Sequence>(seq), make_callable(std::move(pred))) </tt>,
        где @c Seq --- это экземпляр @c filter_sequence с подходящими шаблонными
        параметрами
        */
        template <class Sequence, class Predicate>
        auto operator()(Sequence && seq, Predicate pred) const
        {
            typedef filter_sequence<SequenceType<Sequence>,
                                    FunctionType<Predicate>> Result;

            return Result(::ural::sequence_fwd<Sequence>(seq),
                          ::ural::make_callable(std::move(pred)));
        }
    };

    namespace
    {
        /// @brief Функциональный объект для создания @c filter_sequence
        constexpr auto const & make_filter_sequence
            = odr_const<make_filter_sequence_fn>;

        /** @brief Функциональный объект для создания @c filter_sequence
        в конвейерном стиле
        */
        constexpr auto const & filtered
            = odr_const<pipeable_maker<make_filter_sequence_fn>>;
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_FILTERED_HPP_INCLUDED
