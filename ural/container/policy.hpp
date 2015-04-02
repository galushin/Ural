#ifndef Z_URAL_CONTAINER_POLICY_HPP_INCLUDED
#define Z_URAL_CONTAINER_POLICY_HPP_INCLUDED

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

/** @file ural/container/policy.hpp
 @brief Стратегии проверок для контейнеров и интервалов
*/

/** @defgroup Containers Контейнеры
 @brief Контейнеры --- это объекты, хранящие другие объекты. Они управляют
 размещением и удалением этих объектов через конструкторы, деструкторы, операции
 @c insert и @c delete.
*/

/** @defgroup SequenceContainers Последовательные контейнеры
 @ingroup Containers
 @brief Последовательные контейнеры организуют конечное множество объектов,
 имеющих одинаковый тип, в строго линейно упорядоченную структуру.
*/

/** @defgroup AssociativeContainers Ассоциативные контейнеры
 @ingroup Containers
 @brief Ассоциативные контейнеры предоставляют быстрый доступ к данным по ключу.
*/

/** @defgroup UnorderedAssociativeContainers Неупорядоченные ассоциативные контейнеры
 @ingroup Containers
 @brief Предоставляют возможность быстрого доступа к данным по ключу. Сложность
 большинства операций в худшем случае --- линейная, но в среднем они выполняются
 гораздо быстрее.
*/

#include <ural/math.hpp>

namespace ural
{
    /** @brief Стратегия проверки для контейнеров и последовательностей
    с генерацией исключений в случае нарушений требований
    */
    class container_checking_throw_policy
    {
    protected:
        ~container_checking_throw_policy() = default;

    public:
        /** @brief Проверка допустимости индекса
        @param seq последовательность
        @param index проверяемый индекс
        @throw out_of_range, если <tt> index >= seq.size() </tt>
        */
        template <class Container, class Size>
        static void
        check_index(Container const & c, Size index)
        {
            if(index < 0 || c.size() <= ural::to_unsigned(index))
            {
                // @todo Более подробная диагностика
                throw std::out_of_range("Invalid index!");
            }
        }

        template <class Container, class Size>
        static void
        check_step(Container const & c, Size step)
        {
            if(step < 0 || c.size() < ural::to_unsigned(step))
            {
                // @todo Более подробная диагностика
                throw std::out_of_range("Invalid step!");
            }
        }

        /** @brief Проверяет, что последовательность @c seq не пуста, в
        противном случае возбуждает исключение типа <tt> logic_error </tt>.
        @param seq проверяемая последовательность
        @throw std::logic_error, если <tt> c.empty() == true </tt>
        */
        template <class Container>
        static void
        assert_not_empty(Container const & c)
        {
            if(c.empty() == true)
            {
                throw std::logic_error("Container must be not empty!");
            }
        }
    };

    /** @brief Стратегия проверки для контейнеров и последовательностей,
    основанная на использовании макроса @c assert
    */
    class container_checking_assert_policy
    {
    protected:
        ~container_checking_assert_policy() = default;

    public:
        /** @brief Проверка допустимости индекса
        @param seq последовательность
        @param index проверяемый индекс
        */
        template <class Container>
        static void
        check_index(Container const & c,
                    typename Container::difference_type index)
        {
            assert(0 <= index && ural::to_unsigned(index) < c.size());
        }

        /** @brief Проверяет, что последовательность @c seq не пуста.
        @param seq проверяемая последовательность
        */
        template <class Container>
        static void
        check_not_empty(Container const & c)
        {
            assert(c.empty() == false);
        }
    };

    /** @brief Стратегия проверок контейнеров и последовательностей, не
    выполняющая никаких проверок
    */
    class container_no_checks_policy
    {
    protected:
        ~container_no_checks_policy() = default;
    public:
    };
}
// namespace ural

#endif
// Z_URAL_CONTAINER_POLICY_HPP_INCLUDED
