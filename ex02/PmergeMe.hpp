#ifndef PMERGEME_HPP
#define PMERGEME_HPP

#include <vector>
#include <deque>
#include <string>

class PmergeMe
{
    private:
        std::vector<int> _vec;
        std::deque<int>  _deq;

        int parsePositiveInt(const std::string& str) const;

        std::vector<int> fordJohnsonVector(const std::vector<int>& input);
        std::deque<int>  fordJohnsonDeque(const std::deque<int>& input);

        std::size_t binarySearchInsertPos(const std::vector<int>& arr, int value,
            std::size_t end) const;
        std::size_t binarySearchInsertPos(const std::deque<int>& arr, int value,
            std::size_t end) const;

    public:
        PmergeMe();
        PmergeMe(const PmergeMe& other);
        PmergeMe& operator=(const PmergeMe& other);
        ~PmergeMe();

        void loadInput(int argc, char **argv);
        void process();
};

#endif