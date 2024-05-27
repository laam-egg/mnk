#ifndef State_INCLUDED
#define State_INCLUDED

#include <utility>              // for std::pair
#include <vector>
#include "state/OptimizedBoard.h"
#include "state/CellIterator.h"
#include <nlohmann/json.hpp>
using JSON = nlohmann::json;

enum CellType : std::int8_t {
    EMPTY_CELL = 0,
    SELF_CELL,
    OTHER_CELL,
    NUM_CELL_TYPES
};

struct CellStats {
    int count{ 0 };
};

/**
 * @brief A class representing
 * board state, and supporting
 * queries also.
 * 
 * @details It is used to convert
 * from the board format used by
 * the referee server, to the optimal
 * format used by this application,
 * and vice versa. By the way, it
 * also provides methods to query the
 * board in multiple ways.
 */
class State {
public:
    State(JSON const& boardFromReferee, std::string teamRole);
    ~State() = default;

    CellStats const& getCellMetrics(CellType cellType) const { return m_cellType[cellType]; }
    std::vector<std::pair<int, int>> const& getCenterCellLocations() const { return m_centerCellLocations; }
    std::pair<int, int> getFirstEmptyCellLocation() const;
    inline bool isCellEmpty(std::pair<int, int> const& location) const { return m_board[location] == EMPTY_CELL; }
    inline bool isCellSelf(std::pair<int, int> const& location) const { return m_board[location] == SELF_CELL; }
    inline bool isCellOther(std::pair<int, int> const& location) const { return m_board[location] == OTHER_CELL; }
    inline OptimizedBoard const& getBoard() const { return m_board; }

    inline CellIterator cells() const { return CellIterator(*this); }

    bool operator==(State const& other) const;

private:
    CellStats m_cellType[NUM_CELL_TYPES];
    std::vector<std::pair<int, int>> m_centerCellLocations;

    // this board is the last one to be initialized,
    // so it must be put here, not before the other
    // private member variables !
    OptimizedBoard const m_board;

    OptimizedBoard loadRefereeBoard(JSON const& boardFromReferee, std::string const& teamRole);
};

#endif // State_INCLUDED
