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
     : private default_helper<Allocator, std::allocator<charT>>::type
    {
    public:
        /// @brief Класс характеристик символов
        typedef typename default_helper<traits, std::char_traits<charT>>::type
            traits_type;

        /// @brief Класс распределителя памяти
        typedef typename default_helper<Allocator, std::allocator<charT>>::type
            allocator_type;

        /// @brief Тип значения
        typedef typename traits_type::char_type value_type;

        /// @brief Тип размера
        typedef typename std::allocator_traits<allocator_type>::size_type
            size_type;

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

        /** @brief Конструктор без аргументов
        @post <tt> size() == 0 </tt>
        @post <tt> data() </tt> возвращает ненулевой указатель, который может
        быть скопирован и к которому можно прибавить нуль.
        @post <tt> this->get_allocator() == a </tt>
        */
        explicit flex_string(Allocator const & a)
         : allocator_type{a}
         , data_(1, value_type{})
        {}

        /** @brief Конструктор на основе строкового литерала
        @param s строковый литерал
        @post <tt> std::strcmp(s, this->c_str()) == 0 </tt>
        */
        flex_string(charT const * s)
         : allocator_type{}
        {
            auto const n = traits_type::length(s);
            data_.resize(n+1);

            traits_type::copy(data_.data(), s, n+1);
        }

        /** @brief Размер
        @return Размер
        */
        size_type size() const
        {
            assert(!data_.empty());
            return data_.size() - 1;
        }

        /** @brief Ёмкость
        @return Ёмкость строки, то есть предел, до которого может увеличиваться
        размер строки без перераспределения памяти
        */
        size_type capacity() const
        {
            assert(!data_.empty());
            return data_.capacity() - 1;
        }

        // 21.4.7 Операции со строками
        //@{
        /** @brief Доступ к массиву данных
        @return Указатель @c p такой, что <tt> p + i == &operator[](i) </tt>
        для любого @c i из <tt> [0,size()] </tt>
        */
        const charT * data() const noexcept
        {
            return data_.data();
        }

        const charT * c_str() const noexcept
        {
            return data_.data();
        }
        //@}

        /** @brief Распределитель памяти
        @return Копия распределителя памяти, заданного при конструировании, или,
        если он был заменён, копия самой последней замены.
        */
        allocator_type get_allocator() const noexcept
        {
            return static_cast<allocator_type>(*this);
        }

    private:
        std::vector<value_type, allocator_type> data_;
    };
}
// namespace ural

#endif
// Z_URAL_FLEX_STRING_HPP_INCLUDED
