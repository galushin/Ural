#ifndef Z_URAL_DISJOINT_SET_HPP_INCLUDED
#define Z_URAL_DISJOINT_SET_HPP_INCLUDED

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

#include <ural/numeric/numbers_sequence.hpp>
#include <ural/tuple.hpp>

namespace ural
{
    /** @brief Тип для представления непересекающихся множеств
    @todo Проверка и обработка ошибок
    @todo Оптимизация (сжатие путей, сбалансированность дерева)
    @todo Тесты сложности алгоритмов
    @todo Выделить алгоритмы
    @tparam IntType Тип элементов
    */
    template <class IntType>
    class disjoint_set
    {
        static_assert(std::is_integral<IntType>::value, "Must be integeral");

        typedef tuple<IntType, IntType> Element;
        typedef std::vector<Element> Container;

    public:
        // Типы
        /// @brief Тип для представления размера
        typedef typename Container::size_type size_type;

        /// @brief Тип значений
        typedef IntType value_type;

        // Создание
        /** @brief Конструктор
        @param n количество элементов
        @post <tt> this->size() == n </tt>
        @post Каждый элемент представляет собой отдельное множесвто
        */
        explicit disjoint_set(size_type n)
        {
            data_.reserve(n);

            for(auto i : ural::numbers(0, n))
            {
                data_.emplace_back(i, IntType{0});
            }
        }

        // Свойства
        /** @brief Родительский элемента
        @param p элемент
        @return Родительский элемент элемента @c p
        */
        value_type parent(value_type p) const
        {
            return data_[p][ural::_1];
        }

        /** @brief Корневой элемент множества
        @param p элемент
        @return Корень множества, в которое входит элемент @c p
        */
        value_type root(value_type p) const
        {
            while(p != this->parent(p))
            {
                p = this->parent(p);
            }

            return p;
        }

        /** @brief Проверяет, принадлежат ли два элемента одному множеству
        @param p первый элемент
        @param q второй элемент
        @return @b true, если @c p и @c q принадлежат одному множеству,
        иначе --- @b false
        */
        bool is_united(value_type p, value_type q) const
        {
            return this->root(p) == this->root(q);
        }

        // Размер
        /** @brief Количество элементов
        @return Количество элементов
        */
        size_type size() const
        {
            return data_.size();
        }

        // Объединение
        /** @brief Объединение множеств, в которые входят элементы @c p и @c q
        @post <tt> this->is_united(p, q) </tt>
        @param p первый элемент
        @param q второй элемент
        @post <tt> this->is_united(p, q) == true </tt>
        */
        void unite(value_type const & p, value_type const & q)
        {
            this->root_ref(p) = this->root(q);
        }

    private:
        value_type & root_ref(value_type p)
        {
            /* Преобразование из константной ссылки в неконстантную безопасно,
            так как мы знаем, что исходно данный объект является не константым
            */
            auto const & cr = this->root(p);
            return const_cast<value_type &>(cr);
        }

    private:
        Container data_;
    };
}
// namespace ural

#endif
// Z_URAL_DISJOINT_SET_HPP_INCLUDED
