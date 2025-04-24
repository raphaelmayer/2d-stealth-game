#include "../engine/Engine.hpp"
#include "../modules/SelectionManager.hpp"
#include "../ui/SelectionUIElement.hpp"
#include "System.hpp"
#include <easys/easys.hpp>

// The UIRenderSystem is responsible for rendering ... It is important to note that this system is not a system in the
// traditional ECS sense, it just contains UI updates and UI elements are not entities.
class UIRenderSystem final : public System {
  public:
	UIRenderSystem(Engine &engine, MenuStack &menuStack, const SelectionManager &selectionManager)
	    : engine_(engine), menuStack_(menuStack), selectionManager_(selectionManager)
	{
	}

	void update(Easys::ECS &ecs, const double deltaTime) override
	{
		menuStack_.update();

		if (!selectionManager_.getSelection().empty()) {
			SelectionUIElement(engine_, ecs, selectionManager_).render();
		}
	}

  private:
	Engine &engine_;
	MenuStack &menuStack_;
	const SelectionManager &selectionManager_;
};
