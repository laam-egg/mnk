#ifndef CellIterator_INCLUDED
#define CellIterator_INCLUDED

#include "state/OptimizedBoard.h"

class State;

/**
 * @brief An iterator that iterates over all cells in a state, from the center cells to the outermost ones.
 * 
 * @details This is commonly required when the agent want to
 * get the first empty cell, in which case he might
 * want to do the following:
 * 
 * @code
 * // for (auto cell : CellIterator::begin(state)) {
 * // or even better:
 * for (auto cell : state.cells()) {
 *    if (state.get(cell) == EMPTY_CELL) {
 *       // Do something with the empty cell
 *   }
 * }
 */
class CellIterator {
public:
    CellIterator(State const& state);
    ~CellIterator() = default;

    using iterator = CellIterator;

    bool operator==(CellIterator const& other) const;
    inline bool operator!=(CellIterator const& other) const { return !(m_current == other.m_current); }

    CellIterator begin();
    CellIterator end();

    inline std::pair<int, int> operator*() const { return m_current; }
    CellIterator& operator++();
    inline CellIterator operator++() const {
        CellIterator tmp(*this);
        ++(*this);
        return tmp;
    }
    
    CellIterator(CellIterator const&) = default;
    CellIterator& operator=(CellIterator const&) = default;
    CellIterator(CellIterator&&) = default;

private:
    enum class EdgeType {
        TOP = 0, RIGHT, BOTTOM, LEFT, NUM_EDGE_TYPES
    };

    State const& m_state;
    std::pair<int, int> m_current;
    std::pair<int, int> m_visitedSquare_leftTop;
    EdgeType m_currentEdgeType;
};

#endif // CellIterator_INCLUDED
