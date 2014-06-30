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

        /// @brief Тип ссылки
        typedef value_type & reference;

        /// @brief Тип константной ссылки
        typedef value_type const & const_reference;

        /// @brief Тип размера
        typedef typename std::allocator_traits<allocator_type>::size_type
            size_type;

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

        // Размер и ёмкость
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

        // Доступ к элементам
        //@{
        reference operator[](size_type pos)
        {
            return const_cast<reference>(static_cast<flex_string const&>(*this)[pos]);
        }

        const_reference operator[](size_type pos) const
        {
            assert(pos < this->size());

            return data_[pos];
        }
        //@}

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
        std::vector<value_type, allocator_type> data_;
    };
}
// namespace ural

#endif
// Z_URAL_FLEX_STRING_HPP_INCLUDED
