#pragma once

class Engine;

void ParseConversation(int32 nConvID);

//TODO rename convention w/ Conv first
void StartConversation();

void NPCLines(int32 lineIndex);

void PlayerLines(int32 lineIndex);

void NextLine(int32 lineIndex);

void ResetLayout();
