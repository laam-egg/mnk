#include "state/State.h"

OptimizedBoard State::loadRefereeBoard(JSON const& refereeBoard, std::string const& teamRole) {
    ///////////////////////////////////////
    /// Translate board and count cells ///
    ///////////////////////////////////////

    // std::vector<std::vector<std::string>> refereeBoard = boardFromReferee;
    int N = refereeBoard.size();
    if (N == 0) {
        throw std::runtime_error("invalid board");
    }
    OptimizedBoard b(N + 1, std::vector<std::int8_t>(N + 1, EMPTY_CELL));
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

    return b;
}

State::State(JSON const& boardFromReferee, std::string teamRole)
:
    m_cellType{},
    m_centerCellLocations{},
    m_board{ loadRefereeBoard(boardFromReferee, teamRole) }
{}

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
