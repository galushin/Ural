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
     : public sequence_base<move_sequence<Sequence>>
    {
        typedef typename Sequence::reference Base_reference;

    public:
        // Типы
        /// @brief Тип значения
        typedef ValueType<Sequence> value_type;

        /// @brief Тип ссылки
        typedef typename moved_type<Base_reference>::type
            reference;

        /// @brief Тип расстояния
        typedef DifferenceType<Sequence> distance_type;

        /// @brief Категория обхода
        typedef typename Sequence::traversal_tag traversal_tag;

        /// @brief Тип указателя
        typedef typename Sequence::pointer pointer;

        // Конструирование, копирование, присваивание
        /** @brief Конструктор
        @param seq базовая последовательность
        @post <tt> this->base() == seq </tt>
        */
        explicit move_sequence(Sequence seq)
         : base_{std::move(seq)}
        {}

        // Однопроходная последовательность
        /** @brief Проверка исчерпания последовательностей
        @return <tt> !this->base() </tt>
        */
        bool operator!() const
        {
            return !this->base();
        }

        /** @brief Текущий передний элемент
        @return <tt> std::move(this->base().front()) </tt>
        */
        reference front() const
        {
            return std::move(this->base().front());
        }

        /// @brief Отбрасывает передний элемент
        void pop_front()
        {
            ++ base_;
        }

        // Двусторонняя последовательность
        /** @brief Текущий задний элемент
        @return <tt> std::move(this->base().back()) </tt>
        */
        reference back() const
        {
            return std::move(this->base().back());
        }

        /// @brief Отбрасывает задний элемент
        void pop_back()
        {
            this->base_.pop_back();
        }

        // Адаптор последовательности
        //@{
        /** @brief Базовая последовательность
        @return Базовая последовательность
        */
        Sequence const & base() const &
        {
            return this->base_;
        }

        Sequence && base() &&
        {
            return std::move(this->base_);
        }
        //@}

    private:
        Sequence base_;
    };

    /** @brief Итератор задающий начало последовательности
    @param x последовательность
    @return <tt> std::make_move_iterator(begin(x.base())) </tt>
    */
    template <class Sequence>
    auto begin(move_sequence<Sequence> const & x)
    -> ural::move_iterator<decltype(begin(x.base()))>
    {
        return ural::make_move_iterator(begin(x.base()));
    }

    /** @brief Итератор задающий конец последовательности
    @param x последовательность
    @return <tt> std::make_move_iterator(end(x.base())) </tt>
    */
    template <class Sequence>
    auto end(move_sequence<Sequence> const & x)
    -> ural::move_iterator<decltype(begin(x.base()))>
    {
        return ural::make_move_iterator(end(x.base()));
    }

    /** @brief Создание последовательности rvalue-ссылок базовой
    последовательности.
    @param seq последовательность
    */
    template <class Sequence>
    auto make_move_sequence(Sequence && seq)
    -> move_sequence<decltype(::ural::sequence_fwd<Sequence>(seq))>
    {
        typedef move_sequence<decltype(::ural::sequence_fwd<Sequence>(seq))> Result;
        return Result(::ural::sequence_fwd<Sequence>(seq));
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

    /** @brief Вспомогательный класс для создания последовательностей
    rvalue-ссылок в конвеерной нотации
    */
    struct moved_helper{};

    /** @brief Создание последовательностей rvalue-ссылок в конвеерной нотации
    @param seq последовательность
    @return <tt> make_move_sequence(std::forward<Sequence>(seq)) </tt>
    */
    template <class Sequence>
    auto operator|(Sequence && seq, moved_helper)
    -> decltype(make_move_sequence(std::forward<Sequence>(seq)))
    {
        return make_move_sequence(std::forward<Sequence>(seq));
    }

    auto constexpr moved = moved_helper{};
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_MOVED_HPP_INCLUDED
