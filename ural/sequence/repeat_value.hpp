#ifndef Z_URAL_SEQUENCE_REPEAT_VALUE_HPP_INCLUDED
#define Z_URAL_SEQUENCE_REPEAT_VALUE_HPP_INCLUDED

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

/** @file ural/sequence/repeat_value.hpp
 @brief Последовательность, состаящая из одинаковых элементов.
*/

#include <ural/sequence/adaptors/taken_exactly.hpp>
#include <ural/functional.hpp>
#include <ural/sequence/base.hpp>
#include <ural/sequence/generator.hpp>

namespace ural
{
namespace experimental
{
    struct always_zero_int_type
    {
    public:
        constexpr explicit always_zero_int_type(int)
        {}

        always_zero_int_type &
        operator++()
        {
            return *this;
        }
    };

    /** @brief Курсор последовательности, состаящей из одинаковых элементов.
    @tparam T Тип элементов
    @tparam CursorTag категория курсора
    @tparam D Тип расстояния
    @todo Настройка структуры и интерфейса в зависимости от категории обхода
    Здесь мы встречаемся с ситуацией, когда для реализации операций
    прямой последовательности нужны данные, которые не требуются операциям
    последовательности произвольного доступа (количество пройденных элементов)
    */
    template <class T, class CursorTag = use_default, class D = use_default>
    class repeat_value_cursor
     : public cursor_base<repeat_value_cursor<T, CursorTag, D>>
    {
        /** @brief Оператор "равно"
        @param x, y аргументы
        @return @b true, если аргументы равны, иначе --- @b false
        */
        friend bool operator==(repeat_value_cursor const & x,
                               repeat_value_cursor const & y)
        {
            return x.data_ == y.data_;
        }

    public:
        // Типы
        /// @brief Тип значения
        typedef typename std::decay<T>::type value_type;

        /// @brief Тип ссылки
        typedef value_type const & reference;

        /// @brief Тип расстояния
        using distance_type = experimental::defaulted_type_t<D, std::intmax_t>;

        /// @brief Категория курсора
        using cursor_tag = experimental::defaulted_type_t<CursorTag, input_cursor_tag>;

        /// @brief Тип указателя
        typedef value_type const * pointer;

        // Создание, копирование, уничтожение
        /** @brief Конструктор
        @param args аргументы для создания значения
        @post <tt> this->front() == T(std::forward<Args>(args)...) </tt>
        */
        template <class... Args>
        explicit repeat_value_cursor(Args && ... args)
         : data_(value_type(std::forward<Args>(args)...), Distance(0))
        {}

        // Однопроходый курсор
        /** @brief Проверка исчерпания
        @return @b false
        */
        bool operator!() const
        {
            return false;
        }

        /** @brief Текущее значение
        @return Возвращает значение, заданное конструктором, присваиванием или
        обменом
        */
        reference front() const
        {
            return this->data_[ural::_1];
        }

        /// @brief Переход к следующему элементу
        void pop_front()
        {
            ++ data_[ural::_2];
        }

        // Прямой курсор - есть трудности с traversed_front
        /** @brief Полная последовательность (включая пройденные части)
        @return Полная последовательность
        */
        repeat_value_cursor original() const
        {
            return repeat_value_cursor(this->data_[ural::_1]);
        }

        /** @brief Передняя ройденная часть последовательности
        @return Передняя пройденная часть последовательности
        */
        ::ural::experimental::taken_exactly_cursor<repeat_value_cursor, distance_type>
        traversed_front() const
        {
            return this->original() | ::ural::experimental::taken_exactly(this->data_[ural::_2]);
        }

        /** @brief Отбрасывание передней пройденной части последовательности
        @post <tt> !this->traversed_front() </tt>
        */
        void shrink_front()
        {
            this->data_[ural::_2] = distance_type(0);
        }

        // Курсор произвольного доступа

    private:
        constexpr static auto const is_forward
            = std::is_convertible<cursor_tag, forward_cursor_tag>::value;

        using Distance = typename std::conditional<is_forward, distance_type,
                                                   always_zero_int_type>::type;

        using Data = tuple<value_type, Distance>;

        Data data_;
    };

    /** @brief Функция создания курсора последовательности одинаковых значений
    @param value значение. Если нужна передача по ссылке, нужно воспользоваться
    обёрткой <tt> std::cref </tt>
    @return <tt> repeat_value_cursor<T>(std::move(value)) </tt>
    */
    template <class T>
    repeat_value_cursor<T>
    make_repeat_value_cursor(T value)
    {
        return repeat_value_cursor<T>(std::move(value));
    }
}
// namespace experimental
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_REPEAT_VALUEHPP_INCLUDED
