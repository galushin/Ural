#ifndef Z_URAL_FLEX_STRING_HPP_INCLUDED
#define Z_URAL_FLEX_STRING_HPP_INCLUDED

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

/** @file ural/flex_string.hpp
 @brief Реализация строк, основанная на стратегиях
 @note открытым остаётся вопрос, где целесообразнее обрабатывать терминальный
 символ строки
 @todo проверка, что в конце всех операций data_.size() + 1 == this->size() и
 data_[this->size()] = 0
*/

#include <ural/defs.hpp>

#include <algorithm>
#include <ostream>
#include <vector>
#include <stdexcept>

#include <cassert>

// @todo Интеграция с std::string
// @todo Согласовать size и capacity с begin и end

namespace ural
{
    template <class T, class A>
    class string_allocator_storage
     : private A
    {
    public:
        // @todo Для пустой строки не выделять память

        // Типы
        typedef typename A::size_type size_type;
        typedef typename A::pointer iterator;
        typedef typename A::const_pointer const_iterator;

        // Создание, копирование, уничтожение
        string_allocator_storage(size_type n, A const & a)
         : A{a}
        {
            begin_ = A::allocate(n);
            end_of_storage_ = begin_ + n;
            end_ = begin_;

            // @todo можно ли присваивать без конструирования?
            for(; n > 0; -- n, ++ end_)
            {
                A::construct(end_);
            }
        }

        string_allocator_storage(string_allocator_storage const & x)
         : string_allocator_storage{x.size(), x.get_allocator()}
        {
            std::copy(x.begin(), x.end(), this->begin());
        }

        string_allocator_storage & operator=(string_allocator_storage const & x);

        ~string_allocator_storage()
        {
            for(auto p = begin_; p != end_; ++ p)
            {
                A::destroy(p);
            }

            A::deallocate(begin_, this->size());
        }

        // Свойства
        A get_allocator() const
        {
            return static_cast<A>(*this);
        }

        T const * data() const
        {
            return this->begin_;
        }

        size_type size() const
        {
            return this->end() - this->begin();
        }

        size_type capacity() const
        {
            return this->end_of_storage_ - this->begin();
        }

        iterator begin()
        {
            return this->begin_;
        }

        const_iterator begin() const
        {
            return this->begin_;
        }

        iterator end()
        {
            return this->end_;
        }

        const_iterator end() const
        {
            return this->end_;
        }

        // Модификаторы
        void resize(size_type n, T const & c)
        {
            if(n > this->size())
            {
                this->append(n - this->size(), c);
            }
            else
            {
                this->pop_back(this->size() - n);
            }
        }

        void reserve(size_type n)
        {
            if(n > this->capacity())
            {
                string_allocator_storage tmp(n, this->get_allocator());

                assert(tmp.size() > this->size());

                tmp.pop_back(tmp.size() - this->size());

                assert(tmp.size() == this->size());
                assert(tmp.capacity() >= n);

                std::move(begin_, end_, tmp.begin_);

                tmp.swap(*this);
            }

            assert(this->capacity() >= n);
        }

        void swap(string_allocator_storage & x)
        {
            // @todo обменивать распределители?
            std::swap(this->begin_, x.begin_);
            std::swap(this->end_, x.end_);
            std::swap(this->end_of_storage_, x.end_of_storage_);
        }

        void append(size_type n, T const & c)
        {
            this->reserve(this->size() + n);

            assert(this->size() + n <= this->capacity());

            for(; n > 0; -- n, ++end_)
            {
                A::construct(end_, c);
            }
        }

        void pop_back(size_type n)
        {
            for(; n > 0; -- n, -- end_)
            {
                A::destroy(end_ - 1);
            }
        }

    private:
        typedef typename A::pointer pointer;

        pointer begin_;
        pointer end_;
        pointer end_of_storage_;
    };

    template <class T, class A>
    class string_vector_storage
    {
        typedef std::vector<T, A> Container;

    public:
        // Типы
        typedef typename Container::size_type size_type;
        typedef typename Container::iterator iterator;
        typedef typename Container::const_iterator const_iterator;

        // Конструкторы
        string_vector_storage(size_type n, A const & a)
         : data_{n, T{}, a}
        {}

        // Свойства
        A get_allocator() const
        {
            return data_.get_allocator();
        }

        size_type size() const
        {
            return data_.size();
        }

        size_type capacity() const
        {
            return data_.capacity();
        }

        T const * data() const
        {
            return data_.data();
        }

        // Итераторы
        iterator begin()
        {
            return data_.begin();
        }

        const_iterator begin() const
        {
            return data_.begin();
        }

        iterator end()
        {
            return data_.end();
        }

        const_iterator end() const
        {
            return data_.end();
        }

        // Модификаторы
        void resize(size_type n, T const & c)
        {
            data_.resize(n, c);
        }

        void reserve(size_type n)
        {
            data_.reserve(n);
        }

        void swap(string_vector_storage & x)
        {
            data_.swap(x.data_);
        }

        void append(size_type n, T const & c)
        {
            data_.insert(this->end(), n, c);
        }

        void pop_back(size_type n)
        {
            data_.erase(this->end() - n, this->end());
        }

    private:
        Container data_;
    };

    /** @brief Реализация строк, основанная на стратегиях
    @tparam charT тип символов
    @tparam traits класс характеристик символов
    @tparam Allocator распределитель памяти
    @tparam Storage стратегия хранения
    @todo Оптимизация, в частности - избегать создания временных строк
    @todo Использовать по умолчанию более эффективную стратегию хранения
    */
    template <class charT,
              class traits = use_default,
              class Allocator = use_default,
              class Storage = use_default>
    class flex_string
    {
    public:
        /// @brief Класс характеристик символов
        typedef typename default_helper<traits, std::char_traits<charT>>::type
            traits_type;

        /// @brief Класс распределителя памяти
        typedef typename default_helper<Allocator, std::allocator<charT>>::type
            allocator_type;

        /// @brief Тип значения
        typedef typename traits_type::char_type value_type;

    private:
        typedef typename default_helper<Storage, string_allocator_storage<charT, allocator_type>>::type
            storage_type;

    public:
        /// @brief Тип ссылки
        typedef value_type & reference;

        /// @brief Тип константной ссылки
        typedef value_type const & const_reference;

        /// @brief Тип размера
        typedef typename std::allocator_traits<allocator_type>::size_type
            size_type;

        /// @brief Тип итератора
        typedef typename storage_type::iterator iterator;

        /// @brief Тип итератора констант
        typedef typename storage_type::const_iterator const_iterator;

        /// @brief Тип обратного итератора
        typedef std::reverse_iterator<iterator> reverse_iterator;

        /// @brief Тип обратного константного итератора
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

        /** @brief константа, обозначающая "максимально возможное количество
        символов"
        */
        static const size_type npos = -1;

        // 21.4.2 Конструкторы и присваивание
        /** @brief Конструктор без аргументов
        @post <tt> size() == 0 </tt>
        @post <tt> data() </tt> возвращает ненулевой указатель, который может
        быть скопирован и к которому можно прибавить нуль.

        @note Конструктор объявлен @b explicit, так как даже для пустой строки
        может потребоваться выделение памяти
        */
        explicit flex_string()
         : data_(1, allocator_type{})
        {}

        /** @brief Конструктор без аргументов
        @post <tt> size() == 0 </tt>
        @post <tt> data() </tt> возвращает ненулевой указатель, который может
        быть скопирован и к которому можно прибавить нуль.
        @post <tt> this->get_allocator() == a </tt>
        */
        explicit flex_string(Allocator const & a)
         : data_(1, a)
        {}

        /// @brief Конструктор копий
        flex_string(flex_string const &) = default;

        /** @brief Конструктор на основе <tt> std::string </tt>
        @param s исходная строка
        */
        template <class A1>
        flex_string(std::basic_string<value_type, traits_type, A1> const & s)
         : flex_string{s.data(), s.size()}
        {}

        /** @brief Конструктор на основе списка инициализации
        @param il список инициализации
        @param a распределитель памяти
        */
        flex_string(std::initializer_list<value_type> il,
                    allocator_type const & a = allocator_type{})
         : flex_string{il.begin(), il.end(), a}
        {}

        /** @brief Конструктор с перемещением
        @param str строка
        */
        flex_string(flex_string && str)
         : flex_string{}
        {
            this->swap(str);
        }

        /** @brief Конструктор, копирующий подстроку
        @param str строка, фрагмент которой должен быть скопирован
        @param pos номер элемента, с которого начинается копируемая подстрока
        @param n количество символов, которые должны быть скопированы
        @param a распределитель памяти
        @pre <tt> pos <= this->size() </tt>
        @throw out_of_range, если <tt> pos > this->size() </tt>
        */
        flex_string(flex_string const & str,
                    size_type pos,
                    size_type n = npos,
                    allocator_type const & a = allocator_type{})
         : flex_string{a}
        {
            this->append(str, pos, n);
        }

        /** @brief Конструктор на основе массива
        @param s указатель на начало массива
        @param n количество элементов массива
        @param a распределитель памяти
        @pre <tt> s != nullptr </tt>
        @pre <tt> n < npos </tt>
        */
        flex_string(charT const * s, size_type n,
                    allocator_type const & a = allocator_type{})
         : data_{n+1, a}
        {
            traits_type::copy(&(*this)[0], s, n);
            *this->end() = value_type{};
        }

        /** @brief Конструктор на основе c-строки
        @param s строковый литерал
        @param a распределитель памяти
        @post <tt> std::strcmp(s, this->c_str()) == 0 </tt>
        */
        flex_string(charT const * s,
                    allocator_type const & a = allocator_type{})
         : data_{traits_type::length(s)+1, a}
        {
            traits_type::copy(&(*this)[0], s, this->size()+1);
        }

        /** @brief Создание строки одинаковых символов
        @param n количество символов
        @param c символ
        @param a распределитель памяти
        @post <tt> this->size() == n </tt>
        @post Для всех @c i из интервала <tt> [0; i) </tt> выполняется
        <tt> c == (*this)[i] </tt>
        */
        flex_string(size_type n, value_type c,
                    allocator_type const & a = allocator_type{})
         : data_(n+1, a)
        {
            std::fill(this->begin(), this->end(), c);
            *this->end() = value_type{};
        }

        /** @brief Создание строки на оснвое пары итераторов
        @param first итератор начала последовательности символов
        @param last итератор конца последовательности символов
        @todo Тест, когда @c InputIterator --- целочисленный тип
        */
        template <class InputIterator>
        flex_string(InputIterator first, InputIterator last,
                    allocator_type const & a = allocator_type{})
         : flex_string{a}
        {
            this->append(first, last);
        }

        /** @brief Конструктор
        @param str строка, содержимое которой должно быть скопировано.
        @param a распределитель памяти
        @post <tt> this->get_allocator() == a </tt>
        @post <tt> *this == str </tt>
        */
        flex_string(flex_string const & str, allocator_type const & a)
         : flex_string{str.data(), str.size(), a}
        {}

        /** @brief Конструктор
        @param str строка, содержимое которой должно быть перемещено
        @param a распределитель памяти
        */
        flex_string(flex_string && str, allocator_type const & a);

        /** @brief Оператор присваивания
        @param s присваемая строка
        @return <tt> *this </tt>
        @post <tt> *this == s </tt>
        */
        flex_string & operator=(flex_string const & s)
        {
            flex_string{s}.swap(*this);
            return *this;
        }

        /** @brief Оператор присваивания c-строки
        @param s строка
        @return <tt> *this <tt>
        @post <tt> *this == s </tt>
        */
        flex_string & operator=(value_type const * s)
        {
            return this->assign(s);
        }

        /** @brief Оператор присваивания с перемещением
        @param s строка, содержимое которой, будет перемещено
        @return <tt> *this </tt>
        */
        flex_string & operator=(flex_string && s)
        {
            s.swap(*this);
            return *this;
        }

        // 21.4.3 Итераторы
        //@{
        /** @brief Константный итератор, ссылающийся на первый символ строки
        @return Константный итератор, ссылающийся на первый символ строки
        */
        const_iterator begin() const
        {
            return data_.begin();
        }

        const_iterator cbegin() const
        {
            return this->begin();
        }
        //@}

        /** @brief Итератор, ссылающийся на первый символ строки
        @return Итератор, ссылающийся на первый символ строки
        */
        iterator begin()
        {
            return data_.begin();
        }

        //@{
        /** @brief Константный итератор, ссылающийся на элемент, следующий за
        последним элементом
        @return Константный итератор, ссылающийся на элемент, следующий за
        последним элементом
        */
        const_iterator end() const
        {
            return data_.end() - 1;
        }

        const_iterator cend() const
        {
            return this->end();
        }
        //@}

        /** @brief Итератор, ссылающийся на элемент, следующий за последним
        элементом
        @return Итератор, ссылающийся на элемент, следующий за последним
        элементом
        */
        iterator end()
        {
            return data_.end() - 1;
        }

        /** @brief Итератор, ссылающийся на первый в обратном порядке символ
        @return <tt> {this->end()} </tt>
        */
        reverse_iterator rbegin()
        {
            return reverse_iterator{this->end()};
        }

        //@{
        /** @brief Константный итератор, ссылающийся на первый в обратном
        порядке символ
        @return <tt> {this->end()} </tt>
        */
        const_reverse_iterator rbegin() const
        {
            return const_reverse_iterator{this->end()};
        }

        const_reverse_iterator crbegin() const
        {
            return this->rbegin();
        }
        //@}

        /** @brief Итератор, ссылающийся на символ, следующий за последним,
        в обратном порядке.
        @return <tt> {this->begin()} </tt>
        */
        reverse_iterator rend()
        {
            return reverse_iterator{this->begin()};
        }

        //@{
        /** @brief Константныйы итератор, ссылающийся на символ, следующий за
        последним, в обратном порядке.
        @return <tt> {this->begin()} </tt>
        */
        const_reverse_iterator rend() const
        {
            return const_reverse_iterator{this->begin()};
        }

        const_reverse_iterator crend() const
        {
            return this->rend();
        }
        //@}

        // 21.4.4. Размер и ёмкость
        //@{
        /** @brief Размер
        @return Размер
        */
        size_type size() const noexcept
        {
            return (this->end() - this->begin());
        }

        size_type length() const noexcept
        {
            return this->size();
        }
        //@}

        /** @brief Изменение размера строки
        @param n желаемое количество элементов
        @param c символ-заполнитель
        @pre <tt> n <= this->max_size() </tt>
        @throw std::lenght_error, если <tt> n > this->max_size() </tt>.
        */
        void resize(size_type n, value_type c)
        {
            if(n > this->size())
            {
                this->append(n - this->size(), c);
            }
            else
            {
                data_.pop_back(this->size() - n);
                *this->end() = value_type{};
            }
        }

        /** @brief Изменение размера строки
        @param n желаемое количество элементов
        @pre <tt> n <= this->max_size() </tt>
        @throw std::lenght_error, если <tt> n > this->max_size() </tt>.

        Эквивалентно вызову <tt> resize(n, value_type{}) </tt>
        */
        void resize(size_type n)
        {
            return this->resize(n, value_type{});
        }

        /** @brief Ёмкость
        @return Ёмкость строки, то есть предел, до которого может увеличиваться
        размер строки без перераспределения памяти
        */
        size_type capacity() const
        {
            assert(data_.size() != 0);
            return data_.capacity() - 1;
        }

        /** @brief Резервирует достаточно памяти для @c n символов
        @param n гарантированный размер, до которого может расти строка без
        перераспределения памяти
        */
        void reserve(size_t n)
        {
            return data_.reserve(n+1);
        }

        /** @brief Очистка содержимого строки.

        Эквивалентно вызову <tt> erase(begin(), end()) </tt>
        */
        void clear() noexcept
        {
            this->erase(this->begin(), this->end());
        }

        /** @brief Проверка, что строка пуста
        @return <tt> this->begin() == this->end() </tt>
        */
        bool empty() const
        {
            return this->begin() == this->end();
        }

        // 21.4.5 Доступ к элементам
        //@{
        /** @brief Доступ к символу строки по индексу
        @param pos номер символа
        @pre <tt> pos <= this->size() </tt>
        @return Если <tt> pos < this->size() </tt>, возвращает символ с индексом
        @c pos, если <tt> pos == this->size() </tt>, то возвращает ссылку на
        объект, равный <tt> charT{} </tt>, причём модификация этого объекта
        приводит к неопределённому поведению.
        */
        reference operator[](size_type pos)
        {
            return const_cast<reference>(static_cast<flex_string const&>(*this)[pos]);
        }

        const_reference operator[](size_type pos) const
        {
            assert(pos <= this->size());

            return *(this->data() + pos);
        }
        //@}

        //@{
        /** @brief Доступ к элементам по индексу с проверкой границ
        @param pos индекс элемента
        @pre <tt> pos < this->size() </tt>
        @throw std::out_of_range, если <tt> pos >= size() </tt>
        */
        const_reference at(size_type pos) const
        {
            if(pos >= this->size())
            {
                throw std::out_of_range("flex_string::at");
            }

            return (*this)[pos];
        }

        reference at(size_type pos)
        {
            auto const & c_self = static_cast<const flex_string&>(*this);

            return const_cast<reference>(c_self.at(pos));
        }
        //@}

        //@{
        /** @brief Доступ к первому символу строки
        @pre <tt> !this->empty() </tt>
        @return Ссылка на первый элемент строки
        */
        const_reference front() const
        {
            assert(!this->empty());
            return (*this)[0];
        }

        reference front()
        {
            auto const & c_self = static_cast<const flex_string&>(*this);

            return const_cast<reference>(c_self.front());
        }
        //@}

        //@{
         /** @brief Доступ к последнему элементу строки
        @pre <tt> !this->empty() </tt>
        @return Ссылка на последний элемент строки
        */
        const_reference back() const
        {
            assert(!this->empty());
            return (*this)[this->size() - 1];
        }

        reference back()
        {
            auto const & c_self = static_cast<const flex_string&>(*this);

            return const_cast<reference>(c_self.back());
        }
        //@}

        // 21.4.6 Модификаторы
        // 21.4.6.1 Оператор +=
        /** Дописывает строку @c x в конец данной строки. Эквивалентно
        <tt> this->append(x) </tt>.
        @param x строка
        @return <tt> *this </tt>
        */
        flex_string & operator+=(flex_string const & x)
        {
            return this->append(x);
        }

        /** Дописывает С-строку @c s в конец данной строки. Эквивалентно
        <tt> this->append(x) </tt>.
        @param s строка
        @return <tt> *this </tt>
        */
        flex_string & operator+=(value_type const * s)
        {
            return this->append(s);
        }

        /** Дописывает символ @c c в конец данной строки. Эквивалентно
        <tt> push_back(x) </tt>.
        @param c символ
        @return <tt> *this </tt>
        */
        flex_string & operator+=(value_type c)
        {
            this->push_back(c);
            return *this;
        }

        /** Дописывает символы списка инициализации в конец данной строки
        @param il список инициализации
        @return <tt> *this </tt>
        */
        flex_string & operator+=(std::initializer_list<value_type> il)
        {
            return this->append(il);
        }

        // 21.4.6.2 append
        /** Дописывает строку @c x в конец данной строки. Эквивалентно
        <tt> this->append(x.data(), x.size()) </tt>.
        @param x строка
        @return <tt> *this </tt>
        */
        flex_string & append(flex_string const & x)
        {
            return this->append(x.data(), x.size());
        }

        /** Эквивалентно <tt> append(x.data() + pos, rlen) </tt>, где
        <tt> rlen = min(n, x.size() - pos) </tt>.
        @param x строка
        @param pos индекс первого символа, который должен быть добавлен
        @param n количество элементов, которые должны быть добавлены
        @pre <tt> pos <= x.size() </tt>
        @throw std::out_of_range, если <tt> pos > x.size() </tt>
        @return <tt> *this </tt>
        */
        flex_string & append(flex_string const & x, size_type pos, size_type n)
        {
            if(pos > x.size())
            {
                throw std::out_of_range("flex_string::append");
            }

            return this->append(x.data() + pos, std::min(n, x.size() - pos));
        }

        /** @brief Записывает C-массив в конец данной строки
        @param s указатель на начало массива
        @param n количество элементов массива, которые должны быть дописаны
        @pre @c s должен указывать на массив длины не меньше @c n
        @return <tt> *this </tt>
        @todo генерация исключения, если <tt> size() + n > max_size() </tt>
        */
        flex_string & append(value_type const * s, size_type n)
        {
            data_.append(n, value_type{});
            std::copy(s, s+n, this->end() - n);

            *this->end() = value_type{};

            return *this;
        }

        /** Дописывает С-строку @c s в конец данной строки. Эквивалентно
        <tt> append(s, traits_type::length(s)) </tt>.
        @param s строка
        @return <tt> *this </tt>
        */
        flex_string & append(value_type const * s)
        {
            // @todo +1
            return this->append(s, traits_type::length(s));
        }

        /** @brief Дописывает @c n копий символа @c c в конец строки
        @param n количество копий символа, которые должны быть дописаны
        @param c значение дописываемых символов
        @return <tt> *this </tt>
        */
        flex_string & append(size_type n, value_type c)
        {
            assert(data_.size() > 0);

            data_.pop_back(1);
            data_.append(n, c);
            data_.append(1, value_type{});

            return *this;
        }

        /** @brief Дописывает элементы интервала <tt> [first; last)</tt> в конец
        данной строки.
        @param first начало интервала
        @param last конец интервала
        @pre <tt> [first; last)</tt>  --- допустимый интервал
        @return <tt> *this </tt>
        */
        template <class InputIterator>
        flex_string & append(InputIterator first, InputIterator last)
        {
            // @todo reserve, если позволяет категория итераторов
            data_.pop_back(1);
            for(; first != last; ++ first)
            {
                data_.append(1, *first);
            }
            data_.append(1, value_type{});

            return *this;
        }

        /** Дописывает символы списка инициализации в конец данной строки
        @param il список инициализации
        @return <tt> *this </tt>
        */
        flex_string & append(std::initializer_list<value_type> il)
        {
            return this->append(il.begin(), il.end());
        }

        /** Дописывает символ @c c в конец данной строки.
        @param c символ
        @return <tt> *this </tt>
        @todo Обеспечить, чтобы сложность была амортизированнной постоянной
        */
        void push_back(value_type c)
        {
            this->append(size_type{1}, c);
        }

        // 21.4.6.3 assign
        /** @brief Эквивалент <tt> assign(s, 0, npos) </tt>
        @param s присваиваемая строка
        @return <tt> *this </tt>
        */
        flex_string & assign(flex_string const & s)
        {
            return this->assign(s, 0, npos);
        }

        /** @brief Присваивание временной строки
        @param s присваиваемая временная строка
        @return <tt> *this </tt>
        */
        flex_string & assign(flex_string && s) noexcept
        {
            this->swap(s);
            return *this;
        }

        /** @brief Копирует подстроку строки @c s, начинающуюся с индекса
        @c pos, длинной <tt> min(n, s.size() - pos) </tt>.
        @param s строка
        @param pos начальный индекс
        @param n количество копируемых элементов
        @return <tt> *this </tt>
        */
        flex_string & assign(flex_string const & s, size_type pos, size_type n)
        {
            return this->assign(s.data(), std::min(n, s.size() - pos));
        }

        /** Заменяет строку, управляемую <tt> *this </tt> строкой длинны
        @c n, элементы которой равны соответствующим элементам массива, на
        который указывает @c s.
        @brief Копирование C-массива символов
        @param s указатель на начало массива
        @param n количество элементов
        @pre @c s указывает на массив длинны не меньше @c n
        @todo генерировать исключение, если <tt> n > max_size() </tt>
        */
        flex_string & assign(const value_type * s, size_type n)
        {
            this->clear();
            return this->append(s, n);
        }

        /** @brief Присваивание C-строку
        @param s указатель на C-строку
        @pre @c s указывает на массив символов длинной не менее
        <tt> traits_type::length(s) + 1</tt>
        @return <tt> *this </tt>
        */
        flex_string & assign(const value_type * s)
        {
            return this->assign(s, traits_type::length(s));
        }

        /** @brief Присваивание символов из списка инициализации
        @param il список инициализации
        @return <tt> *this </tt>
        */
        flex_string & assign(std::initializer_list<value_type> il)
        {
            return this->assign(il.begin(), il.end());
        }

        /** @brief Эквивалент <tt> this->assign(flex_string(n, c)) </tt>
        @param n количество символов
        @param c символ
        @return <tt> *this </tt>
        */
        flex_string & assign(size_type n, value_type c)
        {
            return this->assign(flex_string(n, c));
        }

        /** @brief Присваивание символов из интервала, заданного итераторами
        @param first итератор начала интервала
        @param last итератор конца интервала
        @pre <tt> [first; last) </tt> должен быть корректным интервалом
        @return <tt> *this </tt>
        */
        template <class InputIterator>
        flex_string & assign(InputIterator first, InputIterator last)
        {
            return this->assign(flex_string(first, last));
        }

        // 21.4.6.4 insert
        /** @brief Вставка одной строки в другую
        @param pos номер символа, с которого осуществляется вставка
        @param str вставляемая строка
        @pre <tt> pos <= this->size() </tt>
        @throw out_of_range, если <tt> pos > this->size() </tt>
        @return <tt> *this </tt>
        */
        flex_string & insert(size_type pos, flex_string const & str)
        {
            return this->insert(pos, str.data(), str.size());
        }

        /** @brief Вставка подстроки одной строки в другую
        @param pos1 номер символа, с которого осуществляется вставка
        @param str строка, подстрока которой должна быть вставлена
        @param pos2 номер символа, с которого начинается вставляемая подстрока
        @param n максимальная длина подстроки, которая должна быть вставлена
        @pre <tt> pos1 <= this->size() </tt>
        @pre <tt> pos2 <= str.size() </tt>
        @throw out_of_range, если <tt> pos1 > this->size() </tt> или
        <tt> pos2 > str.size() </tt>
        @return <tt> *this </tt>
        */
        flex_string & insert(size_type pos1, flex_string const & str,
                             size_type pos2, size_type n)
        {
            return this->insert(pos1, str.data() + pos2,
                                std::min(n, str.size() - pos2));
        }

        /** @brief Вставка массива символов в строку
        @param pos номер символа, с которого осуществляется вставка
        @param s указатель на начало массива символов
        @param n количество элементов массива, которое должно быть вставлено
        @pre @c s указывает на массив длинной не менее @c n
        @throw out_of_range, если <tt> pos > this->size() </tt>,
        length_error, если <tt> size() + n > max_size </tt>
        @return <tt> *this </tt>
        */
        flex_string & insert(size_type pos, value_type const * s, size_type n)
        {
            this->append(s, n);
            std::rotate(this->begin() + pos, this->end() - n, this->end());
            return *this;
        }

        /** @brief Вставка c-строки в строку
        @param pos номер символа, с которого осуществляется вставка
        @param s указатель на c-строку
        @pre @c s указывает на массив символов длинной не менее
        <tt> traits_type::length(s) + 1 </tt>
        @return <tt> *this </tt>
        */
        flex_string & insert(size_type pos, value_type const * s)
        {
            return this->insert(pos, s, traits_type::length(s));
        }

        /** @brief Вставка заданного числа копий символа в строку
        @param pos номер символа, с которого осуществляется вставка
        @param n количество копий символа
        @param c вставляемый символ
        @return <tt> *this </tt>

        Эквивалентно <tt> insert(pos, flex_string(n, c)) </tt>
        */
        flex_string & insert(size_type pos, size_type n, value_type c)
        {
            this->append(n, c);
            std::rotate(this->begin() + pos, this->end() - n, this->end());
            return *this;
        }

        /** @brief Вставка символа перед позицией, заданной итератором
        @param p итератор, задающий точку вставки
        @param c вставляемый символ
        @pre @c p действительный итератор для <tt> *this </tt>
        @return Итератор, ссылающийся на вставленный элемент
        */
        iterator insert(const_iterator p, value_type c)
        {
            return this->insert(p, size_type{1}, c);
        }

        /** @brief Вставка заданного числа копий символа перед позицией,
        заданной итератором
        @param p итератор, задающий точку вставки
        @param n количество копий символа
        @param c вставляемый символ
        @pre @c p действительный итератор для <tt> *this </tt>
        @return Итератор, ссылающийся на первую вставленную копию символа или
        @c p, если <tt> n == 0 </tt>.
        */
        iterator insert(const_iterator p, size_type n, value_type c)
        {
            auto const pos = p - this->cbegin();
            this->insert(pos, n, c);
            return this->begin() + pos;
        }

        /** @brief Вставка символов из интервала, заданого парой итераторлов, в
        точку, заданную итератором
        @param p итератор, задающий точку вставки
        @param first итератор начала интервала
        @param last итератор конца интервала
        @pre <tt> [first; last) </tt> должен быть корректным интервалом
        @return Итератор, ссылающийся на копию первого вставленного символа, или
        @c p, если <tt> first == last </tt>
        */
        template <class InputIterator>
        iterator
        insert(const_iterator p, InputIterator first, InputIterator last)
        {
            auto const pos = p - this->begin();
            auto const old_size = this->size();

            this->append(first, last);

            auto const n = this->size() - old_size;

            std::rotate(this->begin() + pos, this->end() - n, this->end());
            return this->begin() + pos;
        }

    public:

        /** @brief Вставка символов из списка инициализации в точку, заданную
        итератором
        @param p итератор, задающий точку вставки
        @param il список инициализации
        @return Итератор, ссылающийся на копию первого вставленного символа, или
        @c p, если @c il пуст.
        */
        iterator insert(const_iterator p, std::initializer_list<value_type> il)
        {
            return this->insert(p, il.begin(), il.end());
        }

        // 21.4.6.5 erase
        /** @brief Удаляет не более @c n элементов, начиная с номера @c pos
        @param pos номер первого элемента, который должен быть удалён
        @param n количество элементов, которые должны быть удалены
        @pre <tt> pos <= this->size() </tt>
        @throw out_of_range, если <tt> pos > this->size() </tt>.
        @return <tt> *this </tt>
        */
        flex_string & erase(size_type pos = 0, size_type n = npos)
        {
            auto const xlen = std::min(n, this->size() - pos);
            std::move(this->begin() + pos + xlen, this->end() + 1,
                      this->begin() + pos);
            data_.pop_back(xlen);
            return *this;
        }

        /** @brief Удаляет элемент строки, на который ссылается итератор @c p
        @param p итератор, ссылающийся на элемент, который должен быть удалён
        @return Итератор, ссылающийся на элемент, непосредственно следовавший за
        @c p до того, как элемент был удалён. Если такого элемента нет, то
        возвращает <tt> this->end() </tt>
        */
        iterator erase(const_iterator p)
        {
            assert(p != this->end());

            auto const pos = p - this->begin();
            this->erase(pos, 1);
            return this->begin() + pos;
        }

        /** @brief Удаляет элементы строки, определяемые интервалом
        <tt> [first; last) </tt>
        @param first итератор, задающий начало интервала удаляемых символов
        @param last итератор, задающий конец интервала удаляемых символов
        @pre @c first и @c last является действительными итераторами для
        <tt> *this </tt>, определяющими действительный интервал
        <tt> [first; last) </tt>
        @return Итератор, ссылающийся на элемент, на который ссылался @c last
        до вызова. Если такого элемента нет, то возвращает
        <tt> this->end() </tt>
        */
        iterator erase(const_iterator first, const_iterator last)
        {
            auto const pos = first - this->begin();
            this->erase(pos, last - first);
            return this->begin() + pos;
        }

        /** @brief Удаление последнего элемента строки
        @pre <tt> !this->empty() </tt>
        */
        void pop_back()
        {
            assert(!this->empty());
            this->erase(this->end() - 1);
        }

        // 21.4.6.7 copy
        /** @brief Копирование подстроки в массив
        @param s указатель на начало массива
        @param n размер массива
        @param pos номер элемента, с которого начинается копирование
        @return Количество символов, которое было скопировано
        */
        size_type copy(charT * s, size_type n, size_type pos = 0) const
        {
            if(pos > this->size())
            {
                throw std::out_of_range("flex_string::copy");
            }

            auto const rlen = std::min(n, this->size() - pos);

            std::copy(this->begin() + pos, this->begin() + pos + rlen, s);

            return rlen;
        }

        // 21.4.6.8 swap
        /** @brief Обмен содержимого @c s и <tt> *this </tt>
        @param s строка, с которой будет обменяно содержимое <tt> *this </tt>
        */
        void swap(flex_string & s)
        {
            data_.swap(s.data_);
        }

        // 21.4.7 Операции со строками
        //@{
        /** @brief Доступ к массиву данных
        @return Указатель @c p такой, что <tt> p + i == &operator[](i) </tt>
        для любого @c i из <tt> [0,size()] </tt>
        */
        const charT * data() const noexcept
        {
            return data_.data();
        }

        const charT * c_str() const noexcept
        {
            return this->data();
        }
        //@}

        // 21.4.7.8 substr
        /** @brief Выделение подстроки
        @param pos номер элемента, с которого начинается подстрока
        @param n длина подстроки
        @pre <tt> pos <= this->size() </tt>
        @throw std::out_of_range, если <tt> pos > this->size() </tt>
        @return <tt> flex_string(this->data() + pos, rlen) </tt>, где
        <tt> rlen = std::min(n, this->size() - pos) </tt>
        */
        flex_string substr(size_type pos, size_type n = npos) const
        {
            if(pos > this->size())
            {
                throw std::out_of_range("flex_string::substr");
            }

            return flex_string(this->data() + pos,
                               std::min(n, this->size() - pos));
        }

        // 21.4.7.9 compare
    private:
        int compare_impl(value_type const * s, size_t n) const noexcept
        {
            // @todo Рефакторинг
            auto const rlen = std::min(this->size(), n);
            auto const r = traits_type::compare(this->data(), s, rlen);

            if(r == 0)
            {
                if(this->size() < n)
                {
                    return -1;
                }
                if(this->size() > n)
                {
                    return  +1;
                }
            }
            return r;
        }

    public:
        /** @brief Сравнение строк
        @param s строка, с которой сравнивается данная строка
        @return <tt> traits_type::compare(data(), s.data(), rlen) </tt>, где
        <tt> rlen == std::min(size(), s.size()) </tt>
        */
        int compare(flex_string const & s) const noexcept
        {
            return this->compare_impl(s.data(), s.size());
        }

        /** @brief Сравнение строк
        @param s строка, с которой сравнивается данная строка
        @return <tt> this->compare(flex_string(s)) </tt>
        */
        int compare(value_type const * s) const noexcept
        {
            return this->compare_impl(s, traits_type::length(s));
        }

        /** @brief Распределитель памяти
        @return Копия распределителя памяти, заданного при конструировании, или,
        если он был заменён, копия самой последней замены.
        */
        allocator_type get_allocator() const noexcept
        {
            return data_.get_allocator();
        }

        // 21.4.8.9
        /** @brief Оператор вывода в поток
        @param os поток вывода
        @param x выводимая в поток строка
        @return @c os
        */
        friend std::basic_ostream<value_type, traits_type> &
        operator<<(std::basic_ostream<value_type, traits_type> & os,
                   flex_string const & x)
        {
            // @todo добавить выравнивание
            return os << x.c_str();
        }

    private:
        storage_type data_;
    };

    // 21.4.8.1 operator+
    /** @brief Конкатенация строк
    @param x левый операнд
    @param y правый операнд
    @return <tt> flex_string<charT, traits, Allocator>(x).append(y) </tt>
    */
    template <class charT, class traits, class Allocator>
    flex_string<charT, traits, Allocator>
    operator+(flex_string<charT, traits, Allocator> const & x,
              flex_string<charT, traits, Allocator> const & y)
    {
        return flex_string<charT, traits, Allocator>(x).append(y);
    }

    /** @brief Конкатенация строк
    @param x левый операнд
    @param y правый операнд
    @return <tt> std::move(x.append(y)) </tt>
    */
    template <class charT, class traits, class Allocator>
    flex_string<charT, traits, Allocator>
    operator+(flex_string<charT, traits, Allocator> && x,
              flex_string<charT, traits, Allocator> const & y)
    {
        return std::move(x.append(y));
    }

    /** @brief Конкатенация строк
    @param x левый операнд
    @param y правый операнд
    @return <tt> std::move(y.insert(0, x)) </tt>
    */
    template <class charT, class traits, class Allocator>
    flex_string<charT, traits, Allocator>
    operator+(flex_string<charT, traits, Allocator> const & x,
              flex_string<charT, traits, Allocator> && y)
    {
        return std::move(y.insert(0, x));
    }

    /** @brief Конкатенация строк
    @param x левый операнд
    @param y правый операнд
    @return <tt> std::move(x.append(y)) </tt>
    */
    template <class charT, class traits, class Allocator>
    flex_string<charT, traits, Allocator>
    operator+(flex_string<charT, traits, Allocator> && x,
              flex_string<charT, traits, Allocator> && y)
    {
        return std::move(x.append(y));
    }

    /** @brief Конкатенация c-строки и строки
    @param x с-строка
    @param y строка
    @return <tt> flex_string<charT, traits, Allocator>(y).insert(0, x) </tt>
    */
    template <class charT, class traits, class Allocator>
    flex_string<charT, traits, Allocator>
    operator+(charT const * x, flex_string<charT, traits, Allocator> const & y)
    {
        return flex_string<charT, traits, Allocator>(y).insert(0, x);
    }

    /** @brief Конкатенация c-строки и строки
    @param x с-строка
    @param y строка
    @return <tt> std::move(y.insert(0, x)) </tt>
    */
    template <class charT, class traits, class Allocator>
    flex_string<charT, traits, Allocator>
    operator+(charT const * x, flex_string<charT, traits, Allocator> && y)
    {
        return std::move(y.insert(0, x));
    }

    /** @brief Конкатенация символа и строки
    @param x символ
    @param y строка
    @return <tt> flex_string<charT, traits, Allocator>(1U, x) + y </tt>
    */
    template <class charT, class traits, class Allocator>
    flex_string<charT, traits, Allocator>
    operator+(charT x, flex_string<charT, traits, Allocator> const & y)
    {
        return flex_string<charT, traits, Allocator>(1U, x) + y;
    }

    /** @brief Конкатенация символа и строки
    @param x символ
    @param y строка
    @return <tt> std::move(y.insert(0, 1, x)) </tt>
    */
    template <class charT, class traits, class Allocator>
    flex_string<charT, traits, Allocator>
    operator+(charT x, flex_string<charT, traits, Allocator> && y)
    {
        y.insert(y.begin(), 1, x);
        return std::move(y);
    }

    /** @brief Конкатенация строки и c-строки
    @param x строка
    @param y c-строка
    @return <tt> flex_string<charT, traits, Allocator>(x).append(y) </tt>
    */
    template <class charT, class traits, class Allocator>
    flex_string<charT, traits, Allocator>
    operator+(flex_string<charT, traits, Allocator> const & x,
              charT const * y)
    {
        return flex_string<charT, traits, Allocator>(x).append(y);
    }

    /** @brief Конкатенация строки и c-строки
    @param x строка
    @param y c-строка
    @return <tt> std::move(x.append(y)) </tt>
    */
    template <class charT, class traits, class Allocator>
    flex_string<charT, traits, Allocator>
    operator+(flex_string<charT, traits, Allocator> && x,
              charT const * y)
    {
        return std::move(x.append(y));
    }

    /** @brief Конкатенация строки и символа
    @param x строка
    @param y символ
    @return <tt> flex_string<charT, traits, Allocator>(x).append(1, y) </tt>
    */
    template <class charT, class traits, class Allocator>
    flex_string<charT, traits, Allocator>
    operator+(flex_string<charT, traits, Allocator> const & x,
              charT y)
    {
        return flex_string<charT, traits, Allocator>(x).append(1, y);
    }

    /** @brief Конкатенация строки и символа
    @param x строка
    @param y символ
    @return <tt> std::move(x.append(1, y)) </tt>
    */
    template <class charT, class traits, class Allocator>
    flex_string<charT, traits, Allocator>
    operator+(flex_string<charT, traits, Allocator> && x,
              charT y)
    {
        return std::move(x.append(1, y));
    }

    // 21.4.8.2 Оператор ==
    /** @brief Оператор "равно"
    @param x левый операнд
    @param y правый операнд
    @return <tt> x.compare(y) == 0 </tt>
    */
    template <class charT, class traits, class Allocator, class S>
    bool operator==(flex_string<charT, traits, Allocator, S> const & x,
                    flex_string<charT, traits, Allocator, S> const & y) noexcept
    {
        return x.compare(y) == 0;
    }

    //@{
    /** @brief Проверка на равенство с c-строкой
    @param str строка
    @param s строковый литерал
    @return <tt> str.compare(s) == 0 </tt>
    */
    template <class charT, class traits, class Allocator>
    bool operator==(charT const * s,
                    flex_string<charT, traits, Allocator> const & str) noexcept
    {
        return str == s;
    }

    template <class charT, class traits, class Allocator>
    bool operator==(flex_string<charT, traits, Allocator> const & str,
                    charT const * s) noexcept
    {
        return str.compare(s) == 0;
    }
    //@}

    // 21.4.8.4
    /** @brief Оператор "меньше"
    @param x левый операнд
    @param y правый операнд
    @return <tt> x.compare(y) < 0 </tt>
    */
    template <class charT, class traits, class Allocator>
    bool operator<(flex_string<charT, traits, Allocator> const & x,
                   flex_string<charT, traits, Allocator> const & y) noexcept
    {
        return x.compare(y) < 0;
    }

    /** @brief Оператор "меньше"
    @param x левый операнд
    @param y правый операнд
    @return <tt> y.compare(x) > 0 </tt>
    */
    template <class charT, class traits, class Allocator>
    bool operator<(charT const * x,
                   flex_string<charT, traits, Allocator> const & y) noexcept
    {
        return y.compare(x) > 0;
    }

    /** @brief Оператор "меньше"
    @param x левый операнд
    @param y правый операнд
    @return <tt> x.compare(y) < 0 </tt>
    */
    template <class charT, class traits, class Allocator>
    bool operator<(flex_string<charT, traits, Allocator> const & x,
                   charT const * y) noexcept
    {
        return x.compare(y) < 0;
    }

    // 21.4.8.8 swap
    /** @brief Обмен значениями двух строк
    @param x первый аргумент
    @param y второй аргумент
    Эквивалентно <tt> x.swap(y) </tt>
    */
    template <class charT, class traits, class Allocator>
    void swap(flex_string<charT, traits, Allocator> & x,
              flex_string<charT, traits, Allocator> & y)
    {
        return x.swap(y);
    }
}
// namespace ural

#endif
// Z_URAL_FLEX_STRING_HPP_INCLUDED
