#include "state/State.h"
#include "Configuration.h"
#include <stdexcept>
#include <utility>          // std::swap
using std::swap;

void State::evaluateSelfFromScratch() {
    if (m_cellType[EMPTY_CELL].count == 0) {
        m_value = 0;
        m_isTerminal = true;
        return;
    }

    static struct {
        int sequenceLengthToWin = -1;
        int sumSelfWin;
        int sumOtherWin;
    } memo; // memoization
    
    int currentSequenceLengthToWin = Configuration::getInstance().getSequenceLengthToWin();
    if (currentSequenceLengthToWin != memo.sequenceLengthToWin) {
        memo.sequenceLengthToWin = currentSequenceLengthToWin;
        memo.sumSelfWin = currentSequenceLengthToWin * SELF_CELL;
        memo.sumOtherWin = currentSequenceLengthToWin * OTHER_CELL;
    }

    int N = m_board.size() - 1;

#define CHECK_SUM(sum) { \
    if (sum == memo.sumSelfWin) { \
        m_value = 1; \
        m_isTerminal = true; \
        return; \
    } else if (sum == memo.sumOtherWin) { \
        m_value = -1; \
        m_isTerminal = true; \
        return; \
    } \
} \

    // HORIZONTAL
    for (int i = 1; i <= N; ++i) {
        int jLastStart = N - memo.sequenceLengthToWin + 1;
        int jStart = 1;
        int sum = 0;
        if (jStart <= jLastStart) {
            for (int k = 0; k < memo.sequenceLengthToWin; ++k) {
                sum += m_board[i][jStart + k];
            }
            CHECK_SUM(sum);
        }
        for (++jStart; jStart <= jLastStart; ++jStart) {
            sum -= m_board[i][jStart - 1];
            sum += m_board[i][jStart - 1 + memo.sequenceLengthToWin];
            CHECK_SUM(sum);
        }
    }

    // VERTICAL
    for (int j = 1; j <= N; ++j) {
        int iLastStart = N - memo.sequenceLengthToWin + 1;
        int iStart = 1;
        int sum = 0;
        if (iStart <= iLastStart) {
            for (int k = 0; k < memo.sequenceLengthToWin; ++k) {
                sum += m_board[iStart + k][j];
            }
            CHECK_SUM(sum);
        }
        for (++iStart; iStart <= iLastStart; ++iStart) {
            sum -= m_board[iStart - 1][j];
            sum += m_board[iStart - 1 + memo.sequenceLengthToWin][j];
            CHECK_SUM(sum);
        }
    }

    // DIAGONAL
    for (int j = N + 1 - memo.sequenceLengthToWin; j >= 1; --j) {
        int sum = 0, i = 1;
        for (int k = 0; k < memo.sequenceLengthToWin; ++k) {
            sum += m_board[i + k][j + k];
        }
        CHECK_SUM(sum);
        int jj;
        for (i = i + 1, jj = j + 1; jj <= N + 1 - memo.sequenceLengthToWin; ++i, ++jj) {
            sum -= m_board[i - 1][jj - 1];
            sum += m_board[i + memo.sequenceLengthToWin - 1][jj + memo.sequenceLengthToWin - 1];
            CHECK_SUM(sum);
        }
    }
    for (int i = 2; i <= N + 1 - memo.sequenceLengthToWin; ++i) {
        int sum = 0, j = 1;
        for (int k = 0; k < memo.sequenceLengthToWin; ++k) {
            sum += m_board[i + k][j + k];
        }
        CHECK_SUM(sum);
        int ii;
        for (j = j + 1, ii = i + 1; ii <= N + 1 - memo.sequenceLengthToWin; ++j, ++ii) {
            sum -= m_board[ii - 1][j - 1];
            sum += m_board[ii + memo.sequenceLengthToWin - 1][j + memo.sequenceLengthToWin - 1];
            CHECK_SUM(sum);
        }
    }

    // ANTI-DIAGONAL
    for (int j = memo.sequenceLengthToWin; j <= N; ++j) {
        int sum = 0, i = 1;
        for (int k = 0; k < memo.sequenceLengthToWin; ++k) {
            sum += m_board[i + k][j - k];
        }
        CHECK_SUM(sum);
        int jj;
        for (i = i + 1, jj = j - 1; jj >= memo.sequenceLengthToWin; ++i, --jj) {
            sum -= m_board[i - 1][jj + 1];
            sum += m_board[i + memo.sequenceLengthToWin - 1][jj - memo.sequenceLengthToWin + 1];
            CHECK_SUM(sum);
        }
    }
    for (int i = 2; i <= N + 1 - memo.sequenceLengthToWin; ++i) {
        int sum = 0, j = N;
        for (int k = 0; k < memo.sequenceLengthToWin; ++k) {
            sum += m_board[i + k][j - k];
        }
        CHECK_SUM(sum);
        int ii;
        for (j = j - 1, ii = i + 1; ii <= N - memo.sequenceLengthToWin + 1; --j, ++ii) {
            sum -= m_board[ii - 1][j + 1];
            sum += m_board[ii + memo.sequenceLengthToWin - 1][j - memo.sequenceLengthToWin + 1];
            CHECK_SUM(sum);
        }
    }

    m_value = 0;
    m_isTerminal = false;
#undef CHECK_SUM
}

void State::calculateCenterCellLocations() {
    int N = m_board.size() - 1;

    m_centerCellLocations = {};
    if (N % 2 != 0) {
        int c = int(N) / 2 + 1;
        m_centerCellLocations.push_back({ c, c });
    } else {
        int c = N / 2;
        m_centerCellLocations.push_back({ c, c });
        m_centerCellLocations.push_back({ c, c + 1 });
        m_centerCellLocations.push_back({ c + 1, c });
        m_centerCellLocations.push_back({ c + 1, c + 1 });
    }
}

OptimizedBoard State::loadRefereeBoard(JSON const& refereeBoard, std::string const& teamRole) {
    ///////////////////////////////////////
    /// Translate board and count cells ///
    ///////////////////////////////////////

    // std::vector<std::vector<std::string>> refereeBoard = boardFromReferee;
    int N = refereeBoard.size();
    if (N == 0) {
        throw std::runtime_error("invalid board");
    }

    OptimizedBoard b(N + 1, std::vector<CellType>(N + 1, EMPTY_CELL));
    m_cellType[SELF_CELL].count = m_cellType[OTHER_CELL].count = 0;
    std::string otherTeamRole = (teamRole == "x" ? "o" : "x");
    int i, j;
    for (i = 1; i <= N; ++i) {
        if (refereeBoard[i-1].size() != N) {
            throw std::runtime_error("invalid row in board");
        }
        for (j = 1; j <= N; ++j) {
            if (refereeBoard[i-1][j-1] == teamRole) {
                b[i][j] = SELF_CELL;
                ++m_cellType[SELF_CELL].count;
            } else if (refereeBoard[i-1][j-1] == otherTeamRole) {
                b[i][j] = OTHER_CELL;
                ++m_cellType[OTHER_CELL].count;
            } else if (refereeBoard[i-1][j-1].empty() || refereeBoard[i-1][j-1] == " ") {
            } else {
                throw std::runtime_error("unknown cell: #" + std::string(refereeBoard[i-1][j-1]) + "#");
            }
        }
    }
    m_cellType[EMPTY_CELL].count = N * N - m_cellType[SELF_CELL].count - m_cellType[OTHER_CELL].count;

    ///////////////////////////////////////
    // Calculate center cell location(s) //
    ///////////////////////////////////////

    calculateCenterCellLocations();

    ///////////////////////////////////////
    /////////// Evaluate self /////////////
    ///////////////////////////////////////

    evaluateSelfFromScratch();

    return b;
}

State::State(JSON const& boardFromReferee, std::string teamRole, bool moveFirst)
:
    m_cellType{},
    m_centerCellLocations{},
    m_moveFirst{ moveFirst },
    m_value{},
    m_isTerminal{},
    m_board{ loadRefereeBoard(boardFromReferee, teamRole) }
{}

State::State(State other, std::pair<int, int> const& actionLocation)
:
    m_cellType{ other.m_cellType },
    m_centerCellLocations{}, // will be swapped later
    m_moveFirst{ other.m_moveFirst },
    m_value{ other.m_value },
    m_isTerminal{ other.m_isTerminal },
    m_board{} // will be swapped later
{
    swap(m_centerCellLocations, other.m_centerCellLocations);
    swap(m_board, other.m_board);
    applyAction(actionLocation);
}

void State::applyAction(std::pair<int, int> const& actionLocation) {
    ///////////////////////////////////////
    /// Translate board and count cells ///
    ///////////////////////////////////////

    if (m_board[actionLocation] != EMPTY_CELL) {
        throw std::runtime_error("cell is not empty");
    }

    CellType actionCellType = (
        m_cellType[SELF_CELL].count == m_cellType[OTHER_CELL].count
        ? ( m_moveFirst ? SELF_CELL : OTHER_CELL )
        : ( m_cellType[SELF_CELL].count < m_cellType[OTHER_CELL].count ? SELF_CELL : OTHER_CELL )
    );

    m_board[actionLocation] = actionCellType;

    if (actionCellType == SELF_CELL) {
        ++m_cellType[SELF_CELL].count;
    } else if (actionCellType == OTHER_CELL) {
        ++m_cellType[OTHER_CELL].count;
    } else {
        // unreachable
    }
    --m_cellType[EMPTY_CELL].count;

    ///////////////////////////////////////
    // Calculate center cell location(s) //
    ///////////////////////////////////////

    // calculateCenterCellLocations();
    // This is not needed, because the center cell locations
    // are not changed by the action (they are merely copied
    // from the original state).

    ///////////////////////////////////////
    /////////// Evaluate self /////////////
    ///////////////////////////////////////

    evaluateSelfBasedOnLastAction(actionLocation);
}

void State::evaluateSelfBasedOnLastAction(std::pair<int, int> const& actionLocation) {
    if (m_isTerminal) return; // the original state already calculated it: once a terminal state, forever a terminal state

    if (m_cellType[EMPTY_CELL].count == 0) {
        m_value = 0;
        m_isTerminal = true;
        return;
    }

    CellType cellType = m_board[actionLocation];
    int N = m_board.size() - 1;
    int sequenceLengthToWin = Configuration::getInstance().getSequenceLengthToWin();
    int sequenceLength;

#define CHECK_SEQUENCE_LENGTH(sequenceLength) { \
    if (sequenceLength >= sequenceLengthToWin) { \
        m_value = (cellType == SELF_CELL ? 1 : -1); \
        m_isTerminal = true; \
        return; \
    } \
} \
\

    // HORIZONTAL
    sequenceLength = 1;
    int rightmost = std::min(actionLocation.second + sequenceLengthToWin - 1, N);
    for (int j = actionLocation.second + 1; j <= rightmost; ++j) {
        if (m_board[actionLocation.first][j] == cellType) {
            ++sequenceLength;
        } else {
            break;
        }
    }
    int leftmost = std::max(actionLocation.second - sequenceLengthToWin + 1, 1);
    for (int j = actionLocation.second - 1; j >= leftmost; --j) {
        if (m_board[actionLocation.first][j] == cellType) {
            ++sequenceLength;
        } else {
            break;
        }
    }
    CHECK_SEQUENCE_LENGTH(sequenceLength);

    // VERTICAL
    sequenceLength = 1;
    int topmost = std::max(actionLocation.first - sequenceLengthToWin + 1, 1);
    for (int i = actionLocation.first - 1; i >= topmost; --i) {
        if (m_board[i][actionLocation.second] == cellType) {
            ++sequenceLength;
        } else {
            break;
        }
    }
    int bottommost = std::min(actionLocation.first + sequenceLengthToWin - 1, N);
    for (int i = actionLocation.first + 1; i <= bottommost; ++i) {
        if (m_board[i][actionLocation.second] == cellType) {
            ++sequenceLength;
        } else {
            break;
        }
    }
    CHECK_SEQUENCE_LENGTH(sequenceLength);

    // DIAGONAL
    sequenceLength = 1;
    int topmostDiagonal = std::max(actionLocation.first - sequenceLengthToWin + 1, 1);
    int leftmostDiagonal = std::max(actionLocation.second - sequenceLengthToWin + 1, 1);
    for (int i = actionLocation.first - 1, j = actionLocation.second - 1; i >= topmostDiagonal && j >= leftmostDiagonal; --i, --j) {
        if (m_board[i][j] == cellType) {
            ++sequenceLength;
        } else {
            break;
        }
    }
    int bottommostDiagonal = std::min(actionLocation.first + sequenceLengthToWin - 1, N);
    int rightmostDiagonal = std::min(actionLocation.second + sequenceLengthToWin - 1, N);
    for (int i = actionLocation.first + 1, j = actionLocation.second + 1; i <= bottommostDiagonal && j <= rightmostDiagonal; ++i, ++j) {
        if (m_board[i][j] == cellType) {
            ++sequenceLength;
        } else {
            break;
        }
    }
    CHECK_SEQUENCE_LENGTH(sequenceLength);

    // ANTI-DIAGONAL
    sequenceLength = 1;
    int topmostAntiDiagonal = std::max(actionLocation.first - sequenceLengthToWin + 1, 1);
    int rightmostAntiDiagonal = std::min(actionLocation.second + sequenceLengthToWin - 1, N);
    for (int i = actionLocation.first - 1, j = actionLocation.second + 1; i >= topmostAntiDiagonal && j <= rightmostAntiDiagonal; --i, ++j) {
        if (m_board[i][j] == cellType) {
            ++sequenceLength;
        } else {
            break;
        }
    }
    int bottommostAntiDiagonal = std::min(actionLocation.first + sequenceLengthToWin - 1, N);
    int leftmostAntiDiagonal = std::max(actionLocation.second - sequenceLengthToWin + 1, 1);
    for (int i = actionLocation.first + 1, j = actionLocation.second - 1; i <= bottommostAntiDiagonal && j >= leftmostAntiDiagonal; ++i, --j) {
        if (m_board[i][j] == cellType) {
            ++sequenceLength;
        } else {
            break;
        }
    }
    CHECK_SEQUENCE_LENGTH(sequenceLength);

#undef CHECK_SEQUENCE_LENGTH
}

std::pair<int, int> State::getFirstEmptyCellLocation() const {
    for (auto const& c : cells()) {
        if (m_board[c] == EMPTY_CELL) {
            return c;
        }
    }
    throw std::runtime_error("No empty cell left");
}

bool State::operator==(State const& other) const {
    if (m_board.size() != other.m_board.size()) return false;
    for (int i = 1; i < m_board.size(); ++i) {
        // if (m_board[i].size() != other.m_board[i].size()) return false;
        for (int j = 1; j < m_board[i].size(); ++j) {
            if (m_board[i][j] != other.m_board[i][j]) return false;
        }
    }
    return true;
}

std::ostream& operator<<(std::ostream& os, State const& state) {
    for (int i = 1; i < state.m_board.size(); ++i) {
        for (int j = 1; j < state.m_board[i].size(); ++j) {
            if (j == 1) os << "|";
            os << (state.m_board[i][j] == SELF_CELL ? "x" : (state.m_board[i][j] == OTHER_CELL ? "o" : " ")) << "|";
        }
        os << std::endl;
    }
    return os;
}
