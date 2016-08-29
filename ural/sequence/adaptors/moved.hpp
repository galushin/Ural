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
#include <ural/sequence/iterator_cursor.hpp>
#include <ural/sequence/base.hpp>

namespace ural
{
namespace experimental
{
    /** @brief Адаптор курсора , возвращающие rvalue-ссылки для элементов
    базового курсора.
    @tparam Input тип базового курсора
    */
    template <class Input>
    class move_cursor
     : public cursor_adaptor<move_cursor<Input>, Input>
    {
        typedef cursor_adaptor<move_cursor, Input> Adaptor;

        typedef typename Input::reference Base_reference;

    public:
        // Типы
        /// @brief Тип ссылки
        typedef typename moved_type<Base_reference>::type
            reference;

        // Конструирование, копирование, присваивание
        /** @brief Конструктор
        @param cur базовый курсор
        @post <tt> this->base() == cur </tt>
        */
        explicit move_cursor(Input cur)
         : Adaptor{std::move(cur)}
        {}

        // Однопроходый курсор
        /** @brief Текущий передний элемент
        @return <tt> std::move(this->base().front()) </tt>
        */
        reference front() const
        {
            return std::move(this->base().front());
        }

        // Двусторонний курсор
        /** @brief Текущий задний элемент
        @return <tt> std::move(this->base().back()) </tt>
        */
        reference back() const;

        // Итераторы
        /** @brief Итератор задающий начало последовательности
        @param cur курсор
        @return <tt> std::make_move_iterator(begin(cur.base())) </tt>
        */
        friend auto begin(move_cursor const & cur)
        -> ural::move_iterator<decltype(begin(cur.base()))>
        {
            return ural::make_move_iterator(begin(cur.base()));
        }

        /** @brief Итератор задающий конец последовательности
        @param cur курсор
        @return <tt> std::make_move_iterator(end(x.base())) </tt>
        */
        friend auto end(move_cursor const & cur)
        -> ural::move_iterator<decltype(begin(cur.base()))>
        {
            return ural::make_move_iterator(end(cur.base()));
        }

    private:
        friend Adaptor;

        template <class OtherCursor>
        move_cursor<OtherCursor>
        rebind_base(OtherCursor s) const
        {
            return move_cursor<OtherCursor>(std::move(s));
        }
    };

    /// @brief Тип функционального объекта для создания @c move_cursor.
    class make_move_cursor_fn
    {
    public:
        /** @brief Создание курсора последовательности rvalue-ссылок базовой
        последовательности.
        @param seq последовательность
        */
        template <class Sequence>
        move_cursor<cursor_type_t<Sequence>>
        operator()(Sequence && seq) const
        {
            using Result = move_cursor<cursor_type_t<Sequence>>;
            return Result(::ural::cursor_fwd<Sequence>(seq));
        }
    };

    namespace
    {
        /// @brief Функциональный объект для создания @c move_cursor
        constexpr auto const & make_move_cursor
            = odr_const<make_move_cursor_fn>;

        /// @brief Объект для создания @c move_cursor в конвейерном стиле.
        constexpr auto const & moved
            = odr_const<experimental::pipeable<make_move_cursor_fn>>;
    }

    /** @brief Создание курсора на основе <tt> std::move_iterator </tt>.
    @param first итератор, задающий начало последовательности.
    @param last итератор, задающий конец последовательности.
    @return <tt> make_move_cursor(make_iterator_cursor(first.base(), last.base())) </tt>
    */
    template <class Iterator>
    auto make_iterator_cursor(std::move_iterator<Iterator> first,
                              std::move_iterator<Iterator> last)
    {
        using ::ural::make_iterator_cursor;
        return make_move_cursor(make_iterator_cursor(first.base(), last.base()));
    }

    /** @brief Создание курсора на основе <tt> ural::move_iterator </tt>.
    @param first итератор, задающий начало последовательности.
    @param last итератор, задающий конец последовательности.
    @return <tt> make_move_cursor(make_iterator_cursor(first.base(), last.base())) </tt>
    */
    template <class Iterator>
    auto make_iterator_cursor(ural::move_iterator<Iterator> first,
                              ural::move_iterator<Iterator> last)
    {
        using ::ural::make_iterator_cursor;
        return make_move_cursor(make_iterator_cursor(first.base(), last.base()));
    }
}
// namespace experimental
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_MOVED_HPP_INCLUDED
