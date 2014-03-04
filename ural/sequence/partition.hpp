#ifndef Z_URAL_SEQUENCE_PARTITION_HPP_INCLUDED
#define Z_URAL_SEQUENCE_PARTITION_HPP_INCLUDED

/** @file ural/sequence/partition.hpp
 @brief Последовательность вывода, записывающая значения в одну из двух базовых
 последовательностей, в зависимости от значения предиката
*/

namespace ural
{
    /** @brief Последовательность вывода, записывающая значения в одну из двух
    базовых последовательностей, в зависимости от значения предиката
    @tparam Ouput1 тип последовательности для элементов, удовлетворяющих
    предикату
    @tparam Ouput2 тип последовательности для элементов, не удовлетворяющих
    предикату
    @tparam Predicate тип предикат
    */
    template <class Output1, class Output2, class Predicate>
    class partition_sequence
     : public sequence_base<partition_sequence<Output1, Output2, Predicate>>
    {
    public:
        typedef single_pass_traversal_tag traversal_tag;

        // Конструкторы
        explicit partition_sequence(Output1 out_true, Output2 out_false,
                                    Predicate pred)
         : data_{Bases{std::move(out_true), std::move(out_false)},
                 std::move(pred)}
        {}

        // Однопроходная последовательность
        bool operator!() const
        {
            return !this->true_sequence() && !this->false_sequence();
        }

        partition_sequence & operator*()
        {
            return *this;
        }

        template <class T>
        void operator=(T && x)
        {
            if(this->predicate()(x))
            {
                *data_.first()[ural::_1] = std::forward<T>(x);
                ++ data_.first()[ural::_1];
            }
            else
            {
                *data_.first()[ural::_2] = std::forward<T>(x);
                ++ data_.first()[ural::_2];
            }
        }

        void pop_front()
        {}

        // Адаптор последовательности
        Output1 const & true_sequence() const
        {
            return data_.first()[ural::_1];
        }

        Output2 const & false_sequence() const
        {
            return data_.first()[ural::_2];
        }

        Predicate const & predicate() const
        {
            return data_.second();
        }

    private:
        typedef ural::tuple<Output1, Output2> Bases;
        boost::compressed_pair<Bases, Predicate> data_;
    };

    template <class Output1, class Output2, class Predicate>
    auto make_partition_sequence(Output1 && out_true, Output2 && out_false,
                                 Predicate pred)
    -> partition_sequence<decltype(sequence(std::forward<Output1>(out_true))),
                          decltype(sequence(std::forward<Output2>(out_false))),
                          decltype(make_functor(std::move(pred)))>
    {
        typedef partition_sequence<decltype(sequence(std::forward<Output1>(out_true))),
                          decltype(sequence(std::forward<Output2>(out_false))),
                          decltype(make_functor(std::move(pred)))> Result;
        return Result(sequence(std::forward<Output1>(out_true)),
                      sequence(std::forward<Output2>(out_false)),
                      make_functor(std::move(pred)));
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_PARTITION_HPP_INCLUDED
