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
    /** @brief Адаптор последовательности, посещающий элементы исходной
    последовательности в обратном порядке.
    @tparam BidirectionalSequence двусторонняя последовательность.
    */
    template <class BidirectionalSequence>
    class reverse_sequence
     : public sequence_adaptor<reverse_sequence<BidirectionalSequence>,
                               BidirectionalSequence>
    {
        using Base = sequence_adaptor<reverse_sequence<BidirectionalSequence>,
                                      BidirectionalSequence>;
    public:
        // Типы
        /// @brief Тип ссылки
        using typename Base::reference;

        /// @brief Тип расстояния
        using typename Base::distance_type;

        // Конструкторы
        /** @brief Конструктор
        @param seq базовая последовательность
        @post <tt> this->base() == seq </tt>
        */
        explicit reverse_sequence(BidirectionalSequence seq)
         : Base(std::move(seq))
        {}

        // Однопроходная последовательность
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
        @return <tt> reverse_sequence{this->base().traversed_back()} </tt>
        */
        reverse_sequence traversed_front() const
        {
            return reverse_sequence{this->base().traversed_back()};
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
            return this->mutable_base().exhaust_back();
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
        reverse_sequence traversed_back() const
        {
            return reverse_sequence(this->base().traversed_front());
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
            return this->mutable_base().exhaust_front();
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
        reverse_sequence & operator+=(distance_type n)
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

        friend auto begin(reverse_sequence const & x)
        -> std::reverse_iterator<decltype(begin(x.base()))>
        {
            return std::reverse_iterator<decltype(begin(x.base()))>{end(x.base())};
        }

        friend auto end(reverse_sequence const & x)
        -> std::reverse_iterator<decltype(begin(x.base()))>
        {
            return std::reverse_iterator<decltype(begin(x.base()))>{begin(x.base())};
        }
    };

    /// @brief Тип функционального объекта для создания @c reverse_sequence
    class make_reverse_sequence_fn
    {
    public:
        /** @brief Создание обратной последовательности к обратной
        последовательности
        @param seq обратная последовательность
        @return <tt> seq.base() </tt>
        */
        template <class BidirectionalSequence>
        auto operator()(reverse_sequence<BidirectionalSequence> seq) const
        {
            return std::move(seq.base());
        }

        /** @brief Создание обратной последовательности
        @param seq исходная последовательность
        */
        template <class BidirectionalSequence>
        auto operator()(BidirectionalSequence && seq) const
        {
            typedef reverse_sequence<SequenceType<BidirectionalSequence>> Seq;
            return Seq(::ural::sequence_fwd<BidirectionalSequence>(seq));
        }
    };

    namespace
    {
        /// @brief Функциональный объект для создания @c reverse_sequence
        constexpr auto const & make_reverse_sequence
            = odr_const<make_reverse_sequence_fn>;

        /// @brief Объект для создания @c reverse_sequence
        constexpr auto const & reversed
            = odr_const<pipeable<make_reverse_sequence_fn>>;
    }

    /** @brief Создание обратной последовательности на основе
    <tt> std::reverse_iterator </tt>
    @param first итератор, задающий начало последовательности.
    @param last итератор, задающий конец последовательности.
    @return <tt> make_reverse_sequence(make_iterator_sequence(last.base(), first.base())) </tt>
    */
    template <class Iterator>
    auto make_iterator_sequence(std::reverse_iterator<Iterator> first,
                                std::reverse_iterator<Iterator> last)
    {
        return make_iterator_sequence(last.base(), first.base()) | reversed;
    }
}
// namespace ural

#endif
// REVERSED_HPP_INCLUDED
