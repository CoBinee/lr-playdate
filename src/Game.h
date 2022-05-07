// Game.h - ゲーム
//
#pragma once

// 外部参照
//
#include <stdbool.h>
#include "pd_api.h"


// ゲーム関数
//
typedef void (*GameFunction)(void *game);

// スコア
//
enum {
    kGameScoreGold = 100, 
    kGameScoreJewel = 1000, 
    kGameScoreMaximum = 1000000, 
};

// タイマ
//
enum {
    kGameTimerMaximum = 3 * 60000, 
    kGameTimerLimit = 3000, 
    kGameTimerComplete = -3000, 
};

// ゲーム
//
struct Game {

    // 処理関数
    GameFunction function;

    // 状態
    int state;

    // スコア
    int score;

    // タイマ
    int timer;

    // プレイ中
    bool play;

    // ミリ秒
    int millisecond;

};

// スプライト
//
typedef enum {
    kGameSpriteNameNull = 0, 
    kGameSpriteNameBack, 
    kGameSpriteNameThief,
    kGameSpriteNameOfficer, 
    kGameSpriteNameInspector, 
    kGameSpriteNameDog,  
    kGameSpriteNameGold, 
    kGameSpriteNameJewel, 
    kGameSpriteNameHelicopter, 
    kGameSpriteNameGetoff, 
    kGameSpriteNameResult, 
    kGameSpriteNameSize, 
} GameSpriteName;

// オーディオ
//
enum {
    kGameAudioSampleHelicopterHigh = 0, 
    kGameAudioSampleHelicopterLow, 
    kGameAudioSampleJump, 
    kGameAudioSampleCoin, 
    kGameAudioSampleSize, 
};

// 位置
//
struct GamePosition {
    float a;
    float r;
    float w;
    float h;
    int x;
    int y;
};

// プライオリティ
//
enum {
    kGamePriorityPlayer, 
    kGamePriorityEnemyController, 
    kGamePriorityEnemy, 
    kGamePriorityTreasureController, 
    kGamePriorityTreasure, 
    kGamePriorityBackgroud, 
    kGamePriorityDemo, 
    kGamePriorityStatus, 
};

// タグ
//
enum {
    kGameTagNull = 0, 
    kGameTagPlayer, 
    kGameTagEnemyController, 
    kGameTagEnemy, 
    kGameTagTreasureController, 
    kGameTagTreasure, 
    kGameTagDemo, 
};

// 描画順
//
enum {
    kGameOrderBackground = 0, 
    kGameOrderTreasure, 
    kGameOrderEnemy, 
    kGameOrderPlayer, 
    kGameOrderDemo, 
    kGameOrderStatus, 
};


// 外部参照関数
//
extern void GameUpdate(struct Game *game);
extern const char *GameGetSpriteName(GameSpriteName name);
extern float GameGetR(void);
extern void GameCalcPosition(struct GamePosition *position);
extern float GameAdjustPositionA(float a);
extern float GameGetDistance(float origin, float target);
extern int GameGetScore(void);
extern void GameAddScore(int score);
extern int GameGetTimer(void);
extern bool GameIsPlay(void);
extern bool GameIsOver(void);
extern bool GameIsTimeout(void);
extern bool GameIsComplete(void);
