#pragma once

#include <iostream>
#include <ostream>
#include <utility>
#include <vector>
#include "bases.h"


enum class CurType {
    INT, 
    BOOL, 
    CELL
};


typedef std::pair<IntBase, int> Integer;

class Generic {
    CurType m_type = CurType::INT;
    IntBase m_int = IntBase::NAN;
    int m_value = 0;

public:

    CurType type() const { return m_type; }
    void setType(CurType type) { m_type = type; }

    int weight() const {return m_value;}
    Integer IntValue() const { return std::make_pair(m_int, m_value); }
    BoolBase BoolValue() const {
        switch (m_int) {
            case IntBase::ZERO:
                return BoolBase::FALSE;
            case IntBase::NAN:
                return BoolBase::UNDEF;
            default:
                return BoolBase::TRUE;
        }
    }
    CellBase CellValue() const {
       switch (m_int) {
            case IntBase::ZERO:
                return CellBase::EMPTY;
            case IntBase::INF:
                return CellBase::WALL;
            case IntBase::NEG_INF:
                return CellBase::EXIT;
            case IntBase::NAN:
                return CellBase::UNDEF;
            default:
                return CellBase::BOX;
        }
    }

//  CTORS

    Generic() {}

//  int
    Generic(int new_value) : m_type(CurType::INT), m_value(new_value) {
        switch (new_value) {
            case 0:
                m_int = IntBase::ZERO;
                break;
            default:
                m_int = IntBase::NUM;
        }
    }

    Generic(bool new_val) 
    : m_type(CurType::BOOL), m_value(new_val) {
        m_int = new_val ? IntBase::NUM : IntBase::ZERO;
    }

    Generic(IntBase type) : m_type(CurType::INT), m_int(type) {}

//  cell
    Generic(CellBase type, int weight = 0) : m_type(CurType::CELL), m_value(weight) {
        switch (type) {
            case CellBase::EMPTY:
                m_int = IntBase::ZERO;
                break;
            case CellBase::WALL:
                m_int = IntBase::INF;
                break;
            case CellBase::EXIT:
                m_int = IntBase::NEG_INF;
                break;
            case CellBase::UNDEF:
                m_int = IntBase::NAN;
                break;
            case CellBase::BOX:
                m_int = IntBase::NUM;
        }
    }

//  bool
    Generic(BoolBase type) : m_type(CurType::BOOL) {
        if (type == BoolBase::TRUE) {
            m_int = IntBase::NUM;
            m_value = 1;
        }
        else if (type == BoolBase::FALSE)
            m_int = IntBase::ZERO;
        else {
            m_int = IntBase::NAN; 
        }
    }

//  operators

    Generic operator+(const Generic& other) const {
        return m_value + other.m_value;
    }

    void operator+=(const Generic& other) {
        m_value += other.m_value;
    }

    Generic operator-(const Generic& other) const {
        return m_value - other.m_value;
    }

    Generic operator-() {
        return -m_value;
    }

    bool operator>(const Generic& other) const {
        return m_value > other.m_value;
    }

    bool operator<(const Generic& other) const {
        return m_value < other.m_value;
    }

    bool operator==(const Generic& other) const {
        return m_value == other.m_value;
    }

    Generic operator^(const Generic& other) const {
        return m_value ^ other.m_value;
    }

    friend std::ostream& operator<<(std::ostream& os, const Generic& integer)  {
        
        switch (integer.m_type) {
            case CurType::INT:
                switch (integer.m_int) {
                    case IntBase::INF:
                        os << "inf";
                        break;
                    case IntBase::NEG_INF:
                        os << "ninf";
                        break;
                    case IntBase::NAN:
                        os << "nan";
                        break;
                    default:
                        os << integer.m_value; 
                }
            break;
            case CurType::BOOL:
                switch (integer.m_int) {
                    case IntBase::ZERO:
                        os << "false";
                        break;
                    case IntBase::NAN:
                        os << "undef";
                        break;
                    default:
                        os << "true"; 
                }
            break;
            case CurType::CELL:
                switch (integer.m_int) {
                    case IntBase::INF:
                        os << "wall";
                        break;
                    case IntBase::NEG_INF:
                        os << "exit";
                        break;
                    case IntBase::ZERO:
                        os << "empty";
                        break;
                    case IntBase::NAN:
                        os << "undef";
                        break;
                    default:
                        os << "box " << integer.m_value; 
                }
        }

        return os; 
    }

};


class GenericType {
    std::vector<Generic> m_base;
    CurType m_type;
    bool is_return = 0;

public:
    void setRet() { is_return = 1; }
    bool getRet() const{ return is_return;}


    GenericType() {}
    GenericType(Generic first) {
        m_base.push_back(first);
        m_type = first.type();
    }
    GenericType(CurType type) : m_type(type) {}
    CurType getType() const {
        return m_type;
    }
    void setType(CurType type) {
        for (auto &e : m_base) {
            e.setType(type);
        }
    }

    BoolBase BoolValue() const {

        if (!m_base.size()) 
            return BoolBase::FALSE;
        if (m_base.size() > 1)
            return BoolBase::TRUE;
        
        return m_base[0].BoolValue();
    }

    friend std::ostream& operator<<(std::ostream& os, const GenericType& a)  {
        for (const auto &e : a.m_base) {
            os << e << ' ';
        }
        return os;
    }

    GenericType operator+(const GenericType &other) {
        int size = m_base.size() > other.m_base.size() 
        ? m_base.size() : other.m_base.size();
        GenericType new_vec;
        new_vec = other;
        for (int i = 0; i < m_base.size(); i++) {
            new_vec.m_base[i] += m_base.at(i);
        }

        return new_vec;
    }

    GenericType operator-() const {
        GenericType new_vec = *this;
        for (auto &e : new_vec.m_base) {
            e = -e;
        }
        return new_vec;
    }

    GenericType operator-(const GenericType &other) {
        return *this + -other;
    }

    GenericType operator==(const GenericType &other) {
        return Generic(this->m_base == other.m_base);
    }

    GenericType operator>(const GenericType &other) {
        return Generic(this->m_base > other.m_base);
    }

    GenericType operator<(const GenericType &other) {
        return Generic(this->m_base < other.m_base);
    }

    GenericType operator^(const GenericType &other) {
        return Generic(BoolValue() != other.BoolValue());
    }

};


/*    BoolBase CellToBool (CellBase val) {
        switch (val) {
            case CellBase::EXIT:
            case CellBase::EMPTY:
                return BoolBase::TRUE;
            case CellBase::WALL:
            case CellBase::BOX:
                return BoolBase::FALSE;
            default:
                return BoolBase::UNDEF;
        }
    }*/

    //  Bool to Cell - semantic error!

