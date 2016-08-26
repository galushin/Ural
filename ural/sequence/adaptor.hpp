#ifndef Z_URAL_cursor_adaptor_HPP_INCLUDED
#define Z_URAL_cursor_adaptor_HPP_INCLUDED

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
 @brief Универсальный адаптор курсора
*/

#include <ural/sequence/base.hpp>
#include <ural/defs.hpp>

namespace ural
{
namespace experimental
{
    /** @brief Универсальный адаптор курсора
    @tparam T класс, наследующий от данного cursor_adaptor
    @tparam Cursor тип адаптируемого курсора
    @tparam Payload класс, от которого будет закрыто наследовать данный класс.
    Может использоваться для оптимизации пустых базовых классов.
    */
    template <class T, class Cursor, class Payload = empty_type>
    class cursor_adaptor
     : public cursor_base<T, Payload>
    {
        typedef cursor_base<T, Payload> Base;
    public:
        // Типы
        /// @brief Тип ссылки
        using reference = typename Cursor::reference;

        /// @brief Тип значения
        using value_type = value_type_t<Cursor> ;

        /// @brief Тип расстояния
        using distance_type = difference_type_t<Cursor>;

        /// @brief Категория курсора
        using cursor_tag = typename Cursor::cursor_tag;

        /// @brief Тип указателя
        using pointer = typename Cursor::pointer;

        // Адаптор курсора
        //@{
        /** @brief Базовый курсор
        @return Базовый курсор
        */
        Cursor const & base() const &
        {
            return this->cur_;
        }

        Cursor && base() &&
        {
            return std::move(this->cur_);
        }
        //@}

        /** @brief Дополнительная информация, хранящаяся в объекте
        @return Константная ссылка на дополнительную информацию
        */
        Payload const & payload() const
        {
            return Base::payload();
        }

        // Однопроходный курсор
        /** @brief Проверка исчерпания
        @return <tt> !this->base() </tt>
        */
        bool operator!() const
        {
            return !this->base();
        }

        /** @brief Текущий элемент
        @pre <tt> !*this == false </tt>
        @return Ссылка на текущий элемент
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

        // Прямой курсор
        /** @brief Исходный курсор (вместе с пройденными частями)
        @return Исходный курсор
        */
        T original() const
        {
            return this->derived().rebind_base(this->base().original());
        }

        /** @brief Пройденная передняя часть курсора
        @return Пройденная передняя часть курсора
        */
        auto traversed_front() const
        {
            return this->derived().rebind_base(this->base().traversed_front());
        }

        /** @brief Отбрасывание пройденной части курсора
        @post <tt> !this->traversed_front() </tt>
        */
        void shrink_front()
        {
            return this->mutable_base().shrink_front();
        }

        /** @brief Исчерпание курсора в прямом порядке.
        @post <tt> !*this == true </tt>
        */
        void exhaust_front()
        {
            return ural::exhaust_front(this->mutable_base());
        }

        // Двусторонний курсор
        /** @brief Задний элемент курсора
        @pre <tt> !*this == false </tt>
        */
        reference back() const
        {
            return this->base().back();
        }

        /** @brief Пройденная задняя часть курсора
        @return Пройденная задняя часть курсора
        */
        T traversed_back() const
        {
            return this->derived().rebind_base(this->base().traversed_back());
        }

        /// @brief Пропуск последнего элемента курсора
        void pop_back()
        {
            return this->mutable_base().pop_back();
        }

        /// @brief Отбрасывает пройденную заднюю часть курсора
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

        // Курсор произвольного доступа
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
        курсора
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
        @param base базовый курсор
        @param args аргументы для создания объекта, хранящего дополнительную
        информацию
        @post <tt> this->base() == base </tt>
        @post <tt> this->payload() == Payload(std::forward<Args>(args)...) </tt>
        */
        template <class... Args>
        cursor_adaptor(Cursor base, Args && ... args)
         : Base(std::forward<Args>(args)...)
         , cur_(std::move(base))
        {}

        cursor_adaptor(cursor_adaptor const &)  = default;
        cursor_adaptor(cursor_adaptor &&) = default;

        cursor_adaptor & operator=(cursor_adaptor const &)  = default;
        cursor_adaptor & operator=(cursor_adaptor &&)  = default;

        ~cursor_adaptor() = default;

        Cursor & mutable_base()
        {
            return this->cur_;
        }

        Payload & payload()
        {
            return Base::payload();
        }

        T rebind_base(Cursor s) const
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
        Cursor cur_;
    };

    /** @brief Оператор "равно"
    @param x, y операнды
    @return <tt> x.base() == y.base() && x.payload() == y.payload() </tt>
    */
    template <class T, class Cursor, class Payload>
    bool operator==(cursor_adaptor<T, Cursor, Payload> const & x,
                    cursor_adaptor<T, Cursor, Payload> const & y)
    {
        return x.base() == y.base()
               && x.payload() == y.payload();
    }
}
// namespace experimental
}
// namespace ural

#endif
// Z_URAL_cursor_adaptor_HPP_INCLUDED
