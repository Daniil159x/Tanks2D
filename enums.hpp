#ifndef ENUMS_HPP
#define ENUMS_HPP

#include <QMetaEnum>

enum class typeItems {
    ignoreCollize = 1000,
    breakable,
    player,
    bullet
};


enum class dir {
    No    = 0,
    Up    = 1 << 0,
    Down  = 1 << 1,
    Left  = 1 << 2,
    Right = 1 << 3
};

Q_DECLARE_METATYPE(dir);

template <typename T = unsigned short>
T dir_cast(dir d){
    return static_cast<T>(d);
}

#endif // ENUMS_HPP
