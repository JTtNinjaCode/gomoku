#include "mcts.h"

#include <cmath>
#include <limits>

// ---------------------------------------------------------------------------
// TreeNode
// ---------------------------------------------------------------------------

TreeNode::TreeNode(TreeNode* parent, float prior_p)
    : parent(parent), prior_p(prior_p) {}

float TreeNode::PUCBScore(float c_puct) const {
  float u = c_puct * prior_p * std::sqrt(static_cast<float>(parent->n_visits)) /
            static_cast<float>(1 + n_visits);
  return q_value + u;
}

std::pair<int, TreeNode*> TreeNode::Select(float c_puct) const {
  int best_act = -1;
  TreeNode* best_node = nullptr;
  float best_score = -std::numeric_limits<float>::infinity();

  for (const auto& [act, child] : children) {
    float score = child->PUCBScore(c_puct);
    if (score > best_score) {
      best_score = score;
      best_act = act;
      best_node = child.get();
    }
  }

  return {best_act, best_node};
}

void TreeNode::Expand(const std::vector<std::pair<int, float>>& action_priors) {
  for (auto& [act, prob] : action_priors) {
    if (!children.count(act)) {
      children[act] = std::make_unique<TreeNode>(this, prob);
    }
  }
}

void TreeNode::Update(float leaf_value) {
  ++n_visits;
  q_value += (leaf_value - q_value) / static_cast<float>(n_visits);
}

void TreeNode::BackProgate(float leaf_value) {
  if (parent) parent->BackProgate(-leaf_value);
  Update(leaf_value);
}

// ---------------------------------------------------------------------------
// MCTS
// ---------------------------------------------------------------------------

MCTS::MCTS(float c_puct, int n_playout)
    : c_puct(c_puct),
      n_playout(n_playout),
      root_(std::make_unique<TreeNode>(nullptr, 1.f)) {}

void MCTS::Playout(MCTSState state, const AlphaZero& net) {
  TreeNode* node = root_.get();

  // Selection
  while (!node->is_leaf()) {
    auto [act, next] = node->Select(c_puct);
    state.DoMove(act);
    node = next;
  }

  // Check terminal
  auto [ended, winner] = state.GameEnd();
  float leaf_value = 0.f;

  if (!ended) {
    // Expansion
    auto [probs, value] = net.Infer(state.GetFeature());
    leaf_value = value;

    std::vector<std::pair<int, float>> action_priors;
    auto actions = state.Availables();
    action_priors.reserve(actions.size());
    for (int action : actions) action_priors.push_back({action, probs[action]});

    node->Expand(action_priors);
  } else {
    if (winner == -1) {
      leaf_value = 0.f;
    } else {
      leaf_value = (winner == state.current_player()) ? 1.f : -1.f;
    }
  }

  node->BackProgate(-leaf_value);
}

std::vector<std::pair<int, int>> MCTS::GetMoveVisits(const MCTSState& state,
                                                     const AlphaZero& net) {
  for (int i = 0; i < n_playout; ++i) {
    MCTSState copy = state;
    Playout(copy, net);
  }

  std::vector<std::pair<int, int>> act_visits;
  act_visits.reserve(root_->children.size());

  for (auto& [action, child] : root_->children)
    act_visits.push_back({action, child->n_visits});

  return act_visits;
}

void MCTS::UpdateWithMove(int last_move) {
  if (last_move != -1 && root_->children.count(last_move)) {
    auto new_root = std::move(root_->children[last_move]);
    new_root->parent = nullptr;
    root_ = std::move(new_root);
  } else {
    root_ = std::make_unique<TreeNode>(nullptr, 1.f);
  }
}
