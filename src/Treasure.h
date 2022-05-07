// Treasure.h - 宝
//
#pragma once

// 外部参照
//
#include <stdbool.h>
#include "pd_api.h"
#include "Actor.h"
#include "Aseprite.h"
#include "Game.h"


// カウント
//
enum {
    kTreasureCountMaximum = 5, 
};

// ミリ秒
//
enum {
    kTreasureMillisecondImmidiate = 1500, 
    kTreasureMillisecondInterval = 3000, 
};

// 宝コントローラ
//
struct TreasureController {

    // アクタ
    struct Actor actor;

    // カウント
    int count;
    
    // ミリ秒
    int millisecond;

};

// 種類
//
typedef enum {
    kTreasureTypeGold = 0, 
    kTreasureTypeJewel, 
} TreasureType;

// 宝
//
struct Treasure {

    // アクタ
    struct Actor actor;

    // 種類
    TreasureType type;

    // 位置
    struct GamePosition position;

    // スプライト
    GameSpriteName sprite;

    // スプライトアニメーション
    struct AsepriteSpriteAnimation animation;

};


// 外部参照関数
//
extern void TreasureInitialize(void);
extern void TreasureControllerLoad(void);
