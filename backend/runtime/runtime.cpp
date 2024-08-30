
#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <utility>

#include "runtime.h"


BoolBase Field::move_robot(bool forward) {
    prev_cell = cur_cell;
    Cell tmp_cell = cur_cell;
    BoolBase status = move_from_cell(forward, tmp_cell);
    if (status != BoolBase::TRUE) std::cout << "stuck" << std::endl;
    else cur_cell = tmp_cell;
    return status;
}

Field::Field (const std::string &string) {
    
    std::ifstream fs(string);
    if (!fs.is_open()) {
        throw std::runtime_error("Error: Unable to open the file.\n");
    }

    std::string token_str;
    int cur_cell_idx = 0;
    int depth = 0;
    int width = 1;
    m_width = width;

    while (std::getline(fs, token_str)) {
        std::istringstream strstream(token_str);
        std::string cell_str;

        while (std::getline(strstream, cell_str, ' ')) {

            CellBase cur_cell_type = CellBase::EMPTY;
            int weight = 0;

            switch (cell_str[0]) {
                case 'R': 
                    cur_cell = Cell(depth, width);
                    prev_cell = cur_cell;
                    break;
                case 'B':
                    cur_cell_type = CellBase::BOX;
                    weight = atoi(cell_str.c_str()+1);
                    break;
                case 'Q':
                    cur_cell_type = CellBase::EXIT;
                    exit = Cell(depth, width);
                    break;
                case 'E':
                    cur_cell_type = CellBase::EMPTY;
                    break;
                case 'W':
                    cur_cell_type = CellBase::WALL;
                    break;
            }

            m_cells.push_back(std::pair(Cell(depth, width), Generic(cur_cell_type, weight)));
            width+=2;
            cur_cell_idx++;
        }
        depth++;

        m_width = (m_width >= width) ? m_width : width;

        width = (width%2) ? 0 : 1;
    }

    m_depth = depth;
    m_width--;

    fs.close();

    m_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un server_addr;
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, "/tmp/sock.sock");
    
    int connection_result = connect(m_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (connection_result == -1) {
        perror("Socket does not connected: ");
        close(m_socket);
        m_socket = -1;
    }
}


BoolBase Field::right() {
    int new_dir = (int) direction;
    if (direction != Orient::NORTH_WEST)
        direction = (Orient) ++new_dir;
    else
        direction = Orient::NORTH;

    return BoolBase::TRUE;
}

BoolBase Field::left() {
    int new_dir = (int) direction;
    if (direction != Orient::NORTH)
        direction = (Orient) --new_dir;
    else
        direction = Orient::NORTH_WEST;

    return BoolBase::TRUE;
}

const CellBase get_cell_type(const Cell &to_find, const Field &field) {
    for (const auto &entry : field.cells())
        if (entry.first == to_find)
            return entry.second.CellValue();

    return CellBase::UNDEF;    
}

int get_cell_weight(const Cell &to_find, const Field &field) {
    for (const auto &entry : field.cells())
        if (entry.first == to_find)
            return entry.second.weight();

    return 0; 
}

bool is_barrier (const Cell &to_look, const Field &field) {
    switch 
    (get_cell_type(to_look, field)) {
        case CellBase::BOX:
        case CellBase::WALL:
            return true;
        default:
            return false;
    }
}

BoolBase change_coords (const Field &field, Cell &cell_to_move_from, int d, int w) {
    
    std::cout << "Dir: " << (int) field.orient() << std::endl;
    int new_d = cell_to_move_from.first + d;
    std::cout << "New_d: " << new_d << std::endl;
    if (new_d >= field.depth() || new_d < 0)
        return BoolBase::UNDEF;
    int new_w = cell_to_move_from.second + w;
    std::cout << "New_w: " << new_w << std::endl;
    if (new_w >= field.width() || new_w < 0)
        return BoolBase::UNDEF;

    Cell ret = Cell(new_d, new_w);
    cell_to_move_from = ret;
    if (is_barrier(ret, field))
        return BoolBase::FALSE;

    cell_to_move_from = ret;
    return BoolBase::TRUE;
}

BoolBase move_n(const Field &field, Cell &start_cell) {
    return change_coords(field, start_cell, -2, 0);
}

BoolBase move_s(const Field &field, Cell &start_cell) {
    return change_coords(field, start_cell, 2, 0);
}

BoolBase move_nw(const Field &field, Cell &start_cell) {
    return change_coords(field, start_cell, -1, -1);
}

BoolBase move_ne(const Field &field, Cell &start_cell) {
    return change_coords(field, start_cell, -1, 1);
}

BoolBase move_se(const Field &field, Cell &start_cell) {
    return change_coords(field, start_cell, 1, 1);
}

BoolBase move_sw(const Field &field, Cell &start_cell) {
    return change_coords(field, start_cell, 1, -1);
}


BoolBase Field::move_from_cell(bool forward, Cell &start_cell) {

    switch (direction) {
        case Orient::NORTH:
            return forward ? move_n(*this, start_cell) 
                            : move_s(*this, start_cell);
        case Orient::NORTH_EAST:
            return forward ? move_ne(*this, start_cell) 
                            : move_sw(*this, start_cell);
        case Orient::SOUTH_EAST:
            return forward ? move_se(*this, start_cell) 
                            : move_nw(*this, start_cell);
        case Orient::SOUTH:
            return forward ? move_s(*this, start_cell) 
                            : move_n(*this, start_cell);
        case Orient::SOUTH_WEST:
            return forward ? move_sw(*this, start_cell) 
                            : move_ne(*this, start_cell);
        case Orient::NORTH_WEST:
            return forward ? move_nw(*this, start_cell) 
                            : move_se(*this, start_cell);
    }

    return BoolBase::FALSE;
}




CellBase Field::test() {
    Cell to_test = cur_cell;
    while (move_from_cell(1, to_test) == BoolBase::TRUE);
    return get_cell_type(to_test, *this);
}

int Field::look() {
    Cell to_look = cur_cell;
    int dist = 0;
    while (move_from_cell(1, to_look) == BoolBase::TRUE) {
        dist++;
        if (is_barrier(to_look, *this)) return dist;
    }

    return dist;
}

int limit = 14;

int Field::curWeight() {
    int sum = 0;
    for (auto e : boxes)
        sum += e;
    return sum;
}

BoolBase Field::load() {
    if (test() != CellBase::BOX)
        return BoolBase::UNDEF;

    Cell box = cur_cell;
    move_from_cell(1, box);
    int new_weight = get_cell_weight(box, *this);

    if (curWeight() + new_weight < limit) {
        boxes.push_back(new_weight);
        for (auto &entry : m_cells) {
            if (entry.first == box) {
                entry.second = Generic(CellBase::EMPTY);
                m_changed = box;
                m_weight = 0;
                return BoolBase::TRUE;
            }   
        }
        return BoolBase::UNDEF;
    }
    else {
        return BoolBase::FALSE;
    }
}

BoolBase Field::store() {
    Cell to_store = cur_cell;
    move_from_cell(1, to_store);
    if (get_cell_type(to_store, *this) != CellBase::EMPTY)
        return BoolBase::FALSE;
    
    if (to_store != exit) {
        for (auto &entry : m_cells) {
            if (entry.first == to_store) {
                entry.second = Generic(CellBase::BOX, boxes.back());
                boxes.pop_back();
                m_changed = to_store;
                m_weight = entry.second.weight();
                return BoolBase::TRUE;
            }   
        }
    }
    return BoolBase::UNDEF;
}

void 
Field::send() {
    State state{cur_cell, direction, m_changed, m_weight};
    if (m_socket != -1) {
        write(m_socket, &state, sizeof(state));
    }
    sleep(1);

    m_changed = {0,0};
    m_weight = 0;
}

Generic Field::do_oper(Oper oper) {
    Generic ret;
    switch (oper) {
        case Oper::FORWARD:
        case Oper::BACK:
            ret = move_robot(!((int)oper));
            if (ret == BoolBase::TRUE) send();
            if (cur_cell == exit) throw std::runtime_error("YOU WON");
            return ret;
        case Oper::LEFT:
            return left();
        case Oper::RIGHT:
            return right();
        case Oper::LOOK:
            return look();
        case Oper::TEST:
            return test();
        case Oper::LOAD:
            ret = load();
            if (ret == BoolBase::TRUE) send();
            return ret;
        case Oper::STORE:
            ret = store();
            if (ret == BoolBase::TRUE) send();
            return ret;
        default:
            return Generic();
    }
}
