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
    public:
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
         : iterators_{first, first, last}
        {}

        /** @brief Проверка исчерпания последовательности
        @return @b true, если в последовательности больше нет элементов,
        иначе --- @b false.
        */
        bool operator!() const
        {
            return this->front_() == this->stop_();
        }

        Iterator const & front_iterator() const
        {
            return this->front_();
        }

        /** @brief Доступ к текущему (переднему) элементу последовательности
        @return Ссылка на передний элемент последовательности
        @pre <tt> bool(*this) != false </tt>
        */
        reference operator*() const
        {
            policy_type::assert_not_empty(*this);

            return *(this->front_());
        }

        /** @brief Переход к следующему элементу последовательности
        @pre <tt> bool(*this) != false </tt>
        @return <tt> *this </tt>
        */
        iterator_sequence & operator++()
        {
            policy_type::assert_not_empty(*this);
            ++ this->front_();
            return *this;
        }

        // Многопроходная прямая последовательность
        iterator_sequence traversed_front() const
        {
            return iterator_sequence{this->old_front_(), this->front_()};
        }

        // Двусторонняя последовательность
        void pop_back()
        {
            policy_type::assert_not_empty(*this);
            -- this->stop_();
        }

        // Последовательность произвольного доступа
        reference operator[](distance_type index) const
        {
            policy_type::check_index(*this, index);
            return front_()[index];
        }

        distance_type size() const
        {
            return this->stop_() - this->front_();
        }

        iterator_sequence & operator+=(distance_type index)
        {
            // @todo Проверка индекса
            this->front_() += index;
            return *this;
        }

    private:
        static constexpr auto begin_index = ural::_1;
        static constexpr auto front_index = ural::_2;
        static constexpr auto stop_index = ural::_3;

        Iterator & old_front_()
        {
            return iterators_[begin_index];
        }

        Iterator const & old_front_() const
        {
            return iterators_[begin_index];
        }

        Iterator & front_()
        {
            return iterators_[front_index];
        }

        Iterator const & front_() const
        {
            return iterators_[front_index];
        }

        Iterator & stop_()
        {
            return iterators_[stop_index];
        }

        Iterator const & stop_() const
        {
            return iterators_[stop_index];
        }

    private:
        // @todo Настройка структуры в зависимости от категории
        ural::tuple<Iterator, Iterator, Iterator> iterators_;
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
