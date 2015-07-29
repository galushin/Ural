#ifndef Z_URAL_SEQUENCE_CHUNKS_HPP_INCLUDED
#define Z_URAL_SEQUENCE_CHUNKS_HPP_INCLUDED

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

/** @file ural/sequence/chunks.hpp
 @brief Представление последовательности в виде последовательности
 последовательностей одинаковой длины (кроме, возможно, последней).
 @todo Реализовать функции прямой последовательности
*/

#include <ural/sequence/taken.hpp>
#include <ural/sequence/make.hpp>
#include <ural/concepts.hpp>

namespace ural
{
    /** @brief Адаптор последовательности, разделяющий её на
    подпоследовательности одинаковой длины (кроме, возможно, последней).
    @tparam Sequence базовая последовательность
    @todo Добавить требование, что Sequence должна быть хотя бы прямой
    */
    template <class Sequence>
    class chunks_sequence
     : public sequence_base<chunks_sequence<Sequence>>
    {
    public:
        // Типы
        /// @brief Тип значения
        typedef take_sequence<Sequence> value_type;

        /// @brief Тип ссылки
        typedef value_type reference;

        /// @brief Категория обхода
        typedef forward_traversal_tag traversal_tag;

        /// @brief Тип расстояния
        using distance_type = DifferenceType<Sequence>;

        // Конструкторы
        /** @brief Конструктор
        @param seq базовая последовательность
        @param n размер подпоследовательности
        @post <tt> this->base() == seq </tt>
        @post <tt> this->chunk_size() == n </tt>
        */
        chunks_sequence(Sequence seq, distance_type n)
         : seq_(std::move(seq))
         , n_(n)
        {}

        // Адаптор
        //@{
        /** @brief Базовая последовательность
        @return Константная ссылка на базовую последовательность
        */
        Sequence const & base() const &
        {
            return this->seq_;
        }


        Sequence && base() &&;
        //@}

        /** @brief Размер подпоследовательностей
        @return Размер подпоследовательностей
        */
        distance_type chunk_size() const
        {
            return this->n_;
        }

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
        @return Ссылка на текущий элемент последовательности
        */
        reference front() const
        {
            return seq_ | ural::taken(this->chunk_size());
        }

        /** @brief Переход к следующему элементу
        @pre <tt> !*this == false </tt>
        */
        void pop_front()
        {
            auto s = this->front();
            s.exhaust_front();
            seq_ = std::move(s).base();
        }

        // Прямая последовательность
        /// @brief Отбросить переднюю пройденную часть последовательности
        void shrink_front();

        /** @breif Передняя пройденная часть последовательности
        @return Передняя пройденная часть последовательности
        */
        chunks_sequence traversed_front() const;

        /** @brief Полная последовательность (включая пройденные части)
        @return Полная последовательность
        */
        chunks_sequence original() const;

    private:
        Sequence seq_;
        distance_type n_;
    };

    /** @brief Функция создания @c chunk_sequence
    @param seq базовая последовательность
    @param n количество элементов в подпоследовательности
    @return <tt> chunks_sequence<>(sequence_fwd<Sequenced>(seq), n) </tt>
    */
    template <class Sequenced>
    auto make_chunks_sequence(Sequenced && seq,
                              DifferenceType<SequenceType<Sequenced>> n)
    -> chunks_sequence<SequenceType<Sequenced>>
    {
        assert(n > 0);
        typedef chunks_sequence<SequenceType<Sequenced>> Result;
        return Result(::ural::sequence_fwd<Sequenced>(seq), std::move(n));
    }

    template <class Sequence>
    bool operator==(chunks_sequence<Sequence> const & x,
                    chunks_sequence<Sequence> const & y);
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_CHUNKS_HPP_INCLUDED
