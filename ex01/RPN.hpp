#ifndef RPN_HPP
#define RPN_HPP

#include <stack>
#include <string>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <stdexcept>

class RPN
{
    private:
        std::stack<int> _stack;
        bool isOperator(char c) const;
        int applyOperator(int a, int b, char op) const;
    public:
        RPN();
        RPN(RPN const&);
        RPN& operator=(RPN const&);
        ~RPN();

        int evaluate(std::string const&);
};

#endif /* RPN_HPP */
