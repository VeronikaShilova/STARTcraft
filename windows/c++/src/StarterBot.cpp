#include "StarterBot.h"
#include "Tools.h"
#include "MapTools.h"

StarterBot::StarterBot()
{
    
}

// Called when the bot starts!
void StarterBot::onStart()
{
    // Set our BWAPI options here    
	BWAPI::Broodwar->setLocalSpeed(10);
    BWAPI::Broodwar->setFrameSkip(0);
    
    // Enable the flag that tells BWAPI top let users enter input while bot plays
    BWAPI::Broodwar->enableFlag(BWAPI::Flag::UserInput);

    // Call MapTools OnStart
    m_mapTools.onStart();

    // Build Behaviour Tree
    buildBT();
}

// Called whenever the game ends and tells you if you won or not
void StarterBot::onEnd(bool isWinner) 
{
    std::cout << "We " << (isWinner ? "won!" : "lost!") << "\n";
}

// Called on each frame of the game
void StarterBot::onFrame()
{
    // Update our MapTools information
    m_mapTools.onFrame();

    // update data
    data->supply = BWAPI::Broodwar->self()->supplyUsed();
    data->maxSupply = Tools::GetTotalSupply(true);

    // Run through the Behaviour Tree
    behaviourTree->Evaluate(data);

    
    //ScoutUnexploredMap();

    // Send our idle workers to mine minerals so they don't just stand there
    sendIdleWorkersToMinerals();

    // Train more workers so we can gather more income
    //trainAdditionalWorkers();

    // Build more supply if we are going to run out soon
    //buildAdditionalSupply();
    /*
    // Train Marine
    const BWAPI::UnitType UnittoTrain = BWAPI::UnitTypes::Terran_Marine;
    const BWAPI::UnitType depot = BWAPI::UnitTypes::Terran_Barracks;
    trainUnits(UnittoTrain, depot);

    // Build Barracks
    buildDepot(depot);   */

    // Draw unit health bars, which brood war unfortunately does not do
    Tools::DrawUnitHealthBars();

    // Draw some relevent information to the screen to help us debug the bot
    drawDebugInformation();
}

bool StarterBot::supplySuperiorTo(void* data) {
    Data* dataPtr = (Data *)data;
    return dataPtr->supply > 8;
    std::cout << "arrived at supply" << std::endl;
}

void StarterBot::buildBT() {
    std::cout << "Arrived at the beginning of tree" << std::endl;
    BT_SEQUENCER *root = new BT_SEQUENCER(NULL, 10);
    BT_SELECTOR *getSCV = new BT_SELECTOR(root, 2);
    root->AddChild(getSCV);

    std::cout << "add child SCV succeed" << std::endl;

    BT_CONDITION* testSupply = new BT_CONDITION(getSCV, supplySuperiorTo);
    BT_ACTION* recruitSCV = new BT_ACTION(getSCV, trainAdditionalWorkers);

    getSCV->AddChild(testSupply);
    std::cout << "-----------------------------------------------" << std::endl;

    getSCV->AddChild(recruitSCV);

    behaviourTree = root;
    std::cout << "Tree complete" << std::endl;
}


// Send our idle workers to mine minerals so they don't just stand there
void StarterBot::sendIdleWorkersToMinerals()
{
    // Let's send all of our starting workers to the closest mineral to them
    // First we need to loop over all of the units that we (BWAPI::Broodwar->self()) own
    const BWAPI::Unitset& myUnits = BWAPI::Broodwar->self()->getUnits();
    for (auto& unit : myUnits)
    {
        // Check the unit type, if it is an idle worker, then we want to send it somewhere
        if (unit->getType().isWorker() && unit->isIdle())
        {
            // Get the closest mineral to this worker unit
            BWAPI::Unit closestMineral = Tools::GetClosestUnitTo(unit, BWAPI::Broodwar->getMinerals());

            // If a valid mineral was found, right click it with the unit in order to start harvesting
            if (closestMineral) { unit->rightClick(closestMineral); }
        }
    }
}

// Train more workers so we can gather more income
bool StarterBot::trainAdditionalWorkers(void *data)
{ 
    const BWAPI::UnitType workerType = BWAPI::Broodwar->self()->getRace().getWorker();
    /*const int workersWanted = 10;
    const int workersOwned = Tools::CountUnitsOfType(workerType, BWAPI::Broodwar->self()->getUnits());
    if (workersOwned < workersWanted)
    {
    */
    // get the unit pointer to my depot
    const BWAPI::Unit myDepot = Tools::GetDepot(BWAPI::Broodwar->self()->getRace().getResourceDepot());

    // if we have a valid depot unit and it's currently not training something, train a worker
    // there is no reason for a bot to ever use the unit queueing system, it just wastes resources
        if (myDepot && !myDepot->isTraining() && BWAPI::Broodwar->self()->minerals() >= 50) {
            myDepot->train(workerType);
            return true;
        }
        return false;
    
}

void StarterBot::trainUnits(const BWAPI::UnitType UnittoTrain, const BWAPI::UnitType Depot) {
    // get the unit pointer to my depot
    const BWAPI::Unit myDepot = Tools::GetDepot(Depot);

    // if we have a valid depot unit and it's currently not training something, train a worker
    // there is no reason for a bot to ever use the unit queueing system, it just wastes resources
    if (myDepot && !myDepot->isTraining()) { 
        myDepot->train(UnittoTrain);
    }

}

// Build more supply if we are going to run out soon
void StarterBot::buildAdditionalSupply()
{
    // Get the amount of supply supply we currently have unused
    const int unusedSupply = Tools::GetTotalSupply(true) - BWAPI::Broodwar->self()->supplyUsed();

    // If we have a sufficient amount of supply, we don't need to do anything
    if (unusedSupply >= 2) { return; }

    // Otherwise, we are going to build a supply provider
    const BWAPI::UnitType supplyProviderType = BWAPI::Broodwar->self()->getRace().getSupplyProvider();

    buildDepot(supplyProviderType);
}

void StarterBot::buildDepot(const BWAPI::UnitType DepotType) {
    const bool startedBuilding = Tools::BuildBuilding(DepotType);
    if (startedBuilding)
    {
        BWAPI::Broodwar->printf("Started Building %s", DepotType.getName().c_str());
    }
}

// Explore the map
void StarterBot::ScoutUnexploredMap() {
    const BWAPI::UnitType workerType = BWAPI::Broodwar->self()->getRace().getWorker();
    Scout = Tools::GetUnitOfType(workerType);

    if (!Scout) {
        const BWAPI::UnitType workerType = BWAPI::Broodwar->self()->getRace().getWorker();
        Scout = Tools::GetUnitOfType(workerType);
    }
    auto& startLocations = BWAPI::Broodwar->getStartLocations();

    for (BWAPI::TilePosition tp : startLocations) {
        if (BWAPI::Broodwar->isExplored(tp)) { continue; }

        BWAPI::Position pos(tp);
        BWAPI::Broodwar->drawCircleMap(pos, 32, BWAPI::Colors::Blue, true);

        Scout->move(pos);
    }

}


// Draw some relevent information to the screen to help us debug the bot
void StarterBot::drawDebugInformation()
{
    BWAPI::Broodwar->drawTextScreen(BWAPI::Position(10, 10), "Test version\n");
    Tools::DrawUnitCommands();
    Tools::DrawUnitBoundingBoxes();
}

// Called whenever a unit is destroyed, with a pointer to the unit
void StarterBot::onUnitDestroy(BWAPI::Unit unit)
{
	
}

// Called whenever a unit is morphed, with a pointer to the unit
// Zerg units morph when they turn into other units
void StarterBot::onUnitMorph(BWAPI::Unit unit)
{
	
}

// Called whenever a text is sent to the game by a user
void StarterBot::onSendText(std::string text) 
{ 
    if (text == "/map")
    {
        m_mapTools.toggleDraw();
    }
}

// Called whenever a unit is created, with a pointer to the destroyed unit
// Units are created in buildings like barracks before they are visible, 
// so this will trigger when you issue the build command for most units
void StarterBot::onUnitCreate(BWAPI::Unit unit)
{ 
	
}

// Called whenever a unit finished construction, with a pointer to the unit
void StarterBot::onUnitComplete(BWAPI::Unit unit)
{
	
}

// Called whenever a unit appears, with a pointer to the destroyed unit
// This is usually triggered when units appear from fog of war and become visible
void StarterBot::onUnitShow(BWAPI::Unit unit)
{ 
	
}

// Called whenever a unit gets hidden, with a pointer to the destroyed unit
// This is usually triggered when units enter the fog of war and are no longer visible
void StarterBot::onUnitHide(BWAPI::Unit unit)
{ 
	
}

// Called whenever a unit switches player control
// This usually happens when a dark archon takes control of a unit
void StarterBot::onUnitRenegade(BWAPI::Unit unit)
{ 
	
}