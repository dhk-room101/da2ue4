#pragma once

#include "ldf.h"

//#define DEBUG //moved to global DA2UE4.h

//------------------------------------------------------------------------------
// log_h - Design Log and Debug Framework
//------------------------------------------------------------------------------
/*
Provide designers with standardized functions to add log and debug messages
to the DejaInsight System.

Georg:
By default, the log functions will make use of LogTrace, which I wrote to
directly access DEJA_TRACE. If deja is not running or the game executable
has not been compiled with Deja support, the game will use ECLog instead.

*/
//------------------------------------------------------------------------------
// @author Yaron, georg
//------------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Georg: This is the compiler side killswitch for DEJA/Eclipse Log output
//        after Sunday, Feb 22, 2009, this defaults to commented out.
//
//        If you need log/deja output from your scripts to debug, you need to
//        remove the 'NO' from the symbol. You should do so only ever in a local,
//        temporary copy of this file and never while the file is checked out.
//
//        If you have questions regarding this mechanism, please talk to me.
//
//        Note: SHIP mode executables do not support the LogTrace command and as
//              such will never provide deja/log output, even if this symbol
//              is defined.
//
// -----------------------------------------------------------------------------

// Old Design side log killswitch, just leave it alone.
const int32 LOG_ENABLED = TRUE_;

// severity / priority settings
const int32 LOG_SEVERITY_MESSAGE = 0;
const int32 LOG_SEVERITY_WARNING = 2;
const int32 LOG_SEVERITY_CRITICAL = 3;

// const int32 LOG_LEVEL_ERROR = LOG_SEVERITY_CRITICAL;
 const int32 LOG_LEVEL_DEBUG = LOG_SEVERITY_MESSAGE;
// const int32 LOG_LEVEL_WARNING = LOG_SEVERITY_WARNING;
const int32 LOG_LEVEL_CRITICAL = LOG_SEVERITY_CRITICAL;
// const int32 LOG_RULES_SUBTYPE_ATTACK = 1;
// const int32 LOG_RULES_SUBTYPE_DAMAGE = 2;
// const int32 LOG_SYSTEMS_SUBTYPE_GENERAL = 3;
// const int32 LOG_SYSTEMS_SUBTYPE_TALK_TRIGGER = 4;
// const int32 LOG_SYSTEMS_SUBTYPE_APPROVAL = 5;
// const int32 LOG_RULES_SUBTYPE_DEATH = 6;

/**
*  @brief Write a log message with origin information to a channel
*
*  Standard log handler for designer use. Please provide the file and function you
*  are calling the function from in sOrigin.
*  You can use oTarget to associate the
*  function call with a specific AActor (such as the target of an attack). Do not
*  pass OBJECT_SELF as that information is already contained in the log scope anyway.
*
*  @example: Log_Trace(LOG_CHANNEL_GENERAL,"myscript.myfunction", "hello world", aActor).
*
*  @param nLogChannel A LOG_CHANNEL_* constant defining the deja channel to write to.
*                     If you need additional channels, please talk to georg
*  @param sOrigin   The file and function you are calling from (e.g. "core_h.GetPlayerName")
*  @param sMessage  Your log message
*  @param nSeverity The LOG_SEVERITY_* severity of the mesage.
*
*  @sa Log_Msg.
*
*  @author georg
**/
void Log_Trace(int32 nLogChannel, FString sOrigin = "", FString sMessage = "", AActor* oTarget = nullptr, int32 nPriority = LOG_SEVERITY_MESSAGE);

/**
*  @brief Specialized version of Log_Trace to debug Events firing
*
*  This writes to LOG_CHANNEL_PLOT
*
*  @param sOrigin    The name of the calling function (e.g. "core_h.GetPlayerName")
*  @param ev         The event being debugged.
*
*  @author Georg
**/
void Log_Events(FString sOrigin, FGameEvent ev, FString sMessage = "", int32 nLogChannel = LOG_CHANNEL_EVENTS);

/** @brief Return the name / label of an event by ID.
*
*
* @param nEventId - The event Id to look up.
* @returns The contents of the label column in events.xls
* @author Georg Zoeller
*/
FString Log_GetEventNameById(int32 nEventId);

/**
*  @brief Specialized version of Log_Trace to debug combat
*
*  This writes to LOG_CHANNEL_THREAT
*
*  @param sOrigin    The file and function you are calling from (e.g. "core_h.GetPlayerName")
*  @param sMessage   The log message
*  @param nSeverity   The log severity
*
*  @author Yaron
**/
void Log_Trace_Threat(AActor* OBJECT_SELF, FString sOrigin, FString sMessage = "", int32 nSeverity = LOG_SEVERITY_MESSAGE);

/** @brief Return the name / label of a command by ID.
*
*
* @param nCommandId - The Command Id to look up.
* @returns The contents of the label column in commands.xls
* @author Georg Zoeller
*/
FString Log_GetCommandNameById(int32 nCommandId);

/**
*  @brief Specialized version of Log_Trace to debug combat
*
*  This writes to LOG_CHANNEL_AI
*
*  @param sOrigin    The file and function you are calling from (e.g. "core_h.GetPlayerName")
*  @param sMessage   The log message
*  @param nSeverity   The log severity
*
*  @author Yaron
**/
void Log_Trace_AI(AActor* aActor, FString sOrigin, FString sMessage = "", int32 nSeverity = LOG_SEVERITY_MESSAGE);

/**
*  @brief Write a log message with origin information to a channel
*
*  Standard log handler for designer use. Please use Log_Trace instead when possible
*
*  You can use oTarget to associate the function call with a specific AActor
*  (such as the target of an attack). Do not pass OBJECT_SELF as that information
*  is already contained in the log scope anyway.
*
*  @example: Log_Trace(LOG_CHANNEL_GENERAL,"myscript.myfunction", "hello world", oActor).
*
*  @param nLogChannel A LOG_CHANNEL_* constant defining the deja channel to write to.
*                     If you need additional channels, please talk to georg
*  @param sMessage  Your log message
*  @param nSeverity The LOG_SEVERITY_* severity of the mesage.
*
*  @sa Log_Trace
*
*  @author georg
**/
void Log_Msg(int32 nLogChannel, FString sMessage = "", AActor* oTarget = nullptr, int32 nPriority = LOG_SEVERITY_MESSAGE);

/**
*  @brief Specialized version of Log_Trace to message scripting errors to designers
*
*  This writes to LOG_CHANNEL_DESIGN_SCRIPTERROR
*
*  @param sOrigin   The file and function you are calling from (e.g. "core_h.GetPlayerName")
*  @param sMessage  The message you want to put into the log
*  @param oTarget   Optional target AActor
*
*  @author georg
**/
void Log_Trace_Scripting_Error(FString sOrigin, FString sMessage, AActor* oTarget = nullptr);

/**
*  @brief Specialized version of Log_Trace for command queue debugging
*

*  @param sOrigin   The file and function you are calling from (e.g. "core_h.GetPlayerName")
*  @param cmd       The Command to debug
*  @param oTarget   The command target
*
*  @author georg
**/
void Log_Trace_Commands(FString sOrigin, FCommand cmd, AActor* oTarget);

/**
*  @brief Specialized version of Log_Trace to debug combat
*
*  This writes to LOG_CHANNEL_COMBAT, unless changed.
*
*  @param sOrigin    The file and function you are calling from (e.g. "core_h.GetPlayerName")
*  @param sMessage   The log message
*  @param oAttacker  Attacking Object
*  @param oTarget    Target / Defender / Victim AActor
*  @param oTarget    Target / Defender / Victim AActor
*  @param nLogChannel The log channel to write to (default LOG_CHANNEL_COMBAT)
*  @param nSeverity   The log severity
*
*  @author georg
**/
void Log_Trace_Combat(FString sOrigin, FString sMessage = "", AActor* oAttacker = nullptr, AActor* oTarget = nullptr, int32 nLogChannel = LOG_CHANNEL_COMBAT, int32 nSeverity = LOG_SEVERITY_MESSAGE);

/**
*  @brief Specialized version of Log_Trace to debug effects
*
*  This writes to LOG_CHANNEL_EFFECTS
*
*  @param sOrigin    The file and function you are calling from (e.g. "core_h.GetPlayerName")
*  @param eEffect    The Effect to debug
*  @param sMessage   Optional message
*  @param oTarget    Optional target information
*
*  @author georg
**/
void Log_Trace_Effects(FString sOrigin, FEffect eEffect, FString sMessage, AActor* oTarget = nullptr, int32 nDurationType = -1, int32 nAbilityId = -1);

/** @brief Return the name / label of a effect by ID.
*
*
* @param nEventId - The effect Id to look up.
* @returns The contents of the label column in events.xls
* @author Georg Zoeller
*/
FString Log_GetEffectNameById(int32 nEffectId);

FString Log_GetAttackResultNameById(int32 nAttackResult);