#ifndef Z_URAL_SEQUENCE_ITERATOR_SEQUENCE_HPP_INCLUDED
#define Z_URAL_SEQUENCE_ITERATOR_SEQUENCE_HPP_INCLUDED

/** @file ural/sequence/iterator_sequence.hpp
 @brief Последовательность на основе пары итераторов
*/

#include <ural/sequence/base.hpp>
#include <ural/tuple.hpp>

namespace ural
{
    /** @brief Строгая стратегия проверок для последовательности: возбуждает
    исключения в случае ошибок
    */
    class strict_sequence_policy
    {
    protected:
        ~strict_sequence_policy() = default;

    public:
        /** @param seq проверяемая последовательность
        @brief Проверяет, что последовательность @c seq не пуста, в противном
        случае возбуждает исключение типа <tt> logic_error </tt>.
        */
        template <class Seq>
        static void assert_not_empty(Seq const & seq)
        {
            if(!seq)
            {
                throw std::logic_error("Sequence must be not empty");
            }
        }

        template <class Seq>
        static void check_index(Seq const & seq,
                                typename Seq::distance_type index)
        {
            if(index >= seq.size())
            {
                // @todo Передавать больше информации
                throw std::logic_error("Invalid index");
            }
        }
    };

    /** @brief Последовательность на основе пары итераторов
    @tparam Iterator тип итератора
    @tparam Policy тип политики обработки ошибок
    */
    template <class Iterator, class Policy = strict_sequence_policy>
    class iterator_sequence
     : public sequence_base<iterator_sequence<Iterator, Policy>>
    {
        static void decrement_if_bidirectional(Iterator & s)
        {
            typename std::iterator_traits<Iterator>::iterator_category helper{};
            return decrement_if_bidirectional(s, helper);
        }

        static void decrement_if_bidirectional(Iterator &, std::input_iterator_tag)
        {}

        static void decrement_if_bidirectional(Iterator & s, std::bidirectional_iterator_tag)
        {
            -- s;
        }

    public:
        typedef Iterator iterator;

        /// @brief Тип ссылки
        typedef typename std::iterator_traits<Iterator>::reference reference;

        /// @brief Тип значения
        typedef typename std::iterator_traits<Iterator>::value_type value_type;

        typedef typename std::iterator_traits<Iterator>::difference_type
            distance_type;

        /// @brief Тип политики обработки ошибок
        typedef Policy policy_type;

        /** @brief Конструктор
        @param first итератор, задающий начало интервала
        @param last итератор, задающий конец интервала
        @pre <tt> [first; last) </tt> должен быть допустимым интервалом
        */
        explicit iterator_sequence(Iterator first, Iterator last)
         : iterators_{first, first, last, last, last}
        {
            if(first != last)
            {
                decrement_if_bidirectional(iterators_[back_index]);
            }
        }

        /** @brief Проверка исчерпания последовательности
        @return @b true, если в последовательности больше нет элементов,
        иначе --- @b false.
        */
        bool operator!() const
        {
            return iterators_[front_index] == iterators_[stop_index];
        }

        Iterator const & front_iterator() const
        {
            return iterators_[front_index];
        }

        /** @brief Доступ к текущему (переднему) элементу последовательности
        @return Ссылка на передний элемент последовательности
        @pre <tt> bool(*this) != false </tt>
        */
        reference front() const
        {
            policy_type::assert_not_empty(*this);

            return *(iterators_[front_index]);
        }

        /** @brief Переход к следующему элементу последовательности
        @pre <tt> bool(*this) != false </tt>
        @return <tt> *this </tt>
        */
        void pop_front()
        {
            policy_type::assert_not_empty(*this);
            ++ iterators_[front_index];
        }

        // Многопроходная прямая последовательность
        iterator_sequence traversed_front() const
        {
            return iterator_sequence{iterators_[begin_index],
                                     iterators_[front_index]};
        }

        void shrink_front()
        {
            iterators_[begin_index] = iterators_[front_index];
        }

        // Двусторонняя последовательность
        void pop_back()
        {
            policy_type::assert_not_empty(*this);
            -- iterators_[stop_index];

            if(!!*this)
            {
                -- iterators_[back_index];
            }
        }

        reference back() const
        {
            policy_type::assert_not_empty(*this);
            return *iterators_[back_index];
        }

        // Последовательность произвольного доступа
        reference operator[](distance_type index) const
        {
            policy_type::check_index(*this, index);
            return iterators_[front_index][index];
        }

        distance_type size() const
        {
            return iterators_[stop_index] - iterators_[front_index];
        }

        iterator_sequence & operator+=(distance_type index)
        {
            // @todo Проверка индекса
            iterators_[front_index] += index;
            return *this;
        }

        // Итераторы
        iterator begin() const
        {
            return iterators_[front_index];
        }

        iterator end() const
        {
            return iterators_[stop_index];
        }

        iterator traversed_begin() const
        {
            return iterators_[begin_index];
        }

        iterator traversed_end() const
        {
            return iterators_[end_index];
        }

    private:
        static constexpr auto begin_index = ural::_1;
        static constexpr auto front_index = ural::_2;
        static constexpr auto back_index = ural::_3;
        static constexpr auto stop_index = ural::_4;
        static constexpr auto end_index = ural::_5;

    private:
        // @todo Настройка структуры в зависимости от категории
        ural::tuple<Iterator, Iterator, Iterator, Iterator, Iterator> iterators_;
    };

    template <class Iterator>
    ural::iterator_sequence<Iterator>
    make_iterator_sequence(Iterator first, Iterator last)
    {
        return ural::iterator_sequence<Iterator>{first, last};
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_ITERATOR_SEQUENCE_HPP_INCLUDED
