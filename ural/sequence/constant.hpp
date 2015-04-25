#ifndef Z_URAL_SEQUENCE_CONSTANT_HPP_INCLUDED
#define Z_URAL_SEQUENCE_CONSTANT_HPP_INCLUDED

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

/** @file ural/sequence/constant.hpp
 @brief Последовательность, состаящая из одинаковых элементов.
*/

#include <ural/functional.hpp>
#include <ural/sequence/base.hpp>
#include <ural/sequence/generator.hpp>

namespace ural
{
    /** @brief Последовательность, состаящая из одинаковых элементов.
    @tparam T Тип элементов
    @tparam D Тип расстояния
    @todo Настройка структуры и интерфейса в зависимости от категории обхода
    Здесь мы встречаемся с ситуацией, когда для реализации операций
    прямой последовательности нужны данные, которые не требуются операциям
    последовательности произвольного доступа (количество пройденных элементов)
    */
    template <class T, class Traversal = use_default, class D = use_default>
    class constant_sequence
     : public sequence_base<constant_sequence<T>>
    {
    public:
        // Типы
        /// @brief Тип значения
        typedef typename std::decay<T>::type value_type;

        /// @brief Тип ссылки
        typedef value_type const & reference;

        /// @brief Тип расстояния
        typedef typename default_helper<D, std::intmax_t>::type distance_type;

        /// @brief Категория обхода
        typedef single_pass_traversal_tag traversal_tag;

        /// @brief Тип указателя
        typedef value_type const * pointer;

        // Создание, копирование, уничтожение
        /** @brief Конструктор
        @param args аргументы для создания значения
        @post <tt> this->front() == T(std::forward<Args>(args)...) </tt>
        */
        template <class... Args>
        explicit constant_sequence(Args && ... args)
         : value_(std::forward<Args>(args)...)
        {}

        // Однопроходная последовательность
        /** @brief Проверка исчерпания последовательности
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
            return this->value_;
        }

        /// @brief Переход к следующему элементу
        void pop_front()
        {}

        // Прямая последовательность - есть трудности с traversed_front

        // Последовательность произвольного доступа

    private:
        value_type value_;
    };

    /** @brief Функция создания последовательности одинаковых значений
    @param value значение. Если нужна передача по ссылке, нужно воспользоваться
    обёрткой <tt> std::cref </tt>
    @return <tt> constant_sequence<T>(std::move(value)) </tt>
    */
    template <class T>
    constant_sequence<T>
    make_constant_sequence(T value)
    {
        return constant_sequence<T>(std::move(value));
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_CONSTANT_HPP_INCLUDED
