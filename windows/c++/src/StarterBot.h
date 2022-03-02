#pragma once

#include "MapTools.h"
//#include "BT_NODE.h"
#include <BWAPI.h>

class StarterBot
{
    MapTools m_mapTools;

public:

	BWAPI::Unit Scout = nullptr;

    StarterBot();

    // helper functions to get you started with bot programming and learn the API
    void sendIdleWorkersToMinerals();
    void trainAdditionalWorkers();
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
};