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

#include <utility>
#include <typeinfo>

namespace ural
{
    /** @brief Контейнер, который может хратить не более одного значения любого
    типа.
    @todo Поддержка распределителей памяти
    @todo Конструктор с размещением any(emplace_ctor<T>{}, args...)?
    @todo Ещё какая-либо функциональность контейнеров
    @todo Оптимальная структура виртуальных таблиц: что лучше съэкономить --
    память или один уровень косвенности? Можно ли реализовать так, чтобы можно
    было настраивать поддерживаемые операции, но при этом размер объекта не
    зависел от количества поддерживаемых значений?
    @todo Чем должен быть any: контейнером (хранит значение и даёт к нему доступ)
    или обёрткой (имитирует функциональность исходного объекта)?
    */
    class any
    {
    public:
        // Создание, копирование, уничтожение
        /** @brief Конструктор без параметров
        @post <tt> this->type() == typeid(void) </tt>
        @post <tt> this->empty() == true </tt>
        */
        any()
         : type_(nullptr)
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
         , ptr_(new T(std::forward<T>(x)))
         , destroy_(&any::destructor_impl<T>)
        {}

        /// @brief Деструктор
        ~any()
        {
            this->destroy_(ptr_);
        }

        // Свойства
        /** @brief Проверка того, что данный объект не хранит значение
        @return @b true, если данный объект не хранит значение, иначе --
        @b false
        */
        bool empty() const
        {
            return type_ == nullptr;
        }

        /** @brief Доступ к информации о типе хранимого объекта
        @return Если <tt> this->empty() == true </tt>, то
        <tt> typeid(void) </tt>, иначе -- информацию о типе, хранящемся в данном
        объекте.
        */
        std::type_info const & type() const
        {
            return type_ == nullptr ? typeid(void) : *type_;
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
        void(*destroy_)(void*);
    };
}
// namespace ural

#endif
// Z_URAL_ANY_HPP_INCLUDED
