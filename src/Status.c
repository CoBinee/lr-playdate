// Status.c - ステータス
//

// 外部参照
//
#include <string.h>
#include "pd_api.h"
#include "Iocs.h"
#include "Actor.h"
#include "Game.h"
#include "Status.h"

// 内部関数
//
static void StatusUnload(struct Status *status);
static void StatusDraw(struct Status *status);
static void StatusLoop(struct Status *status);

// 内部変数
//


// ステータスを初期化する
//
void StatusInitialize(void)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // アクタの確認
    if (sizeof (struct Status) > kActorBlockSize) {
        playdate->system->error("%s: %d: status actor size is over: %d bytes.", __FILE__, __LINE__, sizeof (struct Status));
    }
}

// ステータスを読み込む
//
void StatusLoad(void)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // アクタの登録
    struct Status *status = (struct Status *)ActorLoad((ActorFunction)StatusLoop, kGamePriorityStatus);
    if (status == NULL) {
        playdate->system->error("%s: %d: status actor is not loaded.", __FILE__, __LINE__);
    }

    // ステータスの初期化
    {
        // 解放処理の設定
        ActorSetUnload(&status->actor, (ActorFunction)StatusUnload);
    }
}

// ステータスを解放する
//
static void StatusUnload(struct Status *status)
{
    ;
}

// ステータスを描画する
//
static void StatusDraw(struct Status *status)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // フォントの設定
    IocsSetFont(kIocsFontGame);

    // スコアの描画
    {
        char *text;
        playdate->system->formatString(&text, "$%d", GameGetScore());
        playdate->graphics->setDrawMode(kDrawModeFillWhite);
        playdate->graphics->drawText(text, strlen(text), kUTF8Encoding, 4, 4);
        playdate->system->realloc(text, 0);
    }

    // タイマの描画
    {
        int timer = GameGetTimer();
        int millisecond = (timer % 1000) / 100;
        timer /= 1000;
        int second = timer % 60;
        int minute = timer / 60;
        char *text;
        playdate->system->formatString(&text, "%1d'%02d\"%1d", minute, second, millisecond);
        int width = IocsGetTextWidth(kIocsFontGame, text);
        playdate->graphics->setDrawMode(kDrawModeFillWhite);
        playdate->graphics->drawText(text, strlen(text), kUTF8Encoding, LCD_COLUMNS - width - 4, 4);
        playdate->system->realloc(text, 0);
    }
}

// ステータスが待機する
//
static void StatusLoop(struct Status *status)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // 初期化
    if (status->actor.state == 0) {

        // 初期化の完了
        ++status->actor.state;
    }

    // 描画処理の設定
    ActorSetDraw(&status->actor, (ActorFunction)StatusDraw, kGameOrderStatus);
}


