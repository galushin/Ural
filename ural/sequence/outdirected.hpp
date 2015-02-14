#ifndef Z_URAL_SEQUENCE_OUTDIRECTED_HPP_INCLUDED
#define Z_URAL_SEQUENCE_OUTDIRECTED_HPP_INCLUDED

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

/** @file ural/sequence/outdirected.hpp
 @brief Адаптор последовательности, возвращающий в качестве значения исходную
 последовательность
*/

namespace ural
{
    /** @brief Адаптор последовательности, возвращающий в качестве значения
    исходную последовательность
    @tparam Sequence последовательность
    */
    template <class Sequence>
    class outdirected_sequence
     : public sequence_base<outdirected_sequence<Sequence>>
    {
        template <class U>
        static size_t
        decl_distance_type(...);

        template <class U>
        static decltype(std::declval<U>() - std::declval<U>())
        decl_distance_type(void *);

        template <class U>
        static typename U::distance_type
        decl_distance_type(declare_type<Sequence> *);

    public:
        // Типы
        /// @brief Категория обхода
        typedef typename Sequence::traversal_tag traversal_tag;

        /// @brief Тип значения
        typedef Sequence value_type;

        /// @brief Тип ссылки
        typedef value_type const & reference;

        /// @brief Тип указателя
        typedef value_type pointer;

        /// @brief Тип расстояния
        typedef decltype(decl_distance_type<Sequence>(nullptr)) distance_type;

        // Конструктор
        /** @brief Конструктор
        @param s базовая последовательность
        @post <tt> this->base() == s </tt>
        */
        explicit outdirected_sequence(Sequence s)
         : base_{std::move(s)}
        {}

        // Однопроходная последовательность
        /** @brief Проверка исчерпания последовательностей
        @return @b true, если последовательность исчерпана, иначе --- @b false.
        */
        bool operator!() const
        {
            return !this->base();
        }

        /** @brief Текущий элемент
        @return <tt> this->base() </tt>
        */
        reference front() const
        {
            return this->base();
        }

        /** @brief Переход к следующему элементу
        @pre <tt> !!*this </tt>
        */
        void pop_front()
        {
            ++ base_;
        }

        // Адаптор последовательности
        /** @brief Базовая последовательность
        @return Константная ссылка на базовую последовательность
        */
        reference base() const
        {
            return this->base_;
        }

    private:
        Sequence base_;
    };

    /** Функция создания @c outdirected_sequence. Данная функция не преобразует
    свой аргумент в последовательность, прежде чем применять адаптор. Это
    связано с тем, что основное предназначение этой функции --- преобразовывать
    типы с оператором ++ в последовательности.
    @brief Функция создания @c outdirected_sequence
    @return <tt> outdirected_sequence<Sequence>{std::move(x)} </tt>
    */
    template <class Sequence>
    outdirected_sequence<Sequence>
    make_outdirected_sequence(Sequence x)
    {
        return outdirected_sequence<Sequence>{std::move(x)};
    }

    struct outdirected_helper {};
    auto constexpr outdirected = outdirected_helper{};

    /** @brief Создание @c outdirected_sequence в "конвейерном" стиле
    @param seq базовая последовательность
    @return <tt> make_outdirected_sequence(std::move(seq)) </tt>
    */
    template <class Sequence>
    outdirected_sequence<Sequence>
    operator|(Sequence seq, outdirected_helper)
    {
        return make_outdirected_sequence(std::move(seq));
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_OUTDIRECTED_HPP_INCLUDED
