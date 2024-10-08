#pragma once
#include <string>
#include <utility>
#include <vector>
#include "../ast/genericType.h"


enum class Orient {
    NORTH, 
    NORTH_EAST,
    SOUTH_EAST,
    SOUTH,
    SOUTH_WEST,
    NORTH_WEST
};

enum class Oper {
    FORWARD, 
    BACK, 
    LEFT, 
    RIGHT, 
    LOOK, 
    TEST, 
    LOAD, 
    STORE
};

typedef std::pair <int, int> Cell;

struct State {
    Cell cell;
    Orient direction;
    Cell changed;
    int weight;
};

class Field {

    Orient direction = Orient::NORTH;
    Cell cur_cell;
    Cell prev_cell;
    Cell exit;
    int m_width;
    int m_depth;

    //  box
    Cell m_changed = {0, 0};
    int m_weight = 0;

    std::vector<std::pair<Cell, Generic>> m_cells;

public:

    void print() {
        for (const auto &e : m_cells) {
            std::cout << e.second << ' ' << e.first.first << e.first.second << ' ';
        }
        std::cout << std::endl;
    }

    void send();

    const auto cells()  const {return m_cells;}
    const auto orient() const {return direction;}
    const auto prev()   const {return prev_cell;}
    const auto cur()    const {return cur_cell;}
    const auto depth()  const {return m_depth;}
    const auto width()  const {return m_width;}

    Field (const std::string &string);

    Generic do_oper(Oper oper);

private:

    BoolBase move_robot(bool forward);
    BoolBase right();
    BoolBase left();
    CellBase test();
    int look();
    BoolBase load();
    BoolBase store();
    int curWeight();

    BoolBase move_from_cell(bool forward, Cell &start_cell);
    int m_socket = 0;
    std::vector<int> boxes;
};