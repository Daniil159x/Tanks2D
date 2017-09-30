#ifndef ENUMS_HPP
#define ENUMS_HPP

enum class typeItems {
    ignoreCollize = 1000,
    breakable,
    player
};

enum class dir {
    No    = 0,
    Up    = 1 << 0,
    Down  = 1 << 1,
    Left  = 1 << 2,
    Right = 1 << 3
};

template <typename T = unsigned short>
T dir_cast(dir d){
    return static_cast<T>(d);
}

#endif // ENUMS_HPP
