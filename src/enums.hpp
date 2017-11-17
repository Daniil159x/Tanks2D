#ifndef ENUMS_HPP
#define ENUMS_HPP

#include <QMetaEnum>
#include <QtGlobal>

// TODO: добавть всем MetaEnum

enum class typeItems {
    ignoreCollision = 1000,
    breakable,
    player,
    bullet,
    block
};


enum class motion_vector {
    No    = 0,
    Up    = 1 << 0,
    Down  = 1 << 1,
    Left  = 1 << 2,
    Right = 1 << 3
};


namespace enum_ns_impl {

    Q_NAMESPACE

    enum class complexity {
        easy, normal, crazy, non_physical
    };

    Q_ENUM_NS(complexity)
}
using complexity = enum_ns_impl::complexity;

Q_DECLARE_METATYPE(motion_vector);
//Q_DECLARE_METATYPE(complexity);

//enum_to_value
//enum_to_index
//enum_to_string<string>
//index_to_enum
//index_to_value<enum>
//index_to_string<enum, string>
//value_to_enum<enum>
//value_to_index<enum>
//value_to_string<enum, string>

//template <typename E>
//int enum_to_index(E e){
//    int idx = 0;
//    QMetaEnum metaEnum = QMetaEnum::fromType<E>();

//}



template <typename T = int, typename Enum>
inline T enum_cast(Enum e) noexcept {
    return static_cast<T>(e);
}

template <typename T = int, typename Enum>
inline Enum enum_cast(T e) noexcept {
    return static_cast<T>(e);
}

template <typename E>
const QMetaEnum& meta() noexcept {
    static QMetaEnum metaEnum = QMetaEnum::fromType<complexity>();
    return metaEnum;
}

#endif // ENUMS_HPP
