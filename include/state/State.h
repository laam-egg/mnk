#ifndef State_INCLUDED
#define State_INCLUDED

#include <utility>              // for std::pair
#include <vector>
#include <string>
#include <array>
#include "state/CellType.h"
#include "state/OptimizedBoard.h"
#include "state/CellIterator.h"

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
    State(std::vector<std::vector<std::string>> const& boardFromReferee, std::string teamRole, int lineLengthToWin, bool moveFirst);
    explicit State(State other, std::pair<int, int> const& actionLocation);
    State(State const& other) = default;
    State(State&& other) = default;
    State& operator=(State const& other) = default;
    State& operator=(State&& other) = default;
    ~State() = default;

    CellStats const& getCellMetrics(CellType cellType) const { return m_cellType[cellType]; }
    std::vector<std::pair<int, int>> const& getCenterCellLocations() const { return m_centerCellLocations; }
    std::pair<int, int> getFirstEmptyCellLocation() const;
    inline bool isCellEmpty(std::pair<int, int> const& location) const { return m_board[location] == EMPTY_CELL; }
    inline bool isCellSelf(std::pair<int, int> const& location) const { return m_board[location] == SELF_CELL; }
    inline bool isCellOther(std::pair<int, int> const& location) const { return m_board[location] == OTHER_CELL; }
    inline OptimizedBoard const& getBoard() const { return m_board; }
    void applyAction(std::pair<int, int> const& actionLocation);

    inline CellIterator cells() const { return CellIterator(*this); }

    inline bool isTerminal() const { return m_isTerminal; }
    inline float getValue() const { return m_value; }

    bool operator==(State const& other) const;
    inline bool operator!=(State const& other) const { return !(*this == other); }

    friend std::ostream& operator<<(std::ostream& os, State const& state);

private:
    std::array<CellStats, NUM_CELL_TYPES> m_cellType;
    std::vector<std::pair<int, int>> m_centerCellLocations;
    bool m_moveFirst;
    float m_value;
    bool m_isTerminal;
    int m_lineLengthToWin;
    // this board is the last one to be initialized,
    // so it must be put here, after all the other
    // private member variables !
    OptimizedBoard m_board;

    void calculateCenterCellLocations();

    OptimizedBoard loadRefereeBoard(std::vector<std::vector<std::string>> const& boardFromReferee, std::string const& teamRole);

    void evaluateSelfFromScratch();

    void evaluateSelfBasedOnLastAction(std::pair<int, int> const& actionLocation);
};

#endif // State_INCLUDED
