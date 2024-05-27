#include "Agent.h"
#include "Configuration.h"

Agent::Agent()
: m_size{ 0 }, m_moveFirst{ false }, m_analyzing{ false }, m_activeMode{ false }
{}

void Agent::setSize(int size) {
    m_size = size;
}

void Agent::setMoveFirst(bool moveFirst) {
    m_moveFirst = moveFirst;
    m_activeMode = moveFirst;
}

void Agent::startAnalyzing() {}
void Agent::stopAnalyzing() {}

std::pair<int, int> Agent::getMove(State const& state) {
    std::cout << "There are " << state.getCellMetrics(EMPTY_CELL).count << " empty cells." << std::endl;
    return state.getFirstEmptyCellLocation();
}
