#ifndef BITCOINEXCHANGE_HPP
#define BITCOINEXCHANGE_HPP

# include <map>
# include <string>

class BitCoinExchange
{
    private:
        std::map<std::string, double> _db;
        bool isValidDate(std::string const& date) const;
        bool isLeapYear(int year) const;
    public:
        BitCoinExchange();
        BitCoinExchange(BitCoinExchange const&);
        BitCoinExchange& operator=(BitCoinExchange const&);
        ~BitCoinExchange();

        void loadDataBase(std::string const&);
        void processInput(std::string const&);
};

#endif /* BITCOINEXCHANGE_HPP */
