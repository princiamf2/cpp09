#include "BitcoinExchange.hpp"
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>

BitCoinExchange::BitCoinExchange() {}

BitCoinExchange::BitCoinExchange(BitCoinExchange const& other)
    : _db(other._db) {}

BitCoinExchange& BitCoinExchange::operator=(BitCoinExchange const& other)
{
    if (this != &other)
        _db = other._db;
    return *this;
}

BitCoinExchange::~BitCoinExchange() {}

static std::string trim(std::string const& s)
{
    std::string::size_type start = 0;
    std::string::size_type end = s.size();

    while (start < s.size()
        && std::isspace(static_cast<unsigned char>(s[start])))
        start++;
    while (end > start
        && std::isspace(static_cast<unsigned char>(s[end - 1])))
        end--;
    return s.substr(start, end - start);
}

static double parseDouble(std::string const& s)
{
    char *end = NULL;
    double value = std::strtod(s.c_str(), &end);

    if (end == s.c_str())
        throw std::runtime_error("invalid number");
    while (*end)
    {
        if (!std::isspace(static_cast<unsigned char>(*end)))
            throw std::runtime_error("invalid number");
        ++end;
    }
    return value;
}

bool BitCoinExchange::isLeapYear(int year) const
{
    if (year % 400 == 0)
        return true;
    if (year % 100 == 0)
        return false;
    return (year % 4 == 0);
}

bool BitCoinExchange::isValidDate(std::string const& date) const
{
    int year;
    int month;
    int day;
    int daysInMonth[12];

    if (date.size() != 10)
        return false;
    if (date[4] != '-' || date[7] != '-')
        return false;

    for (std::string::size_type i = 0; i < date.size(); i++)
    {
        if (i == 4 || i == 7)
            continue;
        if (!std::isdigit(static_cast<unsigned char>(date[i])))
            return false;
    }

    year = std::atoi(date.substr(0, 4).c_str());
    month = std::atoi(date.substr(5, 2).c_str());
    day = std::atoi(date.substr(8, 2).c_str());

    if (month < 1 || month > 12)
        return false;
    if (day < 1)
        return false;

    daysInMonth[0] = 31;
    daysInMonth[1] = 28;
    daysInMonth[2] = 31;
    daysInMonth[3] = 30;
    daysInMonth[4] = 31;
    daysInMonth[5] = 30;
    daysInMonth[6] = 31;
    daysInMonth[7] = 31;
    daysInMonth[8] = 30;
    daysInMonth[9] = 31;
    daysInMonth[10] = 30;
    daysInMonth[11] = 31;

    if (isLeapYear(year))
        daysInMonth[1] = 29;
    if (day > daysInMonth[month - 1])
        return false;
    return true;
}

void BitCoinExchange::loadDataBase(std::string const& csvFile)
{
    std::ifstream       in(csvFile.c_str());
    std::string         line;
    std::string         date;
    std::string         rateStr;
    std::string::size_type comma;
    double              rate;

    if (!in)
        throw std::runtime_error("Error: could not open database file.");

    if (!std::getline(in, line))
        throw std::runtime_error("Error: empty database file.");

    while (std::getline(in, line))
    {
        if (line.empty())
            continue;
        comma = line.find(',');
        if (comma == std::string::npos)
            continue;

        date = trim(line.substr(0, comma));
        rateStr = trim(line.substr(comma + 1));

        if (!isValidDate(date) || rateStr.empty())
            continue;
        try
        {
            rate = parseDouble(rateStr);
        }
        catch (const std::exception&)
        {
            continue;
        }
        _db[date] = rate;
    }
    if (_db.empty())
        throw std::runtime_error("Error: database loaded but empty.");
}

void BitCoinExchange::processInput(std::string const& filename)
{
    std::ifstream in(filename.c_str());
    std::string line;
    std::string date;
    std::string valueStr;
    std::string::size_type pipe;
    double value;
    std::map<std::string, double>::iterator it;

    if (!in)
        throw std::runtime_error("Error: could not open file.");

    if (!std::getline(in, line))
        throw std::runtime_error("Error: empty file.");

    if (trim(line) != "date | value")
        std::cerr << "Warning: unexpected header => " << line << std::endl;

    while (std::getline(in, line))
    {
        if (line.empty())
            continue;

        pipe = line.find('|');
        if (pipe == std::string::npos)
        {
            std::cerr << "Error: bad input => " << line << std::endl;
            continue;
        }

        date = trim(line.substr(0, pipe));
        valueStr = trim(line.substr(pipe + 1));

        if (!isValidDate(date))
        {
            std::cerr << "Error: bad input => " << line << std::endl;
            continue;
        }

        try
        {
            value = parseDouble(valueStr);
        }
        catch (const std::exception&)
        {
            std::cerr << "Error: bad input => " << line << std::endl;
            continue;
        }

        if (value < 0)
        {
            std::cerr << "Error: not a positive number." << std::endl;
            continue;
        }

        if (value > 1000)
        {
            std::cerr << "Error: too large a number." << std::endl;
            continue;
        }

        it = _db.lower_bound(date);

        if (it == _db.end())
            --it;
        else if (it->first != date)
        {
            if (it == _db.begin())
            {
                std::cerr << "Error: bad input => " << line << std::endl;
                continue;
            }
            --it;
        }

        std::cout << date << " => " << value
                  << " = " << (value * it->second) << std::endl;
    }
}