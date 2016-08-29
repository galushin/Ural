#ifndef Z_URAL_cursor_adaptorS_ASSUMED_FINITE_HPP_INCLUDED
#define Z_URAL_cursor_adaptorS_ASSUMED_FINITE_HPP_INCLUDED

/** @file ural/sequnece/adaptors/assumed_finited.hpp
 @brief Адаптор, предпалагающий, что последовательность, то есть гарантировано
 будет исчерпанаю в данном контексте.
*/

#include <ural/utility/pipeable.hpp>
#include <ural/sequence/adaptor.hpp>
#include <ural/sequence/make.hpp>

namespace ural
{
namespace experimental
{
    /** @brief Шаблон адаптора, предпалагающий, что курсор конечен, то есть
    будет исчерпан в данном контексте.
    @tparam Cursor базовый курсор
    */
    template <class Cursor>
    class assumed_finite_cursor
     : public cursor_adaptor<assumed_finite_cursor<Cursor>, Cursor>
    {
        using Inherited = cursor_adaptor<assumed_finite_cursor, Cursor>;

    public:
        /// @brief Категория курсора
        using cursor_tag = make_finite_cursor_tag_t<typename Inherited::cursor_tag>;

        // Создание, копирование, уничтожение, свойства
        /** @brief Конструктор
        @param seq базовая последовательность
        @post <tt> this->base() == seq </tt>
        */
        explicit assumed_finite_cursor(Cursor seq)
         : Inherited(std::move(seq))
        {}

    private:
        friend Inherited;

        template <class OtherCursor>
        assumed_finite_cursor<OtherCursor>
        rebind_base(OtherCursor seq) const;
    };

     /** @brief Тип функционального объекта для создания адаптора
    последовательности, предпалагающий, что последовательность бесконечна,
    то есть не будет исчерпана в данном контексте.
    */
    struct assume_finite_fn
    {
    public:
        /** @brief Создания @c assumed_infinite_cursor
        @param seq последовательность
        @return <tt> Result(ural::cursor_fwd<Sequence>(seq)) </tt>, где
        @c Result -- <tt> assumed_infinite_cursor<cursor_type_t<Sequence>> </tt>
        */
        template <class Sequence>
        assumed_finite_cursor<cursor_type_t<Sequence>>
        operator()(Sequence && seq) const
        {
            using Result = assumed_finite_cursor<cursor_type_t<Sequence>>;
            return Result(ural::cursor_fwd<Sequence>(seq));
        }
    };

    namespace
    {
        /** @brief Функциональный объект для создания адаптора
        последовательности, предпалагающий, что последовательность бесконечна,
        то есть не будет исчерпана в данном контексте.
        */
        constexpr auto const & assume_finite = odr_const<assume_finite_fn>;

        /** @brief Объект для создания адаптора последовательности,
        предпалагающий, что последовательность бесконечна, то есть не будет
        исчерпана в данном контексте, в функциональном стиле.
        */
        constexpr auto const & assumed_finite
            = odr_const<experimental::pipeable<assume_finite_fn>>;
    }
}
// namespace experimental
}
// namespace ural

#endif
// Z_URAL_cursor_adaptorS_ASSUMED_FINITE_HPP_INCLUDED
