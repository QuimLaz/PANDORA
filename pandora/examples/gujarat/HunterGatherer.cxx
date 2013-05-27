
#include <HunterGatherer.hxx>
#include <GujaratWorld.hxx>
#include <Exceptions.hxx>
#include <Action.hxx>
#include <Sector.hxx>
#include <cmath>
#include <cassert>
#include <AgentController.hxx>

#include <GujaratState.hxx>
#include <GeneralState.hxx>
#include <HGMindFactory.hxx>
#include <Logger.hxx>

#include <GujaratConfig.hxx>

#include <HGMindFactory.hxx>

#include <MDPAction.hxx>

namespace Gujarat
{

HunterGatherer::HunterGatherer( const std::string & id ) 
	: GujaratAgent(id)/*, _surplusForReproductionThreshold(2), _surplusWanted(1)*/, _homeRange(50),
	_numSectors( -1 ),
	_myHGMind(*(HGMindFactory::getHGMind(*(GujaratWorld*)_world)))
{
	//_myHGMind = *(HGMindFactory::getHGMind(*(GujaratWorld*)_world));
	/*
	static Gujarat::HGMind* Gujarat::HGMindFactory::getHGMind(Gujarat::HGMindFactory::GujaratWorld&)
	./HGMindFactory.hxx:29:17: note:   no known conversion for argument 1 from ‘Gujarat::GujaratWorld’ to ‘Gujarat::HGMindFactory::GujaratWorld&’
	*/
	
}

void HunterGatherer::registerAttributes()
{
	std::stringstream logName;
	logName << "Serializer_" << _world->getId();
	
	log_DEBUG(logName.str(), "registering attributes for type: " << getType());

	registerIntAttribute("MoveHome actions");
//	registerIntAttribute("Forage actions");
	registerIntAttribute("agent age");
//	registerIntAttribute("male alive");
//	registerIntAttribute("male age");
//	registerIntAttribute("female alive");
//	registerIntAttribute("female age");
	registerIntAttribute("children");
	registerIntAttribute("collected resources");
	registerIntAttribute("starving %");
//	registerIntAttribute("starving days x 100");
	log_DEBUG(logName.str(), "registering attributes for type: " << getType() << " finished");
}

HunterGatherer::~HunterGatherer()
{
	_myHGMind.clearSectorKnowledge();	
}

//************************************************************************
// REFACTORED updateKnowledge & updateKnowledge(par1,par2...)const
//************************************************************************
// some slight changes in the methods allows to disable omniscience i
// perform update knowledge after exploring a sector

									   
void HunterGatherer::updateKnowledge()
{	
	_collectedResources = 0;	
	
	std::stringstream logName;
	logName << "agents_" << _world->getId() << "_" << getId();
	
	_myHGMind.updateKnowledge(_position);

}


void	HunterGatherer::updateKnowledge( 	const Engine::Point2D<int>& agentPos, const Engine::Raster& dataRaster, std::vector< Sector* >& HRSectors, std::vector< Sector* >& LRSectors  ) const
{	
	_myHGMind.updateKnowledge(agentPos, dataRaster, HRSectors, LRSectors);
}


void HunterGatherer::executeActions()
{	
	std::list<Engine::Action *>::iterator it = _actions.begin();
	while(it!=_actions.end())
	{
		//Engine::Action * nextAction = _actions[i];
		MDPAction * nextAction = (MDPAction*)*it;
		// world info retrieved due to execute an action
		_myHGMind.updateDueToExecuteAction(((MDPAction*)nextAction)->getVisitedSector());
		it++;
	}
	// Ensure that no action alters the internal knowledge retrieved
	// by knowledgeDueToExecuteAction
	// MoveHomeAction : 
	//		erases sectors : HR, LR
	//		does not touch timestamps
	//		does not touch private rasters
	// ForageAction :
	//		touches nothing
	
	GujaratAgent::executeActions();
	
}

void HunterGatherer::clearSectorKnowledge() 
	{ 
		_myHGMind.clearSectorKnowledge();
	}	
	
void HunterGatherer::selectActions()
{
	std::list<MDPAction*> actions;
	GujaratState::controller().selectActions(*this, actions);
	std::list<MDPAction*>::iterator it=actions.begin();
	while(it!=actions.end())
	{
		_actions.push_back((Engine::Action*)(*it));
		it = actions.erase(it);
	}
}

GujaratAgent * HunterGatherer::createNewAgent()
{	
	GujaratWorld * world = (GujaratWorld*)_world;
	std::ostringstream oss;
	oss << "HunterGatherer_" << world->getId() << "-" << world->getNewKey();

	//*? 
	// NO CHILDREN
	return 0;
	/*
	HunterGatherer * agent = new HunterGatherer(oss.str());

	agent->setSocialRange( _socialRange );
	agent->setHomeMobilityRange( _homeMobilityRange );
	agent->setHomeRange( _homeRange );
	agent->setLowResHomeRange( _lowResHomeRange );
	
	//agent->setSurplusForReproductionThreshold( _surplusForReproductionThreshold );
	//agent->setSurplusWanted( _surplusWanted );
	//agent->setSurplusSpoilageFactor( _surplusSpoilageFactor );
	//agent->setFoodNeedsForReproduction( _foodNeedsForReproduction );			

	agent->setWalkingSpeedHour( _walkingSpeedHour );
	agent->setForageTimeCost( _forageTimeCost );
	//agent->setAvailableForageTime( _availableForageTime );
	agent->setMassToCaloriesRate( _massToCaloriesRate );
	agent->setNumSectors( ((GujaratConfig)((GujaratWorld*)_world)->getConfig())._numSectors );
	
	// initially the agent will be a couple
	agent->_populationAges.resize(2);

	return agent;
	*/
}

/*
bool HunterGatherer::needsResources()
{
	return _collectedResources < (_surplusForReproductionThreshold + _surplusWanted);
}
*/

Engine::Raster & HunterGatherer::getLRResourcesRaster() 
{ 
	//return _world->getDynamicRaster(eLRResources); 	
	return _myHGMind.getLRResourcesRaster();
}

bool HunterGatherer::cellValid( Engine::Point2D<int>& loc )
{
	if ( !_world->getOverlapBoundaries().isInside(loc) )
		return false;
	// Check that the home of another agent resides in loc
	std::vector<Agent * > agents = _world->getAgent(loc);
	if(agents.size()==0)
	{
		agents.clear();
		return true;
	}

	for(int i=0; i<agents.size(); i++)
	{
		Agent * agent = agents.at(i);
		if(agent->exists() && agent!=this)
		{
			agents.clear();
			return false;
		}
	}
	agents.clear();
	return true;
}

bool HunterGatherer::cellRelevant( Engine::Point2D<int>& loc )
{
	Soils soilType = (Soils) _world->getValue(eSoils, loc);
	int resourceType = _world->getValue(eResourceType, loc);
	return soilType == INTERDUNE && resourceType == WILD;
}

void HunterGatherer::serialize()
{
	serializeAttribute("agent age", _age);
/*
	if(_populationAges[0]!=-1)
	{
		serializeAttribute("male alive", 1);
		serializeAttribute("male age", _populationAges[0]);
	}
	else
	{
		serializeAttribute("male alive", 0);
		serializeAttribute("male age", std::numeric_limits<int>::max());
	}
	
	if(_populationAges[1]!=-1)
	{
		serializeAttribute("female alive", 1);
		serializeAttribute("female age", _populationAges[1]);
	}
	else
	{
		serializeAttribute("female alive", 0);
		serializeAttribute("female age", std::numeric_limits<int>::max());
	}
*/
	int numChildren = 0;
	for(unsigned i=2; i<_populationAges.size(); i++)
	{
		if(_populationAges[i]!=-1)
		{
			numChildren++;
		}
	}
	serializeAttribute("children", numChildren);
	serializeAttribute("collected resources", _collectedResources);
	serializeAttribute("starving %", getPercentageOfStarvingDays());
//	serializeAttribute("starving days x 100", _starved*100.0f);
	serializeAttribute("MoveHome actions", _moveHomeActionsExecuted);
//	serializeAttribute("Forage actions", _forageActionsExecuted);
}

} // namespace Gujarat

