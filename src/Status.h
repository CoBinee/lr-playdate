// Status.h - ステータス
//
#pragma once

// 外部参照
//
#include <stdbool.h>
#include "pd_api.h"
#include "Actor.h"


// ステータス
//
struct Status {

    // アクタ
    struct Actor actor;

};

// 外部参照関数
//
extern void StatusInitialize(void);
extern void StatusLoad(void);
