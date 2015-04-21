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

#include <ural/container/container_facade.hpp>
#include <ural/memory.hpp>
#include <ural/sequence/iterator_sequence.hpp>
#include <ural/container/policy.hpp>
#include <ural/sequence/taken.hpp>
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

            static_assert(std::tuple_size<decltype(x.members_)>::value == 4, "");
            static_assert(std::tuple_size<decltype(y.members_)>::value == 4, "");

            using std::swap;

            // Первый обменивать не нужно, так как это объект и ссылка на него
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
            x.begin_ref() = nullptr;
            x.end_ref() = nullptr;
            x.storage_end_ref() = nullptr;
        }

        buffer(buffer && x, allocator_type const & a)
         : buffer(a, 0)
        {
            if(x.allocator_ref() == this->allocator_ref())
            {
                this->unsafe_swap_pointers(x);
            }
            else
            {
                this->reserve(x.size());
                ural::move(x, *this | ural::back_inserter);
            }
        }

        buffer & operator=(buffer const & x)
        {
            // Если нельзя передать владение, то придётся освобождать память
            if(this->allocator_ref() != x.allocator_ref())
            {
                this->destroy_impl();
            }

            if(Traits::propagate_on_container_copy_assignment::value == true)
            {
                this->allocator_ref() = x.allocator_ref();
            }

            this->reserve(x.size());

            // Сначала присваиваем уже созданным элементам
            auto r = ural::copy(x, *this);

            // Обрабатываем лишние или недостающие элементы
            if(x.size() < this->size())
            {
                this->pop_back(this->size() - x.size());
            }
            else
            {
                ural::copy(r[ural::_1], *this | ural::back_inserter);
            }

            return *this;
        }

        buffer & operator=(buffer && x) noexcept
        {
            // @todo Устранить дублирование с копирующим присваиванием
            static_assert(Traits::propagate_on_container_move_assignment::value
                          || ural::allocator_is_always_equal<allocator_type>::value, "");

            // Если нельзя передать владение, то придётся освобождать память
            // А если можно, то просто обмениваем указатели
            if(this->allocator_ref() != x.allocator_ref())
            {
                this->destroy_impl();
            }

            if(Traits::propagate_on_container_move_assignment::value)
            {
                this->allocator_ref() = std::move(x.allocator_ref());
            }

            if(this->allocator_ref() == x.allocator_ref())
            {
                this->unsafe_swap_pointers(x);
                return *this;
            }

            this->reserve(x.size());

            // Сначала присваиваем уже созданным элементам
            auto r = ural::move(x, *this);

            // Обрабатываем лишние или недостающие элементы
            if(x.size() < this->size())
            {
                this->pop_back(this->size() - x.size());
            }
            else
            {
                ural::move(r[ural::_1], *this | ural::back_inserter);
            }


            return *this;
        }

        ~buffer()
        {
            this->destroy_impl();
        }

        Alloc get_allocator() const
        {
            return this->allocator_ref();
        }

        void swap(buffer & x)
        {
            ural::swap_allocators{}(this->allocator_ref(), x.allocator_ref());
            this->unsafe_swap_pointers(x);
        }

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

        size_type max_size() const
        {
            return Traits::max_size(this->allocator_ref());
        }

        void reserve(size_type n)
        {
            // Согласно 23.6.3.3 абзац 3, уменьшение ёмкости произойти не может
            if(n <= this->capacity())
            {
                return;
            }

            this->reserve_aux(n);
        }

        void shrink_to_fit()
        {
            this->reserve_aux(this->size());
        }

        // Добавление элементов
        void push_back(value_type const & x)
        {
            this->emplace_back(x);
        }

        void push_back(value_type && x)
        {
            this->emplace_back(std::move(x));
        }

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
        void destroy_impl() noexcept
        {
            this->pop_back(this->size());

            if(this->capacity() > 0)
            {
                Traits::deallocate(this->allocator_ref(),
                                   this->begin(), this->capacity());
            }

            this->begin_ref() = nullptr;
            this->end_ref() = nullptr;
            this->storage_end_ref() = nullptr;
        }

        void reserve_aux(size_type n)
        {
            assert(n >= this->size());

            buffer<value_type, allocator_type &>
                new_buffer(this->allocator_ref(), n);

            ural::move_if_noexcept(*this, new_buffer | ural::back_inserter);

            buffer_swapper{}(*this, new_buffer);
        }

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

        void unsafe_swap_pointers(buffer & x)
        {
            using std::swap;
            swap(this->begin_ref(), x.begin_ref());
            swap(this->end_ref(), x.end_ref());
            swap(this->storage_end_ref(), x.storage_end_ref());
        }

    private:
        tuple<allocator_type, pointer, pointer, pointer> members_;
    };

    /** @c vector --- это последовательный контейнер, который предоставляет
    операции с (амортизированной) постоянной сложностью для вставки и удаления
    в конце последовательности, вставка или удаление в середине требуют
    линейного времени. Управление хранением осуществляется автоматически, но
    можно дать подсказки, чтобы увеличить эффективность.
    @ingroup SequenceContainers
    @brief Аналог <tt> std::vector </tt>
    @tparam T тип элементов
    @tparam Alloc тип распределителя памяти
    @tparam Policy тип стратегии (в основном, отвечает за обработку ошибок)
    */
    template <class T, class Alloc = use_default, class Policy = use_default>
    class vector
     : ural::container_facade<vector<T, Alloc, Policy>>
    {
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

        /// @brief Тип указателя
        typedef typename std::allocator_traits<allocator_type>::pointer pointer;

        /// @brief Тип указателя на константу
        typedef typename std::allocator_traits<allocator_type>::const_pointer
            const_pointer;

        /// @brief Тип обратного итератора
        typedef std::reverse_iterator<iterator> reverse_iterator;

        /// @brief Тип константного обратного итератора
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

        /// @brief Класс-стратегия проверок
        typedef typename default_helper<Policy, container_checking_assert_policy>::type
            policy_type;

        // Конструкторы
        /** @brief Создание пустого контейнера
        @post <tt> this->empty() </tt>
        @post <tt> this->get_allocator() == allocator_type{} </tt>
        */
        vector() noexcept
         : vector(allocator_type())
        {}

        /** @brief Создание пустого контейнера с заданным распределителем
        памяти
        @post <tt> this->empty() </tt>
        @post <tt> this->get_allocator() == a </tt>
        */
        explicit vector(allocator_type const & a) noexcept
         : data_(a)
        {}

        /** @brief Создаёт вектор, содержащий @c n элементов, созданных
        конструктором по умолчанию, с помощью распределителя памяти @c a
        @param n количество элементов
        @param a распределитель памяти
        @pre @c value_type должен быть @c DefaultInsertable для <tt> *this </tt>
        @post <tt> this->size() == n </tt>
        @post <tt> this->get_allocator() == a </tt>
        */
        explicit vector(size_type n, allocator_type const & a = allocator_type())
         : data_(a, n)
        {
            this->resize(n);
        }

        /** @brief Создание контейнера заданного размера, каждый элемент
        которого равен заданному значению
        @param n количество элементов
        @param value значение
        @pre @c T должно быть @c CopyConstructible для @c vector
        @post <tt> this->size() == n </tt>
        @post Для любого @c i из интервала <tt> [0; this->size()) </tt>
        выполняется <tt> (*this)[i] == value </tt>
        @post <tt> this->get_allocator() == a </tt>
        */
        vector(size_type const n, value_type const & value,
               allocator_type const & a = allocator_type{})
         : data_(a, n)
        {
            this->insert(this->end(), n, value);
        }

        /** @brief Конструктор на основе интервала, заданного парой итераторов
        @tparam InputIterator тип итератора
        @param first итератор, задающий начало интервала
        @param last итератор, задающий конец интервала
        @pre <tt> [first; last) </tt> должен быть действительным интервалом
        @pre @c T должен быть @c EmplaceConstructible для @c vector из
        <tt> *i </tt>.
        @pre Если @c InputIterator не удовлетворяет требованиям к прямым
        итераторам, то @c T должен быть @c MoveInsertable для @c vector.
        @post <tt> this->size() == std::distance(first, last) </tt>
        @post <tt> std::equal(this->begin(), this->end(), first) </tt>
        @post <tt> this->get_allocator() == a </tt>
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
        @pre @c T должно быть @c CopyInsertable для @c vector
        @post <tt> *this == xs </tt>
        @post <tt> this->get_allocator() </tt> равен
        <tt> AT::select_on_container_copy_construction(xs) </tt>, где
        @c AT --- это <tt> std::allocator_traits<allocator_type> </tt>
        */
        vector(vector const & xs) = default;

        /** @brief Конструктор перемещения
        @param x вектор, содержимое которого должно быть перемещено
        @pre Конструктор перемещения типа @c allocator_type не должен возбуждать
        исключений
        @post <tt> *this </tt> будет иметь то же значение, что было у @c x
        перед выполнением конструктора
        @post <tt> this->get_allocator() </tt> равен распределителю памяти,
        который был у @c x до выполнения конструктора.
        */
        vector(vector && x) noexcept
         : data_(std::move(x.data_))
        {}

        /** @brief Создание копии вектора с другим распределителем памяти
        @param xs копируемый контейнер
        @param a распределитель памяти
        @post <tt> *this == xs </tt>
        @post <tt> this->get_allocator() == a </tt>
        */
        vector(vector const & xs, allocator_type const & a)
         : data_(a, xs.size())
        {
            this->assign(xs.begin(), xs.end());
        }

        /** @brief Контруктор перемещения с другим распределителем памяти
        @param x контейнер, содрежимое которого должно быть перемещено
        @param a распределитель памяти
        @post <tt> this->get_allocator() == a </tt>
        @post Если <tt> x.get_allocator() == a </tt>, то <tt> *this </tt>
        будет владеть элементами, которые до вызова конструктора, принадлежали
        @c x, в противном случае, создаёт контейнер, элементы которого
        создаются с помощью конструктора перемещения из соответствующих
        элементов контейнера @c x.
        */
        vector(vector && x, allocator_type const & a) noexcept
         : data_(std::move(x.data_), a)
        {}

        /** @brief Конструктор на основе списка инициализаторов
        @param values список инициализаторов
        @post Эквивалентно <tt> vector(values.begin(), value.end()) </tt>
        @post <tt> this->get_allocator() </tt>
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
        */
        vector & operator=(vector && x)
            noexcept(std::allocator_traits<allocator_type>::propagate_on_container_move_assignment::value
                     || ural::allocator_is_always_equal<allocator_type>::value)
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

        template <class InputSequence>
        void assign(InputSequence && seq)
        {
            auto r = ural::copy(std::forward<InputSequence>(seq), *this);

            if(!r[ural::_1])
            {
                this->erase(r[ural::_2].begin(), r[ural::_2].end());
            }
            else
            {
                assert(!r[ural::_2]);

                this->insert(r[ural::_2].begin(), r[ural::_1]);
            }
        }

        /** @brief Заменяет элементы контейнера на копии элементов интервала
        <tt> [first; last) </tt>
        @param first итератор, задающий начало интервала
        @param last итератор, задающий конец интервала
        @pre @c first и @c last не должны быть итераторами <tt> *this </tt>
        @post <tt> *this == vector(first, last) </tt>
        */
        template <class InputIterator>
        typename disable_if<std::is_integral<InputIterator>::value, void>::type
        assign(InputIterator first, InputIterator const last)
        {
            // @todo Проверка, что first и last не ссылаются внутрь контейнера
            return this->assign(ural::make_iterator_sequence(first, last));
        }

        /** @brief Замена элементов контейнера на @c n элементов, каждый из
        которых равен @c value
        @param n количество элементов
        @param value значение элементов
        @post <tt> *this == vector(n, value) </tt>
        */
        void assign(size_type n, value_type const & value)
        {
            // @todo Устранить дублирование
            auto gen = ural::make_value_functor(std::cref(value));
            auto seq = ural::make_generator_sequence(std::move(gen)) | ural::taken(n);

            return this->assign(std::move(seq));
        }

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

        /** @brief Наибольший размер
        @return Наибольший возможный размер контейнера
        */
        size_type max_size() const noexcept
        {
            return data_.max_size();
        }

        /** @brief Изменение размера контейнера
        @param new_size новый размер контейнера
        @post Если <tt> new_size > this->size() </tt>, то вставляет в конец
        контейнера <tt> new_size - this->size() </tt> элементов, созданных
        конструктором без аргументов, в противном случае эквивалентно
        вызову функции @c pop_back <tt> this->size() - new_size </tt> раз.
        */
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

        /** @brief Изменение размера контейнера
        @param new_size новый размер контейнера
        @param c значение новых элементов
        @post Если <tt> new_size > this->size() </tt>, то вставляет в конец
        контейнера <tt> new_size - this->size() </tt> копий @c c, в противном
        случае эквивалентно вызову функции @c pop_back
        <tt> this->size() - new_size </tt> раз.
        */
        void resize(size_type new_size, value_type const & c)
        {
            if(new_size < this->size())
            {
                this->erase(this->cbegin() + new_size, this->cend());
            }
            else
            {
                this->reserve(new_size);
                this->insert(this->end(), new_size - this->size(), c);
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

        /** @breif Проверка пустоты контейнера
        @return <tt> this->begin() == this->end() </tt>
        */
        bool empty() const noexcept
        {
            return this->begin() == this->end();
        }

        /** @brief Резервирование памяти для последующего использования
        @param n желаемая ёмкость
        @post Перераспределение памяти не будет производится, пока размер
        контейнера не превысит @c n. Не может привести к уменьшению ёмкости.
        Если распределение памяти произошло, то все ссылки, указатели и
        итераторы становятся недействительными.
        */
        void reserve(size_type n)
        {
            data_.reserve(n);
        }

        /** @brief Не обязатыельный к выполнению запрос на уменьшение ёмкости
        до <tt> this->size() </tt>
        */
        void shrink_to_fit()
        {
            this->data_.shrink_to_fit();
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
            policy_type::check_index(*this, index);

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
            // @todo просто использовать метод проверки из container_checking_throw_policy
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
            policy_type::check_not_empty(*this);

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
            policy_type::check_not_empty(*this);
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
        /** @brief Размещение нового элемента в конце контейнера
        @param args аргументы конструктора для создания нового элемента
        @post <tt> this->back() </tt> равно
        <tt> T(std::forward<Args>(args)...) </tt>
        @post Приводит к перераспределению памяти, если
        <tt> this->size() == this->capacity() </tt>. В этом случае все ссылки,
        указатели и итераторы становятся недействительными.
        */
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
        /** @brief Вставка элемента в конец контейнера
        @param x значение, которое должно быть вставлено
        @post <tt> this->back() </tt> равно значению, которое @c x имел до
        вызова функции.
        @post Приводит к перераспределению памяти, если
        <tt> this->size() == this->capacity() </tt>. В этом случае все ссылки,
        указатели и итераторы становятся недействительными.
        */
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
            policy_type::check_not_empty(*this);

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

        template <class InputSequence>
        iterator insert(const_iterator position, InputSequence && seq)
        {
            policy_type::assert_can_insert_before(this->cbegin(), this->cend(),
                                                  position);

            using ural::sequence;
            auto s = sequence(seq);

            typedef typename decltype(s)::traversal_tag Category;

            return this->insert_impl(position - this->cbegin(),
                                     std::move(s), Category{});
        }

        /** @brief Вставка копий заданного значения в заданную точку
        @param position позиция, перед которой должны быть вставлены новые
        элементы
        @param n количесвто элементов
        @param value значение элементов
        @return Итератор, ссылающийся на первый вставленный элемент
        */
        iterator insert(const_iterator position, size_type const n, value_type const & value)
        {
            auto gen = ural::make_value_functor(std::cref(value));
            auto seq = ural::make_generator_sequence(std::move(gen)) | ural::taken(n);

            return this->insert(position, std::move(seq));
        }

        /** @brief Вставка последовательности элементов в середину контейнера
        @param position итератор, перед которым будут вставлены новые элементы
        @param first итератор, задающий начало интервала элементов для вставки
        @param last итератор, задающий конец интервала элементов для вставки
        @pre @c first и @c last не являются итераторами элементов контейнера
        <tt> *this </tt>
        */
        template <class InputIterator>
        typename disable_if<std::is_integral<InputIterator>::value, iterator>::type
        insert(const_iterator position, InputIterator first, InputIterator last)
        {
            return this->insert(position, ural::make_iterator_sequence(first, last));
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

        /** @brief Удаление элемента из контейнера
        @param position итератор, ссылающийся на элемент который должен быть
        удалён
        @pre @c position должен быть итератором, ссылающимся на один из
        элементов <tt> *this </tt>
        @return Итератор, следующий за @c position до выполнения данной функции.
        Если такого итератора нет, то <tt> this->end() </tt>
        */
        iterator erase(const_iterator position)
        {
            policy_type::assert_can_erase(this->cbegin(), this->cend(), position);

            return this->erase(position, position+1);
        }

        /** @brief Удаление последовательности элементов из контейнера
        @param first итератор, задающий начало последовательности элементов,
        которые должны быть удалены
        @param last итератор, задающий конец последовательности элементов,
        которые должны быть удалены
        @pre <tt> [first; last) </tt> должен быть действительным интервалом
        элеметов контейнера <tt> *this </tt>
        @return Итератор, ссылающийся на тот же элемент, что @c last до начала
        выполнения функции. Если такого элемента нет, то <tt> this->end() </tt>
        */
        iterator erase(const_iterator first, const_iterator last)
        {
            policy_type::assert_can_erase(this->cbegin(), this->cend(),
                                          first, last);

            // 1. Переходим к неконстантным итераторам
            auto const result = this->begin() + (first - this->cbegin());
            auto sink = result;
            auto source = this->begin() + (last - this->cbegin());

            // 2. Перемещаем последние элементы на места удаляемых
            auto in = ural::make_iterator_sequence(source, this->end());

            // @todo Можно доказать, что эта последовательность исчерпаетс позже
            auto out = ural::make_iterator_sequence(sink, this->end());

            out = ural::move_if_noexcept(in, out)[ural::_2];

            // 3. Удаляем последние элементы
            data_.pop_back(out.size());

            return result;
        }

        /** @brief Обмен содержимого @c x и <tt> *this </tt>
        @post Содержимое <tt> *this </tt> и @c x меняются местами
        @post Обменивает распределители памяти контейнеров, если
        <tt> std::allocator_traits<allocator_type>::propagate_on_container_swap::value </tt>
        равно @b true. Если же это значение равно @b false, а распределители
        памяти не равны, то поведение не определено.
        */
        void swap(vector & x)
            noexcept(std::allocator_traits<allocator_type>::propagate_on_container_swap::value
                     || ural::allocator_is_always_equal<allocator_type>::value)
        {
            data_.swap(x.data_);
        }

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
        template <class InputSequence>
        iterator insert_impl(size_type index,
                             InputSequence seq,
                             single_pass_traversal_tag)
        {
            auto const old_size = this->size();

            assert(index <= old_size);

            ural::copy(seq, *this | ural::back_inserter);

            std::rotate(this->begin() + index, this->begin() + old_size, this->end());

            return this->begin() + index;
        }

        template <class InputSequence>
        iterator insert_impl(size_type index,
                             InputSequence seq,
                             forward_traversal_tag)
        {
            this->reserve(this->size() + ural::size(seq));

            return this->insert_impl(index, std::move(seq),
                                     single_pass_traversal_tag{});
        }

    private:
        buffer<value_type, allocator_type> data_;
    };
}
// namespace ural

#endif
// Z_URAL_CONTAINER_VECTOR_HPP_INCLUDED
