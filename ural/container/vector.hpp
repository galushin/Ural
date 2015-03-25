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
    /** @brief Минималистичный буфер
    @tparam T тип элементов
    @tparam Alloc тип распределителя памяти
    @todo Поддержка ссылок на распределители памяти в качестве шаблонного
    параметра
    */
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
            if(capacity > 0)
            {
                begin_ = Traits::allocate(*this, capacity);
            }
            else
            {
                begin_ = nullptr;
            }

            end_ = begin_;
            storage_end_ = begin_ + capacity;
        }

        buffer(buffer const & xs)
         : buffer(Traits::select_on_container_copy_construction(xs), xs.capacity())
        {
            // @todo заменить на алгоритм
            for(auto const & x : xs)
            {
                this->emplace_back(x);
            }

        }

        buffer(buffer && x);

        buffer & operator=(buffer const & x);
        buffer & operator=(buffer && x);

        ~buffer()
        {
            this->pop_back(this->size());

            Traits::deallocate(*this, this->begin(), this->capacity());
        }

        Alloc get_allocator() const
        {
            return Alloc(*this);
        }

        void swap(buffer & x)
        {
            // @todo swap с функцией проекцией swap_member
            std::swap(this->begin_, x.begin_);
            std::swap(this->end_, x.end_);
            std::swap(this->storage_end_, x.storage_end_);
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
        size_type size() const
        {
            return this->end() - this->begin();
        }
        size_type capacity() const
        {
            return this->storage_end_ - this->begin();
        }

        // Добавление элементов
        template <class... Args>
        void emplace_back(Args && ... args)
        {
            assert(end_ != storage_end_);

            Traits::construct(*this, end_, std::forward<Args>(args)...);
            ++ end_;
        }

        // Удаление элементов
        void pop_back(size_type n)
        {
            assert(n <= this->size());

            for(; n > 0; -- n)
            {
                -- end_;
                Traits::destroy(*this, end_);
            }
        }

    private:
        pointer begin_;
        pointer end_;
        pointer storage_end_;
    };

    /** @c vector --- это последовательный контейнер, который предоставляет
    операции с (амортизированной) постоянной сложностью для вставки и удаления
    в конце последовательности, вставка или удаление в середине требуют
    линейного времени. Управление хранением осуществляется автоматически, но
    можно дать подсказки, чтобы увеличить эффективность.
    @todo 23.3.6.1 пункт 2
    @brief Аналог <tt> std::vector </tt>
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
         : vector(allocator_type())
        {}

        explicit vector(allocator_type const & a) noexcept
         : data_(a)
        {}

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
                data_.emplace_back(value);
            }
        }

        /** @brief Конструктор на основе интервала, заданного парой итераторов
        @tparam InputIterator тип итератора
        @param first итератор, задающий начало интервала
        @param last итератор, задающий конец интервала
        @todo добавить требование из таблицы 100
        @post <tt> this->size() == std::distance(first, last) </tt>
        @todo Запретить, если @c InputIterator не удовлетворяет требованиям к
        входному итератору
        */
        template <class InputIterator>
        vector(InputIterator first, InputIterator last,
               allocator_type const & a = allocator_type())
         : vector(a)
        {
            static_assert(std::is_integral<InputIterator>::value == false, "");

            this->insert(this->cend(), first, last);
        }

        /** @brief Конструктор копий
        @param xs копируемый вектор
        @post <tt> *this == xs </tt>
        */
        vector(vector const & xs) = default;

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

        // 23.3.6.3 Размер и ёмкость
        size_type size() const
        {
            return std::distance(this->begin(), this->end());
        }

        size_type capacity() const
        {
            return data_.capacity();
        }

        bool empty() const
        {
            return this->begin() == this->end();
        }

        void reserve(size_type n)
        {
            if(n > this->capacity())
            {
                typedef std::allocator_traits<allocator_type> ATraits;
                typedef buffer<value_type, allocator_type> Buffer;

                // @note На самом деле, копирование распределителя на обязательно
                Buffer new_buffer(this->get_allocator(), n);

                for(auto & x : *this)
                {
                    new_buffer.emplace_back(std::move(x));
                }

                new_buffer.swap(data_);
            }
            else if(n > this->size())
            {
                // @todo Обработать этот случай
            }
        }

        // Доступ к элементам
        //@{
        /** @brief Доступ к элементам вектора без проверки индекса
        @param index индекс
        @return <tt> *(this->begin() + n) </tt>
        @pre <tt> index < this->size() </tt>
        */
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
        //@}

        //@{
        /** @brief Доступ к элементам вектора с проверкой индекса
        @param index индекс
        @return <tt> *(this->begin() + n) </tt>
        @throw std::out_of_range если <tt> index >= this->size() </tt>
        */
        reference at(size_type index)
        {
            vector const & c_self = *this;
            return const_cast<reference>(c_self.at(index));
        }

        const_reference at(size_type index) const
        {
            if(0U <= index && index < this->size())
            {
                return (*this)[index];
            }
            else
            {
                // @todo Более подробная диагностика
                throw std::out_of_range("ural::vector::at");
            }
        }
        //@}

        //@{
        /** @brief Первый элемент
        @return (Константная) ссылка на первый элемент контейнера
        @pre <tt> this->empty() == false </tt>
        */
        reference front()
        {
            vector const & c_self = *this;
            return const_cast<reference>(c_self.front());
        }

        const_reference front() const
        {
            // @todo Добавить проверку через стратегию
            return (*this)[0];
        }
        //@}

        //@{
        /** @brief Последний элемент
        @return (Константная) ссылка на последний элемент контейнера
        @pre <tt> this->empty() == false </tt>
        */
        reference back()
        {
            vector const & c_self = *this;
            return const_cast<reference>(c_self.back());
        }

        const_reference back() const
        {
            // @todo Добавить проверку через стратегию
            assert(!this->empty());
            return (*this)[this->size() - 1];
        }
        //@}

        // 23.3.6.4 Доступ к данным
        //@{
        /** @brief (Константный) указатель на начало выделенной области памяти
        @return (Константный) указатель на начало выделенной области памяти
        */
        value_type * data()
        {
            vector const & c_self = *this;
            return const_cast<value_type *>(c_self.data());
        }

        value_type const * data() const
        {
            return data_.begin();
        }
        //@}

        // 23.3.6.5 Модификаторы
        // Вставка элементов
        template <class... Args>
        void emplace_back(Args && ... args)
        {
            if(this->size() == this->capacity())
            {
                this->reserve(this->size() * 2 + 10);
            }

            data_.emplace_back(std::forward<Args>(args)...);
        }

        //@{
        void push_back(value_type const & x)
        {
            this->emplace_back(x);
        }

        void push_back(value_type && x)
        {
            this->emplace_back(std::move(x));
        }
        //@}

        /** @brief Уничтожает последний элемент
        @pre <tt> this->empty() == false </tt>
        */
        void pop_back()
        {
            // @todo Улучшить диагностику
            assert(!this->empty());

            data_.pop_back(1);
        }

        /** Вставляет объект типа @c T, сконструированный с параметрами
        <tt> std::forward<Args>(args)... </tt> перед @c position
        @param position константный итератор, определяющий позицию, перед
        которой должен быть вставлен новый элемент
        @param args аргументы конструктора для создания нового объекта
        @return итератор, ссылающийся на новый элемент.
        */
        template <class... Args>
        iterator emplace(const_iterator position, Args && ... args)
        {
            auto const index = position - this->begin();
            this->emplace_back(std::forward<Args>(args)...);

            std::rotate(this->begin() + index, this->end() - 1, this->end());

            return this->begin() + index;
        }

        iterator insert(const_iterator position, value_type const & x)
        {
            return this->emplace(position, std::move(x));
        }

        iterator insert(const_iterator position, value_type && x);

        /**
        @pre @c first и @c last не являются итераторами элементов контейнера
        <tt> *this </tt>
        */
        template <class InputIterator>
        iterator insert(const_iterator position,
                        InputIterator first, InputIterator last)
        {
            typedef typename std::iterator_traits<InputIterator>::iterator_category
                Category;
            return this->insert_impl(position - this->cbegin(),
                                     first, last, Category());
        }

        /** @brief Вставка элементов списка инициализаторов перед указанной
        позицией
        @param position константный итератор, перед которым будут вставлены
        новые элементы
        @param values список инициализаторов, элементы которого должны быть
        вставлены в контейнер
        @return Итератор, ссылающийся на первый вставленный элемент, или
        @c position, если таких элементов нет.
        @note Может быть стоит определить перегрузку, принимающую любой объект,
        для которого выражение
        <tt> this->insert(position, begin(value), end(values)) </tt>
        имеет смысл?
        */
        iterator insert(const_iterator position,
                        std::initializer_list<value_type> values)
        {
            using std::begin;
            using std::end;
            return this->insert(position, begin(values), end(values));
        }

    private:
        template <class InputIterator>
        iterator insert_impl(size_type index,
                             InputIterator first, InputIterator last,
                             std::input_iterator_tag)
        {
            auto const old_size = this->size();

            // @todo Заменить на алгоритмы ural?
            std::copy(first, last, *this | ural::back_inserter);

            std::rotate(this->begin() + index, this->begin() + old_size, this->end());

            return this->begin() + index;
        }

        template <class InputIterator>
        iterator insert_impl(size_type index,
                             InputIterator first, InputIterator last,
                             std::forward_iterator_tag)
        {
            auto const n = std::distance(first, last);

            this->reserve(this->size() + n);

            return this->insert_impl(index, first, last,
                                     std::input_iterator_tag());
        }

    private:
        buffer<value_type, allocator_type> data_;
    };
}
// namespace ural

#endif
// Z_URAL_CONTAINER_VECTOR_HPP_INCLUDED
