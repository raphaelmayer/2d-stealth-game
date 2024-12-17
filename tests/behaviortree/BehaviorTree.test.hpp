#include "../../src/behaviortree/BehaviorStatus.hpp"
#include "../../src/Behaviortree/BehaviorTree.hpp"
#include "../../src/behaviortree/ControlNodes.hpp"
#include "../../src/behaviortree/DecoratorNodes.hpp"
#include "../../src/behaviortree/TreeNode.hpp"
#include <catch2/catch.hpp>
#include <memory>

class MockBlackboard {
	// Empty blackboard for testing purposes
};

// Mock Leaf Nodes
template <typename BlackboardType>
class SuccessNode : public TreeNode<BlackboardType> {
  public:
	BehaviorStatus tick(BlackboardType &blackboard) override { return BehaviorStatus::SUCCESS; }
};

template <typename BlackboardType>
class FailureNode : public TreeNode<BlackboardType> {
  public:
	BehaviorStatus tick(BlackboardType &blackboard) override { return BehaviorStatus::FAILURE; }
};

template <typename BlackboardType>
class RunningNode : public TreeNode<BlackboardType> {
  public:
	BehaviorStatus tick(BlackboardType &blackboard) override { return BehaviorStatus::RUNNING; }
};

TEST_CASE("SequenceNode with all Success children returns Success", "[SequenceNode]")
{
	MockBlackboard blackboard;
	SequenceNode<MockBlackboard> sequenceNode;
	sequenceNode.addChild(std::make_unique<SuccessNode<MockBlackboard>>());
	sequenceNode.addChild(std::make_unique<SuccessNode<MockBlackboard>>());
	sequenceNode.addChild(std::make_unique<SuccessNode<MockBlackboard>>());

	BehaviorStatus status = sequenceNode.tick(blackboard);
	REQUIRE(status == BehaviorStatus::SUCCESS);
}

TEST_CASE("SequenceNode with a Failure child returns Failure", "[SequenceNode]")
{
	MockBlackboard blackboard;
	SequenceNode<MockBlackboard> sequenceNode;
	sequenceNode.addChild(std::make_unique<SuccessNode<MockBlackboard>>());
	sequenceNode.addChild(std::make_unique<FailureNode<MockBlackboard>>());
	sequenceNode.addChild(std::make_unique<SuccessNode<MockBlackboard>>());

	BehaviorStatus status = sequenceNode.tick(blackboard);
	REQUIRE(status == BehaviorStatus::FAILURE);
}

TEST_CASE("SequenceNode with a Running child returns Running", "[SequenceNode]")
{
	MockBlackboard blackboard;
	SequenceNode<MockBlackboard> sequenceNode;
	sequenceNode.addChild(std::make_unique<SuccessNode<MockBlackboard>>());
	sequenceNode.addChild(std::make_unique<RunningNode<MockBlackboard>>());
	sequenceNode.addChild(std::make_unique<SuccessNode<MockBlackboard>>());

	BehaviorStatus status = sequenceNode.tick(blackboard);
	REQUIRE(status == BehaviorStatus::RUNNING);
}

TEST_CASE("SelectorNode with a Success child returns Success", "[SelectorNode]")
{
	MockBlackboard blackboard;
	SelectorNode<MockBlackboard> selectorNode;
	selectorNode.addChild(std::make_unique<FailureNode<MockBlackboard>>());
	selectorNode.addChild(std::make_unique<SuccessNode<MockBlackboard>>());
	selectorNode.addChild(std::make_unique<FailureNode<MockBlackboard>>());

	BehaviorStatus status = selectorNode.tick(blackboard);
	REQUIRE(status == BehaviorStatus::SUCCESS);
}

TEST_CASE("SelectorNode with all Failure children returns Failure", "[SelectorNode]")
{
	MockBlackboard blackboard;
	SelectorNode<MockBlackboard> selectorNode;
	selectorNode.addChild(std::make_unique<FailureNode<MockBlackboard>>());
	selectorNode.addChild(std::make_unique<FailureNode<MockBlackboard>>());
	selectorNode.addChild(std::make_unique<FailureNode<MockBlackboard>>());

	BehaviorStatus status = selectorNode.tick(blackboard);
	REQUIRE(status == BehaviorStatus::FAILURE);
}

TEST_CASE("SelectorNode with a Running child returns Running", "[SelectorNode]")
{
	MockBlackboard blackboard;
	SelectorNode<MockBlackboard> selectorNode;
	selectorNode.addChild(std::make_unique<FailureNode<MockBlackboard>>());
	selectorNode.addChild(std::make_unique<RunningNode<MockBlackboard>>());
	selectorNode.addChild(std::make_unique<SuccessNode<MockBlackboard>>());

	BehaviorStatus status = selectorNode.tick(blackboard);
	REQUIRE(status == BehaviorStatus::RUNNING);
}

TEST_CASE("BehaviorTree root tick returns correct status", "[BehaviorTree]")
{
	MockBlackboard blackboard;
	auto rootNode = std::make_unique<SequenceNode<MockBlackboard>>();
	rootNode->addChild(std::make_unique<SuccessNode<MockBlackboard>>());
	rootNode->addChild(std::make_unique<SuccessNode<MockBlackboard>>());

	BehaviorTree<MockBlackboard> tree(std::move(rootNode));
	BehaviorStatus status = tree.tick(blackboard);

	REQUIRE(status == BehaviorStatus::SUCCESS);
}

TEST_CASE("Complex tree test", "[BehaviorTree]")
{
	MockBlackboard blackboard;

	auto rootNode = std::make_unique<SelectorNode<MockBlackboard>>();

	auto sequenceNode1 = std::make_unique<SequenceNode<MockBlackboard>>();
	sequenceNode1->addChild(std::make_unique<FailureNode<MockBlackboard>>());
	sequenceNode1->addChild(std::make_unique<SuccessNode<MockBlackboard>>());

	auto sequenceNode2 = std::make_unique<SequenceNode<MockBlackboard>>();
	sequenceNode2->addChild(std::make_unique<RunningNode<MockBlackboard>>());
	sequenceNode2->addChild(std::make_unique<SuccessNode<MockBlackboard>>());

	rootNode->addChild(std::move(sequenceNode1));
	rootNode->addChild(std::move(sequenceNode2));

	BehaviorTree<MockBlackboard> tree(std::move(rootNode));

	BehaviorStatus status = tree.tick(blackboard);

	REQUIRE(status == BehaviorStatus::RUNNING);
}

TEST_CASE("Subtree test - Basic integration", "[BehaviorTree]")
{
	MockBlackboard blackboard;

	// Create a subtree
	auto subtreeRoot = std::make_unique<SequenceNode<MockBlackboard>>();
	subtreeRoot->addChild(std::make_unique<SuccessNode<MockBlackboard>>());
	subtreeRoot->addChild(std::make_unique<SuccessNode<MockBlackboard>>());
	auto subtree = std::make_unique<BehaviorTree<MockBlackboard>>(std::move(subtreeRoot));

	// Create the main tree
	auto mainTreeRoot = std::make_unique<SelectorNode<MockBlackboard>>();
	mainTreeRoot->addChild(std::move(subtree)); // Add the subtree
	mainTreeRoot->addChild(std::make_unique<FailureNode<MockBlackboard>>());

	BehaviorTree<MockBlackboard> tree(std::move(mainTreeRoot));

	// Tick the tree
	BehaviorStatus status = tree.tick(blackboard);

	// The subtree succeeds, so the main tree should succeed
	REQUIRE(status == BehaviorStatus::SUCCESS);
}

TEST_CASE("Subtree test - Subtree returning Failure", "[BehaviorTree]")
{
	MockBlackboard blackboard;

	// Create a subtree
	auto subtreeRoot = std::make_unique<SequenceNode<MockBlackboard>>();
	subtreeRoot->addChild(std::make_unique<FailureNode<MockBlackboard>>());
	subtreeRoot->addChild(std::make_unique<SuccessNode<MockBlackboard>>());
	auto subtree = std::make_unique<BehaviorTree<MockBlackboard>>(std::move(subtreeRoot));

	// Create the main tree
	auto mainTreeRoot = std::make_unique<SelectorNode<MockBlackboard>>();
	mainTreeRoot->addChild(std::move(subtree)); // Add the subtree
	mainTreeRoot->addChild(std::make_unique<SuccessNode<MockBlackboard>>());

	BehaviorTree<MockBlackboard> tree(std::move(mainTreeRoot));

	// Tick the tree
	BehaviorStatus status = tree.tick(blackboard);

	// The subtree fails, so the selector moves to the second child which succeeds
	REQUIRE(status == BehaviorStatus::SUCCESS);
}

TEST_CASE("Subtree test - Subtree returning Running", "[BehaviorTree]")
{
	MockBlackboard blackboard;

	// Create a subtree
	auto subtreeRoot = std::make_unique<SequenceNode<MockBlackboard>>();
	subtreeRoot->addChild(std::make_unique<RunningNode<MockBlackboard>>());
	subtreeRoot->addChild(std::make_unique<SuccessNode<MockBlackboard>>());
	auto subtree = std::make_unique<BehaviorTree<MockBlackboard>>(std::move(subtreeRoot));

	// Create the main tree
	auto mainTreeRoot = std::make_unique<SelectorNode<MockBlackboard>>();
	mainTreeRoot->addChild(std::move(subtree)); // Add the subtree
	mainTreeRoot->addChild(std::make_unique<SuccessNode<MockBlackboard>>());

	BehaviorTree<MockBlackboard> tree(std::move(mainTreeRoot));

	// Tick the tree
	BehaviorStatus status = tree.tick(blackboard);

	// The subtree returns Running, so the main tree should return Running
	REQUIRE(status == BehaviorStatus::RUNNING);
}

TEST_CASE("Subtree test - Multiple subtrees", "[BehaviorTree]")
{
	MockBlackboard blackboard;

	// Create first subtree
	auto subtree1Root = std::make_unique<SequenceNode<MockBlackboard>>();
	subtree1Root->addChild(std::make_unique<SuccessNode<MockBlackboard>>());
	subtree1Root->addChild(std::make_unique<SuccessNode<MockBlackboard>>());
	auto subtree1 = std::make_unique<BehaviorTree<MockBlackboard>>(std::move(subtree1Root));

	// Create second subtree
	auto subtree2Root = std::make_unique<SelectorNode<MockBlackboard>>();
	subtree2Root->addChild(std::make_unique<FailureNode<MockBlackboard>>());
	subtree2Root->addChild(std::make_unique<RunningNode<MockBlackboard>>());
	auto subtree2 = std::make_unique<BehaviorTree<MockBlackboard>>(std::move(subtree2Root));

	// Create the main tree
	auto mainTreeRoot = std::make_unique<SequenceNode<MockBlackboard>>();
	mainTreeRoot->addChild(std::move(subtree1)); // Add first subtree
	mainTreeRoot->addChild(std::move(subtree2)); // Add second subtree

	BehaviorTree<MockBlackboard> tree(std::move(mainTreeRoot));

	// Tick the tree
	BehaviorStatus status = tree.tick(blackboard);

	// The first subtree succeeds, the second subtree returns Running, so the main tree should return Running
	REQUIRE(status == BehaviorStatus::RUNNING);
}
