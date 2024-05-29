#ifndef OptimizedBoard_INCLUDED
#define OptimizedBoard_INCLUDED

#include <cstdint>
#include <vector>
#include <utility>      // std::pair
#include "state/CellType.h"

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
using OptimizedBoard = GenericOptimizedBoard<CellType>;

#endif // OptimizedBoard_INCLUDED
