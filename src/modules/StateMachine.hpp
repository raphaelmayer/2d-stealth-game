#pragma once

enum class State { IDLE = 0, AIMING, SHOOTING, FIRERATE, RELOADING };

class StateMachine;

class StateNode {
  public:
	virtual void onEnter(StateMachine &sm) = 0;
	virtual void onUpdate(StateMachine &sm) = 0;
	virtual void onExit(StateMachine &sm) = 0;
};

class StateMachine {
  public:
	StateMachine(std::unique_ptr<StateNode> initialState) 
	{
		switchState(std::move(initialState));
	}

	void switchState(std::unique_ptr<StateNode> newState)
	{
		if (currentState) {
			currentState->onExit(*this);
		}
		currentState = std::move(newState);
		if (currentState) {
			currentState->onEnter(*this);
		}
	}

	void update()
	{
		if (currentState) {
			currentState->onUpdate(*this);
		}
	}

  private:
	std::unique_ptr<StateNode> currentState;
};

class NodeA : public StateNode {
  public:
	void onEnter(StateMachine &sm) override { std::cout << "NodeA::enter\n"; }

	void onUpdate(StateMachine &sm) override { std::cout << "NodeA::update\n"; }

	void onExit(StateMachine &sm) override { std::cout << "NodeA::exit\n"; }

  private:
	State state = State::IDLE;
};

class NodeB : public StateNode {
  public:
	void onEnter(StateMachine &sm) override { std::cout << "NodeB::enter\n"; }

	void onUpdate(StateMachine &sm) override { std::cout << "NodeB::update\n"; }

	void onExit(StateMachine &sm) override { std::cout << "NodeB::exit\n"; }

  private:
	State state = State::IDLE;
};

// int main() {
//	auto state = std::make_unique<SomeNode>();
//	StateMachine sm(std::move(state));
//
//	sm.update();
//
//	return 0;
// }