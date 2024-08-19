#pragma once

enum class BoolBase {
    TRUE, 
    FALSE, 
    UNDEF
};

enum class IntBase {
    INF, 
    NEG_INF, 
    NAN,
    ZERO, 
    NUM
};

enum class CellBase {
    BOX, 
    WALL, 
    EMPTY,
    EXIT,
    UNDEF
};