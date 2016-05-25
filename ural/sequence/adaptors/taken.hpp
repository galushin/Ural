#ifndef Z_URAL_SEQUENCE_TAKEN_HPP_INCLUDED
#define Z_URAL_SEQUENCE_TAKEN_HPP_INCLUDED

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

/** @file ural/sequence/taken.hpp
 @brief Адаптер последовательности, извлекающий из базовой последовательности
 не более заданного числа элементов.
*/

#include <ural/sequence/adaptor.hpp>
#include <ural/sequence/adaptors/taken_exactly.hpp>
#include <ural/sequence/base.hpp>
#include <ural/sequence/make.hpp>
#include <ural/utility.hpp>

namespace ural
{
namespace experimental
{
    /** @brief Адаптор курсора, ограничивающий размер базового курсора
    @tparam Input Тип базового курсора
    @tparam Size тип количества элементов, которые должны быть взяты из базового
    курсора
    @todo take_cursor может быть двусторонним только если исходная
    последовательность имеет произвольный доступ, уточнить traversal_tag.
    @todo Для последовательностей произвольного доступа можно оптимизировать:
    узнать точный размер в конструкторе, а следовательно делать меньше проверок
    в operator!, быстрее выполнять exhaust_front.
    */
    template <class Input, class Size = DifferenceType<Input>>
    class take_cursor
     : public cursor_adaptor<take_cursor<Input, Size>, Input>
    {
        using Base = cursor_adaptor<take_cursor, Input>;
    public:
        // Типы
        /// @brief Категория курсора
        using cursor_tag = make_finite_cursor_tag_t<typename Base::cursor_tag>;

        /// @brief Тип расстояния
        using typename Base::distance_type;

        // Создание, копирование
        /** @brief Конструктор
        @param in базовый курсор
        @param count число элементов, которое должно быть извлечено
        @pre @c in должен содержать по меньшей мере @c count элементов
        @post <tt> this->base() == seq </tt>
        @post <tt> this->count() == count </tt>
        @todo Добавить проверку, что @c count - конечное число
        */
        explicit take_cursor(Input in, Size count)
         : Base(std::move(in))
         , count_(std::move(count))
        {}

        // Однопроходый курсор
        /** @brief Проверка исчерпания последовательностей
        @return @b true, если последовательность исчерпана, иначе --- @b false.
        */
        bool operator!() const
        {
            return this->count() == 0 || !this->base();
        }

        /** @brief Переход к следующему элементу
        @pre <tt> !*this == false </tt>
        */
        void pop_front()
        {
            Base::pop_front();

            assert(this->count() > 0);
            -- ural::experimental::get(count_);
        }

        /** @brief Пройденная часть последовательности
        @return Пройденная часть последовательности
        */
        auto traversed_front() const
        {
            return this->base().traversed_front()
                   | ::ural::experimental::taken_exactly(count_.old_value() - count_.value());
        }

        /** @brief Исчерпание последовательности за константное время в прямом
        порядке
        @post <tt> !*this == true </tt>
        @pre <tt> *this </tt> должна быть конечной
        */
        void exhaust_front()
        {
            for(; !!*this; ++*this)
            {}
        }

        /** @brief Полная последовательность (включая пройденные части)
        @return Полная последовательность
        */
        take_cursor original() const;

        /** @brief Отбрасывание пройденной части последовательности
        @post <tt> !this->traversed_front() </tt>
        */
        void shrink_front()
        {
            Base::shrink_front();
            count_.commit();
        }

        // Курсор производного доступа
        /** @brief Пропуск заданного количества элементов в передней части
        последовательности
        @param n количество элементов, которое нужно пропустить.
        @pre <tt> n <= this->size() </tt>
        @return <tt> *this </tt>
        */
        take_cursor & operator+=(distance_type n)
        {
            assert(0 <= n && n <= this->count());

            Base::operator+=(n);
            ::ural::experimental::get(count_) -= n;

            return *this;
        }

        // Адаптор курсора
        /** @brief Оставшееся количество элементов
        @return Оставшееся количество элементов
        */
        Size const & count() const
        {
            return ::ural::experimental::get(count_);
        }

    private:
        Size const & init_count() const
        {
            return count_.old_value();
        }

    private:
        using Count_type = wrap_with_old_value_if_forward_t<cursor_tag, Size>;

        Count_type count_;
    };

    /** @brief Оператор "равно"
    @param x левый операнд
    @param y правый операнд
    @return <tt> x.count() == y.count() && x.base() == y.base() </tt>
    */
    template <class Cursor, class Size>
    bool operator==(take_cursor<Cursor, Size> const & x,
                    take_cursor<Cursor, Size> const & y)
    {
        return x.base() == y.base() && x.count() == y.count();
    }

    /// @brief Тип Функционального объекта для создания @c take_cursor
    // @todo Оптимизация для последовательностей произвольного доступа
    // @todo Оптимизация для последовательностей известного размера
    struct make_take_cursor_fn
    {
    public:
        /** @brief Создание @c take_cursor
        @param seq входная последовательность
        @param n количество элементов, которое нужно взять
        */
        template <class Sequence, class Size>
        take_cursor<cursor_type_t<Sequence>, Size>
        operator()(Sequence && seq, Size n) const
        {
            using Result = take_cursor<cursor_type_t<Sequence>, Size>;
            return Result(::ural::cursor_fwd<Sequence>(seq), std::move(n));
        }

        template <class Cursor, class Size1, class Size2>
        take_cursor<Cursor, CommonType<Size1, Size2>>
        operator()(take_cursor<Cursor, Size1> cur, Size2 n) const
        {
            using Size = CommonType<Size1, Size2>;
            using Result = take_cursor<Cursor, Size>;

            auto n_new = std::min(Size(cur.count()), Size(std::move(n)));

            return Result(std::move(cur).base(), std::move(n_new));
        }
    };

    namespace
    {
        /// @brief Функциональный объект для создания @c take_cursor
        constexpr auto const & taken
            = odr_const<experimental::pipeable_maker<make_take_cursor_fn>>;
    }
}
// namespace experimental
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_TAKEN_HPP_INCLUDED
