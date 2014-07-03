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
*/

#include <ural/defs.hpp>

#include <ostream>

// @todo Интеграция с std::string

namespace ural
{
    /** @brief Реализация строк, основанная на стратегиях
    @tparam charT тип символов
    @tparam traits класс характеристик символов
    @tparam Allocator распределитель памяти
    @todo добавить Storage --- стратегия хранения
    */
    template <class charT,
              class traits = use_default,
              class Allocator = use_default>
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
        typedef std::vector<value_type, allocator_type> Container;

    public:
        /// @brief Тип ссылки
        typedef value_type & reference;

        /// @brief Тип константной ссылки
        typedef value_type const & const_reference;

        /// @brief Тип размера
        typedef typename std::allocator_traits<allocator_type>::size_type
            size_type;

        /// @brief Тип итератора
        typedef typename Container::iterator iterator;

        /// @brief Тип итератора констант
        typedef typename Container::const_iterator const_iterator;

        // 21.4.2 Конструкторы и присваивание
        /** @brief Конструктор без аргументов
        @post <tt> size() == 0 </tt>
        @post <tt> data() </tt> возвращает ненулевой указатель, который может
        быть скопирован и к которому можно прибавить нуль.

        @note Конструктор объявлен @b explicit, так как даже для пустой строки
        может потребоваться выделение памяти
        */
        explicit flex_string()
         : data_(1, value_type{})
        {}

        /** @brief Конструктор без аргументов
        @post <tt> size() == 0 </tt>
        @post <tt> data() </tt> возвращает ненулевой указатель, который может
        быть скопирован и к которому можно прибавить нуль.
        @post <tt> this->get_allocator() == a </tt>
        */
        explicit flex_string(Allocator const & a)
         : data_(1, value_type{}, a)
        {}

        /** @brief Конструктор на основе строкового литерала
        @param s строковый литерал
        @param a распределитель памяти
        @post <tt> std::strcmp(s, this->c_str()) == 0 </tt>
        */
        flex_string(charT const * s,
                    allocator_type const & a = allocator_type{})
         : data_{a}
        {
            auto const n = traits_type::length(s);
            data_.resize(n+1);

            traits_type::copy(data_.data(), s, n+1);
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
         : data_(n+1, c, a)
        {
            data_.back() = value_type{};
        }

        /** @brief Создание строки на оснвое пары итераторов
        @param first итератор начала последовательности символов
        @param last итератор конца последовательности символов
        @todo Тест, когда @c InputIterator --- целочисленный тип
        */
        template <class InputIterator>
        flex_string(InputIterator first, InputIterator last,
                    allocator_type const & a = allocator_type{})
         : data_(first, last, a)
        {
            data_.push_back(value_type{});
        }

        // 21.4.3 Итераторы
        const_iterator begin() const
        {
            return data_.begin();
        }

        iterator begin()
        {
            return data_.begin();
        }

        const_iterator end() const
        {
            return data_.end() - 1;
        }

        iterator end()
        {
            return data_.end() - 1;
        }

        // 21.4.4. Размер и ёмкость
        //@{
        /** @brief Размер
        @return Размер
        */
        size_type size() const noexcept
        {
            assert(!data_.empty());
            return data_.size() - 1;
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
            data_.pop_back();

            data_.resize(n, c);

            data_.push_back(value_type{});
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
            assert(!data_.empty());
            return data_.capacity() - 1;
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

            return data_[pos];
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
            data_.insert(this->end(), s, s+n);
            return *this;
        }

        /** Дописывает С-строку @c s в конец данной строки. Эквивалентно
        <tt> append(s, traits_type::length(s)) </tt>.
        @param s строка
        @return <tt> *this </tt>
        */
        flex_string & append(value_type const * s)
        {
            return this->append(s, traits_type::length(s));
        }

        /** @brief Дописывает @c n копий символа @c c в конец строки
        @param n количество копий символа, которые должны быть дописаны
        @param c значение дописываемых символов
        @return <tt> *this </tt>
        */
        flex_string & append(size_type n, value_type c)
        {
            data_.insert(this->end(), n, c);
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
            return this->append(flex_string{first, last});
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
        */
        void push_back(value_type c)
        {
            this->append(size_type{1}, c);
        }

        // 21.4.6.5 erase
        iterator erase(const_iterator first, const_iterator last)
        {
            return data_.erase(data_.begin() + (first - data_.begin()),
                               data_.begin() + (last - data_.begin()));
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
            return data_.data();
        }
        //@}

        /** @brief Распределитель памяти
        @return Копия распределителя памяти, заданного при конструировании, или,
        если он был заменён, копия самой последней замены.
        */
        allocator_type get_allocator() const noexcept
        {
            return data_.get_allocator();
        }

        friend std::basic_ostream<value_type, traits_type> &
        operator<<(std::basic_ostream<value_type, traits_type> & os,
                   flex_string const & x)
        {
            // @todo добавить выравнивание
            return os << x.c_str();
        }

    private:
        Container data_;
    };
}
// namespace ural

#endif
// Z_URAL_FLEX_STRING_HPP_INCLUDED
