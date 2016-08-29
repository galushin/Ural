#ifndef REVERSED_HPP_INCLUDED
#define REVERSED_HPP_INCLUDED

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

/** @file ural/sequence/reversed.hpp
 @brief Адаптер последовательности, проходящий элементы исходной последовательности
 в обратном порядке.
*/

#include <ural/sequence/adaptor.hpp>
#include <ural/utility/pipeable.hpp>
#include <ural/sequence/make.hpp>
#include <ural/sequence/base.hpp>

namespace ural
{
namespace experimental
{
    /** @brief Адаптор курсора последовательности, посещающий элементы исходной
    последовательности в обратном порядке.
    @tparam BidirectionalCursor двунаправленный курсор.
    */
    template <class BidirectionalCursor>
    class reverse_cursor
     : public cursor_adaptor<reverse_cursor<BidirectionalCursor>,
                             BidirectionalCursor>
    {
        using Base = cursor_adaptor<reverse_cursor, BidirectionalCursor>;
    public:
        // Типы
        /// @brief Тип ссылки
        using typename Base::reference;

        /// @brief Тип расстояния
        using typename Base::distance_type;

        // Конструкторы
        /** @brief Конструктор
        @param cur базовый курсор
        @post <tt> this->base() == cur </tt>
        */
        explicit reverse_cursor(BidirectionalCursor cur)
         : Base(std::move(cur))
        {}

        // Однопроходый курсор
        /** @brief Текущий элемент последовательности
        @pre <tt> !*this == false </tt>
        @return <tt> this->base().back() </tt>
        */
        reference front() const
        {
            return this->base().back();
        }

        /// @brief Отбрасывает переднюю пройденную часть последовательности
        void pop_front()
        {
            return this->mutable_base().pop_back();
        }

        // Прямая многопроходная последовательность
        /** @brief Пройденная передняя часть последовательности
        @return <tt> reverse_cursor{this->base().traversed_back()} </tt>
        */
        reverse_cursor traversed_front() const
        {
            return reverse_cursor{this->base().traversed_back()};
        }

        /// @brief Отбрасывание передней пройденной части последовательности
        void shrink_front()
        {
            return this->mutable_base().shrink_back();
        }

        /** @brief Исчерпание последовательности за константное время в прямом
        порядке
        @post <tt> !*this == true </tt>
        @pre <tt> *this </tt> должна быть конечной
        */
        void exhaust_front()
        {
            return ural::exhaust_back(this->mutable_base());
        }

        // Двусторонняя последовательность
        /** @brief Задний элемент последовательности
        @pre <tt> !*this == false </tt>
        @return <tt> this->base().front() </tt>
        */
        reference back() const
        {
            return this->base().front();
        }

        /// @brief Отбрасывание задней пройденной части последовательности
        void pop_back()
        {
            return this->mutable_base().pop_front();
        }

        /** @brief Пройденная задняя часть последовательности
        @return Пройденная задняя часть последовательности
        */
        reverse_cursor traversed_back() const
        {
            return reverse_cursor(this->base().traversed_front());
        }

        /// @brief Отбрасывает пройденную заднюю часть последовательности
        void shrink_back()
        {
            return this->mutable_base().shrink_front();
        }

        /** @brief Исчерпание последовательности в обратном порядке за
        константное время
        @post <tt> !*this == true </tt>
        */
        void exhaust_back()
        {
            return ural::exhaust_front(this->mutable_base());
        }

        // Последовательность произвольного доступа
        /** @brief Индексированный доступ
        @param n индекс
        @pre <tt> 0 <= n && n < this->size() </tt>
        */
        reference operator[](distance_type n) const
        {
            return this->base()[this->size() - n - 1];
        }

        /** @brief Продвижение на заданное число элементов в передней части
        последовательности
        @param n число элементов, которые будут пропущены
        @pre <tt> 0 <= n && n <= this->size() </tt>
        @return <tt> *this </tt>
        */
        reverse_cursor & operator+=(distance_type n)
        {
            this->mutable_base().pop_back(n);
            return *this;
        }

        /** @brief Продвижение на заданное число элементов в задней части
        последовательности
        @param n число элементов, которые будут пропущены
        @pre <tt> 0 <= n && n <= this->size() </tt>
        */
        void pop_back(distance_type n)
        {
            this->mutable_base() += n;
        }

        friend auto begin(reverse_cursor const & x)
        -> std::reverse_iterator<decltype(begin(x.base()))>
        {
            return std::reverse_iterator<decltype(begin(x.base()))>{end(x.base())};
        }

        friend auto end(reverse_cursor const & x)
        -> std::reverse_iterator<decltype(begin(x.base()))>
        {
            return std::reverse_iterator<decltype(begin(x.base()))>{begin(x.base())};
        }
    };

    /// @brief Тип функционального объекта для создания @c reverse_cursor
    class make_reverse_cursor_fn
    {
    public:
        /** @brief Создание курсора обратной последовательности для курсора
        обратной последовательности.
        @param cur курсор обратной последовательности
        @return <tt> cur.base() </tt>
        */
        template <class BidirectionalCursor>
        BidirectionalCursor
        operator()(reverse_cursor<BidirectionalCursor> cur) const
        {
            return std::move(cur.base());
        }

        /** @brief Создание обратной последовательности
        @param seq исходная последовательность
        */
        template <class BidirectionalSequence>
        reverse_cursor<cursor_type_t<BidirectionalSequence>>
        operator()(BidirectionalSequence && seq) const
        {
            using Cursor = reverse_cursor<cursor_type_t<BidirectionalSequence>>;
            return Cursor(::ural::cursor_fwd<BidirectionalSequence>(seq));
        }
    };

    namespace
    {
        /// @brief Функциональный объект для создания @c reverse_cursor
        constexpr auto const & make_reverse_cursor
            = odr_const<make_reverse_cursor_fn>;

        /// @brief Объект для создания @c reverse_sequence
        constexpr auto const & reversed
            = odr_const<experimental::pipeable<make_reverse_cursor_fn>>;
    }

    /** @brief Создание обратного курсора на основе <tt> std::reverse_iterator </tt>
    @param first итератор, задающий начало последовательности.
    @param last итератор, задающий конец последовательности.
    @return <tt> make_reverse_cursor(make_iterator_cursor(last.base(), first.base())) </tt>
    */
    template <class Iterator>
    auto make_iterator_cursor(std::reverse_iterator<Iterator> first,
                              std::reverse_iterator<Iterator> last)
    {
        using ::ural::make_iterator_cursor;
        return make_iterator_cursor(last.base(), first.base()) | reversed;
    }
}
// namespace experimental
}
// namespace ural

#endif
// REVERSED_HPP_INCLUDED
