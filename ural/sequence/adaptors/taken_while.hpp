#ifndef Z_URAL_SEQUENCE_TAKEN_WHILE_HPP_INCLUDED
#define Z_URAL_SEQUENCE_TAKEN_WHILE_HPP_INCLUDED

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

/** @file ural/sequence/adaptors/taken_while.hpp
 @brief Адаптор последовательности, возвращающий наибольший префикс, все
 элементы которого удовлетворяют заданному предикату.
*/

#include <ural/functional/make_callable.hpp>
#include <ural/sequence/make.hpp>
#include <ural/sequence/adaptor.hpp>
#include <ural/utility/pipeable.hpp>

namespace ural
{
    /** @brief Адаптор последовательности, возвращающий наибольший префикс, все
    элементы которого удовлетворяют заданному предикату.
    @tparam Sequence тип базовой последовательности
    @tparam Predicate тип унарного предиката
    */
    template <class Sequence, class Predicate>
    class taken_while_sequence
     : public sequence_adaptor<taken_while_sequence<Sequence, Predicate>,
                               Sequence>
    {
        using Inherited = sequence_adaptor<taken_while_sequence<Sequence, Predicate>, Sequence>;

    /** @brief Оператор "равно"
    @param x, y аргументы
    @return <tt> x.base() == y.base() && x.predicate() == y.predicate() </tt>
    */
    friend bool operator==(taken_while_sequence const & x,
                           taken_while_sequence const & y)
    {
        return x.base() == y.base() && x.predicate() == y.predicate();
    }

    public:
        /// @brief Категория обхода
        using traversal_tag
            = CommonType<typename Sequence::traversal_tag, forward_traversal_tag>;

        /** @brief Конструктор
        @param seq базовая последовательность
        @param pred предикат
        @post <tt> this->base() == seq </tt>
        @post <tt> this->predicate() == pred </tt>
        */
        taken_while_sequence(Sequence seq, Predicate pred)
         : Inherited(std::move(seq))
         , pred_(std::move(pred))
        {}

        /** @brief Проверка исчерпания
        @return <tt> !this->base() || !this->predicate()(*this->base()) </tt>
        */
        bool operator!() const
        {
            return !this->base() || !this->predicate()(*this->base());
        }

        /** @brief Используемый предикат
        @return Используемый предикат
        */
        Predicate const & predicate() const
        {
            return this->pred_;
        }

    private:
        friend Inherited;

        taken_while_sequence
        rebind_base(Sequence seq) const
        {
            return taken_while_sequence(std::move(seq), this->predicate());
        }

        Predicate pred_;
    };

    /// @brief Функциональный объект для создания @c taken_while_sequence
    class make_taken_while_fn
    {
    public:
        /** @brief Создание @c taken_while_sequence
        @param seq базовая последовательность
        @param Predicate унарный предикат
        */
        template <class Sequenced, class Predicate>
        taken_while_sequence<SequenceType<Sequenced>, FunctionType<Predicate>>
        operator()(Sequenced && seq, Predicate pred) const
        {
            using Result = taken_while_sequence<SequenceType<Sequenced>,
                                                FunctionType<Predicate>>;
            return Result(ural::sequence_fwd<Sequenced>(seq),
                          ural::make_callable(std::move(pred)));
        }
    };

    namespace
    {
        /// @brief Функциональный объект создания @c taken_while_sequence
        constexpr auto const & make_taken_while
            = odr_const<make_taken_while_fn>;

        /** @brief Функциональный объект создания @c taken_while_sequence
        в конвейерном стиле.
        */
        constexpr auto const & taken_while
            = odr_const<pipeable_maker<make_taken_while_fn>>;

    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_TAKEN_WHILE_HPP_INCLUDED
