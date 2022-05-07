// Player.h - プレイヤ
//
#pragma once

// 外部参照
//
#include <stdbool.h>
#include "pd_api.h"
#include "Actor.h"
#include "Aseprite.h"
#include "Game.h"


// スプライト
//
enum {
    kPlayerSpriteThief = 0, 
    kPlayerSpriteTarget, 
    kPlayerSpriteSize, 
};

// プレイヤ
//
struct Player {

    // アクタ
    struct Actor actor;

    // 位置
    struct GamePosition position;

    // 目標
    struct GamePosition target;

    // 向き
    float direction;

    // 回転
    float rotation;

    // 速度
    float speed;
    float speedMaximum;
    float accel;

    // ジャンプ
    float jump;
    float jumpMaximum;
    float gravity;

    // 一周
    float roundDistance;
    int roundCount;

    // プレイ中
    bool play;

    // スプライトアニメーション
    struct AsepriteSpriteAnimation animations[kPlayerSpriteSize];

};

// 外部参照関数
//
extern void PlayerInitialize(void);
extern void PlayerLoad(void);
extern bool PlayerIsLive(void);
extern float PlayerGetPositionA(void);
extern float PlayerGetDistanceTo(float a);
extern bool PlayerIsHit(struct GamePosition *position);
extern bool PlayerIsHitThenMiss(struct GamePosition *position);
extern void PlayerClearRoundCount(void);
extern int PlayerGetRoundCount(void);
