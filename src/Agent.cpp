#include "Agent.h"
#include "Configuration.h"
#include "state/CellType.h"
#include <vector>
#include <chrono>           // std::chrono::high_resolution_clock
#include <random>           // std::random_device, std::mt19937, std::uniform_int_distribution

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

struct DEEP_COPY_STATE_t {};
struct SHALLOW_COPY_STATE_t {};
auto const DEEP_COPY_STATE = DEEP_COPY_STATE_t{};
auto const SHALLOW_COPY_STATE = SHALLOW_COPY_STATE_t{};

class Node {
private:
    bool m_owningState;

public:
    State const& state;
    Node* parent;
    std::pair<int, int> actionFromParent;
    std::vector<Node*> children;
    int numSimulations;
    int numWinsSoFar;
    int numLostsSoFar;

    Node(Node* _parent, SHALLOW_COPY_STATE_t const& _, State const& _state, std::pair<int, int> _actionFromParent)
        :
        state{ _state },
        m_owningState{ false },
        parent{ _parent },
        actionFromParent{ _actionFromParent },
        children{},
        numSimulations{ 0 },
        numWinsSoFar{ 0 },
        numLostsSoFar{ 0 }
    {}

    Node(Node* _parent, DEEP_COPY_STATE_t const& _, State const& _state, std::pair<int, int> _actionFromParent)
        :
        state{ *(new State(_state)) },
        m_owningState{ true },
        parent{ _parent },
        actionFromParent{ _actionFromParent },
        children{},
        numSimulations{ 0 },
        numWinsSoFar{ 0 },
        numLostsSoFar{ 0 }
    {}

    ~Node() {
        for (Node* child : children) {
            delete child;
        }
        if (m_owningState) {
            delete &state;
        }
    }
};

class MCTS {
private:
    Node* m_root;

public:
    MCTS(Node* root)
    :
        m_root{ root }
    {}

    using duration_t = std::chrono::duration<float>;

    std::pair<int, int> const run(duration_t const& duration, int& numSimulations) {
        int const SIMULATIONS_PER_ITERATION = 100;
        auto start = std::chrono::high_resolution_clock::now();
        int loop = 0;
        do {
            Node* current;
            for (int i = 0; i < SIMULATIONS_PER_ITERATION; ++i) {
                // std::cout << "calling select" << std::endl;
                current = select(m_root);
                // std::cout << "calling expand" << std::endl;
                current = expand(current);
                // std::cout << "calling rollout" << std::endl;
                rollout(current);
                // std::cout << "done one simulation" << std::endl;
            }
            ++loop;
        } while (
            std::chrono::duration_cast<duration_t>(
                std::chrono::high_resolution_clock::now() - start
            ) < duration
        );

        numSimulations = loop * SIMULATIONS_PER_ITERATION;

        Node* bestChildSoFar = nullptr;
        int maxNumSimulations = 0;

        for (Node* child : m_root->children) {
            if (child->numSimulations > maxNumSimulations) {
                maxNumSimulations = child->numSimulations;
                bestChildSoFar = child;
            }
        }

        if (!bestChildSoFar) {
            throw std::runtime_error("algorithm fault, absolutely no children found.");
        }
        return bestChildSoFar->actionFromParent;
    }

    Node* select(Node* node) {
        if (!node->children.empty()) {
            Node* bestChild = nullptr;
            float bestUCB = std::numeric_limits<float>::min();
            for (auto const& child : node->children) {
                float ucb = UCB(child);
                if (ucb > bestUCB) {
                    bestUCB = ucb;
                    bestChild = child;
                }
            }
            node = bestChild;
        }
        return node;
    }

    Node* expand(Node* node) {
        if (node->numSimulations == 0) {
            return node;
        }

        if (node->children.empty()) {
            for (auto const& cell : node->state.cells()) {
                if (node->state.isCellEmpty(cell)) {
                    node->children.push_back(new Node(node, DEEP_COPY_STATE, State(node->state, cell), cell));
                }
            }
        }

        // TODO: always return node->children[0];
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, node->children.size() - 1);
        return node->children[dis(gen)];
    }

    void rollout(Node* node) {
        assert(node != nullptr);
        while (!node->state.isTerminal()) {
            if (node->children.empty()) {
                thread_local std::random_device rd;
                thread_local std::mt19937 gen(rd());
                thread_local std::uniform_int_distribution<> dis(0, 9);

                std::unique_ptr<State const> statePtr{ &node->state };
                while (!statePtr->isTerminal()) {
                    for (std::pair<int, int> const& actionLocation : statePtr->cells()) {
                        if (statePtr->isCellEmpty(actionLocation) && dis(gen) < 3) {
                            if (statePtr.get() == std::addressof(node->state)) {
                                statePtr = std::make_unique<State>(*statePtr.release(), actionLocation);
                            } else {
                                statePtr = std::make_unique<State>(std::move(*statePtr), actionLocation);
                            }
                            break;
                        }
                    }
                }
                backpropagate(node, statePtr->getValue());
                return;
            } else {
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> dis(0, node->children.size() - 1);

                node = node->children[dis(gen)];
                assert(node != nullptr);
            }
        }
        backpropagate(node, node->state.getValue());
    }

    void backpropagate(Node* node, int result) {
        while (node != nullptr) {
            ++node->numSimulations;
            if (result == 1) {
                ++node->numWinsSoFar;
            } else if (result == -1) {
                ++node->numLostsSoFar;
            }
            node = node->parent;
        }
    }

    float UCB(Node const* const node) const {
        if (node->numSimulations == 0) {
            return std::numeric_limits<float>::max();
        }
        // TODO: There are rooms for optimization here
        // TODO: Take account of the number of losts
        return (float)node->numWinsSoFar / node->numSimulations + 1.41f * std::sqrt(std::log(m_root->numSimulations) / node->numSimulations);
    }
};

std::pair<int, int> Agent::getMove(State const& state) {
    // std::cout << "There are " << state.getCellMetrics(EMPTY_CELL).count << " empty cells." << std::endl;
    // return state.getFirstEmptyCellLocation();
    Node root(nullptr, SHALLOW_COPY_STATE, state, { -1, -1 });
    MCTS mcts(&root);
    float const analyzeTimeInSeconds = 1.0f;
    int numSimulations;
    std::pair<int, int> move = mcts.run(std::chrono::duration<float>(analyzeTimeInSeconds), numSimulations);
    std::cout << "Number of simulations in " << analyzeTimeInSeconds << " seconds: " << numSimulations << std::endl;
    return move;
}
