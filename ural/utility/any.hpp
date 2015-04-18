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
    class any_base
    {
    public:
        // Создание, копирование, уничтожение
        any_base()
         : ptr_(nullptr)
         , destroy_(&any_base::destructor_empty)
        {}

        template <class Copy>
        any_base(any_base const & x, Copy copy_fn)
         : ptr_(copy_fn(x.ptr_))
         , destroy_(x.destroy_)
        {}

        template <class T>
        explicit any_base(T && x)
         : ptr_(new typename std::decay<T>::type(std::forward<T>(x)))
         , destroy_(&any_base::destructor_impl<typename std::decay<T>::type>)
        {}

        any_base(any_base && x)
         : ptr_(std::move(x.ptr_))
         , destroy_(std::move(x.destroy_))
        {
            x.ptr_ = nullptr;
            x.destroy_ = &any_base::destructor_empty;
        }

        any_base(any_base &) = delete;
        any_base(any_base const &) = delete;

        ~any_base()
        {
            this->destroy_(ptr_);
        }

        void swap(any_base & x)
        {
            using std::swap;
            swap(this->ptr_, x.ptr_);
            swap(this->destroy_, x.destroy_);
        }

        // Доступ к данным
        void * data() const
        {
            return ptr_;
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
        void * ptr_;
        ural::function_ptr_wrapper<void(void*)> destroy_;
    };

    /** @brief Контейнер, который может хратить не более одного значения любого
    типа.
    @todo Операции копирования
    @todo Реализовать typeid и копирование за счёт концепций
    @todo Операторы == и <
    @todo Поддержка распределителей памяти
    @todo Конструктор с размещением any(emplace_ctor<T>{}, args...)?
    @todo Ещё какая-либо функциональность контейнеров
    @todo Оптимальная структура виртуальных таблиц: что лучше сэкономить --
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

    friend void swap(any & x, any & y)
    {
        return x.swap(y);
    }

    public:
        // Создание, копирование, уничтожение
        /** @brief Конструктор без параметров
        @post <tt> this->type() == typeid(void) </tt>
        @post <tt> this->empty() == true </tt>
        */
        any()
         : data_()
         , type_(&typeid(void))
         , copy_(&any::copy_empty)
        {}

        /** @brief Конструктор
        @param x значение
        @post <tt> this->type() == typeid(x) </tt>
        @post <tt> this->empty() == false </tt>
        */
        template <class T>
        explicit any(T && x)
         : data_(std::forward<T>(x))
         , type_(&typeid(x))
         , copy_(&any::copy_impl<typename std::decay<T>::type>)
        {}

        //@{
        any(any & x)
         : any(static_cast<any const &>(x))
        {}

        any(any const & x)
         : data_(x.data_, x.copy_)
         , type_(x.type_)
         , copy_(x.copy_)
        {}
        //@}

        /** @brief Конструктор перемещения
        @param x объект, содержимое которого будет перемещенно в данный объект
        @post <tt> *this </tt> будет равен @c x до вызова конструктора
        @post <tt> x.empty() == true </tt>
        */
        any(any && x)
         : data_(std::move(x.data_))
         , type_(std::move(x.type_))
         , copy_(std::move(x.copy_))
        {
            x.type_= &typeid(void);
            x.copy_ = &any::copy_empty;
        }

        //@{
        any & operator=(any & x)
        {
            return *this = ural::as_const(x);
        }

        any & operator=(any const & x)
        {
            any(x).swap(*this);
            return *this;
        }
        //@}

        /** @brief Оператор присваивания с перемещением
        @param x присваиваемый объект
        @return <tt> *this </tt>
        @post <tt> *this </tt> равен значению, которое @c x имел до вызова
        оператора.
        */
        any & operator=(any && x)
        {
            this->swap(x);
            return *this;
        }

        template <class T>
        any & operator=(T && x);

        /// @brief Деструктор
        ~any() = default;

        /** @brief Обмен содержимого данного объекта и @c x
        @param x объект, с которым производится обмен
        @post Содержимое <tt> *this </tt> и @c x меняется местами
        */
        void swap(any & x)
        {
            using std::swap;
            data_.swap(x.data_);
            swap(this->type_, x.type_);
            swap(this->copy_, x.copy_);
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
                return static_cast<T const *>(data_.data());
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
            return data_.data() == nullptr;
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
        static void * copy_empty(void *)
        {
            return nullptr;
        }

        template <class T>
        static void * copy_impl(void * ptr)
        {
            return new T(*static_cast<T*>(ptr));
        }

    private:
        any_base data_;
        std::type_info const * type_;
        ural::function_ptr_wrapper<void*(void*)> copy_;
    };
}
// namespace ural

#endif
// Z_URAL_ANY_HPP_INCLUDED
