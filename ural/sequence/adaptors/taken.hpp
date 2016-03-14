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
    /** @brief Адаптор последовательности, ограничивающий длину базовой
    последовательности
    @tparam Sequence тип последовательности
    @tparam Size тип количества элементов, которые должны быть взяты из базовой
    последовательности
    @todo take_sequence может быть двусторонней только если исходная
    последовательность имеет произвольный доступ, уточнить traversal_tag.
    @todo Для последовательностей произвольного доступа можно оптимизировать:
    узнать точный размер в конструкторе, а следовательно делать меньше проверок
    в operator!, быстрее выполнять exhaust_front.
    */
    template <class Sequence, class Size = DifferenceType<Sequence>>
    class take_sequence
     : public sequence_adaptor<take_sequence<Sequence, Size>, Sequence>
    {
        using Base = sequence_adaptor<take_sequence<Sequence, Size>, Sequence>;
    public:
        // Типы
        /// @brief Категория курсора
        using cursor_tag = make_finite_cursor_tag_t<typename Base::cursor_tag>;

        /// @brief Тип расстояния
        using typename Base::distance_type;

        // Создание, копирование
        /** @brief Конструктор
        @param seq исходная последовательность
        @param count число элементов, которое должно быть извлечено
        @pre @c seq должна содержать по меньшей мере @c count элементов
        @post <tt> this->base() == seq </tt>
        @post <tt> this->count() == count </tt>
        @todo Добавить проверку, что @c count - конечное число
        */
        explicit take_sequence(Sequence seq, Size count)
         : Base(std::move(seq))
         , count_(std::move(count))
        {}

        // Однопроходная последовательность
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
        take_sequence original() const;

        /** @brief Отбрасывание пройденной части последовательности
        @post <tt> !this->traversed_front() </tt>
        */
        void shrink_front()
        {
            Base::shrink_front();
            count_.commit();
        }

        // Последовательность производного доступа
        /** @brief Пропуск заданного количества элементов в передней части
        последовательности
        @param n количество элементов, которое нужно пропустить.
        @pre <tt> n <= this->size() </tt>
        @return <tt> *this </tt>
        */
        take_sequence & operator+=(distance_type n)
        {
            assert(0 <= n && n <= this->count());

            Base::operator+=(n);
            ::ural::experimental::get(count_) -= n;

            return *this;
        }

        // Адаптор последовательности
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
    template <class Sequence, class Size>
    bool operator==(take_sequence<Sequence, Size> const & x,
                    take_sequence<Sequence, Size> const & y)
    {
        return x.base() == y.base() && x.count() == y.count();
    }

    /// @brief Тип Функционального объекта для создания @c take_sequence
    // @todo Оптимизация для последовательностей произвольного доступа
    // @todo Оптимизация для последовательностей известного размера
    struct make_take_sequence_fn
    {
    public:
        //@{
        /** @brief Создание @c take_sequence
        @param seq входная последовательность
        @param n количество элементов, которое нужно взять
        */
        template <class Sequenced, class Size>
        take_sequence<SequenceType<Sequenced>, Size>
        operator()(Sequenced && seq, Size n) const
        {
            using Result = take_sequence<SequenceType<Sequenced>, Size>;
            return Result(::ural::sequence_fwd<Sequenced>(seq), std::move(n));
        }

        template <class Sequence, class Size1, class Size2>
        take_sequence<Sequence, CommonType<Size1, Size2>>
        operator()(take_sequence<Sequence, Size1> seq, Size2 n) const
        {
            using Size = CommonType<Size1, Size2>;
            using Result = take_sequence<Sequence, Size>;

            auto n_new = std::min(Size(seq.count()), Size(std::move(n)));

            return Result(std::move(seq).base(), std::move(n_new));
        }
        //@}
    };

    namespace
    {
        /// @brief Функциональный объект для создания @c take_sequence
        constexpr auto const & taken
            = odr_const<experimental::pipeable_maker<make_take_sequence_fn>>;
    }
}
// namespace experimental
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_TAKEN_HPP_INCLUDED
