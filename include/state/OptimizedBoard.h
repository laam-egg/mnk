#ifndef OptimizedBoard_INCLUDED
#define OptimizedBoard_INCLUDED

#include <cstdint>
#include <vector>
#include <utility>      // std::pair

template<typename T>
class GenericOptimizedBoard final : public std::vector<std::vector<T>> {
public:
    using std::vector<std::vector<T>>::vector;

    using std::vector<std::vector<T>>::operator[];
    inline T& operator[](std::pair<int, int> const& location) {
        return (*this)[location.first][location.second];
    }
    inline T const& operator[](std::pair<int, int> const& location) const {
        return (*this)[location.first][location.second];
    }
};

/**
 * The purpose of this class
 * is to represent the board
 * in memory to accelerate
 * internal processing of
 * states.
 */
using OptimizedBoard = GenericOptimizedBoard<std::int8_t>;

#endif // OptimizedBoard_INCLUDED
