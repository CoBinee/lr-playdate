// Title.h - タイトル
//
#pragma once

// 外部参照
//
#include <stdbool.h>
#include "pd_api.h"


// タイトル関数
//
typedef void (*TitleFunction)(void *game);

// タイトル
//
struct Title {

    // 処理関数
    TitleFunction function;

    // 状態
    int state;

    // ミリ秒
    int millisecond;

};

// スプライト
//
typedef enum {
    kTitleSpriteNameNull = 0, 
    kTitleSpriteNameLogo, 
    kTitleSpriteNameCar,
    kTitleSpriteNameSize, 
} TitleSpriteName;

// プライオリティ
//
enum {
    kTitlePriorityStarter, 
    kTitlePriorityDemo, 
};

// タグ
//
enum {
    kTitleTagNull = 0, 
    kTitleTagStarter, 
    kTitleTagDemo, 
};

// 描画順
//
enum {
    kTitleOrderLogo = 0, 
    kTitleOrderCar, 
    kTitleOrderStarter, 
};


// 外部参照関数
//
extern void TitleUpdate(struct Title *game);
extern const char *TitleGetSpriteName(TitleSpriteName name);
