#include "DA2UE4.h"
#include "DA2UE4Creature.h"

#include "m2da_constants_h.h"
#include "log_h.h"
#include "core_h.h"

#include "ldf.h"
#include "ui_h.h"

void UI_DisplayAbilityMessage(AActor* oTarget, int32 nAbility, int32 nDeactivate)
{
	if (!IsPartyMember(oTarget))
	{
		if (nAbility == ABILITY_TALENT_STEALTH)
		{
			return;
		}

		if (!GetCombatState(oTarget))
		{
			return;
		}
	}

	if (!nDeactivate && (!GetStealthEnabled(oTarget) || IsPartyMember(oTarget)))
	{
		if (GetAttributeBool("ShowSpecialMove"))//GetShowSpecialMoveFloaties()
		{
			int32 nStringId = GetM2DAInt(TABLE_ABILITIES_SPELLS, "Namestrref", nAbility);
			FString sDisplay = "";
			if (nStringId == 0)
			{
#ifdef DEBUG
				if (GetDebugHelpersEnabled())
				{
					sDisplay = "?- NO STRINGREF - " + GetM2DAString(TABLE_ABILITIES_SPELLS, "Label", nAbility);
				}
#endif
				return;
			}
			else
			{
				sDisplay = GetStringByStringId(nStringId);
			}

			/*  if(nDeactivate && GetDebugHelpersEnabled())
			{
			//    s = "?DEACTIVATING: " + s;
			}*/

			DisplayFloatyMessage(oTarget, sDisplay, FLOATY_MESSAGE, UI_DISPLAY_COLOR_ABILITY, UI_DISPLAY_DURATION_ABILITY);
		}
	}
}

void UI_DisplayMessage(AActor* oTarget, int32 nMessage, FString sParam, int32 nColor)
{
	/*
	FLOATY_HIT - Rising white number
	FLOATY_CRITICAL_HIT - Rising white number that grows and turns red
	FLOATY_MESSAGE - Yellow text (for debugging)
	*/

	if (GetM2DAInt(TABLE_UI_MESSAGES, "bDisabled", nMessage))
	{
#ifdef DEBUG
		Log_Trace(LOG_CHANNEL_UIMESSAGES, "UI_DisplayMessage " + IntToString(nMessage), "Not showing message " + IntToString(nMessage) + " - bDisabled set.", oTarget);
#endif
		return;
	}

	int32 bShow = UI_CheckObjectMask(nMessage, oTarget);
	if (!bShow)
	{
#ifdef DEBUG
		Log_Trace(LOG_CHANNEL_UIMESSAGES, "UI_DisplayMessage " + IntToString(nMessage), "Not showing message " + IntToString(nMessage) + " - Object Mask Failed", oTarget);
#endif
	}


	bShow = bShow && UI_CheckDisplayMask(nMessage, oTarget) && UI_CheckObjectMask(nMessage, oTarget);


	if (bShow)
	{
		int32 nId = GetM2DAInt(TABLE_UI_MESSAGES, "StringId", nMessage);
		FString sMessage;

		int32 nType = GetM2DAInt(TABLE_UI_MESSAGES, "Type", nMessage);

		float fDuration = GetM2DAFloat(TABLE_UI_MESSAGES, "Duration", nMessage);
		if (fDuration <= 0.2f)
		{
			fDuration = UI_DISPLAY_DURATION_DEFAULT;
		}


		if (nType == 2 /*debug*/ && !GetDebugHelpersEnabled())
		{
#ifdef DEBUG
			Log_Trace(LOG_CHANNEL_UIMESSAGES, "UI_DisplayMessage " + IntToString(nMessage), "Not showing type 2 (debug) message " + IntToString(nMessage) + " - Debug Helpers disabled", oTarget);
#endif
			return;
		}

		// -----------------------------------------------------------------
		// special messages are defined in the scripted param to this function
		// -----------------------------------------------------------------
		if (nMessage >= UI_MESSAGE_TYPE_SPECIAL)
		{
			sMessage = sParam;
		}
		else
		{
			if (nType == 2 && !LOG_ENABLED)
			{
#ifdef DEBUG
				Log_Trace(LOG_CHANNEL_UIMESSAGES, "UI_DisplayMessage " + IntToString(nMessage), "Not showing message " + IntToString(nMessage) + " - Debug Type 2 but not log enabled.", oTarget);
#endif
				return;
			}

			if (nId == 0)
			{
				sMessage = "?" + GetM2DAString(TABLE_UI_MESSAGES, "StringText", nMessage);
			}
			else
			{
				sMessage = GetStringByStringId(nId);

				if (sMessage == "")
				{
#ifdef DEBUG
					LogWarning("String table lookup on UI message failed. Blocking bug to georg. Details:" + IntToString(nMessage) + " strid:" + IntToString(nId));
#endif
				}
			}
		}


		if (GetObjectType(oTarget) == OBJECT_TYPE_CREATURE)
		{
			if (GetStealthEnabled(oTarget) && !IsPartyMember(oTarget))
			{
#ifdef DEBUG
				Log_Trace(LOG_CHANNEL_UIMESSAGES, "UI_DisplayMessage", "Not showing message: creature stealthy", oTarget);
#endif
				return;
			}

		}

		if (nColor == 0)
		{
			nColor = GetM2DAInt(TABLE_UI_MESSAGES, "Color", nMessage);
		}
#ifdef DEBUG
		Log_Trace(LOG_CHANNEL_UIMESSAGES, "UI_DisplayMessage " + IntToString(nMessage) + " " + IntToHexString(nColor), sMessage, oTarget);
#endif
		if (nType == UI_MESSAGE_TYPE_PORTRAIT)
		{
			DisplayPortraitMessage(oTarget, sMessage, nColor);
		}
		else  if (nType == UI_MESSAGE_TYPE_PORTRAIT_FOLLOWER)
		{
			if (IsFollower(oTarget))
			{
				DisplayPortraitMessage(oTarget, sMessage, nColor);
			}
			else
			{
				DisplayFloatyMessage(oTarget, sMessage, FLOATY_MESSAGE, nColor, fDuration);
			}
		}
		else
		{
			if (nMessage == UI_MESSAGE_MISSED || nMessage == UI_MESSAGE_IMMUNE)
			{
				DisplayFloatyMessage(oTarget, sMessage, FLOATY_HIT, nColor, fDuration);
			}
			else
			{
				DisplayFloatyMessage(oTarget, sMessage, FLOATY_MESSAGE, nColor, fDuration);
			}
		}
	}
	else
	{
#ifdef DEBUG
		Log_Trace(LOG_CHANNEL_UIMESSAGES, "UI_DisplayMessage", "not showing " + IntToString(nMessage), oTarget);
#endif
	}
}

int32 UI_CheckObjectMask(int32 nMessage, AActor* oTarget)
{
	int32 nDispMask = GetM2DAInt(TABLE_UI_MESSAGES, "ObjectMask", nMessage);

	if (nDispMask != 0)
	{
		int32 bShow = ((nDispMask & 0xffffff) == 0xffffff);

		if (!bShow)
		{
			bShow = ((nDispMask & GetObjectType(oTarget)) == GetObjectType(oTarget));
		}

		return bShow;
	}
	return FALSE_;
}

int32 UI_CheckDisplayMask(int32 nMessage, AActor* oTarget)
{
	int32 nDispMask = GetM2DAInt(TABLE_UI_MESSAGES, "DisplayMask", nMessage);


	if (nDispMask != 0)
	{
		int32 bShow = ((nDispMask & UI_DISPLAY_MASK_ALL) == UI_DISPLAY_MASK_ALL);

		if (!bShow)
		{
			int32 bPlayer = IsControlled(oTarget);
			int32 bParty = IsFollower(oTarget);
			int32 bHostile = IsObjectHostile(GetHero(), oTarget);

			bShow = ((nDispMask & UI_DISPLAY_MASK_PLAYER) == UI_DISPLAY_MASK_PLAYER) && bPlayer;
			bShow = bShow || (((nDispMask & UI_DISPLAY_MASK_PARTY) == UI_DISPLAY_MASK_PARTY) && bParty);
			bShow = bShow || (((nDispMask & UI_DISPLAY_MASK_HOSTILE) == UI_DISPLAY_MASK_HOSTILE) && bParty);
		}

		return bShow;
	}
	return FALSE_;
}