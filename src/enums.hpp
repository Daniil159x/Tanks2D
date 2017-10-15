#ifndef ENUMS_HPP
#define ENUMS_HPP

#include <QMetaEnum>

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

Q_DECLARE_METATYPE(motion_vector);

template <typename T = int>
T motion_vector_cast(motion_vector d){
    return static_cast<T>(d);
}

template <typename T = int>
motion_vector motion_vector_cast(T d){
    return static_cast<motion_vector>(d);
}

#endif // ENUMS_HPP
