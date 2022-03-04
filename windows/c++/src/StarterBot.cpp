#include "StarterBot.h"
#include "Tools.h"
#include "MapTools.h"
#include "BT_LEAF.h"
#include "BT_LIST.h"
#include "BT_SEQUENCER.h"

using namespace std;

constexpr inline BT_NODE::State status(bool condition) {
    return condition ? BT_NODE::SUCCESS : BT_NODE::FAILURE;
}

// callbacks used in the condition leaves of the BT
template <int limitSupply>
BT_NODE::State supplySuperiorTo(void* data) {
    StarterBot::Data* dataPtr = (StarterBot::Data*)data;

    //cout << " Is supply " << dataPtr->supply << " greater than " << limitSupply << "?" << endl;
    return status(dataPtr->supply > limitSupply);
}

// callbacks used in the condition leaves of the BT
template <int limitSupply>
BT_NODE::State maxSupplySuperiorTo(void* data) {
    StarterBot::Data* dataPtr = (StarterBot::Data*)data;

    //cout << " Is max supply " << dataPtr->maxSupply << " greater than " << limitSupply << "?" << endl;
    return status(dataPtr->maxSupply > limitSupply);
}

// callbacks used in the condition leaves of the BT
template <int limitNbBarracks>
BT_NODE::State nbBarracksSuperiorTo(void* data) {
    StarterBot::Data* dataPtr = (StarterBot::Data*)data;

    //cout << " Is nb. of barracks " << dataPtr->nbBarracks << " greater than " << limitNbBarracks << "?" << endl;
    return status(dataPtr->nbBarracks > limitNbBarracks);
}

// callbacks used in the action leaves of the BT
BT_NODE::State trainAdditionalWorkers(void* data) {
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
        //cout << "Workers are trained" << endl;
        return BT_NODE::SUCCESS;
    }

    //cout << "Workers are not trained" << endl;
    return BT_NODE::FAILURE;
}

BT_NODE::State buildDepot(void* data) {
    const BWAPI::UnitType depotType = BWAPI::Broodwar->self()->getRace().getResourceDepot();
    if (Tools::BuildBuilding(depotType)) {
        BWAPI::Broodwar->printf("Started Building %s", depotType.getName().c_str());
        //cout << "Depot is built" << endl;
        return BT_NODE::SUCCESS;
    }

    //cout << "Depot is not built" << endl;
    return BT_NODE::FAILURE;
}

BT_NODE::State buildBarracks(void* data) {
    cout << "Want to build barracks" << endl;
    const BWAPI::UnitType unitType = BWAPI::UnitTypes::Terran_Barracks;
    if (Tools::BuildBuilding(unitType)) {
        BWAPI::Broodwar->printf("Started Building %s", unitType.getName().c_str());
        cout << "Barrack is built" << endl;
        return BT_NODE::SUCCESS;
    }
    return BT_NODE::FAILURE;
}

// Build more supply if we are going to run out soon
void buildAdditionalSupply()
{
    // Get the amount of supply supply we currently have unused
    const int unusedSupply = Tools::GetTotalSupply(true) - BWAPI::Broodwar->self()->supplyUsed();

    // If we have a sufficient amount of supply, we don't need to do anything
    if (unusedSupply < 2) {
        buildDepot(nullptr);
    }
}

// Function to build the Behaviour Tree
BT_NODE* buildBT() {
    //cout << "Start building a tree" << endl;

    BT_NODE* behaviourTree = new BT_LIST(nullptr, 6);
    BT_NODE* stage;

    stage = new BT_SEQUENCER(behaviourTree, 2);
    new BT_LEAF(stage, supplySuperiorTo<8>);
    new BT_LEAF(stage, trainAdditionalWorkers);

    stage = new BT_SEQUENCER(behaviourTree, 2);
    new BT_LEAF(stage, maxSupplySuperiorTo<10>);
    new BT_LEAF(stage, buildDepot);

    stage = new BT_SEQUENCER(behaviourTree, 2);
    new BT_LEAF(stage, supplySuperiorTo<10>);
    new BT_LEAF(stage, trainAdditionalWorkers);

    stage = new BT_SEQUENCER(behaviourTree, 2);
    new BT_LEAF(stage, nbBarracksSuperiorTo<-1>);
    new BT_LEAF(stage, buildBarracks);

    stage = new BT_SEQUENCER(behaviourTree, 2);
    new BT_LEAF(stage, supplySuperiorTo<13>);
    new BT_LEAF(stage, trainAdditionalWorkers);

    stage = new BT_SEQUENCER(behaviourTree, 2);
    new BT_LEAF(stage, nbBarracksSuperiorTo<1>);
    new BT_LEAF(stage, buildBarracks);

    return behaviourTree;
}

StarterBot::StarterBot() {
    data = new StarterBot::Data();
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
    behaviourTree = buildBT();
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
    data->nbBarracks = BWAPI::Broodwar->self()->allUnitCount(BWAPI::UnitTypes::Terran_Barracks);

    cout << data->nbBarracks << endl;

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

void StarterBot::trainUnits(const BWAPI::UnitType UnittoTrain, const BWAPI::UnitType Depot) {
    // get the unit pointer to my depot
    const BWAPI::Unit myDepot = Tools::GetDepot(Depot);

    // if we have a valid depot unit and it's currently not training something, train a worker
    // there is no reason for a bot to ever use the unit queueing system, it just wastes resources
    if (myDepot && !myDepot->isTraining()) { 
        myDepot->train(UnittoTrain);
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