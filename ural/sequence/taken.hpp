#ifndef Z_URAL_SEQUENCE_TAKEN_HPP_INCLUDED
#define Z_URAL_SEQUENCE_TAKEN_HPP_INCLUDED

/** @file ural/sequence/taken.hpp
 @brief Адаптер последовательности, ограничивающий базовую последовательность
 заданным числом элементов
*/

namespace ural
{
    /** @note Длина последовательности меньше заданной --- это ошибка или
    нормальная ситуация?
    */
    template <class Sequence, class Size>
    class take_sequence
     : public sequence_base<take_sequence<Sequence, Size>>
    {
    public:
        // Типы
        typedef typename Sequence::reference reference;
        typedef typename Sequence::value_type value_type;

        // Создание, копирование
        explicit take_sequence(Sequence seq, Size count)
         : impl_{std::move(seq), std::move(count)}
        {}

        // Однопроходная последовательность
        bool operator!() const
        {
            return !this->base() || this->count() == 0;
        }

        reference front() const
        {
            return *this->base();
        }

        void pop_front()
        {
            // @todo Проверить, что счётчик не исчерпан
            ++ impl_.first();
            -- impl_.second();
        }

        // Адаптор последовательности
        Sequence const & base() const
        {
            return impl_.first();
        }

        Size const & count() const
        {
            return impl_.second();
        }

    private:
        boost::compressed_pair<Sequence, Size> impl_;
    };

    template <class Size>
    struct taken_helper
    {
        Size count;
    };

    template <class Sequence, class Size>
    auto operator|(Sequence && seq, taken_helper<Size> helper)
    -> take_sequence<decltype(sequence(std::forward<Sequence>(seq))), Size>
    {
        typedef take_sequence<decltype(sequence(std::forward<Sequence>(seq))), Size> Result;
        return Result{sequence(std::forward<Sequence>(seq)), helper.count};
    }

    template <class Size>
    taken_helper<Size> taken(Size n)
    {
        return {n};
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_TAKEN_HPP_INCLUDED
