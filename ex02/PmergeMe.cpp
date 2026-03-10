#include "PmergeMe.hpp"

#include <climits>
#include <cerrno>
#include <cstdlib>
#include <ctime>
#include <deque>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <vector>

struct VectorPair
{
    int small;
    int large;
};

struct DequePair
{
    int small;
    int large;
};

static void printVector(const std::vector<int>& v)
{
    std::size_t i = 0;

    while (i < v.size())
    {
        std::cout << v[i];
        if (i + 1 < v.size())
            std::cout << " ";
        ++i;
    }
    std::cout << std::endl;
}

static std::size_t buildJacobValue(std::size_t n)
{
    if (n == 0)
        return 0;
    if (n == 1)
        return 1;

    std::size_t prev2 = 0;
    std::size_t prev1 = 1;
    std::size_t current = 0;
    std::size_t i = 2;

    while (i <= n)
    {
        current = prev1 + 2 * prev2;
        prev2 = prev1;
        prev1 = current;
        ++i;
    }
    return current;
}

static std::vector<std::size_t> buildJacobOrderVector(std::size_t pairCount)
{
    std::vector<std::size_t> order;
    std::size_t previous;
    std::size_t index;
    std::size_t jacobIndex;
    std::size_t current;

    if (pairCount <= 1)
        return order;

    previous = 1;
    jacobIndex = 3;
    while (true)
    {
        current = buildJacobValue(jacobIndex);
        if (current > pairCount)
            break;
        index = current;
        while (index > previous)
        {
            order.push_back(index);
            --index;
        }
        previous = current;
        ++jacobIndex;
    }

    index = pairCount;
    while (index > previous)
    {
        order.push_back(index);
        --index;
    }
    return order;
}

static std::deque<std::size_t> buildJacobOrderDeque(std::size_t pairCount)
{
    std::deque<std::size_t> order;
    std::size_t previous;
    std::size_t index;
    std::size_t jacobIndex;
    std::size_t current;

    if (pairCount <= 1)
        return order;

    previous = 1;
    jacobIndex = 3;
    while (true)
    {
        current = buildJacobValue(jacobIndex);
        if (current > pairCount)
            break;
        index = current;
        while (index > previous)
        {
            order.push_back(index);
            --index;
        }
        previous = current;
        ++jacobIndex;
    }

    index = pairCount;
    while (index > previous)
    {
        order.push_back(index);
        --index;
    }
    return order;
}

static std::size_t findPartnerPosVector(const std::vector<int>& chain, int value)
{
    std::size_t i = 0;

    while (i < chain.size())
    {
        if (chain[i] == value)
            return i;
        ++i;
    }
    return chain.size();
}

static std::size_t findPartnerPosDeque(const std::deque<int>& chain, int value)
{
    std::size_t i = 0;

    while (i < chain.size())
    {
        if (chain[i] == value)
            return i;
        ++i;
    }
    return chain.size();
}

PmergeMe::PmergeMe() {}

PmergeMe::PmergeMe(const PmergeMe& other)
    : _vec(other._vec), _deq(other._deq) {}

PmergeMe& PmergeMe::operator=(const PmergeMe& other)
{
    if (this != &other)
    {
        _vec = other._vec;
        _deq = other._deq;
    }
    return *this;
}

PmergeMe::~PmergeMe() {}

int PmergeMe::parsePositiveInt(const std::string& str) const
{
    char *end;
    long value;

    if (str.empty())
        throw std::runtime_error("Error");

    errno = 0;
    end = NULL;
    value = std::strtol(str.c_str(), &end, 10);

    if (*end != '\0' || errno == ERANGE || value > INT_MAX || value <= 0)
        throw std::runtime_error("Error");
    return static_cast<int>(value);
}

std::size_t PmergeMe::binarySearchInsertPos(const std::vector<int>& arr,
    int value, std::size_t end) const
{
    std::size_t left = 0;
    std::size_t right = end;
    std::size_t mid;

    while (left < right)
    {
        mid = left + (right - left) / 2;
        if (arr[mid] < value)
            left = mid + 1;
        else
            right = mid;
    }
    return left;
}

std::size_t PmergeMe::binarySearchInsertPos(const std::deque<int>& arr,
    int value, std::size_t end) const
{
    std::size_t left = 0;
    std::size_t right = end;
    std::size_t mid;

    while (left < right)
    {
        mid = left + (right - left) / 2;
        if (arr[mid] < value)
            left = mid + 1;
        else
            right = mid;
    }
    return left;
}

std::vector<int> PmergeMe::fordJohnsonVector(const std::vector<int>& input)
{
    std::vector<VectorPair> pairs;
    std::vector<int> sortedLargeValues;
    std::vector<VectorPair> sortedPairs;
    std::vector<int> used;
    std::vector<int> chain;
    std::vector<std::size_t> order;
    std::size_t i;
    std::size_t j;
    bool hasStraggler;
    int straggler;
    VectorPair p;
    std::size_t pairIndex;
    std::size_t partnerPos;
    std::size_t insertPos;

    if (input.size() <= 1)
        return input;

    hasStraggler = false;
    straggler = 0;
    i = 0;
    while (i + 1 < input.size())
    {
        if (input[i] < input[i + 1])
        {
            p.small = input[i];
            p.large = input[i + 1];
        }
        else
        {
            p.small = input[i + 1];
            p.large = input[i];
        }
        pairs.push_back(p);
        i += 2;
    }
    if (i < input.size())
    {
        hasStraggler = true;
        straggler = input[i];
    }

    if (pairs.empty())
        return input;

    i = 0;
    while (i < pairs.size())
    {
        sortedLargeValues.push_back(pairs[i].large);
        ++i;
    }
    sortedLargeValues = fordJohnsonVector(sortedLargeValues);

    used.assign(pairs.size(), 0);
    i = 0;
    while (i < sortedLargeValues.size())
    {
        j = 0;
        while (j < pairs.size())
        {
            if (!used[j] && pairs[j].large == sortedLargeValues[i])
            {
                sortedPairs.push_back(pairs[j]);
                used[j] = 1;
                break;
            }
            ++j;
        }
        ++i;
    }

    chain.push_back(sortedPairs[0].small);
    i = 0;
    while (i < sortedPairs.size())
    {
        chain.push_back(sortedPairs[i].large);
        ++i;
    }

    order = buildJacobOrderVector(sortedPairs.size());
    i = 0;
    while (i < order.size())
    {
        pairIndex = order[i];
        partnerPos = findPartnerPosVector(chain, sortedPairs[pairIndex - 1].large);
        insertPos = binarySearchInsertPos(chain,
            sortedPairs[pairIndex - 1].small, partnerPos);
        chain.insert(chain.begin() + insertPos, sortedPairs[pairIndex - 1].small);
        ++i;
    }

    if (hasStraggler)
    {
        insertPos = binarySearchInsertPos(chain, straggler, chain.size());
        chain.insert(chain.begin() + insertPos, straggler);
    }
    return chain;
}

std::deque<int> PmergeMe::fordJohnsonDeque(const std::deque<int>& input)
{
    std::deque<DequePair> pairs;
    std::deque<int> sortedLargeValues;
    std::deque<DequePair> sortedPairs;
    std::deque<int> used;
    std::deque<int> chain;
    std::deque<std::size_t> order;
    std::size_t i;
    std::size_t j;
    bool hasStraggler;
    int straggler;
    DequePair p;
    std::size_t pairIndex;
    std::size_t partnerPos;
    std::size_t insertPos;

    if (input.size() <= 1)
        return input;

    hasStraggler = false;
    straggler = 0;
    i = 0;
    while (i + 1 < input.size())
    {
        if (input[i] < input[i + 1])
        {
            p.small = input[i];
            p.large = input[i + 1];
        }
        else
        {
            p.small = input[i + 1];
            p.large = input[i];
        }
        pairs.push_back(p);
        i += 2;
    }
    if (i < input.size())
    {
        hasStraggler = true;
        straggler = input[i];
    }

    if (pairs.empty())
        return input;

    i = 0;
    while (i < pairs.size())
    {
        sortedLargeValues.push_back(pairs[i].large);
        ++i;
    }
    sortedLargeValues = fordJohnsonDeque(sortedLargeValues);

    used.assign(pairs.size(), 0);
    i = 0;
    while (i < sortedLargeValues.size())
    {
        j = 0;
        while (j < pairs.size())
        {
            if (!used[j] && pairs[j].large == sortedLargeValues[i])
            {
                sortedPairs.push_back(pairs[j]);
                used[j] = 1;
                break;
            }
            ++j;
        }
        ++i;
    }

    chain.push_back(sortedPairs[0].small);
    i = 0;
    while (i < sortedPairs.size())
    {
        chain.push_back(sortedPairs[i].large);
        ++i;
    }

    order = buildJacobOrderDeque(sortedPairs.size());
    i = 0;
    while (i < order.size())
    {
        pairIndex = order[i];
        partnerPos = findPartnerPosDeque(chain, sortedPairs[pairIndex - 1].large);
        insertPos = binarySearchInsertPos(chain,
            sortedPairs[pairIndex - 1].small, partnerPos);
        chain.insert(chain.begin() + insertPos, sortedPairs[pairIndex - 1].small);
        ++i;
    }

    if (hasStraggler)
    {
        insertPos = binarySearchInsertPos(chain, straggler, chain.size());
        chain.insert(chain.begin() + insertPos, straggler);
    }
    return chain;
}

void PmergeMe::loadInput(int argc, char **argv)
{
    int i;
    int value;

    if (argc < 2)
        throw std::runtime_error("Error");

    _vec.clear();
    _deq.clear();

    i = 1;
    while (i < argc)
    {
        value = parsePositiveInt(argv[i]);
        _vec.push_back(value);
        _deq.push_back(value);
        ++i;
    }
}

void PmergeMe::process()
{
    std::vector<int> sortedVec;
    std::deque<int> sortedDeq;
    clock_t startVec;
    clock_t endVec;
    clock_t startDeq;
    clock_t endDeq;
    double timeVec;
    double timeDeq;

    std::cout << "Before: ";
    printVector(_vec);

    startVec = clock();
    sortedVec = fordJohnsonVector(_vec);
    endVec = clock();

    startDeq = clock();
    sortedDeq = fordJohnsonDeque(_deq);
    endDeq = clock();

    std::cout << "After:  ";
    printVector(sortedVec);

    timeVec = static_cast<double>(endVec - startVec) * 1000000.0 / CLOCKS_PER_SEC;
    timeDeq = static_cast<double>(endDeq - startDeq) * 1000000.0 / CLOCKS_PER_SEC;

    std::cout << "Time to process a range of " << _vec.size()
              << " elements with std::vector : "
              << std::fixed << std::setprecision(5)
              << timeVec << " us" << std::endl;
    std::cout << "Time to process a range of " << _deq.size()
              << " elements with std::deque  : "
              << std::fixed << std::setprecision(5)
              << timeDeq << " us" << std::endl;
}