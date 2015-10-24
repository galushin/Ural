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

В С++11 контейнеры получили конструктор на основе списков инициализации.
Следующий логичный шаг: конструкторы на основе всего, к чему можно применить
begin и end
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
    /// @cond false
    namespace details
    {
        template <class Iterator>
        bool is_reachable(Iterator first, Iterator last, Iterator q)
        {
            if(q == last)
            {
                return true;
            }

            for(; first != last; ++ first)
            {
                if(first == q)
                {
                    return true;
                }
            }

            return false;
        }

        template <class Iterator>
        bool can_erase(Iterator first, Iterator last, Iterator q)
        {
            return q != last && is_reachable(first, last, q);
        }

        template <class Iterator>
        bool can_erase(Iterator first, Iterator last, Iterator q1, Iterator q2)
        {
            if(q1 == q2)
            {
                return true;
            }

            for(; first != last; ++ first)
            {
                if(first == q1)
                {
                    break;
                }
            }

            if(first == last)
            {
                return false;
            }

            for(; first != last; ++ first)
            {
                if(first == q2)
                {
                    break;
                }
            }

            return first == q2;
        }
    }
    // namespace details
    ///@endcond

    /** @brief Стратегия проверки для контейнеров и последовательностей
    с генерацией исключений в случае нарушений требований
    */
    class container_checking_throw_policy
    {
    protected:
        ~container_checking_throw_policy() = default;
        container_checking_throw_policy() = default;
    public:
        /** @brief Проверка допустимости индекса
        @param c проверяемый контейнер или последовательность
        @param index проверяемый индекс
        @throw out_of_range, если <tt> index >= seq.size() </tt>
        */
        template <class Container, class Size>
        static void
        check_index(Container const & c, Size index)
        {
            if(index < 0 || c.size() <= index)
            {
                // @todo Более подробная диагностика
                throw std::out_of_range("Invalid index!");
            }
        }

        template <class Container, class Size>
        static void
        check_step(Container const & c, Size step)
        {
            if(step < 0 || c.size() < step)
            {
                // @todo Более подробная диагностика
                throw std::out_of_range("Invalid step!");
            }
        }

        /** @brief Проверяет, что последовательность @c seq не пуста, в
        противном случае возбуждает исключение типа <tt> logic_error </tt>.
        @param c проверяемый контейнер или последовательность
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

        template <class ConstIterator>
        static void assert_can_erase(ConstIterator first, ConstIterator last,
                                     ConstIterator q)
        {
            if(!details::can_erase(first, last, q))
            {
                throw std::logic_error("Incorrect position to erase");
            }
        }

        template <class ConstIterator>
        static void assert_can_erase(ConstIterator first, ConstIterator last,
                                     ConstIterator q1, ConstIterator q2)
        {
            if(!details::can_erase(first, last, q1, q2))
            {
                throw std::logic_error("Incorrect range to erase");
            }
        }

        template <class ConstIterator>
        static void
        assert_can_insert_before(ConstIterator first, ConstIterator last,
                                 ConstIterator q)
        {
            if(!details::is_reachable(first, last, q))
            {
                throw std::logic_error("Unreachable poistion");
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
        container_checking_assert_policy() = default;
    public:
        /** @brief Проверка допустимости индекса
        @param c контейнер или последовательность
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
        @param c контейнер или последовательность
        */
        template <class Container>
        static void
        check_not_empty(Container const & c)
        {
            assert(c.empty() == false);
        }

        template <class ConstIterator>
        static void assert_can_erase(ConstIterator first, ConstIterator last,
                                     ConstIterator q)
        {
            assert(details::can_erase(first, last, q));
        }

        template <class ConstIterator>
        static void assert_can_erase(ConstIterator first, ConstIterator last,
                                     ConstIterator q1, ConstIterator q2)
        {
            assert(details::can_erase(first, last, q1, q2));
        }

        template <class ConstIterator>
        static void
        assert_can_insert_before(ConstIterator first, ConstIterator last,
                                 ConstIterator q)
        {
            assert(details::is_reachable(first, last, q));
        }
    };

    /** @brief Стратегия проверок контейнеров и последовательностей, не
    выполняющая никаких проверок
    */
    class container_no_checks_policy
    {
    protected:
        container_no_checks_policy() = default;
        ~container_no_checks_policy() = default;
    public:
        /** @brief Проверка допустимости индекса
        @param c проверяемый контейнер или последовательность
        @param index проверяемый индекс
        */
        template <class Container>
        static void
        check_index(Container const & c,
                    typename Container::difference_type index);

        /** @brief Проверяет, что последовательность @c seq не пуста.
        @param c проверяемый контейнер или последовательность
        */
        template <class Container>
        static void
        check_not_empty(Container const & c);

        template <class ConstIterator>
        static void assert_can_erase(ConstIterator first, ConstIterator last,
                                     ConstIterator q);

        template <class ConstIterator>
        static void assert_can_erase(ConstIterator first, ConstIterator last,
                                     ConstIterator q1, ConstIterator q2);

        template <class ConstIterator>
        static void
        assert_can_insert_before(ConstIterator first, ConstIterator last,
                                 ConstIterator q);
    };
}
// namespace ural

#endif
// Z_URAL_CONTAINER_POLICY_HPP_INCLUDED
