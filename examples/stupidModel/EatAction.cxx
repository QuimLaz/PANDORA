
#include <EatAction.hxx>
#include <World.hxx>
#include <Bug.hxx>
#include <iostream>
#include <algorithm>


namespace Examples
{

EatAction::EatAction() {}

EatAction::~EatAction() {}

void EatAction::execute( Engine::Agent & agent ) {
	if (agent.exists()) {
		Bug & bugAgent = (Bug&)agent;
		Engine::World * world = agent.getWorld();
		int foodAvaliable = world->getValue("food", agent.getPosition());
		int foodConsumed = std::min(bugAgent.getMaxConsumptionRate(),foodAvaliable);
		bugAgent.setSize(bugAgent.getSize() + foodConsumed);
		std::cout << "I'm " << agent.getId() << " and I ate: " << foodConsumed << " and my size now is: " << bugAgent.getSize() << std::endl;
		world->setValue("food", agent.getPosition(), foodAvaliable - foodConsumed);
	}
}

std::string EatAction::describe() const {
	return "Eat action";
}

} // namespace Examples

