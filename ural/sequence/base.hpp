#ifndef Z_URAL_SEQUENCE_BASE_HPP_INCLUDED
#define Z_URAL_SEQUENCE_BASE_HPP_INCLUDED

namespace ural
{
    template <class Seq>
    class sequence_base
    {
    friend Seq sequence(Seq s)
    {
        return s;
    }

    public:
    private:
    };
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_BASE_HPP_INCLUDED
