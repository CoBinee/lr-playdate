// Logo.h - ロゴ
//
#pragma once

// 外部参照
//
#include <stdbool.h>
#include "pd_api.h"
#include "Actor.h"
#include "Aseprite.h"


// ロゴ
//
struct Logo {

    // アクタ
    struct Actor actor;

    // スプライトアニメーション
    struct AsepriteSpriteAnimation animation;

};

// 外部参照関数
//
extern void LogoInitialize(void);
extern void LogoLoad(void);
