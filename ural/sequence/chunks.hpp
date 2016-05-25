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
namespace experimental
{
    /** @brief Адаптор курсора, разделяющий последовательности на
    подпоследовательности одинаковой длины (кроме, возможно, последней).
    @tparam Cursor базовsq курсор, должен быть как минимум прямым, чтобы функция
    @c front была регулярной.
    */
    template <class Cursor>
    class chunks_cursor
     : public cursor_adaptor<chunks_cursor<Cursor>, Cursor>
    {
        using Inherited = cursor_adaptor<chunks_cursor, Cursor>;

        BOOST_CONCEPT_ASSERT((concepts::ForwardCursor<Cursor>));
    public:
        // Типы
        /// @brief Тип значения
        typedef take_cursor<Cursor> value_type;

        /// @brief Тип ссылки
        typedef value_type reference;

        /// @brief Категория курсора
        using cursor_tag = typename value_type::cursor_tag;

        /// @brief Тип расстояния
        using distance_type = DifferenceType<Cursor>;

        // Конструкторы
        /** @brief Конструктор
        @param cur базовый курсор
        @param n размер подпоследовательностей
        @post <tt> this->base() == cur </tt>
        @post <tt> this->chunk_size() == n </tt>
        */
        chunks_cursor(Cursor cur, distance_type n)
         : Inherited(std::move(cur))
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

        // Однопроходый курсор
        /** @brief Текущий элемент
        @pre <tt> !*this == false </tt>
        @return Ссылка на текущий элемент
        */
        reference front() const
        {
            return this->base() | ::ural::experimental::taken(this->chunk_size());
        }

        /** @brief Переход к следующему элементу
        @pre <tt> !*this == false </tt>
        */
        void pop_front()
        {
            auto s = this->front();
            ural::exhaust_front(s);
            this->mutable_base() = std::move(s).base();
        }

        // Курсор произвольного доступа
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
        @return Ссылка на курсор подпоследовательности с номером @c index
        */
        reference operator[](distance_type index) const
        {
            assert(0 <= index && index < this->size());

            return (this->base() + index * this->chunk_size())
                   | ::ural::experimental::taken(this->chunk_size());
        }

        /** @brief Продвижение на заданное число элементов
        @param n количество элементов, которые нужно пропустить
        @pre <tt> 0 <= n && n <= this->size() </tt>
        @return <tt> *this </tt>
        */
        chunks_cursor & operator+=(distance_type n)
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

        template <class OtherCursor>
        chunks_cursor<OtherCursor>
        rebind_base(OtherCursor s) const &
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
    template <class Cursor>
    bool operator==(chunks_cursor<Cursor> const & x,
                    chunks_cursor<Cursor> const & y)
    {
        return x.base() == y.base() && x.chunk_size() == y.chunk_size();
    }

    /// @brief Тип функционального объекта создания @c chunk_cursor
    struct make_chunks_cursor_fn
    {
    public:
        /** @brief Создание @c chunk_cursor
        @param seq базовая последовательность
        @param n количество элементов в подпоследовательности
        @pre n > 0
        @return <tt> chunks_cursor<>(cursor_fwd<Sequence>(seq), n) </tt>
        */
        template <class Sequence>
        chunks_cursor<cursor_type_t<Sequence>>
        operator()(Sequence && seq,
                   DifferenceType<cursor_type_t<Sequence>> n) const
        {
            assert(n > 0);

            typedef chunks_cursor<cursor_type_t<Sequence>> Result;
            return Result(::ural::cursor_fwd<Sequence>(seq), std::move(n));
        }
    };

    namespace
    {
    /// @brief Функциональный объект создания @c chunk_cursor
    constexpr auto const & make_chunks_cursor = odr_const<make_chunks_cursor_fn>;

    /** @brief Функциональный объект для создания @c chunk_cursor в
    конвейерном стиле
    */
    constexpr auto const & chunked = odr_const<pipeable_maker<make_chunks_cursor_fn>>;
    }
    //namespace
}
// namespace experimental
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_CHUNKS_HPP_INCLUDED
