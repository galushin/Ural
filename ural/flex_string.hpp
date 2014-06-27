#ifndef Z_URAL_FLEX_STRING_HPP_INCLUDED
#define Z_URAL_FLEX_STRING_HPP_INCLUDED

#include <ural/defs.hpp>

namespace ural
{
    /** @brief Реализация строк, основанная на стратегиях
    @tparam charT тип символов
    @tparam traits класс характеристик символов
    @tparam Allocator распределитель памяти
    @todo добавить Storage --- стратегия хранения
    */
    template <class charT,
              class traits = use_default,
              class Allocator = use_default>
    class flex_string
    {
    public:
        /// @brief класс характеристик символов
        typedef typename default_helper<traits, std::char_traits<charT>>::type
            traits_type;

        /// @brief Тип значения
        typedef typename traits_type::char_type value_type;

        /// @brief Тип размера
        typedef typename std::allocator_traits<Allocator>::size_type size_type;

        /// @brief Тип распределителя памяти
        typedef Allocator allocator_type;

        /** @brief Конструктор без аргументов
        @post <tt> size() == 0 </tt>
        @post <tt> data() </tt> возвращает ненулевой указатель, который может
        быть скопирован и к которому можно прибавить нуль.

        @note Конструктор объявлен @b explicit, так как даже для пустой строки
        может потребоваться выделение памяти
        */
        explicit flex_string()
         : data_(1, value_type{})
        {}

        /** @brief Размер
        @return Размер
        */
        size_type size() const
        {
            assert(!data_.empty());
            return data_.size() - 1;
        }

        /** @brief Доступ к массиву данных
        @return Указатель @c p такой, что <tt> p + i == &operator[](i) </tt>
        для любого @c i из <tt> [0,size()] </tt>
        */
        const charT * data() const noexcept
        {
            return data_.data();
        }

    private:
        std::vector<value_type, allocator_type> data_;
    };
}
// namespace ural

#endif
// Z_URAL_FLEX_STRING_HPP_INCLUDED
