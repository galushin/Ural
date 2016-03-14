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

#include <ural/sequence/adaptors/remove.hpp>
#include <ural/algorithm/core.hpp>

#include <boost/compressed_pair.hpp>

namespace ural
{
namespace experimental
{
    /** @brief Последовательность элементов базовой последовательности,
    удовлетворяющих заданному предикату.
    @tparam Sequence входная последовательность
    @tparam Predicate унарный предикат
    */
    template <class Sequence, class Predicate>
    class filter_sequence
     : public sequence_adaptor<filter_sequence<Sequence, Predicate>,
                               remove_if_sequence<Sequence, not_function<Predicate>>>
    {
        using Base = sequence_adaptor<filter_sequence<Sequence, Predicate>,
                                      remove_if_sequence<Sequence, not_function<Predicate>>>;

    public:
        // Конструкторы
        /** @brief Конструктор
        @param seq базовая последовательность
        @param pred предикат
        @post <tt> this->base() == seq </tt>
        @post <tt> this->predicate() == pred </tt>
        */
        explicit filter_sequence(Sequence seq, Predicate pred)
         : Base{ural::experimental::make_remove_if_sequence(std::move(seq), ural::not_fn(std::move(pred)))}
        {}

        // Однопроходная последовательность
        /** @brief Пройденная часть последовательности
        @return Пройденная часть последовательности
        */
        filter_sequence<TraversedFrontType<Sequence>, Predicate>
        traversed_front() const
        {
            using Result = filter_sequence<TraversedFrontType<Sequence>, Predicate>;
            return Result(Base::base().traversed_front().base(),
                          this->predicate());
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
            return Base::base().predicate().target();
        }

        //@{
        /** @brief Базовая последовательность
        @return Базовая последовательность
        */
        Sequence const & base() const &;

        Sequence && base() &&
        {
            return static_cast<Base&&>(*this).base().base();
        }
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
            = odr_const<experimental::pipeable_maker<make_filter_sequence_fn>>;
    }
}
// namespace experimental
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_FILTERED_HPP_INCLUDED
