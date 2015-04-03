#ifndef Z_URAL_CONTAINER_FLAT_SET_HPP_INCLUDED
#define Z_URAL_CONTAINER_FLAT_SET_HPP_INCLUDED

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

/** @file ural/container/flat_map.hpp
 @brief Аналог <tt> std::set </tt>, хранящий элементы в виде массива, а не
 дерева.
 @todo Дополнить возможностями, связанными с "векторной реализацией"
 @todo Проверить, что при пустой функции сравнения размер объекта не
 увеличивается
*/

#include <ural/container/vector.hpp>
#include <ural/defs.hpp>

namespace ural
{
    /** @brief Упорядоченное множество, аналог <tt> std::map </tt>
    @tparam Key тип ключа
    @tparam Compare тип функции сравнения
    @tparam Allocator тип распределителя памяти
    @tparam Policy стратегия проверок и реакции на ошибки
    @todo Проверить значения по умолчанию
    */
    template <class Key,
              class Compare = use_default,
              class Allocator = use_default,
              class Policy = use_default>
    class flat_set
    {
        typedef ural::vector<Key, Allocator, Policy> Data;

    public:
        // Типы
        /// @brief Тип ключа
        typedef Key key_type;

        /// @brief Тип элементов
        typedef Key value_type;

        /// @brief Функция сравнения ключей
        typedef typename default_helper<Compare, std::less<value_type>>::type
            key_compare;

        /// @brief Функция сравнения элементов
        typedef key_compare value_compare;

        /// @brief Тип распределителя памяти
        typedef typename Data::allocator_type allocator_type;

        /// @brief Тип ссылки
        typedef value_type & reference;

        /// @brief Тип ссылки на константу
        typedef value_type const & const_reference;

        /// @brief Тип итератора
        typedef typename Data::iterator iterator;

        /// @brief Тип константного итератора
        typedef typename Data::const_iterator const_iterator;

        /// @brief Тип разности
        typedef typename Data::difference_type difference_type;

        /// @brief Тип размера
        typedef typename Data::size_type size_type;

        /// @brief Тип стратегии
        typedef typename Data::policy_type policy_type;


        // Конструирование, копирование, уничтожение
        explicit flat_set(value_compare const & cmp = value_compare(),
                          allocator_type const & a = allocator_type())
         : cmp_(cmp)
         , data_(a)
        {}

        template <class InputIterator>
        flat_set(InputIterator first, InputIterator last,
                 value_compare const & cmp = value_compare(),
                 allocator_type const & a = allocator_type())
         : flat_set(cmp, a)
        {
            this->insert(std::move(first), std::move(last));
        }

        /** @brief Конструктор на основе списка инициализаторов
        @param values список значений элементов
        @param cmp функция сравнения
        @param a распределитель памяти
        @post То же, что для конструктора
        <tt> flat_set(values.begin(), values.end(), cmp, a) </tt>
        */
        flat_set(std::initializer_list<value_type> values,
                 value_compare const & cmp = value_compare(),
                 allocator_type const & a = allocator_type())
         : flat_set(values.begin(), values.end(), cmp, a)
        {}

        // Итераторы
        //@{
        iterator begin() noexcept
        {
            return this->data_.begin();
        }

        const_iterator begin() const noexcept
        {
            return this->data_.begin();
        }
        //@}

        //@{
        iterator end() noexcept
        {
            return this->data_.end();
        }

        const_iterator end() const noexcept
        {
            return this->data_.end();
        }
        //@}

        // Размер и ёмкость
        /** @brief Проверка того, что контейнер является пустым
        @return <tt> this->empty() </tt>
        */
        bool empty() const noexcept
        {
            return data_.empty();
        }

        // Модификаторы
        std::pair<iterator, bool>
        insert(value_type const & x)
        {
            auto pos = this->lower_bound(x);

            if(pos != this->end() && !cmp_(x, *pos))
            {
                return std::make_pair(pos, false);
            }
            else
            {
                pos = data_.insert(pos, x);
                return std::make_pair(pos, true);
            }
        }

        std::pair<iterator, bool> insert(value_type &&);

        template <class InputIterator>
        void insert(InputIterator first, InputIterator last)
        {
            // @todo Оптимизация
            for(; first != last; ++ first)
            {
                this->insert(*first);
            }
        }

        // Операции над множествами
        iterator lower_bound(key_type const & x)
        {
            return std::lower_bound(this->begin(), this->end(), x, this->cmp_);
        }

        const_iterator lower_bound(key_type const & x) const;

    private:
        value_compare cmp_;
        Data data_;
    };
}
// namespace ural

#endif
// Z_URAL_CONTAINER_FLAT_SET_HPP_INCLUDED
