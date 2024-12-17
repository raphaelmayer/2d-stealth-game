#pragma once

class AIStateBase {
  public:
	virtual ~AIStateBase() = default;
	virtual void onEnter(AI &ai, Vision &vision) {}
	virtual void onUpdate(AI &ai, Vision &vision, double deltaTime) {}
	virtual void onExit(AI &ai, Vision &vision) {}
};

class AIStateMachine {
  public:
	void changeState(std::unique_ptr<AIStateBase> newState, Vision &vision)
	{
		if (currentState) {
			currentState->onExit(*this, vision);
		}
		currentState = std::move(newState);
		if (currentState) {
			currentState->onEnter(*this, vision);
		}
	}

	void updateState(Vision &vision, double deltaTime)
	{
		if (currentState) {
			currentState->onUpdate(*this, vision, deltaTime);
		}
	}
};
