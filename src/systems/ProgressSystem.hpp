#include "../constants.hpp"
#include "../ecs/Entity.hpp"
#include "System.hpp"

class ProgressSystem final : public System {
  public:
	void update(ECSManager &ecs, const double deltaTime) override
	{
		uint16_t coins = 0;
		auto &inventory = ecs.getComponent<Inventory>(PLAYER);
		auto &stats = ecs.getComponent<Stats>(PLAYER);
		switch (stats.progress) {
		case 0:
			// player has at least two coins in his inventory and is in interaction with the gardener
			for (Entity item : inventory.items) {
				if (ecs.hasComponent<Collectable>(item) && ecs.getComponent<Collectable>(item).name == "Coin") {
					coins++;
				}
				if (coins >= 2 && ecs.getComponent<Interactable>(NPC_GARDENER).isTextVisible) {
					ecs.getComponent<Interactable>(NPC_GARDENER).text = "Ah you earned your coins";

					stats.progress++;
					break;
				}
			}
			break;
		case 1:
			// player has ended the interaction with the gardener
			if (!ecs.getComponent<Interactable>(NPC_GARDENER).isTextVisible) {
				ecs.getComponent<Positionable>(NPC_GARDENER).position.y += TILE_SIZE;
				ecs.getComponent<Rotatable>(NPC_GARDENER).rotation = NORTH;
				ecs.getComponent<Interactable>(NPC_GARDENER).text = "Thank you and enjoy!";
				removeItemsFromInventory(ecs, inventory, "Coin");

				stats.experience += 100;
				stats.progress++;
			}
			break;
		case 2:
			// player has a health >= 200 (by consuming at least two apples)
			if (stats.health >= 200) {
				ecs.getComponent<Positionable>(NPC_GUARD_1).position.y += 1 * TILE_SIZE;
				ecs.getComponent<Rotatable>(NPC_GUARD_1).rotation = NORTH;
				ecs.getComponent<Interactable>(NPC_GUARD_1).text = "Now go, explore the outside of this island!";

				stats.experience += 100;
				stats.progress++;
			}
			break;
		case 3:
			// player has at least one cat in his inventory
			for (Entity item : ecs.getComponent<Inventory>(PLAYER).items) {
				if (ecs.hasComponent<Collectable>(item) && ecs.getComponent<Collectable>(item).name == "Cat") {
					ecs.getComponent<Positionable>(NPC_GUARD_2).position = {35 * TILE_SIZE, 22 * TILE_SIZE};
					ecs.getComponent<Rotatable>(NPC_GUARD_2).rotation = WEST;
					ecs.getComponent<Interactable>(NPC_GUARD_2).text = "Give her the cat. She will be pleased.";
					ecs.getComponent<Interactable>(NPC_GUARD_1).text = "Now you are good to go to her!";

					stats.experience += 100;
					stats.progress++;
					break;
				}
			}
			break;
		case 4:
			// player is in interaction with shelly
			if (ecs.getComponent<Interactable>(NPC_SHELLY).isTextVisible) {
				stats.progress++;
			}
			break;
		case 5:
			// player has ended the interaction with shelly
			if (!ecs.getComponent<Interactable>(NPC_SHELLY).isTextVisible) {
				removeItemsFromInventory(ecs, inventory, "Cat");
				ecs.getComponent<Interactable>(NPC_SHELLY).text =
				    "Bring me Mush Room, the king of the ants of desert island and you shall be rich!";
				ecs.getComponent<Rotatable>(NPC_SHELLY).rotation = WEST;
				ecs.getComponent<Positionable>(NPC_GUARD_2).position.y += TILE_SIZE;
				ecs.getComponent<Rotatable>(NPC_GUARD_2).rotation = NORTH;
				ecs.getComponent<Interactable>(NPC_GUARD_2).text = "Good Luck young lad!";
				ecs.addComponent(ITEM_CAT, Positionable{31 * TILE_SIZE, 19 * TILE_SIZE});
				ecs.getComponent<Interactable>(SIGN_CENTER_ISLAND).text =
				    "Bring Shelly the mushroom in the labyrinth on desert island.";

				stats.experience += 100;
				stats.progress++;
			}
			break;
		case 6:
			// player has found the sword
			for (Entity item : ecs.getComponent<Inventory>(PLAYER).items) {
				if (ecs.hasComponent<Collectable>(item) && ecs.getComponent<Collectable>(item).name == "Sword") {
					ecs.removeEntity(ITEM_ANT_LABYRINTH_ENTRY);
					ecs.removeEntity(ITEM_ANT_HELMET_1);
					ecs.removeEntity(ITEM_ANT_HELMET_2);

					stats.experience += 100;
					stats.progress++;
					break;
				}
			}
			break;
		case 7:
			if (stats.armor >= 90) {
				ecs.getComponent<Positionable>(NPC_PETER).position = {59 * TILE_SIZE, 19 * TILE_SIZE};
				ecs.getComponent<Interactable>(NPC_PETER).text = "Peter: Swiftly take the shield, I will distract him!";

				stats.experience += 100;
				stats.progress++;
				break;
			}
			break;
		case 8:
			// player has an armor of >= 120 (he has also picked up the shield)
			if (stats.armor >= 120) {
				ecs.removeEntity(SIGN_KING_ROOM);

				stats.experience += 100;
				stats.progress++;
				break;
			}
			break;
		case 9:
			// player has picked up the mushroom
			for (Entity item : ecs.getComponent<Inventory>(PLAYER).items) {
				if (ecs.hasComponent<Collectable>(item) && ecs.getComponent<Collectable>(item).name == "Mush Room") {
					ecs.getComponent<Interactable>(NPC_SHELLY).text = "Wow, you are truly a night in shiny armour!";
					ecs.getComponent<Rotatable>(NPC_SHELLY).rotation = NORTH;
					ecs.getComponent<Interactable>(NPC_GUARD_1).text = "I knew you could make it lad!";

					stats.experience += 100;
					stats.progress++;
					break;
				}
			}
			break;
		case 10:
			// player is in interaction with shelly
			if (ecs.getComponent<Interactable>(NPC_SHELLY).isTextVisible) {
				stats.progress++;
			}
			break;

		case 11:
			// player has ended the interaction with shelly
			if (!ecs.getComponent<Interactable>(NPC_SHELLY).isTextVisible) {
				removeItemsFromInventory(ecs, inventory, "Mush Room");
				ecs.addComponent(ITEM_MUSHROOM, Positionable{32 * TILE_SIZE, 20 * TILE_SIZE});
				ecs.getComponent<Interactable>(NPC_SHELLY).text =
				    "Now go back to the island where I once lived to receive your reward";
				ecs.getComponent<Interactable>(NPC_TINA).text =
				    "This coins are shelly's expression of gratitude, they are yours now!";
				ecs.getComponent<Interactable>(SIGN_CENTER_ISLAND).text = "Collect your reward";

				CollectableItem coin = {{4, 0}, {"Added coin to your inventory."},
				                        {"Coin", "Money.", 0}};
				instantiateCollectableItemEntity(ecs, {30, 6}, coin);
				instantiateCollectableItemEntity(ecs, {33, 6}, coin);
				instantiateCollectableItemEntity(ecs, {29, 7}, coin);
				instantiateCollectableItemEntity(ecs, {31, 7}, coin);
				instantiateCollectableItemEntity(ecs, {29, 8}, coin);
				instantiateCollectableItemEntity(ecs, {30, 8}, coin);
				instantiateCollectableItemEntity(ecs, {33, 8}, coin);

				stats.experience += 100;
				stats.progress++;
			}
			break;
		case 12:
			// Player has collected all seven reward coins
			for (Entity item : inventory.items) {
				if (ecs.hasComponent<Collectable>(item) && ecs.getComponent<Collectable>(item).name == "Coin") {
					coins++;
				}
				if (coins >= 7) {
					ecs.getComponent<Interactable>(SIGN_CENTER_ISLAND).text =
					    "You successfully finished Ants & Islands. We hope you enjoyed it!";

					stats.experience += 100;
					stats.progress++;
					break;
				}
			}
			break;
		}
	}

	void removeItemsFromInventory(ECSManager &ecs, Inventory &inventory, std::string key)
	{
		auto newEnd = std::remove_if(inventory.items.begin(), inventory.items.end(),
		                             [&](Entity item) { return ecs.getComponent<Collectable>(item).name == key; });
		inventory.items.erase(newEnd, inventory.items.end());

		// add audio component for item remove sound
		ecs.addComponent(PLAYER, SoundEmitter{SFX_REMOVE_ITEM});
	}
};