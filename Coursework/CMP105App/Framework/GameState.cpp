#include "GameState.h"


// Set the current state
void GameState::setCurrentState(State s)
{
	currentState = s;
}

// Returns the current state.
State GameState::getCurrentState()
{
	return currentState;
}

void GameState::addResult(std::string attr, float v)
{
	if (attr == "l1time") results->L1Time = v;
	if (attr == "l1deaths") results->L1Deaths = int(v);
	if (attr == "l2time") results->L2Time = v;
	if (attr == "l2deaths") results->L2Deaths = int(v);
	if (attr == "l2distance") results->L2Distance = v;
	if (attr == "l3time") results->L3Time = v;
	if (attr == "l3deaths") results->L3Deaths = int(v);
}

runResults* GameState::getResults()
{
	return results;
}

bool GameState::getSingleRun()
{
	return singleRun;
}

void GameState::setSingleRun(bool t)
{
	singleRun = t;
}

