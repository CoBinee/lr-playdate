// Result.c - 結果
//

// 外部参照
//
#include <stdbool.h>
#include "pd_api.h"
#include "Iocs.h"
#include "Actor.h"
#include "Game.h"
#include "Result.h"

// 内部関数
//
static void ResultDraw(struct Result *result);
static void ResultDrawText(struct Result *result);
static void ResultDrawSpot(struct Result *result);
static void ResultOverUnload(struct Result *result);
static void ResultOverDemo(struct Result *result);
static void ResultCompleteUnload(struct Result *result);
static void ResultCompleteDraw(struct Result *result);
static void ResultCompleteDemo(struct Result *result);

// 内部変数
//


// 結果を初期化する
//
void ResultInitialize(void)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // アクタの確認
    if (sizeof (struct Result) > kActorBlockSize) {
        playdate->system->error("%s: %d: result actor size is over: %d bytes.", __FILE__, __LINE__, sizeof (struct Result));
    }
}

// 結果を描画する
//
static void ResultDraw(struct Result *result)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // スポットの描画
    ResultDrawSpot(result);

    // テキストの描画
    if (result->spotMaskMillisecond > 0) {
        ResultDrawText(result);

    // スプライトの描画
    } else {
        AsepriteDrawRotatedSpriteAnimation(&result->animation, result->spriteX, result->spriteY, 0.0f, 0.5f, 1.0f, 2.0f, 2.0f, kDrawModeFillBlack);
    }
}

// テキストを描画する
//
static void ResultDrawText(struct Result *result)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // メッセージの描画
    int width = IocsGetTextWidth(kIocsFontGame, result->text);
    int height = IocsGetFontHeight(kIocsFontGame);
    int x = (LCD_COLUMNS - width) / 2;
    int y = (LCD_ROWS - height) / 2;
    playdate->graphics->fillRect(0, y - 4, LCD_COLUMNS, height + 8, kColorBlack);
    playdate->graphics->setDrawMode(kDrawModeFillWhite);
    playdate->graphics->drawText(result->text, strlen(result->text), kUTF8Encoding, x, y);
}

// スポットを描画する
//
static void ResultDrawSpot(struct Result *result)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // スポットの描画
    playdate->graphics->pushContext(result->bitmaps[kResultBitmapSpot]);
    playdate->graphics->setDrawMode(kDrawModeCopy);
    playdate->graphics->fillRect(0, 0, LCD_ROWS, LCD_ROWS, kColorBlack);
    if (result->spotLightMillisecond > 0) {
        int r = 128  * result->spotLightMillisecond / result->spotLightLength;
        playdate->graphics->fillEllipse((LCD_ROWS - r) / 2, (LCD_ROWS - r) / 2, r, r, 0.0f, 360.0f, kColorWhite);
    }
    if (result->spotMaskMillisecond > 0) {
        int r = LCD_ROWS  * result->spotMaskMillisecond / result->spotMaskLength;
        playdate->graphics->fillEllipse((LCD_ROWS - r) / 2, (LCD_ROWS - r) / 2, r, r, 0.0f, 360.0f, kColorClear);
    }
    playdate->graphics->popContext();
    playdate->graphics->setDrawMode(kDrawModeCopy);
    playdate->graphics->drawBitmap(result->bitmaps[kResultBitmapSpot], (LCD_COLUMNS - LCD_ROWS) / 2, 0, kBitmapUnflipped);
}

// ゲームオーバーを読み込む
//
void ResultOverLoad(void)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // アクタの登録
    struct Result *result = (struct Result *)ActorLoad((ActorFunction)ResultOverDemo, kGamePriorityDemo);
    if (result == NULL) {
        playdate->system->error("%s: %d: result actor is not loaded.", __FILE__, __LINE__);
    }

    // 結果の初期化
    {
        // 解放処理の設定
        ActorSetUnload(&result->actor, (ActorFunction)ResultOverUnload);

        // タグの設定
        ActorSetTag(&result->actor, kGameTagDemo);

        // ビットマップの作成
        result->bitmaps[kResultBitmapSpot] = playdate->graphics->newBitmap(LCD_ROWS, LCD_ROWS, kColorClear);

        // テキストの設定
        result->text = "GAME OVER";

        // スポットの設定
        result->spotLightMillisecond = kResultSpotLightMillisecond;
        result->spotLightLength = result->spotLightMillisecond;
        result->spotMaskMillisecond = kResultSpotMaskMillisecond;
        result->spotMaskLength = result->spotMaskMillisecond;

        // スプライトの設定
        result->spriteMillisecond = 0;
        result->spriteX = 296;
        result->spriteY = 136;

        // 完了の設定
        result->done = false;
    }
}

// ゲームオーバーを解放する
//
static void ResultOverUnload(struct Result *result)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // ビットマップの解放
    for (int i = 0; i < kResultBitmapSize; i++) {
        if (result->bitmaps[i] != NULL) {
            playdate->graphics->freeBitmap(result->bitmaps[i]);
        }
    }
}

// ゲームオーバーをデモする
//
static void ResultOverDemo(struct Result *result)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // 初期化
    if (result->actor.state == 0) {

        // アニメーションの開始
        AsepriteStartSpriteAnimation(&result->animation, GameGetSpriteName(kGameSpriteNameResult), "Null", true);

        // 初期化の完了
        ++result->actor.state;
    }

    // 1: マスクの更新
    if (result->actor.state == 1) {
        result->spotMaskMillisecond -= IocsGetFrameMillisecond();
        if (result->spotMaskMillisecond <= 0) {
            result->spotMaskMillisecond = 0;

            // アニメーションの開始
            AsepriteStartSpriteAnimation(&result->animation, GameGetSpriteName(kGameSpriteNameResult), "Walk", true);
            ++result->actor.state;
        }
    }

    // 2: スプライトの更新
    if (result->actor.state == 2) {
        result->spriteMillisecond += IocsGetFrameMillisecond();
        if (result->spriteMillisecond >= 200) {
            result->spriteMillisecond -= 200;
            result->spriteX -= 4;
            if (result->spriteX <= 104) {
                ++result->actor.state;
            }
        }
    }

    // 3: ライトの更新
    if (result->actor.state == 3) {
        result->spotLightMillisecond -= IocsGetFrameMillisecond();
        if (result->spotLightMillisecond <= 0) {
            result->spotLightMillisecond = 0;
            result->done = true;
        }
    }

    // スプライトの更新
    AsepriteUpdateSpriteAnimation(&result->animation);

    // 描画処理の設定
    ActorSetDraw(&result->actor, (ActorFunction)ResultDraw, kGameOrderDemo);
}

// ゲームクリアを読み込む
//
void ResultCompleteLoad(bool highscore)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // アクタの登録
    struct Result *result = (struct Result *)ActorLoad((ActorFunction)ResultCompleteDemo, kGamePriorityDemo);
    if (result == NULL) {
        playdate->system->error("%s: %d: result actor is not loaded.", __FILE__, __LINE__);
    }

    // 結果の初期化
    {
        // 解放処理の設定
        ActorSetUnload(&result->actor, (ActorFunction)ResultCompleteUnload);

        // タグの設定
        ActorSetTag(&result->actor, kGameTagDemo);

        // ビットマップの作成
        result->bitmaps[kResultBitmapSpot] = playdate->graphics->newBitmap(LCD_ROWS, LCD_ROWS, kColorClear);

        // テキストの設定
        result->text = "GAME CLEAR";

        // スポットの設定
        result->spotLightMillisecond = kResultSpotLightMillisecond;
        result->spotLightLength = result->spotLightMillisecond;
        result->spotMaskMillisecond = kResultSpotMaskMillisecond;
        result->spotMaskLength = result->spotMaskMillisecond;

        // スプライトの設定
        result->spriteMillisecond = 0;
        result->spriteX = 200;
        result->spriteY = 136;

        // ハイスコアの設定
        result->highscore = highscore;

        // 完了の設定
        result->done = false;
    }
}

// ゲームクリアを解放する
//
static void ResultCompleteUnload(struct Result *result)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // ビットマップの解放
    for (int i = 0; i < kResultBitmapSize; i++) {
        if (result->bitmaps[i] != NULL) {
            playdate->graphics->freeBitmap(result->bitmaps[i]);
        }
    }
}

// ゲームクリアをデモする
//
static void ResultCompleteDemo(struct Result *result)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // 初期化
    if (result->actor.state == 0) {

        // アニメーションの開始
        AsepriteStartSpriteAnimation(&result->animation, GameGetSpriteName(kGameSpriteNameResult), "Null", false);

        // 初期化の完了
        ++result->actor.state;
    }

    // 1: マスクの更新
    if (result->actor.state == 1) {
        result->spotMaskMillisecond -= IocsGetFrameMillisecond();
        if (result->spotMaskMillisecond <= 0) {
            result->spotMaskMillisecond = 0;

            // アニメーションの開始
            AsepriteStartSpriteAnimation(&result->animation, GameGetSpriteName(kGameSpriteNameResult), "Approach", false);
            ++result->actor.state;
        }
    }

    // 2: スプライトの更新
    if (result->actor.state == 2) {
        if (AsepriteIsSpriteAnimationDone(&result->animation)) {
            AsepriteStartSpriteAnimation(&result->animation, GameGetSpriteName(kGameSpriteNameResult), result->highscore ? "Love" : "Kick", false);
            ++result->actor.state;
        }
    }

    // 3: スプライトの更新
    if (result->actor.state == 3) {
        if (AsepriteIsSpriteAnimationDone(&result->animation)) {
            ++result->actor.state;
        }
    }

    // 4: ライトの更新
    if (result->actor.state == 4) {
        result->spotLightMillisecond -= IocsGetFrameMillisecond();
        if (result->spotLightMillisecond <= 0) {
            result->spotLightMillisecond = 0;
            result->done = true;
        }
    }

    // スプライトの更新
    AsepriteUpdateSpriteAnimation(&result->animation);

    // 描画処理の設定
    ActorSetDraw(&result->actor, (ActorFunction)ResultDraw, kGameOrderDemo);
}

// 結果が完了したかどうかを判定する
//
bool ResultIsDone(void)
{
    struct Result *result = (struct Result *)ActorFindWithTag(kGameTagDemo);
    return result != NULL ? result->done : false;
}

