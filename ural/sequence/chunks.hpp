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
*/

#include <ural/sequence/adaptor.hpp>
#include <ural/sequence/adaptors/taken.hpp>
#include <ural/sequence/make.hpp>
#include <ural/concepts.hpp>

namespace ural
{
    /** @brief Адаптор последовательности, разделяющий её на
    подпоследовательности одинаковой длины (кроме, возможно, последней).
    @tparam Sequence базовая последовательность, должна быть как минимум
    прямой, чтобы функция @c front была регулярной.
    */
    template <class Sequence>
    class chunks_sequence
     : public sequence_adaptor<chunks_sequence<Sequence>, Sequence>
    {
        using Inherited = sequence_adaptor<chunks_sequence<Sequence>, Sequence>;

        BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<Sequence>));
    public:
        // Типы
        /// @brief Тип значения
        typedef take_sequence<Sequence> value_type;

        /// @brief Тип ссылки
        typedef value_type reference;

        /// @brief Категория обхода
        typedef typename value_type::traversal_tag traversal_tag;

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
         : Inherited(std::move(seq))
         , n_(n)
        {}

        // Адаптор
        /** @brief Размер подпоследовательностей
        @return Размер подпоследовательностей
        */
        distance_type chunk_size() const
        {
            return this->n_;
        }

        // Однопроходная последовательность
        /** @brief Текущий элемент последовательности
        @pre <tt> !*this == false </tt>
        @return Ссылка на текущий элемент последовательности
        */
        reference front() const
        {
            return this->base() | ural::taken(this->chunk_size());
        }

        /** @brief Переход к следующему элементу
        @pre <tt> !*this == false </tt>
        */
        void pop_front()
        {
            auto s = this->front();
            s.exhaust_front();
            this->mutable_base() = std::move(s).base();
        }

        // Последовательность произвольного доступа
        /** @brief Размер
        @return Количество элементов
        */
        distance_type size() const
        {
            return this->base().size() / this->chunk_size()
                    + (this->base().size() % this->chunk_size() != 0);
        }

        /** @brief Оператор доступа к элементам по индексу
        @param index номер элемента
        @pre <tt> 0 <= index && index < this->size() </tt>
        @return Ссылка на подпоследовательность с номером @c index
        */
        reference operator[](distance_type index) const
        {
            assert(0 <= index && index < this->size());

            return (this->base() + index * this->chunk_size())
                   | ural::taken(this->chunk_size());
        }

        /** @brief Продвижение на заданное число элементов
        @param n количество элементов, которые нужно пропустить
        @pre <tt> 0 <= n && n <= this->size() </tt>
        @return <tt> *this </tt>
        */
        chunks_sequence & operator+=(distance_type n)
        {
            assert(0 <= n && n <= this->size());

            if(n == this->size())
            {
                this->exhaust_front();
            }
            else
            {
                this->mutable_base() += n * this->chunk_size();
            }

            return *this;
        }

    private:
        friend Inherited;

        template <class OtherSequence>
        chunks_sequence<OtherSequence>
        rebind_base(OtherSequence s) const &
        {
            return {std::move(s), this->chunk_size()};
        }

    private:
        distance_type n_;
    };

    /** @brief Оператор "равно"
    @param x, y аргументы
    @return @b true, если базовые последовательности и размеры
    подпоследовательностей равны, иначе --- @b false.
    */
    template <class Sequence>
    bool operator==(chunks_sequence<Sequence> const & x,
                    chunks_sequence<Sequence> const & y)
    {
        return x.base() == y.base() && x.chunk_size() == y.chunk_size();
    }

    /// @brief Тип функционального объекта создания @c chunk_sequence
    struct make_chunks_sequence_fn
    {
    public:
        /** @brief Создание @c chunk_sequence
        @param seq базовая последовательность
        @param n количество элементов в подпоследовательности
        @pre n > 0
        @return <tt> chunks_sequence<>(sequence_fwd<Sequenced>(seq), n) </tt>
        */
        template <class Sequenced>
        chunks_sequence<SequenceType<Sequenced>>
        operator()(Sequenced && seq,
                   DifferenceType<SequenceType<Sequenced>> n) const
        {
            assert(n > 0);

            typedef chunks_sequence<SequenceType<Sequenced>> Result;
            return Result(::ural::sequence_fwd<Sequenced>(seq), std::move(n));
        }
    };

    namespace
    {
        /// @brief Функциональный объект создания @c chunk_sequence
        auto const & make_chunks_sequence = odr_const<make_chunks_sequence_fn>;

        /** @brief Функциональный объект для создания @c chunk_sequence в
        конвейерном стиле
        */
        auto const & chunked = odr_const<pipeable_maker<make_chunks_sequence_fn>>;
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_CHUNKS_HPP_INCLUDED
