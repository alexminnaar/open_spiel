// Copyright 2019 DeepMind Technologies Ltd. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef THIRD_PARTY_OPEN_SPIEL_GAMES_HANABI_H_
#define THIRD_PARTY_OPEN_SPIEL_GAMES_HANABI_H_

// Hanabi is a cooperative card game, described here:
// https://en.wikipedia.org/wiki/Hanabi_(card_game)
//
// See https://arxiv.org/abs/1902.00506 for a motivation of Hanabi as an AI
// challenge and some initial results. Please cite this paper if you use this
// Hanabi wrapper for any research results.
//
// This implementation is a wrapper for the Hanabi Learning Environment, which
// can be found here: https://github.com/deepmind/hanabi-learning-environment
//
// Since Hanabi relies on an (optional) external dependency, it is not included
// in the list of compiled games by default. To enable it, follow these steps:
//   1. git clone the Hanabi Learning Environment above (e.g. in $HOME)
//   2. Set the path of HANABI_HOME in games/hanabi/CMakeLists.txt
//   3. Uncomment hanabi.cc, hanabi.h, and the add_subdirectory (hanabi) in
//      games/CMakeLists.txt
//   4. Uncomment the $<TARGET_OBJECTS:hanabi_learning_environment> in the
//      top-level CMakeLists.txt
//   5. Enjoy the fireworks!

#include <memory>

#include "open_spiel/spiel.h"
#include "hanabi_lib/canonical_encoders.h"
#include "hanabi_lib/hanabi_game.h"
#include "hanabi_lib/hanabi_state.h"

namespace open_spiel {
namespace hanabi {

class OpenSpielHanabiGame : public Game {
 public:
  explicit OpenSpielHanabiGame(const GameParameters& params);
  int NumDistinctActions() const override;
  std::unique_ptr<State> NewInitialState() const override;
  int MaxChanceOutcomes() const override;
  int NumPlayers() const override;
  double MinUtility() const override;
  double MaxUtility() const override;
  std::shared_ptr<const Game> Clone() const override;
  std::vector<int> ObservationNormalizedVectorShape() const override;
  int MaxGameLength() const override;

  const hanabi_learning_env::ObservationEncoder& Encoder() const {
    return encoder_;
  }

  const hanabi_learning_env::HanabiGame& HanabiGame() const { return game_; }

 private:
  std::unordered_map<std::string, std::string> MapParams() const;
  hanabi_learning_env::HanabiGame game_;
  hanabi_learning_env::CanonicalObservationEncoder encoder_;
};

class OpenSpielHanabiState : public State {
 public:
  explicit OpenSpielHanabiState(std::shared_ptr<const Game> game);
  Player CurrentPlayer() const override;
  std::vector<Action> LegalActions() const override;
  std::string ActionToString(Player player, Action action_id) const override;
  std::vector<double> Rewards() const override;
  std::vector<double> Returns() const override;

  // We support observations only, not information states. The information
  // state would have to include the entire history of the game, and is
  // impractically large.
  // The observation by default includes knowledge inferred from past hints.
  std::string Observation(Player player) const override;
  void ObservationAsNormalizedVector(Player player,
                                     std::vector<double>* values) const;

  std::unique_ptr<State> Clone() const override;
  ActionsAndProbs ChanceOutcomes() const override;
  std::string ToString() const override;
  bool IsTerminal() const override;

 protected:
  void DoApplyAction(Action action) override;

 private:
  hanabi_learning_env::HanabiState state_;
  const OpenSpielHanabiGame* game_;
  double prev_state_score_;
};

}  // namespace hanabi
}  // namespace open_spiel

#endif  // THIRD_PARTY_OPEN_SPIEL_GAMES_HANABI_H_
