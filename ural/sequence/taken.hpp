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
     : public sequence_base<take_sequence<Sequence, Size>>
    {
    public:
        // Типы
        /// @brief Тип ссылки
        typedef typename Sequence::reference reference;

        /// @brief Тип значения
        typedef typename Sequence::value_type value_type;

        /// @brief Тип расстояния
        typedef typename Sequence::distance_type distance_type;

        /// @brief Категория обхода
        typedef typename Sequence::traversal_tag traversal_tag;

        /// @brief Тип указателя
        typedef typename Sequence::pointer pointer;

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
         : impl_(std::move(seq), Count_type{std::move(count)})
        {}

        // Однопроходная последовательность
        /** @brief Проверка исчерпания последовательностей
        @return @b true, если последовательность исчерпана, иначе --- @b false.
        */
        bool operator!() const
        {
            return this->count() == 0 || !this->base();
        }

        /** @brief Текущий элемент последовательности
        @pre <tt> !*this == false </tt>
        @return Ссылка на текущий элемент последовательности
        */
        reference front() const
        {
            assert(!!this->base());
            return *this->base();
        }

        /** @brief Переход к следующему элементу
        @pre <tt> !*this == false </tt>
        */
        void pop_front()
        {
            assert(this->count() > 0);
            assert(!!this->base());

            ++ impl_[ural::_1];
            -- ural::get(impl_[ural::_2]);
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

        void exhaust_front()
        {
            for(; !!*this; ++*this)
            {}
        }

        /// @brief Отбрасывание пройденной части последовательности
        void shrink_front()
        {
            impl_[ural::_1].shrink_front();
            impl_[ural::_2].commit();
        }

        // Последовательность производного доступа

        // Адаптор последовательности
        //@{
        /** @brief Базовая последовательность
        @return Базовая последовательность
        */
        Sequence const & base() const &
        {
            return impl_[ural::_1];
        }

        Sequence && base() &&
        {
            return std::move(impl_[ural::_1]);
        }
        //@}

        /** @brief Оставшееся количество элементов
        @return Оставшееся количество элементов
        @todo переименовать в size?
        */
        Size const & count() const
        {
            return ural::get(impl_[ural::_2]);
        }

    private:
        Size const & init_count() const
        {
            return impl_[ural::_2].old_value();
        }

    private:
        static auto constexpr is_forward
            = std::is_convertible<traversal_tag, forward_traversal_tag>::value;

        typedef typename std::conditional<is_forward, with_old_value<Size>, Size>::type
            Count_type;

        ural::tuple<Sequence, Count_type> impl_;
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
    template <class Sequence, class Size>
    auto operator|(Sequence && seq, taken_helper<Size> helper)
    -> take_sequence<decltype(::ural::sequence_fwd<Sequence>(seq)), Size>
    {
        typedef take_sequence<decltype(::ural::sequence_fwd<Sequence>(seq)), Size> Result;
        return Result(::ural::sequence_fwd<Sequence>(seq), helper.count);
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
