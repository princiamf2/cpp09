#include "BitcoinExchange.hpp"
#include <iostream>

int main()
{
    try
    {
        BitCoinExchange btc;
        btc.loadDataBase("data.csv");
        std::cout << "Database loaded OK\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << "\n";
        return 1;
    }
    return 0;
}