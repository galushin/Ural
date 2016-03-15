#ifndef Z_URAL_SIMO_SEQUENCE_HPP_INCLUDED
#define Z_URAL_SIMO_SEQUENCE_HPP_INCLUDED

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

/** @file ural/sequence/simo.hpp
 @brief Адаптор, производящий вывод сразу в несколько курсоров вывода
*/

#include <ural/sequence/make.hpp>

namespace ural
{
namespace experimental
{
    /** @ingroup Sequences
    @brief Адаптор, производящий вывод сразу в несколько курсоров вывода
    */
    template <class... Outputs>
    class simo_cursor_type
     : public cursor_base<simo_cursor_type<Outputs...>>
    {
        typedef tuple<Outputs...> Bases_tuple;
    public:
        friend bool operator==(simo_cursor_type const & x, simo_cursor_type const & y)
        {
            return x.bases() == y.bases();
        }

        // Типы
        /// @brief Категория курсора
        using cursor_tag = output_cursor_tag;

        /// @brief Тип расстояния
        typedef CommonType<difference_type<Outputs>...> distance_type;

        /// @brief Тип ссылки
        using reference = simo_cursor_type &;

        // Конструкторы, присваивание
        /** @brief Конструктор
        @param outs базовые курсоры
        @post <tt> this->bases() == make_tuple(std::move(outs)...) </tt>
        */
        simo_cursor_type(Outputs... outs)
         : bases_(std::move(outs)...)
        {}

        // Однопроходый курсор
        /** @brief Проверка исчерпания
        @return @b true, если курсор исчерпан, иначе --- @b false.
        */
        bool operator!() const
        {
            return experimental::tuples::any_of(this->bases(), ural::logical_not<>{});
        }

        /** @brief Передний элемент
        @return Ссылка на первый элемент курсора
        @pre <tt> !*this == true </tt>
        */
        reference operator*()
        {
            return *this;
        }

        /** @brief Вывод значения
        @param value выводимое значение
        @return *this
        */
        template <class T>
        void operator=(T const & value)
        {
            experimental::tuples::for_each(this->mutable_bases(),
                                   [&value](auto & s) { *s = value; });
        }

        /** @brief Переход к следующему элементу в передней части
        @pre <tt> !*this == true </tt>
        */
        void pop_front()
        {
            assert(!!*this);

            experimental::tuples::for_each(this->mutable_bases(), ural::pop_front);
        }

        // Прямой курсор
        /** @brief Передняя пройденная часть курсора
        @return Передняя пройденная часть курсора
        */
        simo_cursor_type<TraversedFrontType<Outputs>...>
        traversed_front() const
        {
            using Seq = simo_cursor_type<TraversedFrontType<Outputs>...>;
            return this->transform_bases<Seq>(ural::traversed_front);
        }

        // Адаптор курсора
        //@{
        /** @brief Доступ к кортежу базовых курсоров
        @return Константная ссылка на кортеж базовых курсоров
        */
        Bases_tuple const & bases() const &
        {
            return this->bases_;
        }

        Bases_tuple && bases() &&
        {
            return this->bases_;
        }
        //@}

    private:
        template <class R, class F, class Reduce, size_t... I>
        R transform_bases_impl(F f, Reduce reducer, index_sequence<I...>) const
        {
            return reducer(f(std::get<I>(this->bases()))...);
        }

        template <class R, class F>
        R transform_bases(F f) const
        {
            using Indices = index_sequence_for<Outputs...>;

            return this->transform_bases_impl<R>(std::move(f),
                                                 ::ural::experimental::constructor<R>{},
                                                 Indices{});
        }

        Bases_tuple & mutable_bases()
        {
            return this->bases_;
        }

        Bases_tuple bases_;
    };

    /** @brief Функция создания адаптора, производящего вывод сразу в несколько
    последовательностей вывода
    @param outs выходные последовательноти
    @return <tt> S(ural::cursor_fwd<Outputs>(outs)...) </tt>, где
    @c S --- <tt> simo_cursor_type<cursor_type_t<Outputs>...> </tt>.
    */
    template <class... Outputs>
    auto simo_cursor(Outputs && ... outs)
    -> simo_cursor_type<cursor_type_t<Outputs>...>
    {
        using Result = simo_cursor_type<cursor_type_t<Outputs>...>;
        return Result(ural::cursor_fwd<Outputs>(outs)...);
    }
}
// namespace experimental
}
// namespace ural

#endif
// Z_URAL_SIMO_SEQUENCE_HPP_INCLUDED
