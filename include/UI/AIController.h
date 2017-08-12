#pragma once

#include <chrono>
#include <thread>

#include "state/State.h"
#include "MCTS/MOMCTS.h"

namespace ui
{
	class AIController
	{
	private:
		typedef state::State StartingStateGetter(int seed);

	public:
		AIController() :
			first_tree_(), second_tree_(), statistic_()
		{}

		void Run(int duration_sec, int thread_count, StartingStateGetter* state_getter) {
			std::chrono::steady_clock::time_point end = 
				std::chrono::steady_clock::now() +
				std::chrono::seconds(duration_sec);

			std::vector<std::thread> threads;
			for (int i = 0; i < thread_count; ++i) {
				threads.emplace_back([&]() {
					std::mt19937 rand;
					mcts::MOMCTS mcts(first_tree_, second_tree_, statistic_, rand);
					while (true) {
						if (std::chrono::steady_clock::now() > end) break;

						bool ret = mcts.Iterate([&]() {
							return (*state_getter)(rand());
						});

						if (ret) statistic_.IterateSucceeded();
						else statistic_.IterateFailed();
					}
				});
			}

			long long last_show_rest_sec = -1;
			while (true) {
				auto now = std::chrono::steady_clock::now();
				if (now > end) break;
				
				auto rest_sec = std::chrono::duration_cast<std::chrono::seconds>(end - now).count();
				if (rest_sec != last_show_rest_sec) {
					std::cout << "Rest seconds: " << rest_sec << std::endl;
					last_show_rest_sec = rest_sec;
				}
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}

			for (auto & thread : threads) {
				thread.join();
			}
		}

		auto const& GetStatistic() const { return statistic_; }

		auto GetRootNode(state::PlayerIdentifier side) const {
			if (side == state::kPlayerFirst) return &first_tree_;
			assert(side == state::kPlayerSecond);
			return &second_tree_;
		}

	private:
		mcts::builder::TreeBuilder::TreeNode first_tree_;
		mcts::builder::TreeBuilder::TreeNode second_tree_;
		mcts::Statistic<> statistic_;
	};
}