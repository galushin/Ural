#ifndef Z_URAL_SEQUENCE_MOVED_HPP_INCLUDED
#define Z_URAL_SEQUENCE_MOVED_HPP_INCLUDED

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

/** @file ural/sequence/moved.hpp
 @brief Адаптор последовательности, преобразующий ссылки на элементы в
 rvalue-ссылки
*/

#include <ural/sequence/adaptor.hpp>
#include <ural/utility/pipeable.hpp>
#include <ural/iterator/move.hpp>
#include <ural/sequence/make.hpp>
#include <ural/sequence/iterator_sequence.hpp>
#include <ural/sequence/base.hpp>

namespace ural
{
    /** @brief Адаптор последовательности, возвращающие rvalue-ссылки для
    элементов базовой последовательности
    @tparam Sequence тип базовой последовательности
    */
    template <class Sequence>
    class move_sequence
     : public sequence_adaptor<move_sequence<Sequence>, Sequence>
    {
        typedef sequence_adaptor<move_sequence<Sequence>, Sequence> Adaptor;

        typedef typename Sequence::reference Base_reference;

    public:
        // Типы
        /// @brief Тип ссылки
        typedef typename moved_type<Base_reference>::type
            reference;

        // Конструирование, копирование, присваивание
        /** @brief Конструктор
        @param seq базовая последовательность
        @post <tt> this->base() == seq </tt>
        */
        explicit move_sequence(Sequence seq)
         : Adaptor{std::move(seq)}
        {}

        // Однопроходная последовательность
        /** @brief Текущий передний элемент
        @return <tt> std::move(this->base().front()) </tt>
        */
        reference front() const
        {
            return std::move(this->base().front());
        }

        // Двусторонняя последовательность
        /** @brief Текущий задний элемент
        @return <tt> std::move(this->base().back()) </tt>
        */
        reference back() const;

        // Итераторы
        /** @brief Итератор задающий начало последовательности
        @param x последовательность
        @return <tt> std::make_move_iterator(begin(x.base())) </tt>
        */
        friend auto begin(move_sequence const & x)
        -> ural::move_iterator<decltype(begin(x.base()))>
        {
            return ural::make_move_iterator(begin(x.base()));
        }

        /** @brief Итератор задающий конец последовательности
        @param x последовательность
        @return <tt> std::make_move_iterator(end(x.base())) </tt>
        */
        friend auto end(move_sequence<Sequence> const & x)
        -> ural::move_iterator<decltype(begin(x.base()))>
        {
            return ural::make_move_iterator(end(x.base()));
        }

    private:
        friend Adaptor;

        template <class OtherSequence>
        move_sequence<OtherSequence>
        rebind_base(OtherSequence s) const
        {
            return move_sequence<OtherSequence>(std::move(s));
        }
    };

    /** @brief Тип функционального объекта для создания @c move_sequence в
    функциональном стиле.
    */
    class make_move_sequence_fn
    {
    public:
        /** @brief Создание последовательности rvalue-ссылок базовой
        последовательности.
        @param seq последовательность
        */
        template <class Sequenced>
        auto operator()(Sequenced && seq) const
        {
            typedef move_sequence<SequenceType<Sequenced>> Result;
            return Result(::ural::sequence_fwd<Sequenced>(seq));
        }
    };

    namespace
    {
        /// @brief Функциональный объект для создания @c move_sequence в
        constexpr auto const & make_move_sequence
            = odr_const<make_move_sequence_fn>;

        /// @brief Объект для создания @c move_sequence в конвейерном стиле.
        constexpr auto const & moved
            = odr_const<experimental::pipeable<make_move_sequence_fn>>;
    }

    /** @brief Создание последовательности на основе
    <tt> std::move_iterator </tt>.
    @param first итератор, задающий начало последовательности.
    @param last итератор, задающий конец последовательности.
    @return <tt> make_move_sequence(make_iterator_sequence(first.base(), last.base())) </tt>
    */
    template <class Iterator>
    auto make_iterator_sequence(std::move_iterator<Iterator> first,
                                std::move_iterator<Iterator> last)
    -> move_sequence<decltype(make_iterator_sequence(first.base(), last.base()))>
    {
        return make_move_sequence(make_iterator_sequence(first.base(),
                                                         last.base()));
    }

    /** @brief Создание последовательности на основе
    <tt> ural::move_iterator </tt>.
    @param first итератор, задающий начало последовательности.
    @param last итератор, задающий конец последовательности.
    @return <tt> make_move_sequence(make_iterator_sequence(first.base(), last.base())) </tt>
    */
    template <class Iterator>
    auto make_iterator_sequence(ural::move_iterator<Iterator> first,
                                ural::move_iterator<Iterator> last)
    -> move_sequence<decltype(make_iterator_sequence(first.base(), last.base()))>
    {
        return make_move_sequence(make_iterator_sequence(first.base(),
                                                         last.base()));
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_MOVED_HPP_INCLUDED
