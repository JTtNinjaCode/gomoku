#pragma once

#include <map>
#include <memory>
#include <utility>
#include <vector>

#include "alpha_zero.h"
#include "mcts_state.h"

// ---------------------------------------------------------------------------
// TreeNode
// ---------------------------------------------------------------------------

struct TreeNode {
  TreeNode* parent = nullptr;
  std::map<int, std::unique_ptr<TreeNode>> children;

  int n_visits = 0;
  float q_value = 0.f;
  float prior_p = 1.f;

  explicit TreeNode(TreeNode* parent, float prior_p);

  bool is_leaf() const { return children.empty(); }

  // PUCB selection: returns {best_action, best_child_ptr}.
  std::pair<int, TreeNode*> Select(float c_puct) const;

  // Expands this leaf with the given (action, prior_prob) pairs.
  void Expand(const std::vector<std::pair<int, float>>& action_priors);

  // Backs up leaf_value through the tree (sign alternates each level).
  void BackProgate(float leaf_value);

 private:
  void Update(float leaf_value);
  float PUCBScore(float c_puct) const;
};

// ---------------------------------------------------------------------------
// MCTS
// ---------------------------------------------------------------------------

class MCTS {
 public:
  float c_puct = 5.0f;
  int n_playout = 400;

  explicit MCTS(float c_puct = 5.0f, int n_playout = 400);

  // Runs one playout from a copy of state (state passed by value
  // intentionally).
  void Playout(MCTSState state, const AlphaZero& net);

  // Runs n_playout playouts and returns {action, visit_count} pairs.
  std::vector<std::pair<int, int>> GetMoveVisits(const MCTSState& state,
                                                 const AlphaZero& net);

  // Advances the root to last_move (reuses subtree). Pass -1 to reset.
  void UpdateWithMove(int last_move);

 private:
  std::unique_ptr<TreeNode> root_;
};
