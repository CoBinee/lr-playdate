// Car.h - 車
//
#pragma once

// 外部参照
//
#include <stdbool.h>
#include "pd_api.h"
#include "Actor.h"
#include "Aseprite.h"


// ミリ秒
//
enum {
    kCarMillisecondCycle = 16 * 1000, 
};

// 車
//
struct Car {

    // アクタ
    struct Actor actor;

    // スプライトアニメーション
    struct AsepriteSpriteAnimation animation;

    // 位置
    int x;
    int y;

    // ミリ秒
    int millisecond;

};

// 外部参照関数
//
extern void CarInitialize(void);
extern void CarLoad(void);
