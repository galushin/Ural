#ifndef Z_URAL_SEQUENCE_TAKEN_EXACTLY_HPP_INCLUDED
#define Z_URAL_SEQUENCE_TAKEN_EXACTLY_HPP_INCLUDED

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

/** @file ural/sequence/adaptors/taken_exactly.hpp
 @brief Адаптер последовательности, извлекающий из базовой последовательности
 заданное число элементов.
*/

#include <ural/utility/pipeable.hpp>
#include <ural/sequence/make.hpp>
#include <ural/sequence/adaptor.hpp>

namespace ural
{
namespace experimental
{
    template <class Sequence, class Distance = DifferenceType<Sequence>>
    class taken_exactly_cursor;

    /**
    @brief Тип функционального объекта для создания @c taken_exactly_cursor
    */
    struct make_taken_exactly_cursor_fn
    {
    public:
        //@{
        /** @brief Создание @c taken_exactly_cursor
        @param seq входная последовательность
        @param n количество элементов, которое нужно взять
        */
        template <class Sequence, class Distance>
        taken_exactly_cursor<cursor_type_t<Sequence>, Distance>
        operator()(Sequence && seq, Distance n) const
        {
            using Result = taken_exactly_cursor<cursor_type_t<Sequence>, Distance>;
            return Result(::ural::cursor_fwd<Sequence>(seq), std::move(n));
        }

        template <class Sequence, class D1, class D2>
        taken_exactly_cursor<Sequence, CommonType<D1, D2>>
        operator()(taken_exactly_cursor<Sequence, D1> seq, D2 n) const
        {
            using Size = CommonType<D1, D2>;
            using Result = taken_exactly_cursor<Sequence, Size>;

            auto n_new = std::min(Size(seq.size()), Size(std::move(n)));

            return Result(std::move(seq).base(), std::move(n_new));
        }
        //@}
    };

    namespace
    {
        /// @brief Функциональный объект для создания @c taken_exactly_cursor
        constexpr auto const & make_taken_exactly_cursor
            = odr_const<make_taken_exactly_cursor_fn>;

        /** @brief Функциональный объект для создания @c taken_exactly_cursor
        в конвейерном стиле.
        */
        constexpr auto const & taken_exactly
            = odr_const<experimental::pipeable_maker<make_taken_exactly_cursor_fn>>;
    }

    /** @brief Адаптер курсора, извлекающий из базового курсора заданное число
    элементов.
    @tparam Input базовый курсор
    @tparam Distance тип количества элементов
    @todo Правильный алгоритм вычисление категории курсора
    @todo Оптимизация размера
    @todo оптимизация taken_(exactly)(n1) | taken_(exactly)(n2)
    */
    template <class Input, class Distance>
    class taken_exactly_cursor
     : public cursor_adaptor<taken_exactly_cursor<Input, Distance>, Input>
    {
        using Inherited = cursor_adaptor<taken_exactly_cursor, Input>;

    public:
        // Типы
        /// @brief Категория курсора
        using typename Inherited::cursor_tag;

        /// @brief Тип расстояния
        using distance_type = Distance;

        /// @brief Тип ссылки
        using typename Inherited::reference;

        // Конструирование
        /** @brief Конструктор
        @param base базовый курсор
        @param n количество элементов
        */
        taken_exactly_cursor(Input base, distance_type n)
         : Inherited(std::move(base))
         , size_(std::move(n))
         , traversed_front_size_(0)
         , traversed_back_size_(0)
        {}

        // Однопроходный курсор
        /** @brief Проверка исчерпания
        @return @b true, если курсор исчерпан, иначе -- @b false
        */
        bool operator!() const
        {
            return this->size() == distance_type(0);
        }

        /** @brief Переход к следующему элементу
        @pre <tt> !*this == false </tt>
        @pre <tt> !this->base() == false </tt>
        */
        void pop_front()
        {
            assert(!!*this);
            assert(!!this->base());

            Inherited::pop_front();
            ++ this->traversed_front_size_;
            -- this->size_;
        }

        // Прямой курсор
        /** @brief Исходный курсор (вместе с пройденными частями)
        @return Исходный курсор
        */
        taken_exactly_cursor original() const
        {
            return taken_exactly_cursor(this->base().original(),
                                          this->original_size());
        }

        /** @brief Пройденная передняя часть последовательности
        @return Пройденная передняя часть последовательности
        */
        auto traversed_front() const
        {
            return this->base().traversed_front()
                   | ::ural::experimental::taken_exactly(this->traversed_front_size_);
        }

        /** @brief Отбрасывание пройденной части последовательности
        @post <tt> !this->traversed_front() </tt>
        */
        void shrink_front()
        {
            this->traversed_front_size_ = distance_type(0);
            Inherited::shrink_front();
        }

        /** @brief Исчерпание последовательности в прямом порядке
        @note Неконстантное время для прямых
        */
        void exhaust_front()
        {
            ural::advance(*this, this->size());
        }

        // Двусторонний курсор
        /** @brief Задний элемент последовательности
        @pre <tt> !*this == false </tt>
        */
        reference back() const
        {
            assert(!!*this);
            assert(!!this->base());

            return (*this)[this->size() - 1];
        }

        /// @brief Пропуск последнего элемента последовательности
        void pop_back()
        {
            assert(!!*this);
            assert(!!this->base());

            ++ this->traversed_back_size_;
            -- this->size_;
        }

        /** @brief Пройденная задняя часть последовательность
        @return Пройденная задняя часть последовательность
        */
        taken_exactly_cursor traversed_back() const
        {
            auto new_base = this->base();
            new_base += this->size();
            new_base.shrink_front();

            return taken_exactly_cursor(std::move(new_base),
                                          this->traversed_back_size_);
        }

        /// @brief Отбрасывает пройденную заднюю часть последовательности
        void shrink_back()
        {
            this->traversed_back_size_ = distance_type(0);
        }

        /** @brief Исчерпание последовательности в обратном порядке за
        константное время
        @post <tt> !*this == true </tt>
        */
        void exhaust_back()
        {
            this->pop_back(this->size());
        }

        // Курсор произвольного доступа
        /** @brief Количество элементов
        @return Количество непройденных элементов
        */
        constexpr distance_type size() const
        {
            return this->size_;
        }

        /** @brief Индексированный доступ
        @param n индекс
        @pre <tt> 0 < this->size() && this->size() < n </tt>
        @return <tt> this->base()[n] </tt>
        */
        reference operator[](distance_type index) const
        {
            assert(0 <= index && index < this->size());
            return Inherited::operator[](std::move(index));
        }

        /** @brief Продвижение на заданное число элементов в передней части
        последовательности
        @param n число элементов, которые будут пропущены
        @pre <tt> 0 <= n && n <= this->size() </tt>
        @return <tt> *this </tt>
        */
        taken_exactly_cursor & operator+=(distance_type n)
        {
            assert(0 <= n && n <= this->size());

            this->traversed_front_size_ += n;
            this->size_ -= n;
            this->mutable_base() += n;
            return *this;
        }

        /** @brief Продвижение на заданное число элементов в задней части
        последовательности
        @param n число элементов, которые будут пропущены
        @pre <tt> 0 <= n && n <= this->size() </tt>
        */
        void pop_back(distance_type n)
        {
            assert(0 <= n && n <= this->size());
            this->traversed_back_size_ += n;
            this->size_ -= n;
        }

    private:
        distance_type original_size() const
        {
            return this->size_
                   + this->traversed_front_size_
                   + this->traversed_back_size_;
        }
    private:
        distance_type size_;
        distance_type traversed_front_size_;
        distance_type traversed_back_size_;
    };

    /** @brief Оператор "равно"
    @param x, y операнды
    @return <tt> x.front() == y.front() && x.size() == y.size() </tt>
    */
    template <class S1, class D1, class S2, class D2>
    bool operator==(taken_exactly_cursor<S1, D1> const & x,
                    taken_exactly_cursor<S2, D2> const & y)
    {
        return x.base() == y.base() && x.size() == y.size();
    }
}
// namespace experimental
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_TAKEN_EXACTLY_HPP_INCLUDED
