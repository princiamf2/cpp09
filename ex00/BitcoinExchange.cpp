#include "BitcoinExchange.hpp"
#include <cstddef>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstdlib> 
#include <cctype>
#include <string>

BitCoinExchange::BitCoinExchange() {}
BitCoinExchange::BitCoinExchange(BitCoinExchange const& other) : _db(other._db) {}
BitCoinExchange& BitCoinExchange::operator=(BitCoinExchange const& other)
{
    if (this != &other)
        _db = other._db;
    return *this;
}
BitCoinExchange::~BitCoinExchange() {}
static std::string trim(std::string const& s)
{
    size_t start = 0;
    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start])))
        start++;
    size_t end = s.size();
    while (end > start && std::isspace(static_cast<unsigned char>(s[end])))
        end--;
    return s.substr(start, end - start);
}
static double parseDouble(std::string const& s)
{
    char *end = 0;
    const char *cstr = s.c_str();
    double v = std::strtod(cstr, &end);

    if (end == cstr)
        throw std::runtime_error("Invalid rate (not a number)");
    while (*end)
    {
        if (!std::isspace(static_cast<unsigned char>(*end)))
            throw std::runtime_error("Invalid rate (trailing garbage)");
        ++end;
    }
    return v;
}

void BitCoinExchange::loadDataBase(std::string const& csvFile)
{
    std::ifstream in(csvFile.c_str());
    if (!in)
        throw std::runtime_error("Error: could not open database file");
    std::string line;
    if (!std::getline(in, line))
        throw std::runtime_error("Error: empty database file");
    while (std::getline(in, line))
    {
        if (line.empty())
            continue;
        size_t comma = line.find(',');
        if (comma == std::string::npos)
            continue;
        std::string date = trim(line.substr(0, comma));
        std::string rateStr = trim(line.substr(comma + 1));
        if (date.empty() || rateStr.empty())
            continue;
        double rate = parseDouble(rateStr);
        if (date.size() != 10)
            continue;
        _db[date] = rate;
    }
    if (_db.empty())
            throw std::runtime_error("Error; database loaded but empty");
}