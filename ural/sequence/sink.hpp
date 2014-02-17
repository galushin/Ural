#ifndef Z_URAL_SEQUENCE_SINK_HPP_INCLUDED
#define Z_URAL_SEQUENCE_SINK_HPP_INCLUDED

/** @file ural/sequence/sink.hpp
 @brief Последовательность вывода, которая просто игнорирует передаваемое
 значение.
*/

namespace ural
{
    template <class T = auto_tag>
    class sink_functor
    {
    public:
        void operator()(typename boost::call_traits<T>::param_type &) const
        {}
    };

    template <>
    class sink_functor<auto_tag>
    {
    public:
        template <class T>
        void operator()(T const &) const
        {}
    };

    template <class T = auto_tag>
    using sink_sequence = ural::function_output_sequence<sink_functor<T>>;
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_SINK_HPP_INCLUDED
