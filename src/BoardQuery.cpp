#include "BoardQuery.h"

OptimizedBoard BoardQuery::loadRefereeBoard(JSON const& boardFromReferee, std::string const& teamRole) {
    std::vector<std::vector<std::string>> refereeBoard = boardFromReferee;
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
                throw std::runtime_error("unknown cell: #" + refereeBoard[i-1][j-1] + "#");
            }
        }
    }
    m_cellType[EMPTY_CELL].count = N * N - m_cellType[SELF_CELL].count - m_cellType[OTHER_CELL].count;

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

BoardQuery::BoardQuery(JSON const& boardFromReferee, std::string teamRole)
:
    m_cellType{},
    m_centerCellLocations{},
    m_board{ loadRefereeBoard(boardFromReferee, teamRole) }
{}

std::pair<int, int> BoardQuery::getFirstEmptyCellLocation() const {
    for (auto const& c : getCenterCellLocations()) {
        if (m_board[c] == EMPTY_CELL) {
            return c;
        }
    }
    auto leftTopOfVisitedSquare = getCenterCellLocations().front();
    auto rightBottomOfVisitedSquare = getCenterCellLocations().back();

    while (leftTopOfVisitedSquare.first >= 2) {
        --leftTopOfVisitedSquare.first;
        --leftTopOfVisitedSquare.second;
        ++rightBottomOfVisitedSquare.first;
        ++rightBottomOfVisitedSquare.second;

        // TOP EDGE
        auto c = leftTopOfVisitedSquare;
        for (; c.second < rightBottomOfVisitedSquare.second; ++c.second) {
            if (m_board[c] == EMPTY_CELL) return c;
        }
        // RIGHT EDGE
        for (; c.first < rightBottomOfVisitedSquare.first; ++c.first) {
            if (m_board[c] == EMPTY_CELL) return c;
        }
        // BOTTOM EDGE
        for (; c.second > leftTopOfVisitedSquare.second; --c.second) {
            if (m_board[c] == EMPTY_CELL) return c;
        }
        // LEFT EDGE
        for (; c.first > leftTopOfVisitedSquare.first; --c.first) {
            if (m_board[c] == EMPTY_CELL) return c;
        }
    }
    // No empty cell
    throw std::runtime_error("No empty cell left");
}
