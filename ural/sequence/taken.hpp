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
 @brief Адаптер последовательности, ограничивающий базовую последовательность
 заданным числом элементов
 @todo По умолчанию для Size использовать DifferenceType<Size>
*/

#include <ural/sequence/adaptor.hpp>
#include <ural/sequence/make.hpp>
#include <ural/sequence/base.hpp>
#include <ural/utility.hpp>

namespace ural
{
    /** @brief Адаптор последовательности, ограничивающий длину базовой
    последовательности
    @tparam Sequence тип последовательности
    @tparam Size тип количества элементов, которые должны быть взяты из базовой
    последовательности
    @todo take_sequence не может быть двусторонней, уточнить traversal_tag
    @todo Для последовательностей произвольного доступа можно оптимизировать:
    узнать точный размер в конструкторе, а следовательно делать меньше проверок
    в operator!, быстрее выполнять exhaust_front
    */
    template <class Sequence, class Size = DifferenceType<Sequence>>
    class take_sequence
     : public sequence_adaptor<take_sequence<Sequence, Size>, Sequence>
    {
        using Base = sequence_adaptor<take_sequence<Sequence, Size>, Sequence>;
    public:
        // Типы
        /// @brief Категория обхода
        using typename Base::traversal_tag;

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
            -- ural::get(count_);
        }

        // Прямая последовательность
        /** @brief Пройденная часть последовательности
        @return Пройденная часть последовательности
        */
        take_sequence traversed_front() const
        {
            return take_sequence(this->base().traversed_front(),
                                 this->init_count() - this->count());
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
        // Адаптор последовательности
        /** @brief Оставшееся количество элементов
        @return Оставшееся количество элементов
        */
        Size const & count() const
        {
            return ural::get(count_);
        }

    private:
        Size const & init_count() const
        {
            return count_.old_value();
        }

    private:
        static auto constexpr is_forward
            = std::is_convertible<traversal_tag, forward_traversal_tag>::value;

        typedef typename std::conditional<is_forward, with_old_value<Size>, Size>::type
            Count_type;

        Count_type count_;
    };

    /** @brief Оператор "равно"
    @param x левый операнд
    @param y правый операнд
    @return <tt> x.count() == y.count() && x.base() == y.base() </tt>
    */
    template <class Sequence, class Size>
    bool operator==(take_sequence<Sequence, Size> const & x,
                    take_sequence<Sequence, Size> const & y);

    /** @brief Тип вспомогательного объекта для создания @c take_sequence
    @tparam Size тип для представления размера
    */
    template <class Size>
    struct taken_helper
    {
        /// @brief Количество элементов, которым необходимо ограничиться
        Size count;
    };

    /** @brief Создание @c take_sequence в конвейерном стиле
    @param seq входная последовательность
    @param helper объект, хранящий количество элементов
    */
    template <class Sequenced, class Size>
    take_sequence<SequenceType<Sequenced>, Size>
    operator|(Sequenced && seq, taken_helper<Size> helper)
    {
        typedef take_sequence<SequenceType<Sequenced>, Size> Result;
        return Result(::ural::sequence_fwd<Sequenced>(seq), helper.count);
    }

    /** @brief Функция создания @c take_helper
    @param n количество элементов
    @return <tt> taken_helper<Size>{n} </tt>
    */
    template <class Size>
    taken_helper<Size> taken(Size n)
    {
        return {n};
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_TAKEN_HPP_INCLUDED
