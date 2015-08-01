#ifndef Z_URAL_SEQUENCE_CARGO_HPP_INCLUDED
#define Z_URAL_SEQUENCE_CARGO_HPP_INCLUDED

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

/** @file ural/sequence/cargo.hpp
 @brief Адаптор последовательности, добавляющий к ней объект заданного типа
*/

#include <ural/sequence/base.hpp>

namespace ural
{
    /** @ingroup Sequences
    @brief Адаптор последовательности, добавляющий к ней объект заданного типа
    @tparam Sequence тип последовательности
    @tparam T тип дополнительного объекта
    @todo Покрыть тестами все функции, необходимые последовательностям
    */
    template <class Sequence, class T>
    class cargo_sequence
    {
        /** @brief Оператор "равно"
        @param x левый операнд
        @param y правый операнд
        @return <tt> x.base() == y.base() </tt>
        */
        friend bool
        operator==(cargo_sequence const & x, cargo_sequence const & y)
        {
            return x.base() == y.base();
        }

        friend
        Sequence const & sequence(cargo_sequence const & s)
        {
            return s.base();
        }

        friend cargo_sequence sequence(cargo_sequence && s)
        {
            return std::move(s);
        }

    public:
        // Типы
        /// @brief Категория обхода
        typedef typename Sequence::traversal_tag traversal_tag;

        /// @brief Тип ссылки
        typedef typename Sequence::reference reference;

        /// @brief Тип значения
        typedef ValueType<Sequence> value_type;

        /// @brief Тип расстояния
        typedef DifferenceType<Sequence> distance_type;

        /// @brief Тип указателя
        typedef typename Sequence::pointer pointer;

        // Создание, копирование, присваивание
        /** @brief Конструктор
        @param seq последовательность
        @param x груз
        @post <tt> this->base() == seq </tt>
        @post <tt> this->cargo() == x </tt>
        */
        cargo_sequence(Sequence seq, T x)
         : members_(std::move(seq), std::move(x))
        {}

        cargo_sequence(cargo_sequence const &) = delete;
        cargo_sequence(cargo_sequence &&) = default;

        cargo_sequence & operator=(cargo_sequence const &) = delete;
        cargo_sequence & operator=(cargo_sequence &&) = default;

        // Свойства
        /** @brief Доступ к дополнительному объекту
        @return Константная ссылка на дополнительный объект
        */
        T const & cargo() const
        {
            return members_[ural::_2];
        }

        // Однопроходная последовательность
        /** @brief Проверка исчерпания последовательности
        @return @b true, если в последовательности больше нет элементов,
        иначе --- @b false.
        */
        bool operator!() const
        {
            return !this->base();
        }

        /** @brief Доступ к текущему (переднему) элементу последовательности
        @return Ссылка на передний элемент последовательности
        @pre <tt> bool(*this) != false </tt>
        */
        reference front() const
        {
            return this->base().front();
        }

        /** @brief Переход к следующему элементу последовательности
        @pre <tt> bool(*this) != false </tt>
        @return <tt> *this </tt>
        */
        void pop_front()
        {
            return this->members_[ural::_1].pop_front();
        }

        // Прямая последовательность
        /** @breif Передняя пройденная часть последовательности
        @return Передняя пройденная часть последовательности
        */
        Sequence traversed_front() const &
        {
            return this->members_[ural::_1].traversed_front();
        }

        /// @brief Отбросить переднюю пройденную часть последовательности
        void shrink_front()
        {
            return this->members_[ural::_1].shrink_front();
        }

        /** @brief Полная последовательность (включая пройденные части)
        @return Полная последовательность
        */
        cargo_sequence original() const;

        /** @brief Исчерпание последовательности за константное время
        @post <tt> !*this == true </tt>
        @pre <tt> *this </tt> должна быть конечной
        */
        void exhaust_front()
        {
            return this->members_[ural::_1].exhaust_front();
        }

        // Двусторонняя последовательность
        /** @breif Задняя пройденная часть последовательности
        @return Задняя пройденная часть последовательности
        */
        Sequence traversed_back() const &
        {
            return this->members_[ural::_1].traversed_back();
        }

        cargo_sequence traversed_back() &&;

        /** @brief Доступ к последнему непройденному элементу последовательности
        @pre <tt> !*this == false </tt>
        @return Ссылка на последний непройденный элемент последовательности
        */
        reference back() const
        {
            return this->base().back();
        }

        void shrink_back();

        /** @brief Переход к следующему элементу в задней части
        последовательности
        @pre <tt> !*this == false </tt>
        */
        void pop_back()
        {
            return this->members_[ural::_1].pop_back();
        }

        /** @brief Исчерпание последовательности в обратном порядке за
        константное время
        @post <tt> !*this == true </tt>
        */
        void exhaust_back();

        // Последовательность произвольного доступа
        /** @brief Размер последовательности
        @return Размер последовательности
        */
        distance_type size() const
        {
            return this->base().size();
        }

        /** @brief Индексированный доступ
        @param index индекс
        @pre <tt> 0 <= index < this->size() </tt>
        @return Ссылка на элемент с индексом @c index, считая от первого
        непройденного элемента последовательности.
        */
        reference operator[](distance_type index) const
        {
            return this->base()[index];
        }

        /** @brief Пропуск заданного числа элементов в передней части
        последовательности
        @param n количество элементов
        @pre <tt> 0 <= index <= this->size() </tt>
        @return <tt> *this </tt>
        */
        cargo_sequence & operator+=(distance_type n)
        {
            this->members_[ural::_1] += n;
            return *this;
        }

        void pop_back(distance_type n);

    private:
        /** @brief Доступ к базовой последовательности
        @return Константная ссылка на базовую последовательность
        */
        Sequence const & base() const
        {
            return members_[ural::_1];
        }

        tuple<Sequence, T> members_;
    };
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_CARGO_HPP_INCLUDED
