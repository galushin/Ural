#ifndef Z_URAL_SEQUENCE_ITERATOR_SEQUENCE_HPP_INCLUDED
#define Z_URAL_SEQUENCE_ITERATOR_SEQUENCE_HPP_INCLUDED

/** @file ural/sequence/iterator_sequence.hpp
 @brief Последовательность на основе пары итераторов
*/

#include <ural/tuple.hpp>

namespace ural
{
    class strict_sequence_policy
    {
    protected:
        ~strict_sequence_policy() = default;

    public:
        template <class Seq>
        static void assert_not_empty(Seq const & seq)
        {
            if(!seq)
            {
                throw std::logic_error("Sequence must be not empty");
            }
        }
    };

    /** @brief Последовательность на основе пары итераторов
    @tparam Iterator тип итератора
    @tparam Policy тип политики обработки ошибок
    */
    template <class Iterator, class Policy = strict_sequence_policy>
    class iterator_sequence
    {
    public:
        /// @brief Тип ссылки
        typedef typename std::iterator_traits<Iterator>::reference reference;

        /// @brief Тип значения
        typedef typename std::iterator_traits<Iterator>::value_type value_type;

        /// @brief Тип политики обработки ошибок
        typedef Policy policy_type;

        /** @brief Конструктор
        @param first итератор, задающий начало интервала
        @param last итератор, задающий конец интервала
        @pre <tt> [first; last) </tt> должен быть допустимым интервалом
        */
        explicit iterator_sequence(Iterator first, Iterator last)
         : iterators_{first, last}
        {}

        /** @brief Проверка исчерпания последовательности
        @return @b true, если в последовательности больше нет элементов,
        иначе --- @b false.
        */
        bool operator!() const
        {
            return this->front_() == this->stop_();
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

    private:
        Iterator & front_()
        {
            return iterators_[ural::_1];
        }

        Iterator const & front_() const
        {
            return iterators_[ural::_1];
        }

        Iterator & stop_()
        {
            return iterators_[ural::_2];
        }

        Iterator const & stop_() const
        {
            return iterators_[ural::_2];
        }

    private:
        ural::tuple<Iterator, Iterator> iterators_;
    };
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_ITERATOR_SEQUENCE_HPP_INCLUDED
