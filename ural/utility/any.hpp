#ifndef Z_URAL_ANY_HPP_INCLUDED
#define Z_URAL_ANY_HPP_INCLUDED

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

/** @file ural/utility/any.hpp
 @brief Контейнер, который может хратить не более одного значения любого типа.
 Реализация шаблона проектирования "Хранитель"
*/

#include <ural/functional/make_callable.hpp>
#include <ural/utility.hpp>

#include <utility>
#include <typeinfo>

namespace ural
{
    /** @brief Контейнер, который может хратить не более одного значения любого
    типа.
    @todo Операции перемещения и swap (метод и свободная функция)
    @todo Операторы == и <
    @todo Операции копирования
    @todo Поддержка распределителей памяти
    @todo Конструктор с размещением any(emplace_ctor<T>{}, args...)?
    @todo Ещё какая-либо функциональность контейнеров
    @todo Оптимальная структура виртуальных таблиц: что лучше съэкономить --
    память или один уровень косвенности? Можно ли реализовать так, чтобы можно
    было настраивать поддерживаемые операции, но при этом размер объекта не
    зависел от количества поддерживаемых значений?
    @todo Разные методы доступа к данным: проверяемое разыменование (get),
    указатель на базовый класс - через механизм обработи исключений,
    value_or, непроверяемое разыменование, optional и expected
    @todo Оптимизация хранения указателей: как на объекты, так и на функциии,
    переменные-члены и функции-члены
    */
    class any
    {
    friend bool operator==(any const & x, any const & y);
    friend bool operator<(any const & x, any const & y);

    public:
        // Создание, копирование, уничтожение
        /** @brief Конструктор без параметров
        @post <tt> this->type() == typeid(void) </tt>
        @post <tt> this->empty() == true </tt>
        */
        any()
         : type_(&typeid(void))
         , ptr_(nullptr)
         , destroy_(&any::destructor_empty)
        {}

        /** @brief Конструктор
        @param x значение
        @post <tt> this->type() == typeid(x) </tt>
        @post <tt> this->empty() == false </tt>
        */
        template <class T>
        explicit any(T && x)
         : type_(&typeid(x))
         , ptr_(new typename std::decay<T>::type(std::forward<T>(x)))
         , destroy_(&any::destructor_impl<typename std::decay<T>::type>)
        {}

        any(any const & x);

        /** @brief Конструктор перемещения
        @param x объект, содержимое которого будет перемещенно в данный объект
        @post <tt> *this </tt> будет равен @c x до вызова конструктора
        @post <tt> x.empty() == true </tt>
        */
        any(any && x)
         : type_(std::move(x.type_))
         , ptr_(std::move(x.ptr_))
         , destroy_(std::move(x.destroy_))
        {
            x.type_= &typeid(void);
            x.ptr_ = nullptr;
            x.destroy_ = &any::destructor_empty;
        }

        any & operator=(any const & x);
        any & operator=(any && x);

        template <class T>
        any & operator=(T && x);

        /// @brief Деструктор
        ~any()
        {
            this->destroy_(ptr_);
        }

        void swap(any & x)
        {
            using std::swap;
            swap(this->type_, x.type_);
            swap(this->ptr_, x.ptr_);
            swap(this->destroy_, x.destroy_);
        }

        // Доступ к данным
        //@{
        /** @brief Доступ к указателю
        @tparam T тип объекта, к которому производится попытка доступа
        @return Если <tt> *this </tt> содержит объект типа @c T, то указатель
        на данный объект, иначе --- @b nullptr
        */
        template <class T>
        T * get_pointer()
        {
            return const_cast<T*>(ural::as_const(*this).get_pointer<T>());
        }

        template <class T>
        T const * get_pointer() const
        {
            typedef typename std::decay<T>::type DT;

            if(this->type() == typeid(DT))
            {
                return static_cast<T const *>(ptr_);
            }
            else
            {
                return nullptr;
            }
        }
        //@}

        // Свойства
        /** @brief Проверка того, что данный объект не хранит значение
        @return @b true, если данный объект не хранит значение, иначе --
        @b false
        */
        bool empty() const
        {
            assert(type_ != nullptr);
            return *type_ == typeid(void);
        }

        /** @brief Доступ к информации о типе хранимого объекта
        @return Если <tt> this->empty() == true </tt>, то
        <tt> typeid(void) </tt>, иначе -- информацию о типе, хранящемся в данном
        объекте.
        */
        std::type_info const & type() const
        {
            assert(type_ != nullptr);

            return *type_;
        }

    private:
        static void destructor_empty(void *)
        {}

        template <class T>
        static void destructor_impl(void * ptr)
        {
            delete static_cast<T*>(ptr);
        }

    private:
        std::type_info const * type_;
        void * ptr_;
        ural::function_ptr_wrapper<void(void*)> destroy_;
    };
}
// namespace ural

#endif
// Z_URAL_ANY_HPP_INCLUDED
