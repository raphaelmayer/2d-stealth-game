#include "behaviortree_cpp/behavior_tree.h"
#include <algorithm>
#include <random>
#include <vector>

class RandomSelector : public BT::ControlNode {
  public:
	RandomSelector(const std::string &name, const BT::NodeConfig &config)
	    : BT::ControlNode(name, config), generator(std::random_device{}())
	{
	}

	static BT::PortsList providedPorts()
	{
		// clang-format off
		return {
			BT::InputPort<std::vector<int>>("weights")
		};
		// clang-format on
	}

	BT::NodeStatus tick() override
	{
		// Shuffle children indices randomly
		if (!shuffled) {
			shuffled = true;
			indices.clear();
			for (size_t i = 0; i < children_nodes_.size(); i++) {
				indices.push_back(i);
			}
			std::shuffle(indices.begin(), indices.end(), generator);
		}

		while (current_child_index < indices.size()) {
			const auto child_index = indices[current_child_index];
			auto child_status = children_nodes_[child_index]->executeTick();

			switch (child_status) {
			case BT::NodeStatus::SUCCESS:
				shuffled = false; // Reset for the next tick
				return BT::NodeStatus::SUCCESS;
			case BT::NodeStatus::RUNNING:
				return BT::NodeStatus::RUNNING;
			case BT::NodeStatus::FAILURE:
				current_child_index++;
				break;
			}
		}

		shuffled = false;        // Reset for the next tick
		current_child_index = 0; // Reset for next execution
		return BT::NodeStatus::FAILURE;
	}

	void halt() override
	{
		current_child_index = 0;
		shuffled = false;
		ControlNode::halt();
	}

  private:
	std::vector<size_t> indices;
	size_t current_child_index = 0;
	std::default_random_engine generator;
	bool shuffled = false;
};
