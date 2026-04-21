#pragma once

#include <string>
#include <utility>
#include <vector>

#include <torch/script.h>
#include <torch/torch.h>

class AlphaZero {
 public:
  static constexpr int kHeight   = 15;
  static constexpr int kWidth    = 15;
  static constexpr int kChannels = 9;

  // Loads the TorchScript model; moves to CUDA if available and use_gpu=true.
  // Throws std::runtime_error on load failure.
  explicit AlphaZero(const std::string& model_path, bool use_gpu = true);

  // Input: flat vector of size kChannels * kHeight * kWidth = 2025 floats.
  // Output: { action_probs (kHeight*kWidth), value_scalar }
  std::pair<std::vector<float>, float> Infer(
      const std::vector<float>& state) const;

 private:
  mutable torch::jit::script::Module module_;
  torch::Device device_;
};
