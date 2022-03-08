#include "StarterBot.h"
#include "Tools.h"
#include "MapTools.h"

using namespace std;

constexpr inline BT_NODE::State status(bool condition) {
    return condition ? BT_NODE::SUCCESS : BT_NODE::FAILURE;
}

/* Condition Callback */

template <int limitSCV>
BT_NODE::State SCVInferiorTo(void* data) {
    StarterBot::Data* dataPtr = (StarterBot::Data*)data;
    return status(dataPtr->SCV < limitSCV);
}

template <int limitSupply>
BT_NODE::State maxSupplySuperiorTo(void* data) {
    StarterBot::Data* dataPtr = (StarterBot::Data*)data;
    return status(dataPtr->maxSupply > limitSupply);
}

template <int limitNbBarracks>
BT_NODE::State nbBarracksInferiorTo(void* data) {
    StarterBot::Data* dataPtr = (StarterBot::Data*)data;
    return status(dataPtr->nbBarracks < limitNbBarracks);
}

template <int limitMarine>
BT_NODE::State nbMarinesInferiorTo(void* data){
    StarterBot::Data* dataPtr = (StarterBot::Data*)data;
    return status(dataPtr->nbMarines < limitMarine);
}

/* Action Callback */

BT_NODE::State trainSCV(void* data) {
    const BWAPI::Unit myDepot = Tools::GetDepot(BWAPI::Broodwar->self()->getRace().getResourceDepot());

    if (myDepot && !myDepot->isTraining() && BWAPI::Broodwar->self()->minerals() >= 50) {
        myDepot->train(BWAPI::UnitTypes::Terran_SCV);
        BT_NODE::SUCCESS;
    }
    return BT_NODE::FAILURE;
}

BT_NODE::State buildSupply(void* data) {
    const int unusedSupply = Tools::GetTotalSupply(true) - BWAPI::Broodwar->self()->supplyUsed();
    if (unusedSupply <= 2) {
        const BWAPI::UnitType supplyProviderType = BWAPI::Broodwar->self()->getRace().getSupplyProvider();
        StarterBot::buildDepot(supplyProviderType);
        return BT_NODE::SUCCESS;
    }
    return BT_NODE::FAILURE;
}

BT_NODE::State buildBarracks(void* data) {
    const BWAPI::UnitType unitType = BWAPI::UnitTypes::Terran_Barracks;
    if (Tools::BuildBuilding(unitType)) {
        BWAPI::Broodwar->printf("Started Building %s", unitType.getName().c_str());
        cout << "Barrack is built" << endl;
        return BT_NODE::SUCCESS;
    }
    return BT_NODE::FAILURE;
}

BT_NODE::State recruitMarine(void* data) {
    StarterBot::Data* dataPtr = (StarterBot::Data*)data;
    const BWAPI::UnitType depot = BWAPI::UnitTypes::Terran_Barracks;
    if (dataPtr->nbBarracks >= 1) {
        StarterBot::trainUnits(BWAPI::UnitTypes::Terran_Marine, depot);
        return BT_NODE::SUCCESS;
    }
    return BT_NODE::FAILURE;
}

/* Build Behaviour Tree */

BT_NODE* buildBT() {

    BT_NODE* behaviourTree = new BT_LIST(nullptr, 6);
    BT_NODE* root;

    root = new BT_SEQUENCER(behaviourTree, 2);
    new BT_LEAF(root, SCVInferiorTo<8>);
    new BT_LEAF(root, trainSCV);

    root = new BT_SEQUENCER(behaviourTree, 2);
    new BT_LEAF(root, SCVInferiorTo<11>);
    new BT_LEAF(root, buildSupply);

    root = new BT_SEQUENCER(behaviourTree, 2);
    new BT_LEAF(root, nbBarracksInferiorTo<1>);
    new BT_LEAF(root, buildBarracks);

    root = new BT_SEQUENCER(behaviourTree, 2);
    new BT_LEAF(root, nbMarinesInferiorTo<8>);
    new BT_LEAF(root, recruitMarine);

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

// Called on each frame of the game
void StarterBot::onFrame()
{
    // Update our MapTools information
    m_mapTools.onFrame();

    // update data
    data->SCV = Tools::CountUnitsOfType(BWAPI::UnitTypes::Terran_SCV, BWAPI::Broodwar->self()->getUnits());
    data->supply = Tools::CountUnitsOfType(BWAPI::UnitTypes::Terran_Supply_Depot, BWAPI::Broodwar->self()->getUnits());
    data->nbBarracks = Tools::CountUnitsOfType(BWAPI::UnitTypes::Terran_Barracks, BWAPI::Broodwar->self()->getUnits());
    data->nbMarines = Tools::CountUnitsOfType(BWAPI::UnitTypes::Terran_Marine, BWAPI::Broodwar->self()->getUnits());

    // Run through the Behaviour Tree
    behaviourTree->Evaluate(data);

    sendMarinetoAttack(data);
    //ScoutUnexploredMap();

    // Send our idle workers to mine minerals so they don't just stand there
    sendIdleWorkersToMinerals();

    // Draw unit health bars, which brood war unfortunately does not do
    Tools::DrawUnitHealthBars();

    // Draw some relevent information to the screen to help us debug the bot
    drawDebugInformation();
}

// Called whenever the game ends and tells you if you won or not
void StarterBot::onEnd(bool isWinner) 
{
    std::cout << "We " << (isWinner ? "won!" : "lost!") << "\n";
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

void StarterBot::buildDepot(const BWAPI::UnitType DepotType) {
    const bool startedBuilding = Tools::BuildBuilding(DepotType);
    if (startedBuilding)
    {
        BWAPI::Broodwar->printf("Started Building %s", DepotType.getName().c_str());
    }
}

void StarterBot::sendMarinetoAttack(void* data){
    StarterBot::Data* dataPtr = (StarterBot::Data*)data;
    const BWAPI::Unitset& myUnits = BWAPI::Broodwar->self()->getUnits();
    for (auto& unit : myUnits){
        // if the unit is type Marine 
        if (unit->getType() == BWAPI::UnitTypes::Terran_Marine && unit->isIdle()){
            std::cout << "we have marine idle" << std::endl;
            /*
            BWAPI::TilePosition PlayerStart = BWAPI::Broodwar->enemy()->getStartLocation();
            BWAPI::Position p_PlayerStart = BWAPI::Position(PlayerStart);

            std::cout << "Enemy Start Location: " << p_PlayerStart << std::endl;
            
            BWAPI::Unitset PlayerUnit = BWAPI::Broodwar->enemy()->getUnits();
            BWAPI::Unit closestPlayerStart = Tools::GetClosestUnitTo(p_PlayerStart, PlayerUnit); */

            BWAPI::Unitset PlayerUnit = BWAPI::Broodwar->enemy()->getUnits();
            BWAPI::Unit closestPlayerStart = Tools::GetClosestUnitTo(BWAPI::Position(BWAPI::Broodwar->self()->getStartLocation()), PlayerUnit);

            if (closestPlayerStart) {
                std::cout << "Attacking" << std::endl;
                unit->attack(closestPlayerStart);
            }
      
        }

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