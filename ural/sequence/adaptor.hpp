#ifndef Z_URAL_SEQUENCE_ADAPTOR_HPP_INCLUDED
#define Z_URAL_SEQUENCE_ADAPTOR_HPP_INCLUDED

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

/** @file ural/sequence/adaptor.hpp
 @brief Универсальный адаптор последовательности
*/

#include <ural/sequence/base.hpp>
#include <ural/defs.hpp>

namespace ural
{
namespace experimental
{
    /** @brief Универсальный адаптор последовательности
    @tparam T класс, наследующий от данного sequence_adaptor
    @tparam Sequence адаптируемая последовательность
    @tparam Payload класс, от которого будет закрыто наследовать данный класс.
    Может использоваться для оптимизации пустых базовых классов.
    */
    template <class T, class Sequence, class Payload = empty_type>
    class sequence_adaptor
     : public sequence_base<T, Payload>
    {
        typedef sequence_base<T, Payload> Base;
    public:
        // Типы
        /// @brief Тип ссылки
        using reference = typename Sequence::reference;

        /// @brief Тип значения
        using value_type = ValueType<Sequence> ;

        /// @brief Тип расстояния
        using distance_type = DifferenceType<Sequence>;

        /// @brief Категория курсора
        using cursor_tag = typename Sequence::cursor_tag;

        /// @brief Тип указателя
        using pointer = typename Sequence::pointer;

        // Адаптор последовательности
        //@{
        /** @brief Базовая последовательность
        @return Базовая последовательность
        */
        Sequence const & base() const &
        {
            return this->seq_;
        }

        Sequence && base() &&
        {
            return std::move(this->seq_);
        }
        //@}

        /** @brief Дополнительная информация, хранящаяся в объекте
        @return Константная ссылка на дополнительную информацию
        */
        Payload const & payload() const
        {
            return Base::payload();
        }

        // Однопроходная последовательность
        /** @brief Проверка исчерпания последовательностей
        @return <tt> !this->base() </tt>
        */
        bool operator!() const
        {
            return !this->base();
        }

        /** @brief Текущий элемент последовательности
        @pre <tt> !*this == false </tt>
        @return Ссылка на текущий элемент последовательности
        */
        reference front() const
        {
            return this->base().front();
        }

        /** @brief Переход к следующему элементу
        @pre <tt> !*this == false </tt>
        */
        void pop_front()
        {
            return this->mutable_base().pop_front();
        }

        // Прямая последовательность
        /** @brief Полная последовательность (вместе с пройденными частями)
        @return Исходная последовательность
        */
        T original() const
        {
            return this->derived().rebind_base(this->base().original());
        }

        /** @brief Пройденная передняя часть последовательность
        @return Пройденная передняя часть последовательность
        */
        auto traversed_front() const
        {
            return this->derived().rebind_base(this->base().traversed_front());
        }

        /** @brief Отбрасывание пройденной части последовательности
        @post <tt> !this->traversed_front() </tt>
        */
        void shrink_front()
        {
            return this->mutable_base().shrink_front();
        }

        /** @brief Исчерпание последовательности в прямом порядке за константное
        время.
        @post <tt> !*this == true </tt>
        */
        void exhaust_front()
        {
            return ural::exhaust_front(this->mutable_base());
        }

        // Двусторонняя последовательность
        /** @brief Задний элемент последовательности
        @pre <tt> !*this == false </tt>
        */
        reference back() const
        {
            return this->base().back();
        }

        /** @brief Пройденная задняя часть последовательность
        @return Пройденная задняя часть последовательность
        */
        T traversed_back() const
        {
            return this->derived().rebind_base(this->base().traversed_back());
        }

        /// @brief Пропуск последнего элемента последовательности
        void pop_back()
        {
            return this->mutable_base().pop_back();
        }

        /// @brief Отбрасывает пройденную заднюю часть последовательности
        void shrink_back()
        {
            return this->mutable_base().shrink_back();
        }

        /** @brief Исчерпание последовательности в обратном порядке за
        константное время
        @post <tt> !*this == true </tt>
        */
        void exhaust_back()
        {
            return this->mutable_base().exhaust_back();
        }

        // Последовательность произвольного доступа
        /** @brief Количество элементов
        @return Количество непройденных элементов
        */
        distance_type size() const
        {
            return this->base().size();
        }

        /** @brief Индексированный доступ
        @param n индекс
        @pre <tt> 0 < this->size() && this->size() < n </tt>
        @return <tt> this->base()[n] </tt>
        */
        reference operator[](distance_type n) const
        {
            return this->base()[n];
        }

        /** @brief Продвижение на заданное число элементов в передней части
        последовательности
        @param n число элементов, которые будут пропущены
        @pre <tt> 0 <= n && n <= this->size() </tt>
        @return <tt> *this </tt>
        */
        T & operator+=(distance_type n)
        {
            this->mutable_base() += n;
            return static_cast<T&>(*this);
        }

        /** @brief Продвижение на заданное число элементов в задней части
        последовательности
        @param n число элементов, которые будут пропущены
        @pre <tt> 0 <= n && n <= this->size() </tt>
        */
        void pop_back(distance_type n)
        {
            return this->mutable_base().pop_back(n);
        }

    protected:
        /** @brief Конструктор
        @param base базовая последовательность
        @param args аргументы для создания объекта, хранящего дополнительную
        информацию
        @post <tt> this->base() == base </tt>
        @post <tt> this->payload() == Payload(std::forward<Args>(args)...) </tt>
        */
        template <class... Args>
        sequence_adaptor(Sequence base, Args && ... args)
         : Base(std::forward<Args>(args)...)
         , seq_(std::move(base))
        {}

        sequence_adaptor(sequence_adaptor const &)  = default;
        sequence_adaptor(sequence_adaptor &&) = default;

        sequence_adaptor & operator=(sequence_adaptor const &)  = default;
        sequence_adaptor & operator=(sequence_adaptor &&)  = default;

        ~sequence_adaptor() = default;

        Sequence & mutable_base()
        {
            return this->seq_;
        }

        Payload & payload()
        {
            return Base::payload();
        }

        T rebind_base(Sequence s) const
        {
            return T(std::move(s));
        }

    private:
        T const & derived() const
        {
            return static_cast<T const &>(*this);
        }

        T & derived();

    private:
        Sequence seq_;
    };

    /** @brief Оператор "равно"
    @param x, y операнды
    @return <tt> x.base() == y.base() && x.payload() == y.payload() </tt>
    */
    template <class T, class Sequence, class Payload>
    bool operator==(sequence_adaptor<T, Sequence, Payload> const & x,
                    sequence_adaptor<T, Sequence, Payload> const & y)
    {
        return x.base() == y.base()
               && x.payload() == y.payload();
    }
}
// namespace experimental
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_ADAPTOR_HPP_INCLUDED
