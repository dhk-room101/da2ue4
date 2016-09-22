#pragma once

class Engine;
class ADA2UE4Party;

FPlot ParsePlot(int32 nPlotID);

int64 PlotFlagConversion(int32 ResRefID, int64 Flag);

int32 SetPlotFlag(int32 index, int64 nPlotFlag, int32 nPlotValue);

int32 HandlePlotFlag(int32 nPlotEventType /*get/set*/, FString nPlotGUID, int64 nPlotFlag, int32 fPlotValue /*0/1=clear/set*/ = -1);//-1 doesn't set/clear anything

/* @brief Returns the resref of the plot (without extension) as a string
*
* @param strPlot a string containing the guid or resref of the plot
* @author Hesky Fisher
*/
FString GetPlotResRef(int64 nPlotHash);

/** @brief Returns the value of a plot flag
*
* Queries the state of a plot flag from a party's plot table. In order to query DEFINED flags, 
* this function needs to query the plot script associated with that flag.
*
* @param oParty - Party Object ID
* @param strPlot - Plot name to query
* @param nFlag - Plot flag # to query (32-127)
* @param nCallScript - Whether or not to call the plot script. Note: this should not be set to TRUE 
* when this function is used inside a plot script. If nBit is a defined flag then the script will 
* start calling itself recursively.
* @sa SetPartyPlotFlag(), GetPartyPlotVar()
* @returns value of the flag - TRUE or FALSE
* @author Derek Beland
*/
int32 GetPartyPlotFlag(int64 nPlotHash, int32 nFlag);

/** @brief Returns the name of the plot flag as a FString
*
* @param strPlot - a FString containing the guid or resref of the plot
* @param nFlag - plot flag number to query
* @author Hesky Fisher
*/
FString GetPlotFlagName(int64 nPlotHash, int32 nFlag);