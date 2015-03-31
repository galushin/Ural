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
    class buffer;

    struct buffer_swapper
    {
        template <class T, class A>
        void operator()(buffer<T, A> & x, buffer<T, A&> & y)
        {
            assert(std::addressof(x.allocator_ref()) == std::addressof(y.allocator_ref()));

            using std::swap;
            static_assert(std::tuple_size<decltype(x.members_)>::value == 4, "");
            static_assert(std::tuple_size<decltype(y.members_)>::value == 4, "");

            swap(x.members_[ural::_1], y.members_[ural::_1]);
            swap(x.members_[ural::_2], y.members_[ural::_2]);
            swap(x.members_[ural::_3], y.members_[ural::_3]);
            swap(x.members_[ural::_4], y.members_[ural::_4]);
        }
    };

    /** @brief Минималистичный буфер
    @tparam T тип элементов
    @tparam Alloc тип распределителя памяти
    */
    template <class T, class Alloc>
    class buffer
    {
        typedef std::allocator_traits<typename std::remove_reference<Alloc>::type> Traits;
        friend class buffer_swapper;

    public:
        // Типы
        typedef Alloc allocator_type;
        typedef typename Traits::value_type value_type;
        typedef typename Traits::pointer pointer;
        typedef typename Traits::size_type size_type;

        // Создание, копирование, уничтожение
        /**
        @todo Можно ли избавиться от лишних инициализаций или убедиться, что
        оптимизатор их устраняет
        */
        buffer(Alloc const & a, size_type capacity = 0)
         : members_(a, nullptr, nullptr, nullptr)
        {
            if(capacity > 0)
            {
                this->begin_ref() = Traits::allocate(this->allocator_ref(), capacity);
            }
            else
            {
                this->begin_ref() = nullptr;
            }

            this->end_ref() = this->begin();
            this->storage_end_ref() = this->begin() + capacity;
        }

        buffer(buffer const & xs)
         : buffer(Traits::select_on_container_copy_construction(xs.allocator_ref()), xs.capacity())
        {
            ural::copy(xs, *this | ural::back_inserter);
        }

        buffer(buffer && x)
         : members_(std::move(x.members_))
        {
            // @todo обёртка для встроенных указателей, реализующая правильные
            // операции перемещения?
            x.begin_ref() = nullptr;
            x.end_ref() = nullptr;
            x.storage_end_ref() = nullptr;
        }

        buffer & operator=(buffer const & x);

        /**
        @todo Добавить noexcept
        @todo Использовать Traits::is_always_equal
        */
        buffer & operator=(buffer && x)
        {
            static_assert(Traits::propagate_on_container_move_assignment::value, "");

            this->allocator_ref() = x.allocator_ref();

            using std::swap;
            swap(this->begin_ref(), x.begin_ref());
            swap(this->end_ref(), x.end_ref());
            swap(this->storage_end_ref(), x.storage_end_ref());

            return *this;
        }

        ~buffer()
        {
            this->pop_back(this->size());

            Traits::deallocate(this->allocator_ref(), this->begin(), this->capacity());
        }

        Alloc get_allocator() const
        {
            return Alloc(*this);
        }

        void swap(buffer & x);

        // Итераторы
        pointer begin() const
        {
            return this->members_[ural::_2];
        }

        pointer end() const
        {
            return this->members_[ural::_3];
        }

        // Размер и ёмкость
        size_type size() const
        {
            return this->end() - this->begin();
        }

        size_type capacity() const
        {
            return this->members_[ural::_4]- this->begin();
        }

        void reserve(size_type n)
        {
            // Согласно 23.6.3.3 абзац 3, уменьшение ёмкости произойти не может
            if(n <= this->capacity())
            {
                return;
            }

            buffer<value_type, allocator_type &>
                new_buffer(this->allocator_ref(), n);

            // @todo заменить на алгоритм
            for(auto & x : *this)
            {
                new_buffer.emplace_back(std::move(x));
            }

            buffer_swapper{}(*this, new_buffer);
        }

        // Добавление элементов
        void push_back(value_type const & x)
        {
            this->emplace_back(x);
        }

        void push_back(value_type && x);

        template <class... Args>
        void emplace_back(Args && ... args)
        {
            assert(this->end_ref() != this->storage_end_ref());

            Traits::construct(this->allocator_ref(), this->end(),
                              std::forward<Args>(args)...);
            ++ this->end_ref();
        }

        // Удаление элементов
        void pop_back(size_type n)
        {
            assert(n <= this->size());

            for(; n > 0; -- n)
            {
                -- this->end_ref();
                Traits::destroy(this->allocator_ref(), this->end());
            }
        }

    private:
        pointer & begin_ref()
        {
            return members_[ural::_2];
        }

        pointer & end_ref()
        {
            return members_[ural::_3];
        }

        pointer & storage_end_ref()
        {
            return members_[ural::_4];
        }

        allocator_type & allocator_ref()
        {
            return members_[ural::_1];
        }

        allocator_type const & allocator_ref() const
        {
            return members_[ural::_1];
        }

    private:
        tuple<allocator_type, pointer, pointer, pointer> members_;
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

    friend bool operator<(vector const & x, vector const & y)
    {
        return ural::lexicographical_compare(x, y);
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

        /// @brief Тип размера
        typedef typename std::allocator_traits<allocator_type>::size_type
            size_type;

        /// @brief Тип разности (итераторов)
        typedef typename std::allocator_traits<allocator_type>::difference_type
            difference_type;

        typedef typename std::allocator_traits<allocator_type>::pointer pointer;
        typedef typename std::allocator_traits<allocator_type>::const_pointer
            const_pointer;

        /// @brief Тип обратного итератора
        typedef std::reverse_iterator<iterator> reverse_iterator;

        /// @brief Тип константного обратного итератора
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

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

        explicit vector(size_type n, allocator_type const & a = allocator_type());

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
            // @todo Заменить на алгоритм
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
               typename disable_if<std::is_integral<InputIterator>::value, allocator_type const &>::type a = allocator_type())
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

        /** @brief Конструктор перемещения
        @param x вектор, содержимое которого должно быть перемещено
        @post <tt> *this </tt> будет иметь то же значение, что было у @c x
        перед выполнением конструктора
        */
        vector(vector && x) noexcept
         : data_(std::move(x.data_))
        {}

        vector(vector const & xs, allocator_type const & a);
        vector(vector && x, allocator_type const & a) noexcept;

        /** @brief Конструктор на основе списка инициализаторов
        @param values список инициализаторов
        @post Эквивалентно <tt> vector(values.begin(), value.end()) </tt>
        @todo Тест с распределителем памяти
        */
        vector(std::initializer_list<value_type> values,
               allocator_type const & a = allocator_type())
         : vector(values.begin(), values.end(), a)
        {}

        /// @brief Деструктор
        ~vector() = default;

        /** @brief Оператор копирующего присваивания
        @param xs присваеваемое значение
        @post <tt> *this == xs </tt>
        */
        vector & operator=(vector const & xs) = default;

        /** @brief Оператор присваивания с перемещением
        @param x объект, содержимое которого будет перемещено в данный объект
        @post <tt> *this </tt> будет равен значению, которое @c x имел до
        вызова оператора
        @todo Добавить в noexcept: || std::allocator_traits<allocator_type>::is_always_equal::value
        */
        vector & operator=(vector && x)
            noexcept(std::allocator_traits<allocator_type>::propagate_on_container_move_assignment::value)
        {
            data_ = std::move(x.data_);
            return *this;
        }

        /** @brief Присваивание значений из списка инициализаторов
        @param values список инициализаторов
        @post <tt> *this == vector(values) </tt>
        @return <tt> *this </tt>
        */
        vector & operator=(std::initializer_list<value_type> values)
        {
            this->assign(values);
            return *this;
        }

        /** @brief Заменяет элементы контейнера на копии элементов интервала
        <tt> [first; last) </tt>
        @param first итератор, задающий начало интервала
        @param last итератор, задающий конец интервала
        @pre @c first и @c last не должны быть итераторами <tt> *this </tt>
        @post <tt> *this == vector(first, last) </tt>
        */
        template <class InputIterator>
        void assign(InputIterator first, InputIterator const last)
        {
            // @todo Проверки

            auto sink = this->begin();
            auto const stop = this->end();

            // @todo Выделить алгоритм
            for(; sink != stop && first != last; ++sink, ++ first)
            {
                *sink = *first;
            }

            if(first == last)
            {
                this->erase(sink, stop);
            }
            else
            {
                this->insert(stop, first, last);
            }
        }

        void assign(size_type n, value_type const & value);

        /** @brief Присваивание значений из списка инициализаторов
        @param values список инициализаторов
        @post <tt> *this == vector(values) </tt>
        */
        void assign(std::initializer_list<value_type> values)
        {
            return this->assign(values.begin(), values.end());
        }

        /** @brief Используемый распределитель памяти
        @return Копия используемого данным контейнером распределителя памяти
        */
        allocator_type get_allocator() const
        {
            return data_.get_allocator();
        }

        // Итераторы
        //@{
        /** @brief Итератор начала контейнера
        @return Итератор, ссылающийся на первый элемент контейнера
        */
        iterator begin() noexcept
        {
            return iterator(data_.begin());
        }

        const_iterator begin() const noexcept
        {
            return const_iterator(data_.begin());
        }

        const_iterator cbegin() const noexcept
        {
            return this->begin();
        }
        //@}

        //@{
        /** @brief Итератор конца контейнера
        @return Итератор, указывающий на область памяти сразу за последним
        элементом контейнера
        */
        iterator end() noexcept
        {
            return iterator(data_.end());
        }

        const_iterator end() const noexcept
        {
            return const_iterator(data_.end());
        }

        const_iterator cend() const noexcept
        {
            return this->end();
        }
        //@}

        //@{
        /** @brief Начальный итератор обратного обхода контейнера
        @return <tt> reverse_iterator(this->end()) </tt>
        */
        reverse_iterator rbegin() noexcept
        {
            return reverse_iterator(this->end());
        }

        const_reverse_iterator rbegin() const noexcept
        {
            return const_reverse_iterator(this->end());
        }

        const_reverse_iterator crbegin() const noexcept
        {
            return this->rbegin();
        }
        //@}

        //@{
        /** @brief Конечный итератор обратного обхода контейнера
        @return <tt> reverse_iterator(this->begin()) </tt>
        */
        reverse_iterator rend() noexcept
        {
            return reverse_iterator(this->begin());
        }

        const_reverse_iterator rend() const noexcept
        {
            return const_reverse_iterator(this->begin());
        }

        const_reverse_iterator crend() const noexcept
        {
            return this->rend();
        }
        //@}

        // 23.3.6.3 Размер и ёмкость
        /** @brief Размер, то есть количество элементов, находящихся в контейнере
        @return <tt> std::distance(this->begin(), this->end()) </tt>
        */
        size_type size() const noexcept
        {
            return std::distance(this->begin(), this->end());
        }

        size_type max_size() const noexcept;

        void resize(size_type new_size)
        {
            if(new_size < this->size())
            {
                this->erase(this->cbegin() + new_size, this->cend());
            }
            else
            {
                this->reserve(new_size);

                // @todo Выделить алгоритм
                for(auto dn = new_size - this->size(); dn > 0; -- dn)
                {
                    this->emplace_back();
                }
            }
        }

        void resize(size_type new_size, value_type const & c)
        {
            if(new_size < this->size())
            {
                this->erase(this->cbegin() + new_size, this->cend());
            }
            else
            {
                this->reserve(new_size);

                // @todo Выделить алгоритм
                for(auto dn = new_size - this->size(); dn > 0; -- dn)
                {
                    this->push_back(c);
                }
            }
        }

        /** @brief Ёмкость контейнера
        @return Ёмкость контейнера, то есть количество элементов которое он
        может вместить без перераспределения памяти
        */
        size_type capacity() const noexcept
        {
            return data_.capacity();
        }

        bool empty() const noexcept
        {
            return this->begin() == this->end();
        }

        void reserve(size_type n)
        {
            data_.reserve(n);
        }

        void shrink_to_fit();

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
        value_type * data() noexcept
        {
            vector const & c_self = *this;
            return const_cast<value_type *>(c_self.data());
        }

        value_type const * data() const noexcept
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
        @brief Размещение нового элемента
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

        /** @brief Вставляет копию @c x перед @c position
        @param position константный итератор, определяющий позицию, перед
        которой должен быть вставлен новый элемент
        @param args аргументы конструктора для создания нового объекта
        @return итератор, ссылающийся на новый элемент.
        */
        iterator insert(const_iterator position, value_type const & x)
        {
            return this->emplace(position, x);
        }

        /** @brief Вставляет @c position новый элемент, созданный из @c x с
        помощью конструктора перемещения
        @param position константный итератор, определяющий позицию, перед
        которой должен быть вставлен новый элемент
        @param args аргументы конструктора для создания нового объекта
        @return итератор, ссылающийся на новый элемент.
        */
        iterator insert(const_iterator position, value_type && x)
        {
            return this->emplace(position, std::move(x));
        }

        iterator insert(const_iterator position, size_type n, value_type const & value);

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

        iterator erase(const_iterator position)
        {
            // @todo Проверка через политику
            return this->erase(position, position+1);
        }

        iterator erase(const_iterator first, const_iterator last)
        {
            // @todo Проверка через политику

            // 1. Переходим к неконстантным итераторам
            auto const result = this->begin() + (first - this->cbegin());
            auto sink = result;
            auto source = this->begin() + (last - this->cbegin());
            auto const stop = this->cend();

            // 2. Перемещаем последние элементы на места удаляемых
            // @todo заменить на алгоритм
            for(; source != stop; ++source)
            {
                *sink = std::move_if_noexcept(*source);
                ++ sink;
            }

            // 3. Удаляем последние элементы
            data_.pop_back(this->end() - sink);

            return result;
        }

        void swap(vector & x)
            noexcept(std::allocator_traits<allocator_type>::propagate_on_container_swap::value
                     || std::allocator_traits<allocator_type>::is_always_equal::value);

        /** @brief Уничтожение всех элементов контейнера
        @post <tt> this->empty() </tt>
        @post Делает недействительными все ссылки, указатели и итераторы,
        ссылающиеся на элементы контейнера, может также сделать недействительным
        итератор конца контейнера
        */
        void clear() noexcept
        {
            this->erase(this->cbegin(), this->cend());
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
