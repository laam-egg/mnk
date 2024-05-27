#include "state/CellIterator.h"
#include "state/State.h"

bool CellIterator::operator==(CellIterator const& other) const {
    if (std::addressof(m_state) != std::addressof(other.m_state)) return false;
    return m_current == other.m_current;
}

CellIterator CellIterator::begin() {
    return CellIterator(m_state);
}

CellIterator CellIterator::end() {
    auto b = CellIterator(m_state);
    b.m_current = { -1, -1 };
    return b;
}

CellIterator& CellIterator::operator++() {
    int N = m_state.getBoard().size();

    std::pair<int, int> leftTopOfVisitedSquare = m_visitedSquare_leftTop;
    std::pair<int, int> rightBottomOfVisitedSquare{
        N + 1 - leftTopOfVisitedSquare.first,
        N + 1 - leftTopOfVisitedSquare.second
    };

    switch (m_currentEdgeType) {
        case EdgeType::TOP:
            if (m_current.second < rightBottomOfVisitedSquare.second) {
                ++m_current.second;
                return *this;
            }
            m_currentEdgeType = EdgeType::RIGHT;
            [[fallthrough]];
        
        case EdgeType::RIGHT:
            if (m_current.first < rightBottomOfVisitedSquare.first) {
                ++m_current.first;
                return *this;
            }
            m_currentEdgeType = EdgeType::BOTTOM;
            [[fallthrough]];
        
        case EdgeType::BOTTOM:
            if (m_current.second > leftTopOfVisitedSquare.second) {
                --m_current.second;
                return *this;
            }
            m_currentEdgeType = EdgeType::LEFT;
            [[fallthrough]];
        
        case EdgeType::LEFT:
            [[fallthrough]];
        default:
            if (m_current.first > leftTopOfVisitedSquare.first) {
                --m_current.first;
                return *this;
            }

        // Expand to the outer square
        m_visitedSquare_leftTop = { leftTopOfVisitedSquare.first - 1, leftTopOfVisitedSquare.second - 1 };
        if (m_visitedSquare_leftTop.first < 1) {
            m_current = { -1, -1 };
        } else {
            m_current = m_visitedSquare_leftTop;
        }
        m_currentEdgeType = EdgeType::TOP;
        return *this;
    }
}

CellIterator::CellIterator(State const& state)
:
    m_state{ state },
    m_current{ state.getCenterCellLocations().front() },
    m_visitedSquare_leftTop{ m_current },
    m_currentEdgeType{ EdgeType::TOP }
{}
