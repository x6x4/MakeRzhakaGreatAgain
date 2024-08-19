#pragma once

#include <cstdlib>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>
#include <map>
#include <stack>
#include <string>

#include "genericType.h"
#include "bases.h"
#include "../runtime/runtime.h"


class varTable {
std::map<std::string, GenericType> varMap;

public:
    void setVar(const std::string &name, const GenericType &value) {            
        varMap[name] = value;
    }

    auto getVar(const std::string &name) {
        auto it = varMap[name];
        return it;
    }

    void print() {
        std::cout << "\033[01;35mLocal vars:\033[00m" << std::endl;
        for (const auto &entry : varMap) {
            std::cout << entry.first << ' ';
            std::cout << entry.second;
            std::cout << std::endl;
        }
    }

};

// Define the abstract syntax tree (AST) nodes
class ASTNode {
public:
    virtual GenericType eval(varTable&) { return GenericType(); }
};

typedef std::vector<ASTNode*> nodeList;

inline GenericType GenericRet() {
    auto tmp = GenericType(); tmp.setRet(); 
    return tmp;
}

class RetNode : public ASTNode {
public:
    GenericType eval(varTable&) override { return GenericRet(); }
};

class OperNode : public ASTNode {

    Field &m_field;
    Oper m_oper;

public:
    OperNode(Oper oper, Field &field) :
    m_field(field), m_oper(oper) {}
    GenericType eval(varTable&) { return m_field.do_oper(m_oper); }
};

class NumberNode : public ASTNode {
public:
    NumberNode() {}
    NumberNode(const GenericType &value) : m_value(value) {}
    NumberNode(const GenericType &value, const std::string &name) 
    : m_value(value), m_name(name) {}

    GenericType eval(varTable& vm) override { 
        if (m_name.empty())  //  immediate
            return GenericType(m_value); 

        //  variable
        return vm.getVar(m_name);
    }
private:
    GenericType m_value;
    std::string m_name = std::string();
};

class BinaryOpNode : public ASTNode {
public:
    BinaryOpNode(char op, ASTNode* left, ASTNode* right) : op(op), left(left), right(right) {}
    GenericType eval(varTable& vm) override {
        GenericType a;
        switch (op) {
            case '+': return left->eval(vm) + right->eval(vm);
            case '-': return left->eval(vm) - right->eval(vm);
            case '^': return left->eval(vm) ^ right->eval(vm);
            case '<': return left->eval(vm) < right->eval(vm);
            case '>': 
                return left->eval(vm) > right->eval(vm);
            case '=': return left->eval(vm) == right->eval(vm);
            default: throw std::runtime_error("Invalid binary operator");
        }
    }
private:
    char op;
    ASTNode* left;
    ASTNode* right;
};

class UnaryMinusNode : public ASTNode {
public:
    UnaryMinusNode(ASTNode* operand) : operand(operand) {}
    GenericType eval(varTable& vm) override { return operand->eval(vm); }
private:
    ASTNode* operand;
};

typedef std::pair<CurType, std::string> Signature;
typedef std::function<GenericType(GenericType, nodeList&, Signature)> FuncLambda;
typedef std::pair<FuncLambda, nodeList> Body;
typedef std::pair<Signature, Body> FuncDtor;

class funcTable;

class FunctionCallNode : public ASTNode {
public:
    FunctionCallNode(const std::string &name, ASTNode *arg, 
    funcTable &table);
    GenericType eval(varTable& vm) override;

private:
    FuncDtor m_fdtor;
    funcTable &m_table;
    ASTNode *m_arg;
    std::string m_name;
};

class funcTable {
std::map<std::string, FuncDtor> funcMap;

public:
    void setFuncSign(std::string name, Signature sign) {
        funcMap[name] = std::make_pair(sign, std::make_pair(FuncLambda(), nodeList()));
    }
    void setFuncBody(std::string name, Body body) {
        auto it = funcMap.find(name);
        if (it == funcMap.end()) {
            throw std::runtime_error("Function not found: " + name);
        }
        funcMap[name].second = body;
    }

    auto getFunc(const std::string &name) {
        auto it = funcMap.find(name);

        if (it == funcMap.end()) {
            throw std::runtime_error("Function not found: " + name);
        }

        return it->second;
    }

    void print() {
        std::cout << "\033[01;35mFunc map:\033[00m" << std::endl;
        for (const auto &entry : funcMap) {
            std::cout << entry.first << ' ';
            std::cout << std::endl;
        }
    }    
};

class AssignNode : public ASTNode {

public:
    AssignNode(const std::string &name, ASTNode* arg) : m_name(name), m_arg(arg) {}
    GenericType eval(varTable& vm) override {
        // Look up the variable in the variable table
        vm.setVar(m_name, m_arg->eval(vm));
        return GenericType();
    }

    std::string m_name;
    ASTNode* m_arg;
};

class WhileNode : public ASTNode {
public:
    WhileNode(ASTNode* condition, nodeList body, nodeList finish) 
    : m_condition(condition), m_body(body), m_finish(finish) {}

    WhileNode(ASTNode* condition, nodeList body) 
    : m_condition(condition), m_body(body) {}

    GenericType eval(varTable& vm) override {

        if (m_condition->eval(vm).BoolValue() == BoolBase::UNDEF) 
            return GenericType();

        while (m_condition->eval(vm).BoolValue() == BoolBase::TRUE) {            
            for (auto &stmt : m_body) {
                if (stmt->eval(vm).getRet()) return GenericRet();
            }
        }

        for (auto &stmt : m_finish)
            if (stmt->eval(vm).getRet()) return GenericRet();

        return GenericType(); // Return a dummy value
    }
private:
    ASTNode* m_condition;
    nodeList m_body;
    nodeList m_finish;
};

class IfNode : public ASTNode {
public:
    IfNode(ASTNode* condition, nodeList trueBlock, nodeList eldefBlock, nodeList elundBlock)
        : condition(condition), 
        m_true(trueBlock), m_eldef(eldefBlock), m_elund(elundBlock) {}

    IfNode(ASTNode* condition, nodeList trueBlock) 
        : condition(condition), m_true(trueBlock) {}

    GenericType eval(varTable& vm) override {
        switch (condition->eval(vm).BoolValue()) {
            case BoolBase::TRUE:
                for (auto &stmt : m_true) {
                    if (stmt->eval(vm).getRet()) return GenericRet();
                }
                break;
            case BoolBase::FALSE:
                for (auto &stmt : m_eldef) {
                    if (stmt->eval(vm).getRet()) return GenericRet();
                }
                break;
            case BoolBase::UNDEF:
                for (auto &stmt : m_elund) {
                    if (stmt->eval(vm).getRet()) return GenericRet();
                }
                break;
        }
        return GenericType(); // Return a dummy value
    }
private:
    ASTNode* condition;
    nodeList m_true;
    nodeList m_eldef;
    nodeList m_elund;
};

