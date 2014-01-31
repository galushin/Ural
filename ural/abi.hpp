#ifndef Z_URAL_ABI_HPP_INCLUDED
#define Z_URAL_ABI_HPP_INCLUDED

#include <cxxabi.h>

namespace ural
{
namespace abi
{
    inline std::string demangle_name(char const * name)
    {
        // @todo Поддержка разных компиляторов
        int status = 0;
        auto realname = ::__cxxabiv1::__cxa_demangle(name, nullptr, nullptr, &status);
        std::string result(realname);
        std::free(realname);
        return result;
    }
}
// namespace abi
}
// namespace ural

#endif
// Z_URAL_ABI_HPP_INCLUDED
