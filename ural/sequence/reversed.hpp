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

namespace ural
{
    /** @brief Адаптор последовательности, посещающий элементы исходной
    последовательности в обратном порядке.
    @tparam BidirectionalSequence двусторонняя последовательность.
    */
    template <class BidirectionalSequence>
    class reverse_sequence
     : public sequence_base<reverse_sequence<BidirectionalSequence>>
    {

    /** @brief Создание обратной последовательности
    @param seq последовательность
    @return <tt> seq.base() </tt>
    */
    friend BidirectionalSequence make_reverse_sequence(reverse_sequence seq)
    {
        return std::move(seq.base_);
    }
    public:
        // Типы
        /// @brief Тип ссылки
        typedef typename BidirectionalSequence::reference reference;

        /// @brief Тип значения
        typedef typename BidirectionalSequence::value_type value_type;

        /// @brief Тип расстояния
        typedef typename BidirectionalSequence::distance_type distance_type;

        /// @brief Категория обхода
        typedef typename BidirectionalSequence::traversal_tag traversal_tag;

        /// @brief Тип указателя
        typedef typename BidirectionalSequence::pointer pointer;

        // Конструкторы
        /** @brief Конструктор
        @param seq базовая последовательность
        @post <tt> this->base() == seq </tt>
        */
        explicit reverse_sequence(BidirectionalSequence seq)
         : base_(std::move(seq))
        {}

        // Однопроходная последовательность
        /** @brief Проверка исчерпания последовательностей
        @return @b true, если последовательность исчерпана, иначе --- @b false.
        */
        bool operator!() const
        {
            return !this->base();
        }

        /** @brief Текущий элемент последовательности
        @pre <tt> !*this == false </tt>
        @return <tt> this->base().back() </tt>
        */
        reference front() const
        {
            return base_.back();
        }

        /// @brief Отбрасывает переднюю пройденную часть последовательности
        void pop_front()
        {
            return base_.pop_back();
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
            return base_.shrink_back();
        }

        // Двусторонняя последовательность
        /** @brief Задний элемент последовательности
        @pre <tt> !*this == false </tt>
        @return <tt> this->base().front() </tt>
        */
        reference back() const
        {
            return base_.front();
        }

        /// @brief Отбрасывание задней пройденной части последовательности
        void pop_back()
        {
            return base_.pop_front();
        }

        // @todo Пройденная задняя часть последовательности

        // Последовательность произвольного доступа
        /** @brief Продвижение на заданное число элементов в передней части
        @param n число элементов, которые будут пропущены
        @return <tt> *this </tt>
        */
        reverse_sequence & operator+=(distance_type n)
        {
            base_.pop_back(n);
            return *this;
        }

        // Адаптор последовательности
        /** @brief Базовая последовательность
        @return Базовая последовательность
        */
        BidirectionalSequence const & base() const
        {
            return this->base_;
        }

    private:
        BidirectionalSequence base_;
    };

    /** @brief Оператор "равно"
    @param x левый операнд
    @param y правый операнд
    @return <tt> x.base() == y.base() </tt>
    */
    template <class Sequence>
    bool operator==(reverse_sequence<Sequence> const & x,
                    reverse_sequence<Sequence> const & y);

    struct reversed_helper{};

    /** @brief Создание обратной последовательности
    @param seq исходная последовательность
    */
    template <class BidirectionalSequence>
    auto make_reverse_sequence(BidirectionalSequence && seq)
    -> reverse_sequence<decltype(sequence(std::forward<BidirectionalSequence>(seq)))>
    {
        typedef reverse_sequence<decltype(sequence(std::forward<BidirectionalSequence>(seq)))> Seq;
        return Seq{sequence(std::forward<BidirectionalSequence>(seq))};
    }

    template <class BidirectionalSequence>
    auto operator|(BidirectionalSequence && seq, reversed_helper)
    -> decltype(make_reverse_sequence(std::forward<BidirectionalSequence>(seq)))
    {
        return make_reverse_sequence(std::forward<BidirectionalSequence>(seq));
    }

    constexpr auto reversed = reversed_helper{};

    /** @brief Создание обратной последовательности на основе
    <tt> std::reverse_iterator </tt>
    @param first итератор, задающий начало последовательности.
    @param last итератор, задающий конец последовательности.
    @return <tt> make_reverse_sequence(make_iterator_sequence(last.base(), first.base())) </tt>
    */
    template <class Iterator>
    auto make_iterator_sequence(std::reverse_iterator<Iterator> first,
                                std::reverse_iterator<Iterator> last)
    -> reverse_sequence<decltype(make_iterator_sequence(last.base(), first.base()))>
    {
        return make_reverse_sequence(make_iterator_sequence(last.base(),
                                                            first.base()));
    }
}
// namespace ural

#endif
// REVERSED_HPP_INCLUDED
