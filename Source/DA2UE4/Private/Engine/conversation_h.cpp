#include "DA2UE4.h"
#include "DA2UE4GameInstance.h"
#include "DA2UE4PlayerController.h"
#include "DA2UE4Party.h"
#include "DA2UE4Creature.h"
#include "DA2UE4AnimInstance.h"

#include "AnimationRuntime.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequence.h"
#include "Runtime/Engine/Classes/Animation/AnimMontage.h" // 4.13 error

#include "conversation_h.h"

#include "ldf.h"
#include "plot_h.h"
#include "events_h.h"
#include "UserWidgetConversation.h"
#include "UMG.h"
#include "STypes.h"

#include "Kismet/KismetMathLibrary.h"

void ParseConversation(int32 nConvID)
{
	FString sValue;

	FString sConv = FString::FromInt(nConvID);

	FString aFullPath = FPaths::GameDevelopersDir();
	aFullPath += "Source/JSON/CNV/";
	aFullPath += *sConv;
	aFullPath += ".json";
	FString JsonStr;
	FFileHelper::LoadFileToString(JsonStr, *aFullPath);

	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonStr);
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
	{
		TSet<FString> animations;
		FConversation conversation;
		conversation.ResRefID = nConvID;

		TMap<FString, TSharedPtr<FJsonValue>> JsonValuesMap = JsonObject->Values;
		TSharedPtr<FJsonValue> rootValues;
		JsonValuesMap.RemoveAndCopyValue("Resource", rootValues);
		TSharedPtr<FJsonObject> rootObject = rootValues->AsObject();

		TMap<FString, TSharedPtr<FJsonValue>> rootValuesMap = rootObject->Values;
		TSharedPtr<FJsonValue> agentValues;
		rootValuesMap.RemoveAndCopyValue("Agent", agentValues);
		TSharedPtr<FJsonObject> agentObject = agentValues->AsObject();

		TMap<FString, TSharedPtr<FJsonValue>> agentValuesMap = agentObject->Values;

		TSharedPtr<FJsonValue> StartListValues;
		agentValuesMap.RemoveAndCopyValue("StartList", StartListValues);
		TSharedPtr<FJsonObject> StartListObject = StartListValues->AsObject();

		TArray<TSharedPtr<FJsonValue>> StartArray = StartListObject->GetArrayField("Agent");

		if (StartArray.Num() > 0)
		{
			int32 counter = 0;
			do {
				TSharedPtr<FJsonValue> StartLineValue = StartArray[counter];
				TSharedPtr<FJsonObject> StartLineObject = StartLineValue->AsObject();

				TMap<FString, TSharedPtr<FJsonValue>> StartLineIndexValuesMap = StartLineObject->Values;

				for (auto const& z : StartLineIndexValuesMap)
				{
					if (z.Key == "LineIndex")
					{
						z.Value.Get()->TryGetString(sValue);
						conversation.StartList.Add(FCString::Atoi(*sValue));
					}
				}

				counter++;
			} while (counter < StartArray.Num());
		}
		else
		{
			TMap<FString, TSharedPtr<FJsonValue>> StartValuesMap = StartListObject->Values;
			TSharedPtr<FJsonValue> StartAgentValues;
			StartValuesMap.RemoveAndCopyValue("Agent", StartAgentValues);
			TSharedPtr<FJsonObject> StartAgentObject = StartAgentValues->AsObject();

			TMap<FString, TSharedPtr<FJsonValue>> StartLineIndexValuesMap = StartAgentObject->Values;

			for (auto const& z : StartLineIndexValuesMap)
			{
				if (z.Key == "LineIndex")
				{
					z.Value.Get()->TryGetString(sValue);
					conversation.StartList.Add(FCString::Atoi(*sValue));
				}
			}
		}

		TSharedPtr<FJsonValue> NPCLineListValues;
		agentValuesMap.RemoveAndCopyValue("NPCLineList", NPCLineListValues);
		TSharedPtr<FJsonObject> NPCLineListObject = NPCLineListValues->AsObject();

		TSharedPtr<FJsonValue> PlayerLineListValues;
		agentValuesMap.RemoveAndCopyValue("PlayerLineList", PlayerLineListValues);
		TSharedPtr<FJsonObject> PlayerLineListObject = PlayerLineListValues->AsObject();

		TArray<TSharedPtr<FJsonValue>> NPCArray = NPCLineListObject->GetArrayField("Agent");

		if (NPCArray.Num() > 0)
		{
			int32 counter = 0;
			do {
				FConvNode npcNode;

				npcNode.lineIndex = counter;

				TSharedPtr<FJsonValue> NPCLineValue = NPCArray[counter];
				TSharedPtr<FJsonObject> NPCLineObject = NPCLineValue->AsObject();

				TMap<FString, TSharedPtr<FJsonValue>> NPCLineValuesMap = NPCLineObject->Values;
				for (auto const& x : NPCLineValuesMap)
				{
					if (x.Key == "StringID")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.StringID = FCString::Atoi(*sValue);
					}
					if (x.Key == "LanguageID")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.LanguageID = FCString::Atoi(*sValue);
					}
					if (x.Key == "ConditionScriptURI")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.ConditionScriptURI = FCString::Atoi(*sValue);
					}
					if (x.Key == "ConditionParameter")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.ConditionParameter = FCString::Atoi(*sValue);
					}
					if (x.Key == "ConditionParameterText")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.ConditionParameterText = sValue;
					}
					if (x.Key == "ConditionPlotURI")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.ConditionPlotURI = FCString::Atoi(*sValue);
					}
					if (x.Key == "ConditionPlotFlag")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.ConditionPlotFlag = FCString::Atoi(*sValue);
					}
					if (x.Key == "ConditionResult")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.ConditionResult = sValue == "False" ? false : true;
					}
					if (x.Key == "ActionScriptURI")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.ActionScriptURI = FCString::Atoi(*sValue);
					}
					if (x.Key == "ActionParameter")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.ActionParameter = FCString::Atoi(*sValue);
					}
					if (x.Key == "ActionParameterText")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.ActionParameterText = sValue;
					}
					if (x.Key == "ActionPlotURI")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.ActionPlotURI = FCString::Atoi(*sValue);
					}
					if (x.Key == "ActionPlotFlag")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.ActionPlotFlag = FCString::Atoi(*sValue);
					}
					if (x.Key == "ActionResult")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.ActionResult = sValue == "False" ? false : true;
					}
					if (x.Key == "text")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.dwText = sValue;
					}
					if (x.Key == "TextRequiresReTranslation")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.TextRequiresReTranslation = sValue == "False" ? false : true;
					}
					if (x.Key == "TextRequiresReRecording")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.TextRequiresReRecording = sValue == "False" ? false : true;
					}
					if (x.Key == "Speaker")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.Speaker = sValue;
					}
					if (x.Key == "PreviousSpeaker")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.PreviousSpeaker = sValue;
					}
					if (x.Key == "Listener")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.Listener = sValue;
					}
					if (x.Key == "icon")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.icon = FCString::Atoi(*sValue);
					}
					if (x.Key == "Comment")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.Comment = sValue;
					}
					if (x.Key == "FastPath")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.FastPath = FCString::Atoi(*sValue);
					}
					if (x.Key == "SlideShowTexture")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.SlideShowTexture = sValue;
					}
					if (x.Key == "VoiceOverTag")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.VoiceOverTag = sValue;
					}
					if (x.Key == "VoiceOverComment")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.VoiceOverComment = sValue;
					}
					if (x.Key == "EditorComment")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.EditorComment = sValue;
					}
					if (x.Key == "LineVisibility")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.LineVisibility = FCString::Atoi(*sValue);
					}
					if (x.Key == "Ambient")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.Ambient = sValue == "False" ? false : true;
					}
					if (x.Key == "SkipLine")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.SkipLine = sValue == "False" ? false : true;
					}
					if (x.Key == "StageURI")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.StageURI = FCString::Atoi(*sValue);
					}
					if (x.Key == "StageTag")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.StageTag = sValue;
					}
					if (x.Key == "StageAtCurrentLocation")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.StageAtCurrentLocation = sValue == "False" ? false : true;
					}
					if (x.Key == "CameraTag")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.CameraTag = sValue;
					}
					if (x.Key == "CameraLocked")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.CameraLocked = sValue == "False" ? false : true;
					}
					if (x.Key == "SecondaryCameratag")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.SecondaryCameratag = sValue;
					}
					if (x.Key == "SecondaryCameraDelay")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.SecondaryCameraDelay = FCString::Atof(*sValue);
					}
					if (x.Key == "Emotion")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.Emotion = FCString::Atoi(*sValue);
					}
					if (x.Key == "CustomCutsceneURI")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.CustomCutsceneURI = FCString::Atoi(*sValue);
					}
					if (x.Key == "SpeakerAnimation")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.SpeakerAnimation = sValue;
					}
					if (x.Key == "RevertAnimation")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.RevertAnimation = sValue == "False" ? false : true;
					}
					if (x.Key == "LockAnimations")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.LockAnimations = sValue == "False" ? false : true;
					}
					if (x.Key == "PlaySoundEvents")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.PlaySoundEvents = sValue == "False" ? false : true;
					}
					if (x.Key == "RoboBradSeed")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.RoboBradSeed = FCString::Atoi(*sValue);
					}
					if (x.Key == "RoboBradSeedOverride")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.RoboBradSeedOverride = sValue == "False" ? false : true;
					}
					if (x.Key == "RoboBradLocked")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.RoboBradLocked = sValue == "False" ? false : true;
					}
					if (x.Key == "PreviewAreaURI")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.PreviewAreaURI = FCString::Atoi(*sValue);
					}
					if (x.Key == "PreviewStageUseFirstMatch")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.PreviewStageUseFirstMatch = sValue == "False" ? false : true;
					}
					if (x.Key == "PreviewStagePosition")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.PreviewStagePosition = FVector(0.f);//FCString::Atoi(*sValue);
					}
					if (x.Key == "PreviewStageOrientation")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.PreviewStageOrientation = FVector(0.f);//FCString::Atoi(*sValue);
					}
					if (x.Key == "UseAnimationDuration")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.UseAnimationDuration = sValue == "False" ? false : true;
					}
					if (x.Key == "NoVOInGame")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.NoVOInGame = sValue == "False" ? false : true;
					}
					if (x.Key == "Narration")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.Narration = sValue == "False" ? false : true;
					}
					if (x.Key == "PreCacheVO")
					{
						x.Value.Get()->TryGetString(sValue);
						npcNode.PreCacheVO = sValue == "False" ? false : true;
					}
					if (x.Key == "TransitionList")
					{
						TSharedPtr<FJsonObject> NPCTransitionListObject = x.Value->AsObject();
						TArray<TSharedPtr<FJsonValue>> NPCTransitionListArray = NPCTransitionListObject->GetArrayField("Agent");
						if (NPCTransitionListArray.Num() > 0)
						{
							int32 j = 0;
							do {
								FConvTransition npcNodeTransition;

								TSharedPtr<FJsonValue> NPCTransitionListValue = NPCTransitionListArray[j];
								TSharedPtr<FJsonObject> NPCTransitionListJson = NPCTransitionListValue->AsObject();

								TMap<FString, TSharedPtr<FJsonValue>> NPCTransitionLineValuesMap = NPCTransitionListJson->Values;

								for (auto const& y : NPCTransitionLineValuesMap)
								{
									if (y.Key == "IsLink")
									{
										y.Value.Get()->TryGetString(sValue);
										npcNodeTransition.IsLink = sValue == "False" ? false : true;
									}
									if (y.Key == "LineIndex")
									{
										y.Value.Get()->TryGetString(sValue);
										npcNodeTransition.LineIndex = FCString::Atoi(*sValue);
									}
								}

								npcNode.TransitionList.Add(npcNodeTransition);

								j++;
							} while (j < NPCTransitionListArray.Num());
						}
						else //only one transition
						{
							FConvTransition npcNodeTransition;

							TMap<FString, TSharedPtr<FJsonValue>> NPCTransitionValuesMap = NPCTransitionListObject->Values;
							TSharedPtr<FJsonValue> NPCTransitionListValues;
							NPCTransitionValuesMap.RemoveAndCopyValue("Agent", NPCTransitionListValues);
							TSharedPtr<FJsonObject> NPCTransitionListJson = NPCTransitionListValues->AsObject();

							TMap<FString, TSharedPtr<FJsonValue>> NPCTransitionLineValuesMap = NPCTransitionListJson->Values;

							for (auto const& y : NPCTransitionLineValuesMap)
							{
								if (y.Key == "IsLink")
								{
									y.Value.Get()->TryGetString(sValue);
									npcNodeTransition.IsLink = sValue == "False" ? false : true;
								}
								if (y.Key == "LineIndex")
								{
									y.Value.Get()->TryGetString(sValue);
									npcNodeTransition.LineIndex = FCString::Atoi(*sValue);
								}
							}

							npcNode.TransitionList.Add(npcNodeTransition);
						}
					}
					if (x.Key == "AnimationListList")
					{
						TSharedPtr<FJsonValue> AnimationListListValues;
						NPCLineValuesMap.RemoveAndCopyValue("AnimationListList", AnimationListListValues);
						TSharedPtr<FJsonObject> AnimationListListObject = AnimationListListValues->AsObject();

						TArray<TSharedPtr<FJsonValue>> AnimationListListArray = AnimationListListObject->GetArrayField("Agent");

						if (AnimationListListArray.Num() > 0)
						{
							int32 index = 0;
							do {
								FConvAnimationListList npcAnimationListListNode;

								TSharedPtr<FJsonValue> NPCLineAnimationListListValue = AnimationListListArray[index];
								TSharedPtr<FJsonObject> NPCLineAnimationListListObject = NPCLineAnimationListListValue->AsObject();

								TMap<FString, TSharedPtr<FJsonValue>> NPCLineAnimationListListValuesMap = NPCLineAnimationListListObject->Values;
								for (auto const& z : NPCLineAnimationListListValuesMap)
								{
									if (z.Key == "AnimationList")
									{
										TSharedPtr<FJsonObject> NPCAnimationListObject = z.Value->AsObject();
										TArray<TSharedPtr<FJsonValue>> NPCAnimationListArray = NPCAnimationListObject->GetArrayField("Agent");
										if (NPCAnimationListArray.Num() > 0)
										{
											int32 j = 0;
											do {
												FConvAnimationList npcAnimationList;

												TSharedPtr<FJsonValue> NPCAnimationListValue = NPCAnimationListArray[j];
												TSharedPtr<FJsonObject> NPCAnimationListJson = NPCAnimationListValue->AsObject();

												TMap<FString, TSharedPtr<FJsonValue>> NPCAnimationListLineValuesMap = NPCAnimationListJson->Values;

												for (auto const& y : NPCAnimationListLineValuesMap)
												{
													if (y.Key == "Animation")
													{
														y.Value.Get()->TryGetString(sValue);
														npcAnimationList.Animation = sValue;
													}
													if (y.Key == "StartTime")
													{
														y.Value.Get()->TryGetString(sValue);
														npcAnimationList.StartTime = FCString::Atof(*sValue);
													}
													if (y.Key == "Weight")
													{
														y.Value.Get()->TryGetString(sValue);
														npcAnimationList.Weight = FCString::Atof(*sValue);
													}
													if (y.Key == "Speed")
													{
														y.Value.Get()->TryGetString(sValue);
														npcAnimationList.Speed = FCString::Atof(*sValue);
													}
													if (y.Key == "StartOffset")
													{
														y.Value.Get()->TryGetString(sValue);
														npcAnimationList.StartOffset = FCString::Atof(*sValue);
													}
													if (y.Key == "BlendIn")
													{
														y.Value.Get()->TryGetString(sValue);
														npcAnimationList.BlendIn = FCString::Atof(*sValue);
													}
													if (y.Key == "EndOffset")
													{
														y.Value.Get()->TryGetString(sValue);
														npcAnimationList.EndOffset = FCString::Atof(*sValue);
													}
													if (y.Key == "BlendOut")
													{
														y.Value.Get()->TryGetString(sValue);
														npcAnimationList.BlendOut = FCString::Atof(*sValue);
													}
												}

												npcAnimationListListNode.AnimationList.Add(npcAnimationList);

												j++;
											} while (j < NPCAnimationListArray.Num());
										}
										else //only one AnimationList
										{
											FConvAnimationList npcAnimationList;

											TMap<FString, TSharedPtr<FJsonValue>> NPCAnimationListListValuesMap = NPCAnimationListObject->Values;
											TSharedPtr<FJsonValue> NPCAnimationListValues;
											NPCAnimationListListValuesMap.RemoveAndCopyValue("Agent", NPCAnimationListValues);
											TSharedPtr<FJsonObject> NPCAnimationListJson = NPCAnimationListValues->AsObject();

											TMap<FString, TSharedPtr<FJsonValue>> NPCAnimationListLineValuesMap = NPCAnimationListJson->Values;

											for (auto const& y : NPCAnimationListLineValuesMap)
											{
												if (y.Key == "Animation")
												{
													y.Value.Get()->TryGetString(sValue);
													npcAnimationList.Animation = sValue;
												}
												if (y.Key == "StartTime")
												{
													y.Value.Get()->TryGetString(sValue);
													npcAnimationList.StartTime = FCString::Atof(*sValue);
												}
												if (y.Key == "Weight")
												{
													y.Value.Get()->TryGetString(sValue);
													npcAnimationList.Weight = FCString::Atof(*sValue);
												}
												if (y.Key == "Speed")
												{
													y.Value.Get()->TryGetString(sValue);
													npcAnimationList.Speed = FCString::Atof(*sValue);
												}
												if (y.Key == "StartOffset")
												{
													y.Value.Get()->TryGetString(sValue);
													npcAnimationList.StartOffset = FCString::Atof(*sValue);
												}
												if (y.Key == "BlendIn")
												{
													y.Value.Get()->TryGetString(sValue);
													npcAnimationList.BlendIn = FCString::Atof(*sValue);
												}
												if (y.Key == "EndOffset")
												{
													y.Value.Get()->TryGetString(sValue);
													npcAnimationList.EndOffset = FCString::Atof(*sValue);
												}
												if (y.Key == "BlendOut")
												{
													y.Value.Get()->TryGetString(sValue);
													npcAnimationList.BlendOut = FCString::Atof(*sValue);
												}
											}

											npcAnimationListListNode.AnimationList.Add(npcAnimationList);
										}
									}
									if (z.Key == "Tag")
									{
										z.Value.Get()->TryGetString(sValue);
										npcAnimationListListNode.Tag = sValue;
									}
									if (z.Key == "Locked")
									{
										z.Value.Get()->TryGetString(sValue);
										npcAnimationListListNode.Locked = sValue == "False" ? false : true;
									}
								}

								npcNode.AnimationListList.Add(npcAnimationListListNode);

								index++;
							} while (index < AnimationListListArray.Num());
						}
					}
				}

				conversation.NPCLineList.Add(npcNode);

				counter++;
			} while (counter < NPCArray.Num());
		}

		TArray<TSharedPtr<FJsonValue>> PlayerArray = PlayerLineListObject->GetArrayField("Agent");

		if (PlayerArray.Num() > 0)
		{
			int32 counter = 0;
			do {
				FConvNode playerNode;

				playerNode.lineIndex = counter;

				TSharedPtr<FJsonValue> PlayerLineValue = PlayerArray[counter];
				TSharedPtr<FJsonObject> PlayerLineObject = PlayerLineValue->AsObject();

				TMap<FString, TSharedPtr<FJsonValue>> PlayerLineValuesMap = PlayerLineObject->Values;
				for (auto const& x : PlayerLineValuesMap)
				{
					if (x.Key == "StringID")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.StringID = FCString::Atoi(*sValue);
					}
					if (x.Key == "LanguageID")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.LanguageID = FCString::Atoi(*sValue);
					}
					if (x.Key == "ConditionScriptURI")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.ConditionScriptURI = FCString::Atoi(*sValue);
					}
					if (x.Key == "ConditionParameter")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.ConditionParameter = FCString::Atoi(*sValue);
					}
					if (x.Key == "ConditionParameterText")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.ConditionParameterText = sValue;
					}
					if (x.Key == "ConditionPlotURI")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.ConditionPlotURI = FCString::Atoi(*sValue);
					}
					if (x.Key == "ConditionPlotFlag")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.ConditionPlotFlag = FCString::Atoi(*sValue);
					}
					if (x.Key == "ConditionResult")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.ConditionResult = sValue == "False" ? false : true;
					}
					if (x.Key == "ActionScriptURI")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.ActionScriptURI = FCString::Atoi(*sValue);
					}
					if (x.Key == "ActionParameter")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.ActionParameter = FCString::Atoi(*sValue);
					}
					if (x.Key == "ActionParameterText")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.ActionParameterText = sValue;
					}
					if (x.Key == "ActionPlotURI")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.ActionPlotURI = FCString::Atoi(*sValue);
					}
					if (x.Key == "ActionPlotFlag")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.ActionPlotFlag = FCString::Atoi(*sValue);
					}
					if (x.Key == "ActionResult")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.ActionResult = sValue == "False" ? false : true;
					}
					if (x.Key == "text")
					{
						x.Value.Get()->TryGetString(sValue);

						const TCHAR *delim;
						delim = TEXT("`");

						TArray<FString> Parsed;
						sValue.ParseIntoArray(Parsed, delim);

						playerNode.dwWheel = FCString::Atoi(*Parsed[0]);
						playerNode.dwMood = Parsed[1];
						playerNode.dwText = Parsed[2];
						playerNode.spokenText = Parsed[3];

						//playerNode.dwText = sValue;
					}
					if (x.Key == "TextRequiresReTranslation")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.TextRequiresReTranslation = sValue == "False" ? false : true;
					}
					if (x.Key == "TextRequiresReRecording")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.TextRequiresReRecording = sValue == "False" ? false : true;
					}
					if (x.Key == "Speaker")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.Speaker = sValue;
					}
					if (x.Key == "PreviousSpeaker")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.PreviousSpeaker = sValue;
					}
					if (x.Key == "Listener")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.Listener = sValue;
					}
					if (x.Key == "icon")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.icon = FCString::Atoi(*sValue);
					}
					if (x.Key == "Comment")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.Comment = sValue;
					}
					if (x.Key == "FastPath")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.FastPath = FCString::Atoi(*sValue);
					}
					if (x.Key == "SlideShowTexture")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.SlideShowTexture = sValue;
					}
					if (x.Key == "VoiceOverTag")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.VoiceOverTag = sValue;
					}
					if (x.Key == "VoiceOverComment")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.VoiceOverComment = sValue;
					}
					if (x.Key == "EditorComment")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.EditorComment = sValue;
					}
					if (x.Key == "LineVisibility")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.LineVisibility = FCString::Atoi(*sValue);
					}
					if (x.Key == "Ambient")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.Ambient = sValue == "False" ? false : true;
					}
					if (x.Key == "SkipLine")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.SkipLine = sValue == "False" ? false : true;
					}
					if (x.Key == "StageURI")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.StageURI = FCString::Atoi(*sValue);
					}
					if (x.Key == "StageTag")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.StageTag = sValue;
					}
					if (x.Key == "StageAtCurrentLocation")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.StageAtCurrentLocation = sValue == "False" ? false : true;
					}
					if (x.Key == "CameraTag")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.CameraTag = sValue;
					}
					if (x.Key == "CameraLocked")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.CameraLocked = sValue == "False" ? false : true;
					}
					if (x.Key == "SecondaryCameratag")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.SecondaryCameratag = sValue;
					}
					if (x.Key == "SecondaryCameraDelay")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.SecondaryCameraDelay = FCString::Atof(*sValue);
					}
					if (x.Key == "Emotion")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.Emotion = FCString::Atoi(*sValue);
					}
					if (x.Key == "CustomCutsceneURI")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.CustomCutsceneURI = FCString::Atoi(*sValue);
					}
					if (x.Key == "SpeakerAnimation")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.SpeakerAnimation = sValue;
					}
					if (x.Key == "RevertAnimation")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.RevertAnimation = sValue == "False" ? false : true;
					}
					if (x.Key == "LockAnimations")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.LockAnimations = sValue == "False" ? false : true;
					}
					if (x.Key == "PlaySoundEvents")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.PlaySoundEvents = sValue == "False" ? false : true;
					}
					if (x.Key == "RoboBradSeed")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.RoboBradSeed = FCString::Atoi(*sValue);
					}
					if (x.Key == "RoboBradSeedOverride")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.RoboBradSeedOverride = sValue == "False" ? false : true;
					}
					if (x.Key == "RoboBradLocked")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.RoboBradLocked = sValue == "False" ? false : true;
					}
					if (x.Key == "PreviewAreaURI")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.PreviewAreaURI = FCString::Atoi(*sValue);
					}
					if (x.Key == "PreviewStageUseFirstMatch")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.PreviewStageUseFirstMatch = sValue == "False" ? false : true;
					}
					if (x.Key == "PreviewStagePosition")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.PreviewStagePosition = FVector(0.f);//FCString::Atoi(*sValue);
					}
					if (x.Key == "PreviewStageOrientation")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.PreviewStageOrientation = FVector(0.f);//FCString::Atoi(*sValue);
					}
					if (x.Key == "UseAnimationDuration")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.UseAnimationDuration = sValue == "False" ? false : true;
					}
					if (x.Key == "NoVOInGame")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.NoVOInGame = sValue == "False" ? false : true;
					}
					if (x.Key == "Narration")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.Narration = sValue == "False" ? false : true;
					}
					if (x.Key == "PreCacheVO")
					{
						x.Value.Get()->TryGetString(sValue);
						playerNode.PreCacheVO = sValue == "False" ? false : true;
					}
					if (x.Key == "TransitionList")
					{
						TSharedPtr<FJsonObject> PlayerTransitionListObject = x.Value->AsObject();
						TArray<TSharedPtr<FJsonValue>> PlayerTransitionListArray = PlayerTransitionListObject->GetArrayField("Agent");
						if (PlayerTransitionListArray.Num() > 0)
						{
							int32 j = 0;
							do {
								FConvTransition playerNodeTransition;

								TSharedPtr<FJsonValue> PlayerTransitionListValue = PlayerTransitionListArray[j];
								TSharedPtr<FJsonObject> PlayerTransitionListJson = PlayerTransitionListValue->AsObject();

								TMap<FString, TSharedPtr<FJsonValue>> PlayerTransitionLineValuesMap = PlayerTransitionListJson->Values;

								for (auto const& y : PlayerTransitionLineValuesMap)
								{
									if (y.Key == "IsLink")
									{
										y.Value.Get()->TryGetString(sValue);
										playerNodeTransition.IsLink = sValue == "False" ? false : true;
									}
									if (y.Key == "LineIndex")
									{
										y.Value.Get()->TryGetString(sValue);
										playerNodeTransition.LineIndex = FCString::Atoi(*sValue);
									}
								}

								playerNode.TransitionList.Add(playerNodeTransition);

								j++;
							} while (j < PlayerTransitionListArray.Num());
						}
						else //only one transition
						{
							FConvTransition playerNodeTransition;

							TMap<FString, TSharedPtr<FJsonValue>> PlayerTransitionValuesMap = PlayerTransitionListObject->Values;
							TSharedPtr<FJsonValue> PlayerTransitionListValues;
							PlayerTransitionValuesMap.RemoveAndCopyValue("Agent", PlayerTransitionListValues);
							TSharedPtr<FJsonObject> PlayerTransitionListJson = PlayerTransitionListValues->AsObject();

							TMap<FString, TSharedPtr<FJsonValue>> PlayerTransitionLineValuesMap = PlayerTransitionListJson->Values;

							for (auto const& y : PlayerTransitionLineValuesMap)
							{
								if (y.Key == "IsLink")
								{
									y.Value.Get()->TryGetString(sValue);
									playerNodeTransition.IsLink = sValue == "False" ? false : true;
								}
								if (y.Key == "LineIndex")
								{
									y.Value.Get()->TryGetString(sValue);
									playerNodeTransition.LineIndex = FCString::Atoi(*sValue);
								}
							}

							playerNode.TransitionList.Add(playerNodeTransition);
						}
					}
					if (x.Key == "AnimationListList")
					{
						TSharedPtr<FJsonValue> AnimationListListValues;
						PlayerLineValuesMap.RemoveAndCopyValue("AnimationListList", AnimationListListValues);
						TSharedPtr<FJsonObject> AnimationListListObject = AnimationListListValues->AsObject();

						TArray<TSharedPtr<FJsonValue>> AnimationListListArray = AnimationListListObject->GetArrayField("Agent");

						if (AnimationListListArray.Num() > 0)
						{
							int32 index = 0;
							do {
								FConvAnimationListList playerAnimationListListNode;

								TSharedPtr<FJsonValue> PlayerLineAnimationListListValue = AnimationListListArray[index];
								TSharedPtr<FJsonObject> PlayerLineAnimationListListObject = PlayerLineAnimationListListValue->AsObject();

								TMap<FString, TSharedPtr<FJsonValue>> PlayerLineAnimationListListValuesMap = PlayerLineAnimationListListObject->Values;
								for (auto const& z : PlayerLineAnimationListListValuesMap)
								{
									if (z.Key == "AnimationList")
									{
										TSharedPtr<FJsonObject> PlayerAnimationListObject = z.Value->AsObject();
										TArray<TSharedPtr<FJsonValue>> PlayerAnimationListArray = PlayerAnimationListObject->GetArrayField("Agent");
										if (PlayerAnimationListArray.Num() > 0)
										{
											int32 j = 0;
											do {
												FConvAnimationList playerAnimationList;

												TSharedPtr<FJsonValue> PlayerAnimationListValue = PlayerAnimationListArray[j];
												TSharedPtr<FJsonObject> PlayerAnimationListJson = PlayerAnimationListValue->AsObject();

												TMap<FString, TSharedPtr<FJsonValue>> PlayerAnimationListLineValuesMap = PlayerAnimationListJson->Values;

												for (auto const& y : PlayerAnimationListLineValuesMap)
												{
													if (y.Key == "Animation")
													{
														y.Value.Get()->TryGetString(sValue);
														playerAnimationList.Animation = sValue;
													}
													if (y.Key == "StartTime")
													{
														y.Value.Get()->TryGetString(sValue);
														playerAnimationList.StartTime = FCString::Atof(*sValue);
													}
													if (y.Key == "Weight")
													{
														y.Value.Get()->TryGetString(sValue);
														playerAnimationList.Weight = FCString::Atof(*sValue);
													}
													if (y.Key == "Speed")
													{
														y.Value.Get()->TryGetString(sValue);
														playerAnimationList.Speed = FCString::Atof(*sValue);
													}
													if (y.Key == "StartOffset")
													{
														y.Value.Get()->TryGetString(sValue);
														playerAnimationList.StartOffset = FCString::Atof(*sValue);
													}
													if (y.Key == "BlendIn")
													{
														y.Value.Get()->TryGetString(sValue);
														playerAnimationList.BlendIn = FCString::Atof(*sValue);
													}
													if (y.Key == "EndOffset")
													{
														y.Value.Get()->TryGetString(sValue);
														playerAnimationList.EndOffset = FCString::Atof(*sValue);
													}
													if (y.Key == "BlendOut")
													{
														y.Value.Get()->TryGetString(sValue);
														playerAnimationList.BlendOut = FCString::Atof(*sValue);
													}
												}

												playerAnimationListListNode.AnimationList.Add(playerAnimationList);

												j++;
											} while (j < PlayerAnimationListArray.Num());
										}
										else //only one AnimationList
										{
											FConvAnimationList playerAnimationList;

											TMap<FString, TSharedPtr<FJsonValue>> PlayerAnimationListListValuesMap = PlayerAnimationListObject->Values;
											TSharedPtr<FJsonValue> PlayerAnimationListValues;
											PlayerAnimationListListValuesMap.RemoveAndCopyValue("Agent", PlayerAnimationListValues);
											TSharedPtr<FJsonObject> PlayerAnimationListJson = PlayerAnimationListValues->AsObject();

											TMap<FString, TSharedPtr<FJsonValue>> PlayerAnimationListLineValuesMap = PlayerAnimationListJson->Values;

											for (auto const& y : PlayerAnimationListLineValuesMap)
											{
												if (y.Key == "Animation")
												{
													y.Value.Get()->TryGetString(sValue);
													playerAnimationList.Animation = sValue;
												}
												if (y.Key == "StartTime")
												{
													y.Value.Get()->TryGetString(sValue);
													playerAnimationList.StartTime = FCString::Atof(*sValue);
												}
												if (y.Key == "Weight")
												{
													y.Value.Get()->TryGetString(sValue);
													playerAnimationList.Weight = FCString::Atof(*sValue);
												}
												if (y.Key == "Speed")
												{
													y.Value.Get()->TryGetString(sValue);
													playerAnimationList.Speed = FCString::Atof(*sValue);
												}
												if (y.Key == "StartOffset")
												{
													y.Value.Get()->TryGetString(sValue);
													playerAnimationList.StartOffset = FCString::Atof(*sValue);
												}
												if (y.Key == "BlendIn")
												{
													y.Value.Get()->TryGetString(sValue);
													playerAnimationList.BlendIn = FCString::Atof(*sValue);
												}
												if (y.Key == "EndOffset")
												{
													y.Value.Get()->TryGetString(sValue);
													playerAnimationList.EndOffset = FCString::Atof(*sValue);
												}
												if (y.Key == "BlendOut")
												{
													y.Value.Get()->TryGetString(sValue);
													playerAnimationList.BlendOut = FCString::Atof(*sValue);
												}
											}

											playerAnimationListListNode.AnimationList.Add(playerAnimationList);
										}
									}
									if (z.Key == "Tag")
									{
										z.Value.Get()->TryGetString(sValue);
										playerAnimationListListNode.Tag = sValue;
									}
									if (z.Key == "Locked")
									{
										z.Value.Get()->TryGetString(sValue);
										playerAnimationListListNode.Locked = sValue == "False" ? false : true;
									}
								}

								playerNode.AnimationListList.Add(playerAnimationListListNode);

								index++;
							} while (index < AnimationListListArray.Num());
						}
					}
				}

				conversation.PlayerLineList.Add(playerNode);

				counter++;
			} while (counter < PlayerArray.Num());
		}
#ifdef DEBUG
		LogWarning("Conversation " + IntToString(nConvID) + " parsed!!");

#endif // DEBUG

		//TODO conversation pre-load per level/world
		GetCurrentPlayerController()->ConversationUI->conversation = conversation;
	}
	else {
#ifdef DEBUG
		LogError("Not Deserialized" + aFullPath);
#endif
	}
}

void StartConversation()
{
	if (GetCurrentPlayerController()->ConversationUI->conversation.ResRefID == 0) return;

	int32 bStart = FALSE_;
	int32 lIndex = 0;
	FConvNode node;
	int32 nPlotID;
	FPlot plot;
	FPlotElement ePlot;

	for (int32 n : GetCurrentPlayerController()->ConversationUI->conversation.StartList)
	{
		node = GetCurrentPlayerController()->ConversationUI->conversation.NPCLineList[n];
		//Let's analyze the current node conditions/plot, If any
		nPlotID = node.ConditionPlotURI;

		if (nPlotID == 0) //no condition, just play it
		{
			bStart = TRUE_;
			lIndex = n;//set the found starting branch
			break;
		}
		else //if (nPlotID != 0)//If there is an actual condition
		{
			//Check to see if plot already exists, if not create one
			for (FPlot p : GetParty()->Plots)
			{
				if (p.ResRefID == nPlotID)
				{
					plot = p;
					break;
				}
			}
			if (plot.ResRefID == 0) //Not found
			{
				//let's parse and create one
				plot = ParsePlot(nPlotID);
			}

			//Check defined flag versus regular flag
			if (node.ConditionPlotFlag > 255) //Defined flag, That is AND/OR combination of regular flags
			{
				if (HandlePlotFlag(EVENT_TYPE_GET_PLOT, plot.GUID, node.ConditionPlotFlag) == TRUE_)
				{
					bStart = TRUE_;
					lIndex = n;//set the found starting branch
					break;
				}
			}
			else //Regular flag, check directly
			{
#ifdef DEBUG
				LogWarning("Looking for condition ID " + IntToString(PlotFlagConversion(plot.ResRefID, node.ConditionPlotFlag)));
#endif // DEBUG

				for (FPlotElement e : plot.StatusList)
				{
					if (e.pNode.Flag == PlotFlagConversion(plot.ResRefID, node.ConditionPlotFlag))
					{
						ePlot = e;
#ifdef DEBUG
						LogWarning("Found plot flag name " + ePlot.pNode.Name + " for condition ID " + IntToString(PlotFlagConversion(plot.ResRefID, node.ConditionPlotFlag)));
#endif // DEBUG
						break;
					}
				}

#ifdef DEBUG
				LogWarning("Looking for matching condition " + IntToString(ePlot.pValue) + " for flag " + ePlot.pNode.Name + " on plot " + plot.ResRefName);
#endif // DEBUG
				if (ePlot.pNode.Name != "" && ePlot.pValue == UKismetMathLibrary::Conv_BoolToInt(node.ConditionResult))
				{
#ifdef DEBUG
					LogWarning("Found matching condition " + IntToString(ePlot.pValue) + " for flag " + ePlot.pNode.Name + " on plot " + plot.ResRefName);
#endif // DEBUG
					bStart = TRUE_;
					lIndex = n;//set the found starting branch
					break;
				}
			}
		}
	}

	if (bStart == TRUE_) //We actually found the Starting conversation node
	{
		FConvNode nNode = GetCurrentPlayerController()->ConversationUI->conversation.NPCLineList[lIndex];

		NPCLines(lIndex);

#ifdef DEBUG
		LogWarning("Branch start at lineIndex " + IntToString(lIndex));
		LogWarning("Starting at NPC line ID " + IntToString(nNode.StringID) + " w/ text " + nNode.dwText);

#endif // DEBUG

	}
	else //nothing found?
	{
#ifdef DEBUG
		LogError("Did not find conv start point!!! " + IntToString(GetCurrentPlayerController()->ConversationUI->conversation.ResRefID));

#endif // DEBUG

	}
}

void NPCLines(int32 lineIndex)
{
	FConvNode node = GetCurrentPlayerController()->ConversationUI->conversation.NPCLineList[lineIndex];
	if (node.Speaker == "OWNER") node.Speaker = GetCurrentPlayerController()->ConversationUI->CONVERSATION_DEFAULT_SPEAKER->GetName();

	if (node.dwText != "")
	{
		ADA2UE4Creature* oCreature = nullptr;

		for (int32 i = 0; i < node.AnimationListList.Num(); i++)
		{
			FConvAnimationListList animationListList = node.AnimationListList[i];

			if (animationListList.Tag == "player") oCreature = Cast<ADA2UE4Creature>(GetHero());
			else if (animationListList.Tag == "owner") oCreature = Cast<ADA2UE4Creature>(GetCurrentPlayerController()->ConversationUI->CONVERSATION_DEFAULT_SPEAKER);
			else if (animationListList.Tag == "henchman 1")
			{
				for (TActorIterator<ADA2UE4Creature> ActorItr(GetCurrentWorld()); ActorItr; ++ActorItr)
				{
					ADA2UE4Creature* aCreature = *ActorItr;
					if (aCreature->GetName().Contains(GetParty()->Henchman1.ToString()))
					{
						oCreature = aCreature;
						break;
					}
				}
			}
			else if (animationListList.Tag == "henchman 2")
			{
				for (TActorIterator<ADA2UE4Creature> ActorItr(GetCurrentWorld()); ActorItr; ++ActorItr)
				{
					ADA2UE4Creature* aCreature = *ActorItr;
					if (aCreature->GetName().Contains(GetParty()->Henchman2.ToString()))
					{
						oCreature = aCreature;
						break;
					}
				}
			}
			else if (animationListList.Tag == "henchman 3")
			{
				for (TActorIterator<ADA2UE4Creature> ActorItr(GetCurrentWorld()); ActorItr; ++ActorItr)
				{
					ADA2UE4Creature* aCreature = *ActorItr;
					if (aCreature->GetName().Contains(GetParty()->Henchman3.ToString()))
					{
						oCreature = aCreature;
						break;
					}
				}
			}
			else
			{
				for (TActorIterator<ADA2UE4Creature> ActorItr(GetCurrentWorld()); ActorItr; ++ActorItr)
				{
					ADA2UE4Creature* aCreature = *ActorItr;
					if (aCreature->GetName().Contains(animationListList.Tag))
					{
						oCreature = aCreature;
						break;
					}
				}
			}

			if (oCreature)
			{
				USkeletalMeshComponent* skeletalMeshComponent = oCreature->FindComponentByClass<USkeletalMeshComponent>();
				if (skeletalMeshComponent)
				{
					UDA2UE4AnimInstance* AnimInstance = Cast<UDA2UE4AnimInstance>(skeletalMeshComponent->GetAnimInstance());

					if (AnimInstance)
					{
						//AnimInstance->bIsConversation = true;

						for (int32 j = 0; j < animationListList.AnimationList.Num(); j++)
						{
							FString animName = animationListList.AnimationList[j].Animation;

							if (animName.Contains("weightshift"))//ignore idle, handled elsewhere
								continue;
							else oCreature->AnimSequence = GetConvAnimation(GetConvAnimString(animName));

							if (oCreature->AnimSequence)
							{
								int32 nType = FCString::Atoi(*GetConvAnimType(animName));

								switch (nType)
								{
								case SLOT_LOWER:
								{
									break;
								}
								case SLOT_UPPER:
								{
									//play same anim for both left and right
									AnimInstance->PlayMontage(animationListList.AnimationList[j], SLOT_UPPER_RIGHT);
									AnimInstance->PlayMontage(animationListList.AnimationList[j], SLOT_UPPER_LEFT);
									break;
								}
								case SLOT_UPPER_RIGHT:
								{
									AnimInstance->PlayMontage(animationListList.AnimationList[j], SLOT_UPPER_RIGHT);
									break;
								}
								case SLOT_UPPER_LEFT:
								{
									AnimInstance->PlayMontage(animationListList.AnimationList[j], SLOT_UPPER_LEFT);
									break;
								}
								case -1://not found
								default:
#ifdef DEBUG
									LogWarning("switch montageSlot: unknown animation for placing in montage slot");

#endif // DEBUG
									break;
								}
							}
						}
					}
				}
			}
		}


		GetCurrentPlayerController()->PlaySoundCue(node.StringID, node.dwText, node.Speaker);
	}


	if (node.Ambient)//If ambient, we skip the conversation mode
	{
		DisplayFloatyMessage(
			GetCurrentPlayerController()->ConversationUI->CONVERSATION_DEFAULT_SPEAKER, node.dwText, 0, 12345, 2);
		//Switchback to game mode explore, or whatever
#ifdef DEBUG
		LogWarning("ambient dialog by " + node.Speaker + ": " + node.dwText);

#endif // DEBUG

		GetCurrentPlayerController()->EndConversation();
	}
	else
	{
		//Check for actions that needs to be set
		int32 nPlotID;
		FPlot plot;
		FPlotElement ePlot;
		nPlotID = node.ActionPlotURI;
		if (nPlotID != 0)//If there is an actual action
		{
			//Check to see if plot already exists, if not create one
			for (FPlot p : GetParty()->Plots)
			{
				if (p.ResRefID == nPlotID)
				{
					plot = p;
					break;
				}
			}
			if (plot.ResRefID == 0) //Not found
			{
				//let's parse and create one
				plot = ParsePlot(nPlotID);
			}

			if (node.ActionPlotFlag <= 255) //Only regular flags can be set directly
			{
				HandlePlotFlag(EVENT_TYPE_SET_PLOT, plot.GUID, node.ActionPlotFlag, TRUE_);

				for (FPlotElement e : plot.StatusList)
				{
					if (e.pNode.Flag == PlotFlagConversion(plot.ResRefID, node.ActionPlotFlag))
					{
						ePlot = e;
						break;
					}
				}

				if (ePlot.pNode.Name != "")
				{
					ePlot.pValue = ((node.ActionResult == false) ? 1 : 0);

					DisplayFloatyMessage(
						GetHero(), ePlot.pNode.Name, 0, 12345, 2);
#ifdef DEBUG
					LogWarning("NPC: Set Plot Action flag to " + IntToString(ePlot.pValue) + " on flag " + ePlot.pNode.Name + " on plot " + plot.ResRefName);

#endif // DEBUG
				}
			}
			else
			{
#ifdef DEBUG
				LogError("Something tries to directly set a defined flag...");

#endif // DEBUG
			}
		}

		//Get the list of player replies
		TArray<FConvNode> pReplies;

		for (FConvTransition t : GetCurrentPlayerController()->ConversationUI->conversation.NPCLineList[lineIndex].TransitionList)
		{
			node = GetCurrentPlayerController()->ConversationUI->conversation.PlayerLineList[t.LineIndex];
			//Let's analyze the current node conditions/plot, If any
			nPlotID = node.ConditionPlotURI;
			if (nPlotID != 0)//If there is an actual condition
			{
				//Check to see if plot already exists, if not create one
				for (FPlot p : GetParty()->Plots)
				{
					if (p.ResRefID == nPlotID)
					{
						plot = p;
						break;
					}
				}
				if (plot.ResRefID == 0) //Not found
				{
					//let's parse and create one
					plot = ParsePlot(nPlotID);
				}

				//Check defined flag versus regular flag
				if (node.ConditionPlotFlag > 255) //Defined flag, That is AND/OR combination of regular flags
				{
					if (HandlePlotFlag(EVENT_TYPE_GET_PLOT, plot.GUID, node.ConditionPlotFlag) == TRUE_)
					{
						pReplies.Add(node);
					}
				}
				else //Regular flag, check directly
				{
#ifdef DEBUG
					LogWarning("Looking for condition " + IntToString(PlotFlagConversion(plot.ResRefID, node.ConditionPlotFlag)));
#endif // DEBUG

					for (FPlotElement e : plot.StatusList)
					{
						if (e.pNode.Flag == PlotFlagConversion(plot.ResRefID, node.ConditionPlotFlag))
						{
							ePlot = e;
#ifdef DEBUG
							LogWarning("Found plot " + ePlot.pNode.Name + " for condition " + IntToString(PlotFlagConversion(plot.ResRefID, node.ConditionPlotFlag)));
#endif // DEBUG
							break;
						}
					}

					if (ePlot.pNode.Name != "" && ePlot.pValue == UKismetMathLibrary::Conv_BoolToInt(node.ConditionResult))
					{
						pReplies.Add(node);
					}
				}
			}
			else //No condition, just add It
			{
				pReplies.Add(GetCurrentPlayerController()->ConversationUI->conversation.PlayerLineList[t.LineIndex]);
			}
		}

		//If an actual multinode conversation
		if (pReplies.Num() > 0)
		{
			//activate text lines
			int32 counter = 0;
			for (FConvNode pNode : pReplies)
			{
				if (pNode.dwText != "") //If null, It's either or CONTINUE or End Dialog
				{
					if (pNode.dwWheel == 0)
					{
						GetCurrentPlayerController()->ConversationUI->ButtonPlayer0->SetVisibility(ESlateVisibility::Visible);
						GetCurrentPlayerController()->ConversationUI->TextPlayer0->SetText(FText::FromString(pNode.dwText));
					}
					else if (pNode.dwWheel == 1)
					{
						GetCurrentPlayerController()->ConversationUI->ButtonPlayer1->SetVisibility(ESlateVisibility::Visible);
						GetCurrentPlayerController()->ConversationUI->TextPlayer1->SetText(FText::FromString(pNode.dwText));
					}
					else if (pNode.dwWheel == 2)
					{
						GetCurrentPlayerController()->ConversationUI->ButtonPlayer2->SetVisibility(ESlateVisibility::Visible);
						GetCurrentPlayerController()->ConversationUI->TextPlayer2->SetText(FText::FromString(pNode.dwText));
					}
					else if (pNode.dwWheel == 3)
					{
						GetCurrentPlayerController()->ConversationUI->ButtonPlayer3->SetVisibility(ESlateVisibility::Visible);
						GetCurrentPlayerController()->ConversationUI->TextPlayer3->SetText(FText::FromString(pNode.dwText));
					}
					else if (pNode.dwWheel == 4)
					{
						GetCurrentPlayerController()->ConversationUI->ButtonPlayer4->SetVisibility(ESlateVisibility::Visible);
						GetCurrentPlayerController()->ConversationUI->TextPlayer4->SetText(FText::FromString(pNode.dwText));
					}
					else if (pNode.dwWheel == 5)
					{
						GetCurrentPlayerController()->ConversationUI->ButtonPlayer5->SetVisibility(ESlateVisibility::Visible);
						GetCurrentPlayerController()->ConversationUI->TextPlayer5->SetText(FText::FromString(pNode.dwText));
					}
					else
					{
#ifdef DEBUG
						LogError("Unknown dialog wheel position!!");

#endif // DEBUG
					}
					FConvLine convLine;
					convLine.lineLocation = pNode.dwWheel;
					convLine.iconID = pNode.dwMood;
					convLine.lineIndex = pNode.lineIndex;
					GetCurrentPlayerController()->ConversationUI->wheelInstance.Add(pNode.dwWheel, convLine);

#ifdef DEBUG
					FConvNode nNode = GetCurrentPlayerController()->ConversationUI->conversation.NPCLineList[pNode.lineIndex];
					LogWarning("NPC sets Player branch to lineIndex " + IntToString(pNode.lineIndex));
					LogWarning("Dialog moving to Player line ID " + IntToString(nNode.StringID) + " w/ text " + pNode.dwText);
#endif // DEBUG
				}
				else
				{
					//Check to see if it's CONTINUE
					if (GetCurrentPlayerController()->ConversationUI->conversation.PlayerLineList[pNode.lineIndex].TransitionList.Num() != 0)
					{
						//It's a continue connector
#ifdef DEBUG
						LogWarning("CONTINUE " + IntToString(pNode.lineIndex));

#endif // DEBUG
						FConvLine playerReply;
						playerReply.lineIndex = pNode.lineIndex;
						playerReply.owner = "player";
						GetCurrentPlayerController()->ConversationUI->ConvNextLineReply = playerReply;

						GetCurrentPlayerController()->PlayNextLine();
					}
					else //it's end dialogue
					{
#ifdef DEBUG
						LogWarning("END");

#endif // DEBUG
						nPlotID = node.ActionPlotURI;
						if (nPlotID != 0)//If there is an actual condition
						{
							//Check to see if plot already exists, if not create one
							for (FPlot p : GetParty()->Plots)
							{
								if (p.ResRefID == nPlotID)
								{
									plot = p;
									break;
								}
							}
							if (plot.ResRefID == 0) //Not found
							{
								//let's parse and create one
								plot = ParsePlot(nPlotID);
							}

							if (node.ActionPlotFlag <= 255) //Only regular flags can be set directly
							{
								HandlePlotFlag(EVENT_TYPE_SET_PLOT, plot.GUID, node.ActionPlotFlag, TRUE_);

								for (FPlotElement e : plot.StatusList)
								{
									if (e.pNode.Flag == PlotFlagConversion(plot.ResRefID, node.ActionPlotFlag))
									{
										ePlot = e;
										break;
									}
								}

								if (ePlot.pNode.Name != "")
								{
									ePlot.pValue = ((node.ActionResult == false) ? 1 : 0);

									DisplayFloatyMessage(
										GetHero(), ePlot.pNode.Name, 0, 12345, 2);
#ifdef DEBUG
									LogWarning("END: Set Plot Action flag to " + IntToString(ePlot.pValue) + " on flag " + ePlot.pNode.Name + " on plot " + plot.ResRefName);

#endif // DEBUG
								}
							}
							else
							{
#ifdef DEBUG
								LogError("Something tries to directly set a defined flag...");

#endif // DEBUG
							}
						}

						GetCurrentPlayerController()->ConversationUI->endConversation = true;
					}
				}

				counter++;
			}
		}
		else //it's a one-liner
		{
			GetCurrentPlayerController()->ConversationUI->endConversation = true;
		}
	}
}

void PlayerLines(int32 lineIndex)
{
	int32 bStart = FALSE_;
	int32 lIndex = 0;//Line index current
	int32 nPlotID;
	FPlot plot;
	FPlotElement ePlot;

	FConvNode node = GetCurrentPlayerController()->ConversationUI->conversation.PlayerLineList[lineIndex];

	if (node.dwText != "")
	{
		GetCurrentPlayerController()->PlaySoundCue(node.StringID, node.spokenText, node.Speaker);
	}

	//Check for actions that needs to be set
	nPlotID = node.ActionPlotURI;
	if (nPlotID != 0)//If there is an actual action
	{
		//Check to see if plot already exists, if not create one
		for (FPlot p : GetParty()->Plots)
		{
			if (p.ResRefID == nPlotID)
			{
				plot = p;
				break;
			}
		}
		if (plot.ResRefID == 0) //Not found
		{
			//let's parse and create one
			plot = ParsePlot(nPlotID);
		}

		if (node.ActionPlotFlag <= 255) //Only regular flags can be set directly
		{
			HandlePlotFlag(EVENT_TYPE_SET_PLOT, plot.GUID, node.ActionPlotFlag, TRUE_);

			for (FPlotElement e : plot.StatusList)
			{
				if (e.pNode.Flag == PlotFlagConversion(plot.ResRefID, node.ActionPlotFlag))
				{
					ePlot = e;
					break;
				}
			}

			if (ePlot.pNode.Name != "")
			{
				ePlot.pValue = ((node.ActionResult == false) ? 1 : 0);

				DisplayFloatyMessage(
					GetHero(), ePlot.pNode.Name, 0, 12345, 2);
#ifdef DEBUG
				LogWarning("PLAYER: Set Plot Action flag to " + IntToString(ePlot.pValue) + " on flag " + ePlot.pNode.Name + " on plot " + plot.ResRefName);

#endif // DEBUG
			}
		}
		else
		{
#ifdef DEBUG
			LogError("Something tries to directly set a defined flag...");

#endif // DEBUG
		}
	}

	//Get the list of NPC replies
	TArray<FConvNode> npcReplies;
	TArray<int32> npcRepliesIndex;
	for (FConvTransition t : GetCurrentPlayerController()->ConversationUI->conversation.PlayerLineList[lineIndex].TransitionList)
	{
		npcReplies.Add(GetCurrentPlayerController()->ConversationUI->conversation.NPCLineList[t.LineIndex]);
		npcRepliesIndex.Add(t.LineIndex);
	}

	//If more than one line check to see which condition matches first
	for (int32 n : npcRepliesIndex)
	{
		node = GetCurrentPlayerController()->ConversationUI->conversation.NPCLineList[n];
		//Let's analyze the current node conditions/plot, If any
		nPlotID = node.ConditionPlotURI;
		if (nPlotID == 0) //no condition, just play it
		{
			bStart = TRUE_;
			lIndex = n;//set the found starting branch
			break;
		}
		else //if (nPlotID != 0)//If there is an actual condition
		{
			//Check to see if plot already exists, if not create one
			for (FPlot p : GetParty()->Plots)
			{
				if (p.ResRefID == nPlotID)
				{
					plot = p;
					break;
				}
			}
			if (plot.ResRefID == 0) //Not found
			{
				//let's parse and create one
				plot = ParsePlot(nPlotID);
			}

			//Check defined flag versus regular flag
			if (node.ConditionPlotFlag > 255) //Defined flag, That is AND/OR combination of regular flags
			{
				if (HandlePlotFlag(EVENT_TYPE_GET_PLOT, plot.GUID, node.ConditionPlotFlag) == TRUE_)
				{
					bStart = TRUE_;
					lIndex = n;//set the found starting branch
					break;
				}
			}
			else //Regular flag, check directly
			{
#ifdef DEBUG
				LogWarning("Looking for condition " + IntToString(PlotFlagConversion(plot.ResRefID, node.ConditionPlotFlag)));
#endif // DEBUG

				for (FPlotElement e : plot.StatusList)
				{
					if (e.pNode.Flag == PlotFlagConversion(plot.ResRefID, node.ConditionPlotFlag))
					{
						ePlot = e;
#ifdef DEBUG
						LogWarning("Found plot " + ePlot.pNode.Name + " for condition " + IntToString(PlotFlagConversion(plot.ResRefID, node.ConditionPlotFlag)));
#endif // DEBUG
						break;
					}
				}

				if (ePlot.pNode.Name != "" && ePlot.pValue == UKismetMathLibrary::Conv_BoolToInt(node.ConditionResult))
				{
					bStart = TRUE_;
					lIndex = n;//set the found starting branch
					break;
				}
			}
		}
	}

	if (bStart == TRUE_) //We actually found the Next NPC conversation node
	{
		FConvNode nNode = GetCurrentPlayerController()->ConversationUI->conversation.NPCLineList[lIndex];
		FConvLine npcReply;
		npcReply.lineIndex = nNode.lineIndex;
		npcReply.owner = "npc";
		GetCurrentPlayerController()->ConversationUI->ConvNextLineReply = npcReply;

#ifdef DEBUG
		LogWarning("Player choice sets NPC branch to lineIndex " + IntToString(lIndex));
		LogWarning("Dialog moving to NPC line ID " + IntToString(nNode.StringID) + " w/ text " + nNode.dwText);

#endif // DEBUG

	}
	else //nothing found?
	{
		//If the transition list has one and only one entry, set it
		if (npcRepliesIndex.Num() == 1)
		{
#ifdef DEBUG
			LogError("player transition list has one and only one entry");

#endif // DEBUG
		}
		else if (GetCurrentPlayerController()->ConversationUI->endConversation)
		{
#ifdef DEBUG
			LogError("Player branch should end here!!");

#endif // DEBUG
		}
		else
		{
#ifdef DEBUG
			LogError("PlayerLine should not reach this !!!");
#endif // DEBUG

		}
	}
}

void NextLine(int32 lineIndex)
{
	ResetLayout();

	PlayerLines(lineIndex);
#ifdef DEBUG
	LogWarning("NextLine " + IntToString(lineIndex));

#endif // DEBUG
}

void ResetLayout()
{
	GetCurrentPlayerController()->ConversationUI->BorderMood->SetVisibility(ESlateVisibility::Hidden);
	for (UButton* button : GetCurrentPlayerController()->ConversationUI->pLines)
	{
		button->SetVisibility(ESlateVisibility::Hidden);
	}
}