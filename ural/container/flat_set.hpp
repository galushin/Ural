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
*/

#include <ural/container/vector.hpp>
#include <ural/defs.hpp>

namespace ural
{
    /** Шаблон класса, реализующий упорядоченное множество. Данный контейнер
    предоставляет быстрые (логарифмические) операции поиска. Вставка и удаление
    элементов могут занимать линейное время. Данный алгоритм хранит элементы
    в виде непрерывного блока памяти, поэтому поиск и обход осуществляется
    быстрее, чем при хранении элементов в виде дерева. Данный контейнер следует
    использовать, когда удаление элементов производится гораздо реже, чем
    пиоск, а сохранение действительности итераторов при вставке и удалении
    элементов не требуется.
    В дополнение к операциям, определённым для <tt> std::map </tt>, данный
    шаблон предоставляет некоторые функции, связанные с непрерывностью
    контейнера: @c data, @c capacity, @c reserve и @c shrink_to_fit.
    @brief Упорядоченное множество, аналог <tt> std::map </tt>
    @tparam Key тип ключа
    @tparam Compare тип функции сравнения
    @tparam Allocator тип распределителя памяти
    @tparam Policy стратегия проверок и реакции на ошибки
    */
    template <class Key,
              class Compare = use_default,
              class Allocator = use_default,
              class Policy = use_default>
    class flat_set
    {
        typedef ural::vector<Key, Allocator, Policy> Data;

    public:
        friend bool operator==(flat_set const & x, flat_set const & y)
        {
            return ural::equal(x, y);
        }

        friend bool operator<(flat_set const & x, flat_set const & y)
        {
            return ural::lexicographical_compare(x, y);
        }

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

        /// @brief Тип итератора (смотри 23.2.4, абзац 6)
        typedef typename Data::const_iterator iterator;

        /// @brief Тип константного итератора
        typedef typename Data::const_iterator const_iterator;

        /// @brief Тип разности
        typedef typename Data::difference_type difference_type;

        /// @brief Тип размера
        typedef typename Data::size_type size_type;

        /// @brief Тип обратного итератора (смотри 23.2.4, абзац 6)
        typedef typename Data::const_reverse_iterator reverse_iterator;

        /// @brief Тип константного обратного итератора
        typedef typename Data::const_reverse_iterator const_reverse_iterator;

        /// @brief Тип стратегии
        typedef typename Data::policy_type policy_type;

        // Конструирование, копирование, уничтожение
        /** @brief Конструктор
        @param cmp функция сравнения
        @param a распределитель памяти
        @post <tt> this->empty() </tt>
        @post <tt> this->value_comp() == cmp </tt>
        @post <tt> this->get_allocator() == a</tt>
        */
        explicit flat_set(value_compare const & cmp = value_compare(),
                          allocator_type const & a = allocator_type())
         : members_(cmp, Data(a))
        {}

        /**
        @param first итератор, задающий начало последовательности значений,
        которые должны быть вставлены
        @param last итератор, задающий конец последовательности значений,
        которые должны быть вставлены
        @param cmp функция сравнения
        @param a распределитель памяти
        @post <tt> this->size() == std::distance(first, last) </tt>
        @post <tt> std::equal(first, last, this->begin()) </tt>
        @post <tt> this->value_compare() == cmp </tt>
        @post <tt> this->get_allocator() == a</tt>
        */
        template <class InputIterator>
        flat_set(InputIterator first, InputIterator last,
                 value_compare const & cmp = value_compare(),
                 allocator_type const & a = allocator_type())
         : flat_set(cmp, a)
        {
            this->insert(std::move(first), std::move(last));
        }

        /** @brief Конструктор копий
        @param x копируемый контейнер
        @post <tt> *this == x </tt>
        @post <tt> this->value_comp() == x.value_comp() </tt>
        @post <tt> this->get_allocator() </tt> равен
        <tt> AT::select_on_container_copy_construction(x.get_allocator()) </tt>,
        где @c AT есть <tt> std::allocator_traits<vector::allocator_type> </tt>.
        */
        flat_set(flat_set const & x) = default;

        /** @brief Конструктор перемещения
        @param x контейнер, содержимое которого будет перемещено в <tt> *this </tt>
        @post <tt> *this </tt> равно значению, которое контейнер @c x имел до
        начала выполнения этого оператора
        @post <tt> this->value_comp() </tt> равен значению, которое
        <tt> x.value_comp() </tt> имел до вызова конструктора
        @post <tt> this->get_allocator() </tt> равен значению, которое
        <tt> x.get_allocator() </tt> имел до вызова конструктора
        */
        flat_set(flat_set && x) = default;

        /** @brief Конструктор на основе распределителя памяти
        @param a распределитель памяти
        @post <tt> this->empty() </tt>
        @post <tt> this->value_comp() == cmp </tt>
        @post <tt> this->get_allocator() == a </tt>
        */
        explicit flat_set(allocator_type const & a)
         : members_(value_compare(), Data(a))
        {}

        /** @brief Создание копии контейнера с другим распределителем памяти
        @param x контейнер, элементы которого копируются
        @param a распределитель памяти
        @post <tt> *this == x </tt>
        @post <tt> this->value_comp() == x.value_comp() </tt>
        @post <tt> this->get_allocator() == a </tt>
        */
        explicit flat_set(flat_set const & x, allocator_type const & a)
         : members_(x.members_[ural::_1], Data(x.members_[ural::_2], a))
        {}

        /** @brief Контруктор перемещения с другим распределителем памяти
        @param x контейнер, содрежимое которого должно быть перемещено
        @param a распределитель памяти
        @post <tt> this->value_comp() </tt> равен значению, которое
        @post Если <tt> x.get_allocator() == a </tt>, то <tt> *this </tt>
        будет владеть элементами, которые до вызова конструктора, принадлежали
        @c x, в противном случае, создаёт контейнер, элементы которого
        создаются с помощью конструктора перемещения из соответствующих
        элементов контейнера @c x.
        <tt> x.value_comp() </tt> имел до вызова конструктора
        @post <tt> this->get_allocator() == a </tt>
        */
        explicit flat_set(flat_set && x, allocator_type const & a)
         : members_(std::move(x.members_[ural::_1]),
                    Data(std::move(x.members_[ural::_2]), a))
        {}

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

        /// @brief Деструктор
        ~flat_set() = default;

        /** @brief Оператор копирующего присваивания
        @param x копируемый контейнер
        @post <tt> *this == x </tt>
        @return <tt> *this </tt>
        */
        flat_set & operator=(flat_set const & x) = default;

        /** @brief Оператор присваивания с перемещением
        @param x копируемый контейнер
        @post <tt> *this </tt> равно значению, которое контейнер @c x имел до
        начала выполнения этого оператора
        @return <tt> *this </tt>
        */
        flat_set & operator=(flat_set && x) = default;

        flat_set & operator=(std::initializer_list<value_type> values);

        /** @brief Доступ к распределителю памяти
        @return Копия распределителя памяти, заданного при создании контейнера
        или установлого с помощью присваивания либо обмена.
        */
        allocator_type get_allocator() const noexcept
        {
            return this->members_[ural::_2].get_allocator();
        }

        // Итераторы
        //@{
        /** @brief Итератор начала контейнера
        @return Итератор, ссылающийся на первый элемент контейнера
        */
        iterator begin() noexcept
        {
            return this->members_[ural::_2].begin();
        }

        const_iterator begin() const noexcept
        {
            return this->members_[ural::_2].begin();
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
            return this->members_[ural::_2].end();
        }

        const_iterator end() const noexcept
        {
            return this->members_[ural::_2].end();
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
            return this->members_[ural::_2].rbegin();
        }

        const_reverse_iterator rbegin() const noexcept
        {
            return this->members_[ural::_2].rbegin();
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
            return this->members_[ural::_2].rend();
        }

        const_reverse_iterator rend() const noexcept
        {
            return this->members_[ural::_2].rend();
        }

        const_reverse_iterator crend() const noexcept
        {
            return this->rend();
        }
        //@}

        // Размер и ёмкость
        /** @brief Проверка того, что контейнер является пустым
        @return <tt> this->empty() </tt>
        */
        bool empty() const noexcept
        {
            return this->members_[ural::_2].empty();
        }

        /** @brief Размер контейнера
        @return Количество элементов, хранящихся в контейнере
        */
        size_type size() const noexcept
        {
            return this->members_[ural::_2].size();
        }

        /** @brief Наибольший возможный размер
        @return Размер наибольшего возможного контейнера
        */
        size_type max_size() const noexcept
        {
            return this->members_[ural::_2].max_size();
        }

        /** @brief Ёмкость контейнера
        @return Ёмкость контейнера, то есть количество элементов которое он
        может вместить без перераспределения памяти
        */
        size_type capacity() const noexcept
        {
            return this->members_[ural::_2].capacity();
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
            return this->members_[ural::_2].reserve(n);
        }

        /** @brief Не обязатыельный к выполнению запрос на уменьшение ёмкости
        до <tt> this->size() </tt>
        */
        void shrink_to_fit()
        {
            return this->members_[ural::_2].shrink_to_fit();
        }

        // Доступ к данным
        value_type const * data() const
        {
            return this->members_[ural::_2].data();
        }

        // Модификаторы
        template <class... Args>
        std::pair<iterator, bool>
        emplace(Args && ... args);

        template <class... Args>
        iterator
        emplace_hint(const_iterator position, Args && ... args);

        /** Вставляет элемент @c x в контейнер тогда и только тогда, когда в
        контейнере нет элементов, эквивалентных @c x.
        @brief Вставка элемента в контейнер
        @param x вставляемый элемент
        @return Пара, состоящую из итератора и булевого значения.
        Булево значение показывает, был ли новый элемент вставлен в контейнер.
        Итератор ссыалается на элемент, эквивалентный @c x.
        */
        std::pair<iterator, bool>
        insert(value_type const & x)
        {
            auto pos = this->lower_bound(x);

            if(pos != this->end() && !this->members_[ural::_1](x, *pos))
            {
                return std::make_pair(pos, false);
            }
            else
            {
                pos = this->members_[ural::_2].insert(pos, x);
                return std::make_pair(pos, true);
            }
        }

        std::pair<iterator, bool> insert(value_type &&);

        iterator insert(const_iterator, value_type const & x)
        {
            // @todo оптимизация

            // Используем общий алгоритм
            return this->insert(x).first;
        }

        iterator insert(const_iterator position, value_type && x);

        template <class InputIterator>
        void insert(InputIterator first, InputIterator last)
        {
            if(first == last)
            {
                return;
            }

            auto hint = this->insert(*first).first + 1;
            ++ first;

            for(; first != last; ++ first)
            {
                hint = this->insert(hint, *first) + 1;
            }
        }

        void insert(std::initializer_list<value_type> values);

        iterator erase(const_iterator position);
        size_type erase(key_type const & x);
        iterator erase(const_iterator first, const_iterator last);

        void swap(flat_set & x)
        {
            using std::swap;
            swap(this->members_, x.members_);
        }

        void clear() noexcept;

        // Свойства
        /** @brief Функция сравнения ключей
        @return Функциональный объект, используемый для сравнения ключей
        */
        key_compare key_comp() const
        {
            return this->members_[ural::_1];
        }

        /** @brief Функция сравнения значений
        @return <tt> this->key_comp() </tt>
        */
        value_compare value_comp() const
        {
            return this->key_comp();
        }

        // Операции над множествами
        //@{
        iterator find(key_type const & x);
        const_iterator find(key_type const & x) const;
        //@}

        size_type count(key_type const & x) const;

        //@{
        iterator lower_bound(key_type const & x)
        {
            return std::lower_bound(this->begin(), this->end(), x,
                                    this->value_comp());
        }

        const_iterator lower_bound(key_type const & x) const;
        //@}

        //@{
        iterator upper_bound(key_type const & x);
        const_iterator upper_bound(key_type const & x) const;
        //@}

        //@{
        std::pair<iterator, iterator>
        equal_range(key_type const & x);

        std::pair<const_iterator, const_iterator>
        equal_range(key_type const & x) const;
        //@}

    private:
        tuple<value_compare, Data> members_;
    };
}
// namespace ural

#endif
// Z_URAL_CONTAINER_FLAT_SET_HPP_INCLUDED
