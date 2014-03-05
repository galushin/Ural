#ifndef Z_URAL_MEMORY_HPP_INCLUDED
#define Z_URAL_MEMORY_HPP_INCLUDED

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

/** @file ural/memory.hpp
 @brief Средства для работы с динамической памятью
*/

#include <memory>
#include <ural/defs.hpp>

namespace ural
{
    template <class T, class... Args>
    typename std::enable_if<!std::is_array<T>::value, std::unique_ptr<T>>::type
    make_unique(Args &&... args)
    {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }

    template <class T>
    typename std::enable_if<std::is_array<T>::value && std::extent<T>::value == 0,
                            std::unique_ptr<T>>::type
    make_unique(size_t size)
    {
        typedef typename std::remove_extent<T>::type Elem;
        return std::unique_ptr<T>(new Elem[size]());
    }

    template <class T>
    typename std::enable_if<std::is_array<T>::value && std::extent<T>::value != 0,
                            std::unique_ptr<T>>::type
    make_unique(size_t size) = delete;

    template <class T>
    class copy_ptr
    {
        friend bool operator==(copy_ptr const & x, copy_ptr const & y)
        {
            return x.get() == y.get();
        }

        typedef std::unique_ptr<T> Holder;
    public:
        // Типы
        typedef typename Holder::pointer pointer;
        typedef typename Holder::element_type element_type;
        typedef typename Holder::deleter_type deleter_type;
        typedef typename std::add_lvalue_reference<T>::type reference;

        // Конструкторы
        constexpr copy_ptr() noexcept = default;

        explicit copy_ptr(pointer ptr) noexcept
         : holder_{std::move(ptr)}
        {}

        copy_ptr(copy_ptr const & x)
         : holder_{!x ? nullptr : new element_type{*x}, x.get_deleter()}
        {}

        // Свойства
        deleter_type const & get_deleter() const
        {
            return holder_.get_deleter();
        }

        explicit operator bool() const
        {
            return static_cast<bool>(holder_);
        }

        pointer get() const
        {
            return holder_.get();
        }

        reference operator*() const
        {
            // @todo Проверка?
            return *holder_;
        }

    private:
        Holder holder_;
    };
}
// namespace ural

#endif
// Z_URAL_MEMORY_HPP_INCLUDED
