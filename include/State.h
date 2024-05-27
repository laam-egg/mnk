#ifndef State_INCLUDED
#define State_INCLUDED

#include <utility>              // for std::pair
#include <vector>
#include <nlohmann/json.hpp>
using JSON = nlohmann::json;

template<typename T>
class GenericOptimizedBoard final : public std::vector<std::vector<T>> {
public:
    using std::vector<std::vector<T>>::vector;

    using std::vector<std::vector<T>>::operator[];
    T& operator[](std::pair<int, int> const& location) {
        return (*this)[location.first][location.second];
    }
    T const& operator[](std::pair<int, int> const& location) const {
        return (*this)[location.first][location.second];
    }
};

using OptimizedBoard = GenericOptimizedBoard<std::int8_t>;

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
 * @brief A class for board query.
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
