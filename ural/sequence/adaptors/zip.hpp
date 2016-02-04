#ifndef Z_URAL_SEQUENCE_ZIP_HPP_INCLUDED
#define Z_URAL_SEQUENCE_ZIP_HPP_INCLUDED

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

/** @file ural/sequence/zip.hpp
 @brief Последовательность кортежей соответствующих элементов
 последовательностей
*/

#include <ural/tuple.hpp>

namespace ural
{
    /** @brief Последовательность кортежей значений базовых последовательностей
    @tparam Inputs базовые последовательности
    */
    template <class... Inputs>
    class zip_sequence
     : public sequence_base<zip_sequence<Inputs...>>
    {
        typedef tuple<Inputs...> Bases_tuple;

    public:
        // Типы
        /// @brief Тип значения
        using value_type = tuple<ValueType<Inputs>...>;

        /// @brief Тип ссылки
        using reference = tuple<ReferenceType<Inputs>...>;

        /// @brief Тип указателя
        using pointer = void;

        /// @brief Тип расстояния
        using distance_type = CommonType<DifferenceType<Inputs>...>;

        /// @brief Категория курсора
        using cursor_tag = CommonType<typename Inputs::cursor_tag...>;

        // Конструкторы
        /** @brief Конструктор
        @param ins список входных последовательностей
        @post <tt> this->bases() == make_callable(ins...) </tt>
        */
        zip_sequence(Inputs... ins)
         : bases_{std::move(ins)...}
        {}

        //@{
        /** @brief Кортеж базовых последовательностей
        @return Константная ссылка на кортеж базовых последовательностей
        */
        tuple<Inputs...> const &
        bases() const &
        {
            return this->bases_;
        }

        tuple<Inputs...> && bases() &&
        {
            return std::move(this->bases_);
        }
        //@}

        // Однопроходная последовательность
        /** @brief Проверка исчерпания последовательности
        @return @b true, если последовательность исчерпана, иначе --- @b false
        */
        bool operator!() const
        {
            return experimental::tuples::any_of(this->bases(), ural::logical_not<>{});
        }

        /** @brief Текущий элемент
        @pre <tt> !!*this </tt>
        @return Текущий элемент
        */
        reference front() const
        {
            return this->transform_bases<reference>(ural::front);
        }

        /** @brief Переход к следующему элементу
        @pre <tt> !!*this </tt>
        */
        void pop_front()
        {
            return this->for_each_base(ural::pop_front);
        }

        // Прямая последовательность
        /** @brief Полная последовательность (вместе с пройденными частями)
        @return Исходная последовательность
        */
        zip_sequence original() const
        {
            return this->transform_bases<zip_sequence>(ural::original);
        }

        /** @brief Пройденная передняя часть последовательность
        @return Пройденная передняя часть последовательность
        */
        zip_sequence<TraversedFrontType<Inputs>...>
        traversed_front() const
        {
            using Seq = zip_sequence<TraversedFrontType<Inputs>...>;
            return this->transform_bases<Seq>(ural::traversed_front);
        }

        /** @brief Отбрасывание пройденной части последовательности
        @post <tt> !this->traversed_front() </tt>
        */
        void shrink_front()
        {
            return this->for_each_base(ural::shrink_front);
        }

        /** @brief Исчерпание последовательности в прямом порядке за константное
        время.
        @post <tt> !*this == true </tt>
        */
        void exhaust_front()
        {
            return this->for_each_base(ural::exhaust_front);
        }

        // Двусторонняя последовательность
        /** @brief Задний элемент последовательности
        @pre <tt> !*this == false </tt>
        */
        reference back() const
        {
            return this->transform_bases<reference>(ural::back);
        }

        /// @brief Пропуск последнего элемента последовательности
        void pop_back()
        {
            return this->for_each_base(ural::pop_back);
        }

        /** @brief Пройденная задняя часть последовательность
        @return Пройденная задняя часть последовательность
        */
        zip_sequence traversed_back() const
        {
            return this->transform_bases<zip_sequence>(ural::traversed_back);
        }

        /// @brief Отбрасывает пройденную заднюю часть последовательности
        void shrink_back()
        {
            return this->for_each_base(ural::shrink_back);
        }

        /** @brief Исчерпание последовательности в обратном порядке за
        константное время
        @post <tt> !*this == true </tt>
        */
        void exhaust_back()
        {
            return this->for_each_base(ural::exhaust_back);
        }

        // Последовательность произвольного доступа
        /** @brief Количество элементов последовательности
        @return Минимальный из размеров базовых последовательностей
        */
        distance_type size() const
        {
            // @todo Обобщить, реализовать без псевдо-рекурсии
            return this->size_impl(this->bases()[ural::_1].size(), ural::_2);
        }

        /** @brief Индексированный доступ
        @param n индекс
        @pre <tt> 0 < this->size() && this->size() < n </tt>
        @return <tt> this->base()[n] </tt>
        */
        reference operator[](distance_type n) const
        {
            auto f = [&](auto const & x) -> decltype(auto) { return ural::subscript(x, n); };

            return this->transform_bases<reference>(std::move(f));
        }

        /** @brief Продвижение на заданное число элементов в передней части
        последовательности
        @param n число элементов, которые будут пропущены
        @pre <tt> 0 <= n && n <= this->size() </tt>
        @return <tt> *this </tt>
        */
        zip_sequence & operator+=(distance_type n)
        {
            this->for_each_base([&](auto & x) { return x += n; });
            return *this;
        }

        /** @brief Продвижение на заданное число элементов в задней части
        последовательности
        @param n число элементов, которые будут пропущены
        @pre <tt> 0 <= n && n <= this->size() </tt>
        */
        void pop_back(distance_type n)
        {
            this->for_each_base([&](auto & x) { return ural::pop_back(x, n); });
        }

    private:
        Bases_tuple & mutable_bases()
        {
            return this->bases_;
        }

        template <class R, class F, class Reduce, size_t... I>
        R transform_bases_impl(F f, Reduce reducer, index_sequence<I...>) const
        {
            return reducer(f(std::get<I>(this->bases()))...);
        }

        template <class R, class F>
        R transform_bases(F f) const
        {
            using Indices = index_sequence_for<Inputs...>;

            return this->transform_bases_impl<R>(std::move(f),
                                                 ural::constructor<R>{},
                                                 Indices{});
        }

        distance_type
        size_impl(distance_type result, placeholder<sizeof...(Inputs)>) const
        {
            return result;
        }

        template <size_t index>
        distance_type
        size_impl(distance_type current, placeholder<index> p) const
        {
            return this->size_impl(std::min(current, this->bases()[p].size()),
                                   placeholder<index+1>());
        }

        template <class Index1, class Index2>
        friend
        void indirect_swap_adl_hook(zip_sequence const & x, Index1 ix,
                                    zip_sequence const & y, Index2 iy)
        {
            //using Indices = index_sequence_for<Inputs...>;
            // @todo реализовать без псевдо-рекурсии
            return zip_sequence::indirect_swap_impl(x, ix, y, iy, ural::_1);
        }

        template <class Index1, class Index2>
        static
        void indirect_swap_impl(zip_sequence const &, Index1,
                                zip_sequence const &, Index2,
                                placeholder<sizeof...(Inputs)>)
        {
            return;
        }

        template <class Index1, class Index2, size_t I>
        static
        void indirect_swap_impl(zip_sequence const & x, Index1 ix,
                                zip_sequence const & y, Index2 iy,
                                placeholder<I>)
        {
            ural::indirect_swap(std::get<I>(x.bases()), ix,
                                std::get<I>(y.bases()), iy);
            return zip_sequence::indirect_swap_impl(x, ix, y, iy,
                                                    placeholder<I+1>{});
        }

        template <class Action>
        void for_each_base(Action action)
        {
            experimental::tuples::for_each(this->mutable_bases(), std::move(action));
        }

    private:
        Bases_tuple bases_;
    };

    /** @brief Оператор "равно"
    @param x, y операнды
    @return <tt> x.bases() == y.bases() </tt>
    */
    template <class... Inputs1, class... Inputs2>
    bool operator==(zip_sequence<Inputs1...> const & x,
                    zip_sequence<Inputs2...> const & y)
    {
        return x.bases() == y.bases();
    }

    /// @brief Тип функционального объекта для создания @c zip_sequence
    struct make_zip_sequence_fn
    {
    public:
        /** @brief Создание последовательности соответствующих элементов кортежей
        @param ins базовые последовательности
        */
        template <class... Inputs>
        zip_sequence<SequenceType<Inputs>...>
        operator()(Inputs && ... ins) const
        {
            typedef zip_sequence<SequenceType<Inputs>...> Seq;
            return Seq(::ural::sequence_fwd<Inputs>(ins)...);
        }
    };

    namespace
    {
        //@{
        /// @brief Функциональный объект для создания @c zip_sequence
        constexpr auto const & make_zip_sequence
            = odr_const<make_zip_sequence_fn>;

        constexpr auto const & combine = make_zip_sequence;
        //@}
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_ZIP_HPP_INCLUDED
