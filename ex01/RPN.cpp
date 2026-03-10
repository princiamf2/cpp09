#include "RPN.hpp"

RPN::RPN() {}
RPN::RPN(RPN const& other) : _stack(other._stack) {}
RPN& RPN::operator=(RPN const& other)
{
    if (this != &other)
        _stack = other._stack;
    return *this;
}
RPN::~RPN() {}
bool RPN::isOperator(char c) const
{
    return (c == '+' || c == '-' || c == '*' || c == '/');
}
int RPN::applyOperator(int a, int b, char op) const
{
    if (op == '+')
        return a + b;
    if (op == '-')
        return a - b;
    if (op == '*')
        return a * b;
    if (op == '/')
    {
        if (b == 0)
            throw std::runtime_error("on peut pas diviser par 0");
        return a / b;
    }
    throw std::runtime_error("operateur invalide");
}
int RPN::evaluate(std::string const& expression)
{
    std::stringstream iss(expression);
    std::string token;

    while (iss >> token)
    {
        if (token.length() == 1 && std::isdigit(token[0]))
            _stack.push(token[0] - '0');
        else if (token.length() == 1 && isOperator(token[0]))
        {
            if (_stack.size() < 2)
                throw std::runtime_error("Error.");
            int b = _stack.top();
            _stack.pop();
            int a = _stack.top();
            _stack.pop();
            int res = applyOperator(a, b, token[0]);
            _stack.push(res);
        }
        else
            throw std::runtime_error("Error.");
    }
    if (_stack.size() != 1)
        throw std::runtime_error("Error.");
    return _stack.top();
}