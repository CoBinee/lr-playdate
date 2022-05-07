// Enemy.h - エネミー
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
    kEnemyCountOfficer = 4, 
    kEnemyCountInspector = 2, 
    kEnemyCountDog = 1, 
};

// ミリ秒
//
enum {
    kEnemyMillisecondOfficerStart = 10 * 1000, 
    kEnemyMillisecondOfficerInterval = 40 * 1000, 
    kEnemyMillisecondInspectorStart = 70 * 1000, 
    kEnemyMillisecondInspectorInterval = 40 * 1000, 
    kEnemyMillisecondDogStart = 60 * 1000, 
    kEnemyMillisecondDogInterval = 40 * 1000, 
};

// エネミーコントローラ
//
struct EnemyController {

    // アクタ
    struct Actor actor;

    // 警官
    int officerCount;
    int officerMillisecond;

    // 警部
    int inspectorCount;
    int inspectorMillisecond;

    // 犬
    int dogCount;
    int dogMillisecond;

};

// エネミー
//
struct Enemy {

    // アクタ
    struct Actor actor;

    // 位置
    struct GamePosition position;

    // 目標
    struct GamePosition target;

    // 向き
    float direction;

    // 速度
    float speed;
    float speedMaximum;
    float speedWalk;
    float speedRun;
    float accel;

    // ジャンプ
    float jump;
    float jumpMaximum;
    float gravity;

    // 目視
    float look;

    // カウント
    int count;

    // ミリ秒
    int millisecond;

    // スプライトアニメーション
    struct AsepriteSpriteAnimation animation;

};


// 外部参照関数
//
extern void EnemyInitialize(void);
extern void EnemyControllerLoad(void);
extern void EnemyControllerDecOfficerCount(void);
extern void EnemyControllerDecInspectorCount(void);
extern void EnemyControllerDecDogCount(void);
extern void EnemyMove(struct Enemy *enemy);
extern bool EnemyStop(struct Enemy *enemy);
extern bool EnemyIsLookPlayer(struct Enemy *enemy);
extern void EnemyOfficerLoad(void);
extern void EnemyInspectorLoad(void);
extern void EnemyDogLoad(void);
