// Background.h - 背景
//
#pragma once

// 外部参照
//
#include <stdbool.h>
#include "pd_api.h"
#include "Actor.h"
#include "Aseprite.h"


// 背景
//
struct Background {

    // アクタ
    struct Actor actor;

    // スプライトアニメーション
    struct AsepriteSpriteAnimation animation;

};

// 外部参照関数
//
extern void BackgroundInitialize(void);
extern void BackgroundLoad(void);
