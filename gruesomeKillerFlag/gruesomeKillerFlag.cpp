/*
 Custom flag: Gruesome Killer (+GK)
 Kills explode in a shockwave. Kills from this shock wave also explode in a shock
 wave.
 
 Server Variables:
  _gruesomeKillerBlossomCount - the number of shots in the death blossom explosion at ground level
 
 Special notes:
 - The explosion shots and shock waves are world weapons, which contain metadata 'type' and 'owner'.

 Copyright 2022 Quinn Carmack
 May be redistributed under either the LGPL or MIT licenses.
 
 ./configure --enable-custom-plugins=gruesomeKillerFlag
*/
 
#include "bzfsAPI.h"
#include <math.h>
using namespace std;

class GruesomeKillerFlag : public bz_Plugin
{
	virtual const char* Name()
	{
		return "Gruesome Killer Flag";
	}

	virtual void Init(const char*);
	virtual void Event(bz_EventData*);
	~GruesomeKillerFlag();

	virtual void Cleanup(void)
	{
		Flush();
	}
};

BZ_PLUGIN(GruesomeKillerFlag)

void GruesomeKillerFlag::Init(const char*)
{	
	bz_RegisterCustomFlag("GK", "Gruesome Killer", "Kills explode in a shock wave and shrapnel.", 0, eGoodFlag);
	bz_registerCustomBZDBInt("_gruesomeKillerBlossomCount", 12);
	Register(bz_ePlayerDieEvent);
}

GruesomeKillerFlag::~GruesomeKillerFlag() {}

/*
  fireShot(bz_PlayerDieEventData_V1, float, bool)
  This function is used in the detonate function below. The bool "up" indicates
  whether the shots fire upward at the 45 degree angle.
*/
void fireShot(float playerPos[3], int killerID, float rotation, bool up)
{
	float vel[3] = { cos(rotation), sin(rotation), 0 };
	float pos[3] = { playerPos[0], playerPos[1], (float)bz_getBZDBDouble("_muzzleHeight") };
	
	if (up)
	{
		vel[2] = sqrt(2)/2.0;
		vel[0] *= sqrt(2)/2.0;
		vel[1] *= sqrt(2)/2.0;
	}
	
	// Assigning the shot metadata
	uint32_t shotGUID = bz_fireServerShot("GK", pos, vel, bz_getPlayerTeam(killerID));
	bz_setShotMetaData(shotGUID, "type", "GK");
	bz_setShotMetaData(shotGUID, "owner", killerID);
}

/*
  detonate(bz_PlayerDieEventData_V1)
  This function detonates the gruesome killer explosion effect. There are a ring
  of death blossom shots that explode on the ground, a second ring that fire at a
  45 degree angle, one singluar shot upward, and shock wave. All of these shots
  can cascade and cause more gruesome killer detonations.
  playerPos[3] - the position 
*/
void detonate(float playerPos[3], float rotation, int killerID)
{
	// Shock wave
	float vel[3] = { 0, 0, 0 };
  	uint32_t shotGUID = bz_fireServerShot("SW", playerPos, vel, bz_getPlayerTeam(killerID));
  	bz_setShotMetaData(shotGUID, "type", "GK");
	bz_setShotMetaData(shotGUID, "owner", killerID);

	// Ground-level death blossom
	for (int i = 0; i < bz_getBZDBInt("_gruesomeKillerBlossomCount"); i++)
	{
		fireShot(playerPos, killerID, i*((2*3.142)/bz_getBZDBInt("_gruesomeKillerBlossomCount")) + rotation, false);
	}
	
	// Death blossom at 45 degree angle
	int upCount = bz_getBZDBInt("_gruesomeKillerBlossomCount")/sqrt(2);
	for (int i = 0; i < upCount; i++)
	{
		fireShot(playerPos, killerID, i*((2*3.142)/upCount) + rotation, true);
	}
	
	// Shot going directly upward
	vel[0] = 0;
	vel[1] = 0;
	vel[2] = 1;
	shotGUID = bz_fireServerShot("GK", playerPos, vel, bz_getPlayerTeam(killerID));
	bz_setShotMetaData(shotGUID, "type", "GK");
	bz_setShotMetaData(shotGUID, "owner", killerID);
}


void GruesomeKillerFlag::Event(bz_EventData *eventData)
{
 	if (eventData->eventType == bz_ePlayerDieEvent)
 	{
   		bz_PlayerDieEventData_V2* data = (bz_PlayerDieEventData_V2*) eventData;
    	
    	// Handles player world weapon shots
	    uint32_t shotGUID = bz_getShotGUID(data->killerID, data->shotID);
	    if (bz_shotHasMetaData(shotGUID, "type") && bz_shotHasMetaData(shotGUID, "owner"))
	    {
	        std::string flagType = bz_getShotMetaDataS(shotGUID, "type");

	        if (flagType == "GK")
	        {
	            data->killerID = bz_getShotMetaDataI(shotGUID, "owner");
	            data->killerTeam = bz_getPlayerTeam(data->killerID);
	            data->flagKilledWith = "GK";
	        }
	    }
    	
    	if (data->flagKilledWith == "GK")
		{
			bz_ApiString flagHeldWhenKilled = bz_getFlagName(data->flagHeldWhenKilled);
			// If we have Avenger, detonate our killer
			if (flagHeldWhenKilled == "AV")
			{
				bz_BasePlayerRecord *killerRecord = bz_getPlayerByIndex(data->killerID);
				
				if (killerRecord)
				{
					detonate(killerRecord->lastKnownState.pos, killerRecord->lastKnownState.rotation, data->playerID);
				}
				
				bz_freePlayerRecord(killerRecord);
			}
			// Otherwise, detonate ourselves
			else
			{
				detonate(data->state.pos, data->state.rotation, data->killerID);
			}
		}
 	}
}





















