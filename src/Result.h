// Result.h - 結果
//
#pragma once

// 外部参照
//
#include <stdbool.h>
#include "pd_api.h"
#include "Actor.h"
#include "Aseprite.h"


// ビットマップ
//
enum {
    kResultBitmapSpot = 0, 
    kResultBitmapSize, 
};

// スポット
//
enum {
    kResultSpotLightMillisecond = 800, 
    kResultSpotMaskMillisecond = 1500, 
};

// 結果
//
struct Result {

    // アクタ
    struct Actor actor;

    // ビットマップ
    LCDBitmap *bitmaps[kResultBitmapSize];

    // テキスト
    const char *text;

    // スポット
    int spotLightMillisecond;
    int spotLightLength;
    int spotMaskMillisecond;
    int spotMaskLength;

    // スプライトアニメーション
    struct AsepriteSpriteAnimation animation;

    // スプライト
    int spriteMillisecond;
    int spriteX;
    int spriteY;

    // ハイスコア
    bool highscore;

    // 完了
    bool done;
    
};

// 外部参照関数
//
extern void ResultInitialize(void);
extern void ResultOverLoad(void);
extern void ResultCompleteLoad(bool highscore);
extern bool ResultIsDone(void);
