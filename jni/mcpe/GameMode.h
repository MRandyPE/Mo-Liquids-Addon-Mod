#pragma once

class Player;

class GameMode {
public:
	virtual bool isSurvivalType();
	
	static void initPlayer(Player*);
};