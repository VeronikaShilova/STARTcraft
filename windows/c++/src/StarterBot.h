#pragma once

#include "MapTools.h"
#include "BT_NODE.h"
#include "BT_SELECTOR.h"
#include "BT_SEQUENCER.h"
#include "BT_CONDITION.h"
#include "BT_ACTION.h"

#include <BWAPI.h>

class StarterBot
{
    MapTools m_mapTools;

public:
	struct Data {
		int supply;
		int maxSupply;
		int nbBarracks;
		int nbMarines;

	};
	Data *data;

	BT_SEQUENCER* behaviourTree;

	BWAPI::Unit Scout = nullptr;

    StarterBot();

    // helper functions to get you started with bot programming and learn the API
    void sendIdleWorkersToMinerals();
    void buildAdditionalSupply();
	
	void trainUnits(const BWAPI::UnitType UnitType, const BWAPI::UnitType Depot);
	void buildDepot(const BWAPI::UnitType DepotType);
	
	void ScoutUnexploredMap();
    void drawDebugInformation();

    // functions that are triggered by various BWAPI events from main.cpp
	void onStart();
	void onFrame();
	void onEnd(bool isWinner);
	void onUnitDestroy(BWAPI::Unit unit);
	void onUnitMorph(BWAPI::Unit unit);
	void onSendText(std::string text);
	void onUnitCreate(BWAPI::Unit unit);
	void onUnitComplete(BWAPI::Unit unit);
	void onUnitShow(BWAPI::Unit unit);
	void onUnitHide(BWAPI::Unit unit);
	void onUnitRenegade(BWAPI::Unit unit);

	// callbacks used in the condition leaves of the BT
	static bool supplySuperiorTo(void* data);

	// callbacks used in the action leaves of the BT
	static bool trainAdditionalWorkers(void* data);

	// Function to build the Behaviour Tree
	void buildBT();
};


