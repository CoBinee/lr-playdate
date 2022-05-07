// Intro.h - 導入
//
#pragma once

// 外部参照
//
#include <stdbool.h>
#include "pd_api.h"
#include "Actor.h"
#include "Aseprite.h"


// スプライトアニメーション
//
enum {
    kIntroSpriteAnimationHelicopter = 0, 
    kIntroSpriteAnimationGetoff, 
    kIntroSpriteAnimationMask, 
    kIntroSpriteAnimationSize, 
};

// 導入
//
struct Intro {

    // アクタ
    struct Actor actor;

    // スプライトアニメーション
    struct AsepriteSpriteAnimation animations[kIntroSpriteAnimationSize];

    // ヘリコプター
    int helicopterX;
    int helicopterY;

    // 降り立つ
    int getoffX;
    int getoffY;

    // オーディオ
    int audio;

    // 完了
    bool done;
    
};

// 外部参照関数
//
extern void IntroInitialize(void);
extern void IntroLoad(void);
extern bool IntroIsDone(void);
