#ifndef Z_URAL_CONTAINER_VECTOR_HPP_INCLUDED
#define Z_URAL_CONTAINER_VECTOR_HPP_INCLUDED

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

/** @file ural/container/vector.hpp
 @brief Аналог <tt> std::vector </tt>
*/

#include <ural/algorithm.hpp>
#include <ural/defs.hpp>

#include <cassert>
#include <memory>

namespace ural
{
    template <class T, class Alloc>
    class buffer
     : private Alloc
    {
        typedef std::allocator_traits<Alloc> Traits;
    public:
        // Типы
        typedef typename Traits::pointer pointer;
        typedef typename Traits::size_type size_type;

        // Создание, копирование, уничтожение
        buffer(Alloc const & a, size_type capacity = 0)
         : Alloc(a)
        {
            begin_ = Traits::allocate(*this, capacity);
            end_ = begin_;
            storage_end_ = begin_ + capacity;
        }

        buffer(buffer const & x) = delete;
        buffer(buffer && x);

        ~buffer()
        {
            for(auto i = this->begin(); i != this->end(); ++ i)
            {
                Traits::destroy(*this, i);
            }

            Traits::deallocate(*this, this->begin(), this->capacity());
        }

        Alloc get_allocator() const
        {
            return Alloc(*this);
        }

        // Итераторы
        pointer begin() const
        {
            return this->begin_;
        }

        pointer end() const
        {
            return this->end_;
        }

        // Размер и ёмкость
        size_type capacity() const
        {
            return this->storage_end_ - this->begin();
        }

        // Добавление элементов
        void push_back(T const & value)
        {
            assert(end_ != storage_end_);

            Traits::construct(*this, end_, value);
            ++ end_;
        }

        void push_back(T && value);

    private:
        pointer begin_;
        pointer end_;
        pointer storage_end_;
    };

    /** @brief Аналог <tt> std::vector </tt>
    @tparam T тип элементов
    @tparam Alloc тип распределителя памяти
    @tparam Policy тип стратегии (в основном, отвечает за обработку ошибок)
    */
    template <class T, class Alloc = use_default, class Policy = use_default>
    class vector
    {
    friend bool operator==(vector const & x, vector const & y)
    {
        return ural::equal(x, y);
    }

    public:
        // Типы
        /// @brief Тип значений
        typedef T value_type;

        /// @brief Тип распределителя памяти
        typedef typename default_helper<Alloc, std::allocator<T>>::type
            allocator_type;

        /// @brief Тип ссылки
        typedef value_type & reference;

        /// @brief Тип константной ссылки
        typedef value_type const & const_reference;

        /// @brief Тип итератора
        typedef value_type * iterator;

        /// @brief Тип константного итератора
        typedef value_type const * const_iterator;

        /// @brief Тип обратного итератора
        typedef std::reverse_iterator<iterator> reverse_iterator;

        /// @brief Тип константного обратного итератора
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

        /// @brief Тип разности (итераторов)
        typedef typename std::allocator_traits<allocator_type>::difference_type
            difference_type;

        /// @brief Тип размера
        typedef typename std::allocator_traits<allocator_type>::size_type
            size_type;

        // Конструкторы
        /** @brief Создание пустого контейнера
        @post <tt> this->empty() </tt>
        */
        vector() noexcept
         : vector(allocator_type{})
        {}

        explicit vector(allocator_type const & a) noexcept
         : data_(a)
        {}

        /** @brief Конструктор копий
        @param xs копируемый вектор
        @post <tt> *this == xs </tt>
        */
        vector(vector const & xs)
         : data_(xs.get_allocator(), xs.size())
        {
            for(auto const & x : xs)
            {
                data_.push_back(x);
            }
        }

        /** @brief Создание контейнера заданного размера, каждый элемент
        которого равен заданному значению
        @param n количество элементов
        @param value значение
        @post <tt> this->size() == n </tt>
        @post Для любого @c i из интервала <tt> [0; this->size()) </tt>
        выполняется <tt> (*this)[i] == value </tt>
        */
        vector(size_type const n, value_type const & value,
               allocator_type const & a = allocator_type{})
         : data_(a, n)
        {
            for(auto k = n; k > 0; -- k)
            {
                data_.push_back(value);
            }
        }

        template <class InputIterator>
        vector(InputIterator first, InputIterator last)
         : data_(allocator_type{}, size_type(last - first))
        {
            // @todo Оптимизация и обобщение
            for(; first != last; ++ first)
            {
                data_.push_back(*first);
            }
        }

        /** @brief Конструктор на основе списка инициализаторов
        @param values список инициализаторов
        @post Эквивалентно <tt> vector(values.begin(), value.end()) </tt>
        */
        vector(std::initializer_list<value_type> values)
         : vector(values.begin(), values.end())
        {}

        allocator_type get_allocator() const
        {
            return data_.get_allocator();
        }

        // Итераторы
        //@{
        iterator begin()
        {
            return iterator(data_.begin());
        }

        const_iterator begin() const
        {
            return const_iterator(data_.begin());
        }

        const_iterator cbegin() const
        {
            return this->begin();
        }
        //@}

        //@{
        iterator end()
        {
            return iterator(data_.end());
        }

        const_iterator end() const
        {
            return const_iterator(data_.end());
        }

        const_iterator cend() const
        {
            return this->end();
        }
        //@}

        //@{
        reverse_iterator rbegin()
        {
            return reverse_iterator(this->end());
        }

        const_reverse_iterator rbegin() const
        {
            return const_reverse_iterator(this->end());
        }

        const_reverse_iterator crbegin() const
        {
            return this->rbegin();
        }
        //@}

        //@{
        reverse_iterator rend()
        {
            return reverse_iterator(this->begin());
        }

        const_reverse_iterator rend() const
        {
            return const_reverse_iterator(this->begin());
        }

        const_reverse_iterator crend() const
        {
            return this->rend();
        }
        //@}

        // Размер и ёмкость
        size_type size() const
        {
            return std::distance(this->begin(), this->end());
        }

        bool empty() const
        {
            return this->begin() == this->end();
        }

        // Доступ к элементам
        reference operator[](size_type index)
        {
            vector const & c_self = *this;
            return const_cast<reference>(c_self[index]);
        }

        const_reference operator[](size_type index) const
        {
            // @todo Настройка способа проверки
            assert(0U <= index && index < this->size());
            return *(this->begin() + index);
        }

        // Вставка элементов

    private:
        buffer<value_type, allocator_type> data_;
    };
}
// namespace ural

#endif
// Z_URAL_CONTAINER_VECTOR_HPP_INCLUDED
