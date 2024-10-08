// ZenPixel.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Title  : ZEN PIXEL
// Author : UFTHaq
// Date   : 09/07/2024
//

#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>

#include <raylib.h>
#include <rlgl.h>
#include <raymath.h>
#include <SFML/Graphics.hpp>
#include "ZenPixel.h"

#define FONT_LOC_Sofia_Sans_Condensed_MED       {"resources/Fonts/Sofia_Sans_Condensed/static/SofiaSansCondensed-Medium.ttf"}
#define ICON_ZEN                                {"resources/Icons/Zen.png"}
#define ICON_EXIT                               {"resources/Icons/Exit.png"}

struct RectSize {
    //float w{ 1510 };    // IF i put 1920, or fullscreen, then it will be broken, not transparent anymore, and the FLAG_WINDOW_FULLSCREEN do the same.
    //float h{ 910 };
    float w{ 1260 };
    float h{ 860 };
};

struct ImageSize {
    float w{};
    float h{};
};

struct SliderInput {
    Rectangle area{};
    float value{};
    float minValue{};
    float maxValue{};
    bool dragging{};

    Rectangle minValRect{};
    Rectangle sliderBarRect{};
    Rectangle maxValRect{};

    SliderInput(Rectangle area, float initialValue, float minValue, float maxValue, bool dragging) :
        area(area), value(initialValue), minValue(minValue), maxValue(maxValue), dragging(dragging) 
    {
        float smallRectWidthCoef = 0.235F;
        float spaceRect = 0.04F;

        minValRect = {
            area.x,
            area.y,
            area.width * smallRectWidthCoef,
            area.height
        };

        sliderBarRect = {
            minValRect.x + minValRect.width + (area.width * spaceRect),
            area.y,
            area.width* (1.0F - (2 * (smallRectWidthCoef + spaceRect))),
            area.height
        };

        maxValRect = {
            sliderBarRect.x + sliderBarRect.width + (area.width * spaceRect),
            area.y,
            area.width * smallRectWidthCoef,
            area.height
        };

        float pad = 2;
        float topPadding = 2.5;

        minValRect = {
            minValRect.x + (pad * 2),
            minValRect.y + (pad * topPadding),
            minValRect.width - (pad * 2 * 2),
            minValRect.height - (pad * topPadding * 2),
        };

        sliderBarRect = {
            sliderBarRect.x + (pad * 2),
            sliderBarRect.y + (pad * topPadding),
            sliderBarRect.width - (pad * 2 * 2),
            sliderBarRect.height - (pad * topPadding * 2),
        };

        maxValRect = {
            maxValRect.x + (pad * 2),
            maxValRect.y + (pad * topPadding),
            maxValRect.width - (pad * 2 * 2),
            maxValRect.height - (pad * topPadding * 2),
        };
    };

    
    
    void Draw() {
        float fontSize = 0.8F;
        //DrawRectangleRoundedLines(minValRect, 0.2F, 10, 0.5F, WHITE);
        //DrawRectangleRounded(minValRect, 0.2F, 10, BLACK);
        DrawTextCustom(minValRect, std::to_string((int)minValue), 1, fontSize, 0, WHITE);

        //DrawRectangleRoundedLines(maxValRect, 0.2F, 10, 0.5F, WHITE);
        //DrawRectangleRounded(maxValRect, 0.2F, 10, BLACK);
        DrawTextCustom(maxValRect, std::to_string((int)maxValue), 1, fontSize, 0, WHITE);

        float handleX = sliderBarRect.x + ((value - minValue) / (maxValue - minValue)) * sliderBarRect.width;
        float handleW = 15;
        Rectangle handleRect{
            handleX - (handleW * 0.5F),
            sliderBarRect.y,
            handleW,
            sliderBarRect.height
        };
        const Color color{ 79, 100, 166, 255 };


        Rectangle SliderBarRectWithPad{
            sliderBarRect.x - (handleW * 0.5F),
            sliderBarRect.y,
            sliderBarRect.width + (handleW * 1),
            sliderBarRect.height
        };
        //DrawRectangleRoundedLines(SliderBarRectWithPad, 0.0F, 10, 0.5F, WHITE);
        DrawRectangleRounded(SliderBarRectWithPad, 0.2F, 10, { 25,32,45,255 });
        DrawRectangleRounded(handleRect, 0.2F, 10, color);
        DrawTextCustom(SliderBarRectWithPad, std::to_string((int)value), 1, fontSize, 0, WHITE);
    }


    void Update() {
        Vector2 mousePos = GetMousePosition();

        if (CheckCollisionPointRec(mousePos, sliderBarRect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            dragging = true;
        }

        if (dragging) {
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                dragging = false;
            }
            else {
                float normalizedValue = (mousePos.x - sliderBarRect.x) / sliderBarRect.width;
                value = minValue + normalizedValue * (maxValue - minValue);

            }
        }

        if (CheckCollisionPointRec(mousePos, sliderBarRect)) {
            float mouseWheelStep = 1;
            float wheelDelta = GetMouseWheelMove();
            value += wheelDelta * mouseWheelStep;
        }

        if (value < minValue) value = minValue;
        if (value > maxValue) value = maxValue;
    }

    void Run() {
        Update();
        Draw();
    }

    float GetValue() const {
        return value;
    }

};

enum Numbering {
    OFF_NUMBERING,
    ON_NUMBERING,
};

struct ButtonNumbering {
    const bool isNumbering{};
    bool isChoosen{};

    std::string textDisplay{};
    bool isHover{};
    const Color colorPressed{ 79, 100, 166, 255 };
    const Color colorNormal{ 180, 180, 180, 255 };

    ButtonNumbering(const bool isNumbering, bool isChoosen)
        : isNumbering(isNumbering), isChoosen(isChoosen) {};

    void chooseThisButton() {
        isChoosen = true;
    }

    void resetChosen() {
        isChoosen = false;
    }

    std::string getTextDisplay() {
        if (isNumbering == ON_NUMBERING) return textDisplay = "ON";
        else return textDisplay = "OFF";
    }

    bool getIsNumbering() {
        return isNumbering;
    }

    Color getColorButton() const {
        if (isChoosen || isHover) return colorPressed;
        else return colorNormal;
    }

    Color getColorText() const {
        return isChoosen ? WHITE : BLACK;
    }
};

struct ButtonNumber {
    std::string textDisplay{};
    bool isChoosen{};
    bool isHover{};
    const Color colorPressed{ 79, 100, 166, 255 };
    const Color colorNormal{ 180, 180, 180, 255 };

    ButtonNumber
    (
        const std::string textDisplay, bool isChoosen
    ) :
        textDisplay(textDisplay), isChoosen(isChoosen) {};

    void chooseThisButton() {
        isChoosen = true;
    }

    void resetChosen() {
        isChoosen = false;
    }

    std::string getNumber() const {
        return textDisplay;
    }

    Color getColorButton() const {
        if (isChoosen || isHover) return colorPressed;
        else return colorNormal;
    }

    Color getColorText() const {
        return isChoosen ? WHITE : BLACK;
    }
};

struct ButtonExportFormat {
    std::string textDisplay{};
    bool isChoosen{};
    bool isHover{};
    const Color colorPressed{ 79, 100, 166, 255 };
    const Color colorNormal{ 180, 180, 180, 255 };

    ButtonExportFormat
    (
        const std::string textDisplay, bool isChoosen
    ) :
        textDisplay(textDisplay), isChoosen(isChoosen) {};

    void chooseThisButton() {
        isChoosen = true;
    }

    void resetChosen() {
        isChoosen = false;
    }

    std::string getFormatDisplay() const {
        std::string textFormat = textDisplay;
        std::transform(textFormat.begin(), textFormat.end(), textFormat.begin(), [](unsigned char c) {
            return std::tolower(c);
            });
        return "." + textFormat;
    }

    Color getColorButton() const {
        if (isChoosen || isHover) return colorPressed;
        else return colorNormal;
    }

    Color getColorText() const {
        return isChoosen ? WHITE : BLACK;
    }
};

enum Notification {
    OFF_NOTIFICATION = 0,
    SUCCESS_EXPORT,
    ERROR_EXPORT,
    WARNING_TITLE_EMPTY
};

struct Plug {
    std::string AppTitle{ "Zen Pixel" };
    RectSize Screen{};
    Vector2 mousePosition{};
    //Color ColorLayer1    { 40,40,40, 90 };
    Color ColorLayer1    { BLANK };
    Color ColorLayer2    { 25,32,45,255 };
    Color ColorTitleBar  { 18,26,35,255 };
    Color ColorPanel     { 40,48,55,255 };
    Color ColorPanel2    { 20,25,27,255 };
    Font fontGeneral{};
    Font fontNumberLowRes{};
    Font fontNumberHighRes{};
    Texture2D TexZEN{};
    Texture2D TexExit{};
    float LabelSize{ 30 };
    Image ImageInput{};
    Image ImageOutput{};
    Texture2D texture_input{};
    Texture2D texture_output{};
    Rectangle flexible_panel_input{};
    Rectangle flexible_panel_output{};
    Rectangle flexible_panel_crop{};
    bool reload_setup{ false };
    ImageSize flexibleSize{};
    std::vector<std::vector<Color>> ImagePixels{};
    std::vector<std::string> setupParameter{ "NUMBERING", "NUMBER", "SPACE", "CORNER", "PIXEL RANGE", "TITLE", "RESOLUTION", "FORMAT" };
    std::vector<ButtonNumbering> argumentNumbering{ {OFF_NUMBERING, 1}, {ON_NUMBERING, 0}, };
    std::vector<ButtonNumber> argumentNumber{ {"CASUAL", 0}, {"INDEX", 1} };
    std::vector<ButtonExportFormat> argumentFormat{ {"PNG", 1}, {"JPG", 0} };

    bool g_numbering{};
    std::string g_number{};
    std::string g_title{};
    std::string g_format{};
    int g_pixelatedRange{25};
    int g_corner{3};
    int g_space{2};
    int g_resolution{ 1 };
    std::string g_inputTitle{};

    bool g_exporting{ false };
    std::string g_folderPath{ "Output/" };
    std::string g_exportPath{};
    int notification{OFF_NOTIFICATION};
    float notificationTime{ 0.0F };
    bool closeThisApp{ false };

    Shader shaderRoundedRect{};
    bool redrawTexture{ true };
    RenderTexture2D renderTextureLiveView{};
    Texture2D textureLiveViewSSAA{};

    bool cameraSetup{ false };
    float liveViewZoom{ 1.0F };

    Rectangle DebugCamera{};
};

Plug ZenPlug{};
static Plug* p = &ZenPlug;

enum TextAlign {
    LEFT,
    CENTER,
    RIGHT
};

void InitializedFont(void);
void InitializedIcons(void);
void DrawTextCustom(Rectangle& panel, std::string text, int align, float size, float space, const Font& font, const Color color);
void LoadSetup(int new_width, int new_height);
void UpdateDraw();
void DrawDebug(std::vector<std::string>& data, Rectangle& paramRect);
std::vector<std::string> ExtractRectValue(std::string header, Rectangle& PanelOutputImage);
void redrawRenderTexture(Rectangle& DrawArea);
ImageSize CalculateFlexibleImage();
Rectangle FlexibleRectangle(Rectangle& BaseRect, float ObjectWidth, float ObjectHeight);
void InputTextBox(Rectangle& inputTitleBase);
void DrawNotification(Rectangle& panel, std::string text, int align, float size, float space, const Color color, const Color fillColor);
void ExportingHighResImage();
void OutputFolderTest();


int main()
//int WinMain()
{
    SetConfigFlags(FLAG_WINDOW_ALWAYS_RUN);
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    SetConfigFlags(FLAG_WINDOW_TRANSPARENT);
    InitWindow((int)p->Screen.w, (int)p->Screen.h, p->AppTitle.c_str());
    SetTargetFPS(60);
    SetWindowIcon(LoadImage(ICON_ZEN));
    SetTraceLogLevel(LOG_NONE);

    OutputFolderTest();
    InitializedFont();
    InitializedIcons();

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(p->ColorLayer1);

        UpdateDraw();
        DrawFPS(60, 21);

        EndDrawing();

        if (p->g_exporting) {
            ExportingHighResImage();
        }

        if (p->closeThisApp) break;
    }
    CloseWindow();

}

void InitializedCameraLiveView()
{
    
}

void OutputFolderTest()
{
    std::string outputPath{ p->g_folderPath };
    if (std::filesystem::exists(outputPath) && std::filesystem::is_directory(outputPath)) {
        std::string info{ "[Passed] : Output Directory Exists" };
        TraceLog(LOG_INFO, info.c_str());
    }
    else {
        std::string info{ "[Failed] : Output Directory NOT Exists" };
        TraceLog(LOG_WARNING, info.c_str());

        std::filesystem::create_directory(outputPath);

        info = "Creating " + outputPath + " Directory";
        TraceLog(LOG_INFO, info.c_str());
        if (std::filesystem::exists(outputPath) && std::filesystem::is_directory(outputPath)) {
            std::string info{ "[Passed] : Output Directory Exists" };
            TraceLog(LOG_INFO, info.c_str());
        }
    }
}

void ExportingHighResImage() {
    int highresW{ 1920 };
    int highresH{ 1080 };

    if (p->g_resolution == 1) {
        highresW = 1280;
        highresH = 720;
    }
    else {
        highresH *= (p->g_resolution - 1);
        highresW *= (p->g_resolution - 1);
    }

    int SSAA = 2; // Cant USE 3 OR 4 because in somecase the image will be too big and the app crash. But 2 is enough.

    Rectangle highresRect = { 0,0,float(highresW * SSAA),float(highresH * SSAA) };
    Rectangle flexibleHighres = FlexibleRectangle(highresRect, p->flexibleSize.w, p->flexibleSize.h);

    RenderTexture2D highresTexture = LoadRenderTexture((int)flexibleHighres.width, (int)flexibleHighres.height);

    BeginTextureMode(highresTexture);
    ClearBackground(BLANK);

    float pad = 2;
    Rectangle pixelDrawArea = {
        pad * 1,
        pad * 1,
        flexibleHighres.width - (pad * 2),
        flexibleHighres.height - (pad * 2),
    };

    if (p->texture_input.height != 0) {
        float tiles_w = pixelDrawArea.width / p->ImagePixels[0].size();
        float tiles_h = pixelDrawArea.height / p->ImagePixels.size();

        Rectangle tiles{};
        for (size_t y = 0; y < p->ImagePixels.size(); y++) {
            for (size_t x = 0; x < p->ImagePixels[y].size(); x++) {
                tiles = {
                    pixelDrawArea.x + (x * tiles_w),
                    pixelDrawArea.y + (y * tiles_h),
                    tiles_w,
                    tiles_h
                };

                float pad{};
                if (p->g_resolution == 1) pad = p->g_space * 0.5F * 1.5F;
                else pad = p->g_space * 0.5F * (p->g_resolution - 1) * SSAA;
                float corner = p->g_corner * 0.1F;
                Rectangle pixel = {
                    tiles.x + (pad * 1),
                    tiles.y + (pad * 1),
                    tiles.width - (pad * 2),
                    tiles.height - (pad * 2),
                };

                Color colorTile = p->ImagePixels[y][x];
                DrawRectangleRounded(pixel, corner, 10, colorTile);

                float luminance = 0.2126f * colorTile.r + 0.7152f * colorTile.g + 0.0722f * colorTile.b;
                Color textColor = (luminance > 128) ? BLACK : WHITE;

                size_t w = p->ImagePixels[y].size();
                size_t number = y * w + x;

                if (p->g_number == "CASUAL") {
                    number += 1;
                }

                if (p->g_numbering) {
                    std::string text = std::to_string(number);
                    Font font{ p->fontNumberLowRes };
                    if (p->g_resolution >= 3) font = p->fontNumberHighRes;
                    DrawTextCustom(pixel, text, CENTER, 0.65F, -0.5F, font, textColor);
                }
            }
        }
    }
    EndTextureMode();

    SetTextureFilter(highresTexture.texture, TEXTURE_FILTER_BILINEAR);
    Image highresImage = LoadImageFromTexture(highresTexture.texture);
    ImageFlipVertical(&highresImage);

    // SUPER SAMPLING BY DOWNSCALE
    highresW = (int)(flexibleHighres.width / SSAA);
    highresH = (int)(flexibleHighres.height / SSAA);

    ImageResize(&highresImage, highresW, highresH);

    //ExportImage(highresImage, "highres_output.png");

    if (!p->g_inputTitle.empty()) {
        std::string title = p->g_exportPath;

        if (p->g_format == ".png") {
            // RAYLIB IMAGE EXPORT
            if (ExportImage(highresImage, title.c_str())) {
                p->notification = SUCCESS_EXPORT;
            }
            else {
                p->notification = ERROR_EXPORT;
            }
        }
        else if (p->g_format == ".jpg") {
            // JPG HANDLED BY SFML.
            int w = highresImage.width;
            int h = highresImage.height;

            Color* colorPointer = LoadImageColors(highresImage);

            std::vector<Color> colorsDataRaylib{};
            colorsDataRaylib.reserve(w * h);

            for (size_t i = 0; i < w * h; i++) {
                colorsDataRaylib.push_back(colorPointer[i]);
            }
            UnloadImageColors(colorPointer);

            std::vector<sf::Uint8> colorDataSFML{};
            colorDataSFML.reserve(w * h * 4);

            for (const auto& color : colorsDataRaylib) {
                colorDataSFML.push_back(color.r);
                colorDataSFML.push_back(color.g);
                colorDataSFML.push_back(color.b);
                colorDataSFML.push_back(color.a);
            }

            sf::Image imageSFML{};
            imageSFML.create(w, h, colorDataSFML.data());

            // SFML IMAGE EXPORT
            if (imageSFML.saveToFile(title)) {
                std::string log = "FILEIO: [" + title + "] File exported succesfully";
                TraceLog(LOG_INFO, log.c_str());
                p->notification = SUCCESS_EXPORT;
            }
            else {
                std::string log = "FILEIO: [" + title + "] File exporting failed";
                TraceLog(LOG_ERROR, log.c_str());
                p->notification = ERROR_EXPORT;
            }
        }
    }
    else {
        p->notification = WARNING_TITLE_EMPTY;
    }

    UnloadRenderTexture(highresTexture);
    UnloadImage(highresImage);

    p->g_exporting = false;
}

void UpdateDraw()
{
    p->mousePosition = GetMousePosition();

    RectSize BaseApp{ p->Screen.w - 10, p->Screen.h - 10 };
    //RectSize BaseApp{ 1600, 1000 };
    Rectangle BaseAppRect{
        (p->Screen.w - BaseApp.w) / 2,
        (p->Screen.h - BaseApp.h) / 2,
        BaseApp.w,
        BaseApp.h,
    };
    DrawRectangleRounded(BaseAppRect, 0.01F, 10, p->ColorLayer2);
    //DrawRectangleRoundedLines(BaseAppRect, 0.01F, 10, 4.0F, Fade(SKYBLUE, 0.3F));
    //DrawRectangleRoundedLines(BaseAppRect, 0.01F, 10, 2.0F, Fade(SKYBLUE, 0.1F));
    DrawRectangleRoundedLines(BaseAppRect, 0.01F, 10, 4.0F, Fade(BLACK, 0.8F));
    DrawRectangleRoundedLines(BaseAppRect, 0.01F, 10, 2.0F, Fade(BLACK, 0.5F));

    RectSize TitleBar{ BaseApp.w, 50 };
    Rectangle TitleBarRect{
        BaseAppRect.x,
        BaseAppRect.y,
        TitleBar.w,
        TitleBar.h,
    };
    DrawRectangleRounded(TitleBarRect, 0.2F, 10, p->ColorTitleBar);

    // CUSTOM TITLE BAR
    {
        std::string text{ "ZEN PIXEL" };
        DrawTextCustom(TitleBarRect, text, CENTER, 0.7F, 1.0F, p->fontGeneral, WHITE);

        // ZEN ICON
        {
            float pad{ 8 };
            Rectangle IconZenRect{
                TitleBarRect.x + (pad * 1),
                TitleBarRect.y + (pad * 1),
                TitleBarRect.height - (pad * 2),
                TitleBarRect.height - (pad * 2)
            };
            Rectangle source{ 0,0,200,200 };
            Rectangle dest{ IconZenRect };
            DrawTexturePro(p->TexZEN, source, dest, { 0,0 }, 0, WHITE);
            DrawRectangleRoundedLines(dest, 0.1F, 10, 0.5F, WHITE);
        }

        // EXIT ICON
        {
            float pad{ 8 };
            Rectangle IconExitRect{
                TitleBarRect.x + TitleBarRect.width - (TitleBarRect.height) + (pad * 1),
                TitleBarRect.y + (pad * 1),
                TitleBarRect.height - (pad * 2),
                TitleBarRect.height - (pad * 2)
            };
            Rectangle source{ 0,0,100,100 };
            Rectangle dest{ IconExitRect };
            Color color{};
            Color iconColor{};


            if (CheckCollisionPointRec(p->mousePosition, dest) && IsCursorOnScreen())
            {
                iconColor = WHITE;
                color = RED;
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    p->closeThisApp = true;
                    //CloseWindow(); // LAMBAT KARENA TIDAK CLOSE WINDOW DI AKHIR END DRAWING.
                }
            }
            else {
                color = LIGHTGRAY;
                iconColor = Fade(BLACK, 0.75F);
            }

            DrawRectangleRounded(dest, 0.2F, 10, color);
            DrawTexturePro(p->TexExit, source, dest, {}, 0, iconColor);
        }
    }


    // MAIN
    {
        float pad{ 40 };
        Rectangle PanelBase{
            BaseAppRect.x + pad,
            BaseAppRect.y + TitleBarRect.height + pad,
            BaseAppRect.width - (pad * 2),
            BaseAppRect.height - TitleBarRect.height - (pad * 2)
        };
        //DrawRectangleLinesEx(PanelBase, 0.5F, WHITE);


        // NEW LAYOUT
        if (1)
        {
            float spacing{ 10 };
            Rectangle MainLeftSection{
                PanelBase.x,
                PanelBase.y,
                PanelBase.width * 0.24F + spacing,
                PanelBase.height
            };

            Rectangle MainRightSection{
                MainLeftSection.x + MainLeftSection.width + spacing,
                MainLeftSection.y,
                PanelBase.width - MainLeftSection.width - spacing,
                MainLeftSection.height
            };

            // MainLeftSection
            {
                Rectangle PanelInputImage{
                    MainLeftSection.x,
                    MainLeftSection.y,
                    MainLeftSection.width,
                    MainLeftSection.height * 0.425F + spacing
                };
                //DrawRectangleLinesEx(PanelInputImage, 0.5F, WHITE);
                DrawRectangleRounded(PanelInputImage, 0.05F, 10, p->ColorPanel);

                p->DebugCamera = PanelInputImage;

                // PanelInputImage
                {
                    // DRAGDROP INFORMATION
                    if (p->texture_input.height == 0) 
                    {
                        std::string text{ "DRAG DROP YOUR IMAGE" };
                        DrawTextCustom(PanelInputImage, text, CENTER, 0.07F, 1.0F, p->fontGeneral, WHITE);
                    }

                    // DRAGDROP HANDLER
                    if (IsFileDropped()) {
                        FilePathList dropped_file = LoadDroppedFiles();

                        const char* c_file_path = dropped_file.paths[0];
                        std::string cpp_file_path = std::string(c_file_path);

                        if (IsFileExtension(c_file_path, ".png"))
                        {
                            if (p->ImageInput.height != 0) UnloadImage(p->ImageInput);
                            p->ImageInput = LoadImage(c_file_path);
                            p->flexibleSize = CalculateFlexibleImage();
                            p->flexible_panel_input = FlexibleRectangle(PanelInputImage, p->flexibleSize.w, p->flexibleSize.h);

                        }
                        else if (IsFileExtension(c_file_path, ".jpg") || IsFileExtension(c_file_path, ".jpeg")) {

                            // LOAD JPG USING SFML
                            sf::Image sfmlImage{};
                            if (sfmlImage.loadFromFile(cpp_file_path)) {
                                TraceLog(LOG_INFO, "Success load image from SFML");

                                if (1)
                                {
                                    // Save sfmlLmage to disk in png.
                                    std::string temporaryImage{ "temporary.png" };
                                    sfmlImage.saveToFile(temporaryImage);

                                    UnloadImage(p->ImageInput);
                                    p->ImageInput = LoadImage(temporaryImage.c_str());
                                    p->flexibleSize = CalculateFlexibleImage();
                                    p->flexible_panel_input = FlexibleRectangle(PanelInputImage, p->flexibleSize.w, p->flexibleSize.h);

                                    std::filesystem::remove(temporaryImage);
                                }

                            }
                            else {
                                TraceLog(LOG_WARNING, "Failed load image from SFML");
                            }

                            
                        }

                        p->reload_setup = true;

                        UnloadDroppedFiles(dropped_file);
                    }

                    // DRAW INPUT TEXTURE
                    if (p->reload_setup) {
                        if (p->ImageInput.height != 0) {
                            LoadSetup((int)p->flexibleSize.w, (int)p->flexibleSize.h);
                        }
                        p->reload_setup = false;
                        p->redrawTexture = true;
                        p->cameraSetup = true;
                    }

                    if (p->texture_input.height != 0) {
                        // Draw input
                        {
                            Rectangle source{
                                0,0,(float)p->texture_input.width, (float)p->texture_input.height
                            };
                            Rectangle dest{ p->flexible_panel_input };

                            pad = 5.0F;
                            dest = {
                                dest.x + (pad * 1),
                                dest.y + (pad * 1),
                                dest.width - (pad * 2),
                                dest.height - (pad * 2)
                            };
                            DrawTexturePro(p->texture_input, source, dest, { 0,0 }, 0, WHITE);
                            DrawRectangleLinesEx(dest, 1.5F, p->ColorTitleBar);
                        }
                    }


                }

                Rectangle SetupSection{
                    PanelInputImage.x,
                    PanelInputImage.y + PanelInputImage.height + spacing,
                    PanelInputImage.width,
                    MainLeftSection.height - PanelInputImage.height - spacing
                };
                DrawRectangleRounded(SetupSection, 0.05F, 10, p->ColorPanel);

                // Membuat Margin
                float padTopBot = 10;
                SetupSection = {
                    SetupSection.x,
                    SetupSection.y + (padTopBot * 1),
                    SetupSection.width,
                    SetupSection.height - (padTopBot * 2)
                };

                Rectangle PanelParameter{
                    SetupSection.x,
                    SetupSection.y,
                    SetupSection.width * 0.35F + spacing,
                    SetupSection.height
                };
                //DrawRectangleLinesEx(PanelParameter, 0.5F, WHITE);

                Rectangle PanelArgument{
                    PanelParameter.x + PanelParameter.width + spacing,
                    PanelParameter.y,
                    (SetupSection.width - PanelParameter.width - spacing) * 0.975F,
                    PanelParameter.height
                };
                //DrawRectangleLinesEx(PanelArgument, 0.5F, WHITE);

                // SetupSection
                {
                    size_t parameterCounts = p->setupParameter.size();
                    for (size_t i = 0; i < parameterCounts; i++) {
                        // PanelParameter
                        {
                            Rectangle parameter{
                                PanelParameter.x,
                                PanelParameter.y + (i * PanelParameter.height / parameterCounts),
                                PanelParameter.width,
                                PanelParameter.height / parameterCounts
                            };
                            //DrawRectangleLinesEx(parameter, 0.2F, WHITE);

                            std::string text = p->setupParameter.at(i);
                            DrawTextCustom(parameter, text, LEFT, 0.54F, 1.0F, p->fontGeneral, WHITE);
                        }

                        // PanelArgument
                        {
                            Rectangle argument{
                                PanelArgument.x,
                                PanelArgument.y + (i * PanelArgument.height / parameterCounts),
                                PanelArgument.width,
                                PanelArgument.height / parameterCounts
                            };
                            //DrawRectangleLinesEx(argument, 0.2F, WHITE);

                            float buttonPad = 5;
                            float buttonPadFactor = 1.25F;

                            if (p->setupParameter.at(i) == "NUMBERING") {
                            //if (i == 0) {
                                size_t buttonCounts = p->argumentNumbering.size();
                                for (size_t j = 0; j < buttonCounts; j++) {
                                    auto& ObjectButtonNumbering = p->argumentNumbering.at(j);

                                    Rectangle buttonBase{
                                        argument.x + (j * argument.width / buttonCounts),
                                        argument.y,
                                        argument.width / buttonCounts,
                                        argument.height
                                    };
                                    //DrawRectangleLinesEx(buttonBase, 0.5F, WHITE);

                                    Rectangle ButtonNumberingRect{
                                        buttonBase.x + (buttonPad * 1),
                                        buttonBase.y + (buttonPad * buttonPadFactor),
                                        buttonBase.width - (buttonPad * 2),
                                        buttonBase.height - (buttonPad * 2 * buttonPadFactor)
                                    };

                                    if (CheckCollisionPointRec(p->mousePosition, ButtonNumberingRect)) {
                                        ObjectButtonNumbering.isHover = true;

                                        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                                            for (auto& button : p->argumentNumbering) {
                                                button.resetChosen();
                                            }
                                            ObjectButtonNumbering.chooseThisButton();
                                            p->redrawTexture = true;
                                        }
                                    }
                                    else {
                                        ObjectButtonNumbering.isHover = false;
                                    }

                                    if (ObjectButtonNumbering.isChoosen) {
                                        p->g_numbering = ObjectButtonNumbering.getIsNumbering();
                                    }

                                    DrawRectangleRounded(ButtonNumberingRect, 0.2F, 10, ObjectButtonNumbering.getColorButton());
                                    std::string text = ObjectButtonNumbering.getTextDisplay();
                                    DrawTextCustom(ButtonNumberingRect, text, CENTER, 0.75F, 1.0F, p->fontGeneral, ObjectButtonNumbering.getColorText());
                                }
                            }
                            else if (p->setupParameter.at(i) == "NUMBER") {

                                size_t buttonCounts = p->argumentNumber.size();
                                for (size_t j = 0; j < buttonCounts; j++) {
                                    auto& ObjectButtonNumber = p->argumentNumber.at(j);

                                    Rectangle buttonBase{
                                        argument.x + (j * argument.width / buttonCounts),
                                        argument.y,
                                        argument.width / buttonCounts,
                                        argument.height
                                    };
                                    //DrawRectangleLinesEx(buttonBase, 0.5F, WHITE);

                                    Rectangle ButtonNumberRect{
                                        buttonBase.x + (buttonPad * 1),
                                        buttonBase.y + (buttonPad * buttonPadFactor),
                                        buttonBase.width - (buttonPad * 2),
                                        buttonBase.height - (buttonPad * 2 * buttonPadFactor)
                                    };

                                    if (CheckCollisionPointRec(p->mousePosition, ButtonNumberRect)) {
                                        ObjectButtonNumber.isHover = true;

                                        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                                            for (auto& button : p->argumentNumber) {
                                                button.resetChosen();
                                            }
                                            ObjectButtonNumber.chooseThisButton();
                                            p->redrawTexture = true;
                                        }
                                    }
                                    else {
                                        ObjectButtonNumber.isHover = false;
                                    }

                                    if (ObjectButtonNumber.isChoosen) {
                                        p->g_number = ObjectButtonNumber.getNumber();
                                    }

                                    DrawRectangleRounded(ButtonNumberRect, 0.2F, 10, ObjectButtonNumber.getColorButton());
                                    std::string text = ObjectButtonNumber.textDisplay;
                                    DrawTextCustom(ButtonNumberRect, text, CENTER, 0.75F, 1.0F, p->fontGeneral, ObjectButtonNumber.getColorText());

                                }
                            }
                            else if (p->setupParameter.at(i) == "SPACE") {

                                static SliderInput SliderSpace{ argument, (float)p->g_space, -5, 10, false };
                                SliderSpace.Run();

                                if (CheckCollisionPointRec(p->mousePosition, argument)) {
                                    int newVal = (int)SliderSpace.GetValue();
                                    static int oldVal = newVal;

                                    if (newVal != oldVal) {
                                        p->g_space = newVal;
                                        oldVal = newVal;
                                        p->redrawTexture = true;
                                    }
                                }

                            }
                            else if (p->setupParameter.at(i) == "CORNER") {

                                static SliderInput SliderCorner{ argument, (float)p->g_corner, 0, 10, false };
                                SliderCorner.Run();

                                if (CheckCollisionPointRec(p->mousePosition, argument)) {
                                    int newVal = (int)SliderCorner.GetValue();
                                    static int oldVal = newVal;

                                    if (newVal != oldVal) {
                                        p->g_corner = newVal;
                                        oldVal = newVal;
                                        p->redrawTexture = true;
                                    }
                                }

                            }
                            else if (p->setupParameter.at(i) == "PIXEL RANGE") {

                                static SliderInput SliderPixel{ argument, (float)p->g_pixelatedRange, 2, 80, false };
                                SliderPixel.Run();

                                if (CheckCollisionPointRec(p->mousePosition, argument)) {
                                    int newVal = (int)SliderPixel.GetValue();
                                    static int oldVal = newVal;

                                    if (newVal != oldVal) {
                                        p->g_pixelatedRange = newVal;
                                        oldVal = newVal;
                                        p->reload_setup = true;
                                        p->redrawTexture = true;
                                    }
                                }
                            }
                            else if (p->setupParameter.at(i) == "TITLE") {

                                Rectangle inputTitleBase{
                                    argument.x + (buttonPad * 1),
                                    argument.y + (buttonPad * buttonPadFactor),
                                    argument.width - (buttonPad * 2),
                                    argument.height - (buttonPad * 2 * buttonPadFactor)
                                };
                                DrawRectangleRounded(inputTitleBase, 0.1F, 10, RAYWHITE);

                                InputTextBox(inputTitleBase);

                            }
                            else if (p->setupParameter.at(i) == "RESOLUTION") {
                                static SliderInput SliderResolution{ argument, (float)p->g_resolution, 1, 4, false};
                                SliderResolution.Run();

                                if (CheckCollisionPointRec(p->mousePosition, argument)) {
                                    int newVal = (int)SliderResolution.GetValue();
                                    static int oldVal = newVal;

                                    if (newVal != oldVal) {
                                        p->g_resolution = newVal;
                                        oldVal = newVal;
                                        p->redrawTexture = true;
                                        //p->reload_setup = true;
                                    }
                                }
                            }
                            else if (p->setupParameter.at(i) == "FORMAT") {

                                size_t buttonCounts = p->argumentFormat.size();
                                for (size_t j = 0; j < buttonCounts; j++) {
                                    auto& ObjectButtonFormat = p->argumentFormat.at(j);

                                    Rectangle buttonBase{
                                        argument.x + (j * argument.width / buttonCounts),
                                        argument.y,
                                        argument.width / buttonCounts,
                                        argument.height
                                    };
                                    //DrawRectangleLinesEx(buttonBase, 0.5F, WHITE);
                                    
                                    Rectangle ButtonFormatRect{
                                        buttonBase.x + (buttonPad * 1),
                                        buttonBase.y + (buttonPad * buttonPadFactor),
                                        buttonBase.width - (buttonPad * 2),
                                        buttonBase.height - (buttonPad * 2 * buttonPadFactor)
                                    };

                                    if (CheckCollisionPointRec(p->mousePosition, ButtonFormatRect)) {
                                        ObjectButtonFormat.isHover = true;

                                        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                                            for (auto& button : p->argumentFormat) {
                                                button.resetChosen();
                                            }
                                            ObjectButtonFormat.chooseThisButton();
                                        }
                                    }
                                    else {
                                        ObjectButtonFormat.isHover = false;
                                    }

                                    if (ObjectButtonFormat.isChoosen) {
                                        p->g_format = ObjectButtonFormat.getFormatDisplay();
                                    }

                                    DrawRectangleRounded(ButtonFormatRect, 0.2F, 10, ObjectButtonFormat.getColorButton());
                                    std::string text = ObjectButtonFormat.textDisplay;
                                    DrawTextCustom(ButtonFormatRect, text, CENTER, 0.75F, 1.0F, p->fontGeneral, ObjectButtonFormat.getColorText());

                                }
                            }

                        }



                    }



                }



            }

            // MainRightSection
            {
                Rectangle PanelOutputImage{
                    (float)(int)MainRightSection.x,
                    (float)(int)MainRightSection.y,
                    (float)(int)PanelBase.width - MainLeftSection.width - spacing,
                    (float)(int)MainRightSection.height * 0.915F + spacing,
                };
                //DrawRectangleLinesEx(PanelOutputImage, 0.5F, WHITE);
                //DrawRectangleRounded(PanelOutputImage, 0.025F, 10, p->ColorPanel);
                DrawRectangleRounded(PanelOutputImage, 0.025F, 10, p->ColorTitleBar);

                // PanelOutputImage
                {
                    p->flexible_panel_output = FlexibleRectangle(PanelOutputImage, p->flexible_panel_input.width, p->flexible_panel_input.height);

                    // RenderTextureArea
                    Rectangle TextureLiveSize{ 0, 0, 1920, 1080 };
                    if (p->g_resolution > 2) {
                        TextureLiveSize = {
                            0, 0,
                            TextureLiveSize.width * (p->g_resolution - 1),
                            TextureLiveSize.height * (p->g_resolution - 1)
                        };
                    }
                    Rectangle flexArea = FlexibleRectangle(TextureLiveSize, p->flexible_panel_input.width, p->flexible_panel_input.height);

                    p->flexible_panel_crop = {
                        p->flexible_panel_output.x,
                        p->flexible_panel_output.y,
                        p->flexible_panel_output.width + 0,
                        p->flexible_panel_output.height + 0,
                    };

                    float pad = 10.0F;
                    Rectangle pixelDrawArea = {
                        (float)(int)p->flexible_panel_output.x + (pad * 1),
                        (float)(int)p->flexible_panel_output.y + (pad * 1),
                        (float)(int)p->flexible_panel_output.width - (pad * 2),
                        (float)(int)p->flexible_panel_output.height - (pad * 2),
                    };

                    // NEW
                    if (p->texture_input.height != 0) {

                        pad = 4.0F;
                        Rectangle newPixelDrawArea = {
                            (0),
                            (0),
                            (flexArea.width - (0)),
                            (flexArea.height - (0))
                        };

                        if (p->redrawTexture) {
                            redrawRenderTexture(newPixelDrawArea);
                            p->redrawTexture = false;

                        }

                        Rectangle source{
                            0,
                            0,
                            newPixelDrawArea.width,
                            -newPixelDrawArea.height
                        };

                        static Rectangle dest{};
                        
                        //DrawTexturePro(p->textureLiveViewSSAA, source, dest, { 0 }, 0, WHITE);

                        // NEW SOLUTION
                        if (p->cameraSetup) {
                            p->liveViewZoom = 1.0F;

                            dest = {
                                pixelDrawArea.x,
                                pixelDrawArea.y,
                                pixelDrawArea.width,
                                pixelDrawArea.height,
                            };
                            p->cameraSetup = false;
                        }


                        if (CheckCollisionPointRec(p->mousePosition, PanelOutputImage)) {
                            float zoomSpeed = 0.05F;
                            float zoom = p->liveViewZoom;
                            float wheel = GetMouseWheelMove();
                            static bool zooming = false;
                            static float steps = 0.0F;

                            // ZOOMMING
                            if (wheel != 0) {
                                zooming = true;
                                steps += (wheel * 0.4F);
                            }

                            if (zooming) {
                                Vector2 mouseBeforeZoom{
                                    (p->mousePosition.x - dest.x) / dest.width,
                                    (p->mousePosition.y - dest.y) / dest.height
                                };

                                static float t = 0.0F;

                                float zoomVal = Lerp(0, steps, t);
                                p->liveViewZoom = zoom + (zoomVal * zoomSpeed * zoom);
                                p->liveViewZoom = Clamp(p->liveViewZoom, 0.75F, 20.0F);

                                dest.width = pixelDrawArea.width * p->liveViewZoom;
                                dest.height = pixelDrawArea.height * p->liveViewZoom;

                                dest.x = p->mousePosition.x - mouseBeforeZoom.x * dest.width;
                                dest.y = p->mousePosition.y - mouseBeforeZoom.y * dest.height;

                                t += 0.1F;

                                if (t >= 1.0F) {
                                    zooming = false;
                                    steps = 0.0F;
                                    t = 0.0F;
                                }
                            }

                            // PANNNING
                            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                                Vector2 mouseDelta = GetMouseDelta();
                                dest.x += mouseDelta.x;
                                dest.y += mouseDelta.y;
                            }
                        }

                        Rectangle PanelOutputView{
                            PanelOutputImage.x + (pad * 2),
                            PanelOutputImage.y + (pad * 2),
                            PanelOutputImage.width - (pad * 2 * 2),
                            PanelOutputImage.height - (pad * 2 * 2),
                        };

                        pad = 5.0F;
                        dest.x = Clamp(dest.x, PanelOutputView.x - (dest.width - PanelOutputView.width + (pad * 2.F)), PanelOutputView.x + (pad * 2.F));
                        dest.y = Clamp(dest.y, PanelOutputView.y - (dest.height - PanelOutputView.height + (pad * 2.F)), PanelOutputView.y + (pad * 2.F));

                        if (dest.width < PanelOutputView.width && dest.height < PanelOutputView.height) {
                            // CENTER IN XY
                            dest.x = PanelOutputView.x + ((PanelOutputView.width - dest.width) / 2.0F);
                            dest.y = PanelOutputView.y + ((PanelOutputView.height - dest.height) / 2.0F);

                        }
                        else if (dest.width < PanelOutputView.width) {
                            // CENTER IN X
                            dest.x = PanelOutputView.x + ((PanelOutputView.width - dest.width) / 2.0F);
                        }
                        else if (dest.height < PanelOutputView.height) {
                            // CENTER IN Y
                            dest.y = PanelOutputView.y + ((PanelOutputView.height - dest.height) / 2.0F);
                        }

                        // Apply Scissor Mode to constrain the drawing to the panel area
                        int pad = 5;
                        BeginScissorMode(
                            (int)PanelOutputImage.x + (pad * 1),
                            (int)PanelOutputImage.y + (pad * 1),
                            (int)PanelOutputImage.width - (pad * 2),
                            (int)PanelOutputImage.height - (pad * 2)
                        );

                        // Draw the texture with zoom and panning applied
                        DrawTexturePro(p->textureLiveViewSSAA, source, dest, { 0, 0 }, 0, WHITE);
                        EndScissorMode();

                        pad = 7.0F;
                        Rectangle PanelOutputFrame{
                            PanelOutputImage.x + (pad * 1),
                            PanelOutputImage.y + (pad * 1),
                            PanelOutputImage.width - (pad * 2),
                            PanelOutputImage.height - (pad * 2),
                        };
                        //DrawRectangleRoundedLines(PanelOutputFrame, 0.005F, 10, pad, { 25,28,30,255 });
                        DrawRectangleRoundedLines(PanelOutputFrame, 0.005F, 10, pad, p->ColorPanel);

                        // FOR DEBUGGING PANNING CAMERA DESTINATION POSITION
                        if (0)
                        {
                            static bool openDebug = true;

                            if (CheckCollisionPointRec(p->mousePosition, p->DebugCamera)) {
                                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                                    openDebug = !openDebug;
                                }
                            }

                            if (openDebug) {
                                DrawRectangleRounded(p->DebugCamera, 0.05F, 10, MAROON);

                                float pad = 4.0F;
                                Rectangle debug{
                                    p->DebugCamera.x + (pad * 1 * 1),
                                    p->DebugCamera.y + (pad * 1 * 1),
                                    p->DebugCamera.width - (pad * 1 * 2),
                                    p->DebugCamera.height - (pad * 1 * 2)
                                };

                                std::vector<std::string> debugParams{ "RECT", "X", "Y", "W", "H" };

                                int area = 3;
                                float w_area = debug.width / area;
                                for (size_t i = 0; i < area; i++) {

                                    Rectangle parameter{
                                        debug.x + (i * debug.width / area),
                                        debug.y,
                                        debug.width / area,
                                        debug.height,
                                    };
                                    pad = 4.0F;

                                    parameter = {
                                        parameter.x + (pad * 1),
                                        parameter.y + (pad * 1),
                                        parameter.width - (pad * 2),
                                        parameter.height - (pad * 2)
                                    };
                                    DrawRectangleRounded(parameter, 0.15F, 10, {25,25,28,255});

                                    pad = 2.0F;
                                    Rectangle paramRect{
                                        parameter.x + (pad * 1),
                                        parameter.y + (pad * 1),
                                        parameter.width - (pad * 2),
                                        parameter.height - (pad * 2)
                                    };

                                    if (i == 0) {

                                        auto data = debugParams;

                                        DrawDebug(data, paramRect);
                                    }
                                    else if (i == 1) {
                                        auto data = ExtractRectValue("AREA", PanelOutputImage);

                                        DrawDebug(data, paramRect);
                                    }
                                    else if (i == 2) {
                                        auto data = ExtractRectValue("DEST", dest);

                                        DrawDebug(data, paramRect);
                                    }
                                }
                            }
                        }


                    }
                }

                Rectangle FooterSection{
                    PanelOutputImage.x,
                    PanelOutputImage.y + PanelOutputImage.height + spacing,
                    PanelOutputImage.width,
                    MainRightSection.height - PanelOutputImage.height - spacing
                };
                //DrawRectangleLinesEx(FooterSection, 0.5F, WHITE);
                DrawRectangleRounded(FooterSection, 0.3F, 10, p->ColorPanel);

                // FooterSection
                {
                    // NOTIFICATION
                    {
                        std::string text{};

                        if (p->notification != OFF_NOTIFICATION) {

                            p->notificationTime -= GetFrameTime();

                            if (p->notification == SUCCESS_EXPORT) {
                                text = "SUCCESS EXPORT TO " + p->g_exportPath;
                                DrawNotification(FooterSection, text, LEFT, 0.55F, 1.0, WHITE, DARKBLUE);
                            }
                            else if (p->notification == ERROR_EXPORT) {
                                text = "FAILED TO EXPORT " + p->g_exportPath;
                                DrawNotification(FooterSection, text, LEFT, 0.55F, 1.0, WHITE, RED);
                            }
                            else if (p->notification == WARNING_TITLE_EMPTY) {
                                text = "PLEASE FILL EXPORT TITLE";
                                DrawNotification(FooterSection, text, LEFT, 0.55F, 1.0, WHITE, RED);
                            }

                            if (p->notificationTime < 0.0F) {
                                p->notification = OFF_NOTIFICATION;
                                p->notificationTime = 0.0F;
                            }
                        }
                    }


                    // EXPORT BUTTON
                    {
                        float buttonW{ FooterSection.width * 0.09F };
                        float pad = 5;
                        Rectangle buttonExport{
                            FooterSection.x + FooterSection.width - ((FooterSection.width + buttonW) / 2),
                            FooterSection.y + FooterSection.height - ((FooterSection.height + FooterSection.height) / 2) + (pad * 1),
                            buttonW,
                            FooterSection.height - (pad * 2)
                        };
                        static bool isHover = false;
                        Color color{};
                        Color colorText{};

                        const Color colorPressed{ 52, 148,65, 255 };
                        const Color colorNormal{ 180, 180, 180, 255 };

                        if (CheckCollisionPointRec(p->mousePosition, buttonExport)) {
                            isHover = true;
                            color = colorPressed;
                            colorText = WHITE;

                            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                                p->g_exportPath = p->g_folderPath + p->g_inputTitle + p->g_format;
                                p->g_exporting = true;
                                p->notificationTime = 5.0F;
                            }
                        }
                        else {
                            //color = { 42, 128,55, 255 };
                            color = colorNormal;
                            colorText = BLACK;
                        }

                        DrawRectangleRounded(buttonExport, 0.3F, 10, color);

                        std::string text{ "EXPORT" };
                        DrawTextCustom(buttonExport, text, CENTER, 0.82F, 1.0F, p->fontGeneral, colorText);
                    }

                    // MADE BY UFTHaq
                    std::string text = "UFTHaq";
                    DrawTextCustom(FooterSection, text, RIGHT, 0.7F, 0.0F, p->fontGeneral, WHITE);
                    

                }
            }
        }
    }


}

void DrawDebug(std::vector<std::string>& data, Rectangle& paramRect)
{
    for (size_t j = 0; j < data.size(); j++) {
        Rectangle textRect{
            paramRect.x,
            paramRect.y + (j * paramRect.height / data.size()),
            paramRect.width,
            paramRect.height / data.size()
        };

        float pad = 2.0F;
        textRect = {
            textRect.x + (pad * 1),
            textRect.y + (pad * 1),
            textRect.width - (pad * 2),
            textRect.height - (pad * 2)
        };
        DrawRectangleRounded(textRect, 0.15F, 10, LIGHTGRAY);
        DrawTextCustom(textRect, data.at(j), CENTER, 0.5F, 1, BLACK);
    }
}

std::vector<std::string> ExtractRectValue(std::string header, Rectangle& PanelOutputImage)
{
    std::vector<std::string> vector{
        header,
        std::to_string((int)PanelOutputImage.x),
        std::to_string((int)PanelOutputImage.y),
        std::to_string((int)PanelOutputImage.width),
        std::to_string((int)PanelOutputImage.height),
    };

    return vector;
}

void redrawRenderTexture(Rectangle& DrawArea)
{
    int SSAA = 2;
    Rectangle liveViewArea = {
        DrawArea.x,
        DrawArea.y,
        DrawArea.width * SSAA,
        DrawArea.height * SSAA,
    };

    static bool firstSetup = true;
    if (firstSetup) {
        p->renderTextureLiveView = LoadRenderTexture((int)liveViewArea.width, (int)liveViewArea.height);
        firstSetup = false;
    }

    if (p->renderTextureLiveView.texture.height != 0) {
        UnloadRenderTexture(p->renderTextureLiveView);
        p->renderTextureLiveView = LoadRenderTexture((int)liveViewArea.width, (int)liveViewArea.height);
    }

    BeginTextureMode(p->renderTextureLiveView);
    //ClearBackground(p->ColorPanel);
    ClearBackground(p->ColorTitleBar);

    float tiles_w = liveViewArea.width / p->ImagePixels[0].size();
    float tiles_h = liveViewArea.height / p->ImagePixels.size();

    Rectangle tiles{};
    for (size_t y = 0; y < p->ImagePixels.size(); y++) {
        for (size_t x = 0; x < p->ImagePixels[y].size(); x++) {
            tiles = {
                liveViewArea.x + (x * tiles_w),
                liveViewArea.y + (y * tiles_h),
                tiles_w,
                tiles_h
            };

            float pad{};
            if (p->g_resolution == 1) pad = p->g_space * 0.5F * SSAA;
            else pad = p->g_space * 0.5F * SSAA * (p->g_resolution - 1);

            float corner = p->g_corner * 0.1F;
            Rectangle pixel = {
                (float)(tiles.x + (pad * 1)),
                (float)(tiles.y + (pad * 1)),
                (float)(tiles.width - (pad * 2)),
                (float)(tiles.height - (pad * 2)),
            };

            Color colorTile = p->ImagePixels[y][x];
            DrawRectangleRounded(pixel, corner, 10, colorTile);

            float luminance = 0.2126f * colorTile.r + 0.7152f * colorTile.g + 0.0722f * colorTile.b;

            Color textColor = (luminance > 128) ? BLACK : WHITE;

            size_t w = p->ImagePixels[y].size();
            size_t number = y * w + x;

            if (p->g_number == "CASUAL") {
                number = y * w + x + 1;
            }

            if (p->g_numbering) {
                std::string text = std::to_string(number);
                DrawTextCustom(pixel, text, CENTER, 0.65F, -0.5F, p->fontNumberLowRes, textColor);
            }

        }
    }
    EndTextureMode();
    SetTextureFilter(p->renderTextureLiveView.texture, TEXTURE_FILTER_BILINEAR);

    Image temporary = LoadImageFromTexture(p->renderTextureLiveView.texture);
    
    ImageResize(&temporary, int(liveViewArea.width / SSAA), int(liveViewArea.height / SSAA));

    if (p->textureLiveViewSSAA.height > 0) {
        UnloadTexture(p->textureLiveViewSSAA);
    }
    p->textureLiveViewSSAA = LoadTextureFromImage(temporary);
    SetTextureFilter(p->textureLiveViewSSAA, TEXTURE_FILTER_BILINEAR);

    UnloadImage(temporary);
}

ImageSize CalculateFlexibleImage()
{
    ImageSize imageOldSize = { (float)p->ImageInput.width, (float)p->ImageInput.height };

    int new_height = p->ImageInput.height;
    int new_width = int((float(new_height) / imageOldSize.h) * imageOldSize.w);
    //std::cout << "height: " << new_height << "\nwidth: " << new_width << std::endl;
    ImageSize imageNewSize = { (float)new_width, (float)new_height };

    return imageNewSize;
}

void InputTextBox(Rectangle& inputTitleBase)
{
    // INPUT BOX
    static bool inputBoxActive = false;
    static int framesCounter = 0;
    int maxInputChars = 12;

    if (CheckCollisionPointRec(p->mousePosition, inputTitleBase)) {
        SetMouseCursor(MOUSE_CURSOR_IBEAM);
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            inputBoxActive = true;
        }

    }
    else { SetMouseCursor(MOUSE_CURSOR_DEFAULT); }

    if (!CheckCollisionPointRec(p->mousePosition, inputTitleBase)) {
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            inputBoxActive = false;
        }
    }

    size_t letterSize = p->g_inputTitle.size();
    if (inputBoxActive) {

        framesCounter++;

        int key = GetKeyPressed();

        while (key > 0) {
            // Input small letters, numbers, and underscore
            if (((key >= 'A') && (key <= 'Z')) || ((key >= '0') && (key <= '9')) || (key == '_')) {
                if (letterSize < maxInputChars) {
                    p->g_inputTitle += static_cast<char>(key); // Directly append the character
                }
            }

            key = GetKeyPressed();
        }

        // Backspace delete chars
        if (IsKeyPressed(KEY_BACKSPACE) && !p->g_inputTitle.empty()) {
            p->g_inputTitle.pop_back();
        }
        else if (IsKeyDown(KEY_BACKSPACE) && !p->g_inputTitle.empty()) {
            static int timeDown_Backspace = 0;
            timeDown_Backspace++;
            if (((timeDown_Backspace) % 15) == 0) p->g_inputTitle.pop_back();
        }
    }
    else framesCounter = 0;

    if (inputBoxActive) {
        if (((framesCounter / 30) % 2) == 0) {
            // BLINKING CURSOR
            const char* text = "|";
            float fontSize = inputTitleBase.height * 0.8F;
            float fontSpace = 0.0F;
            Vector2 text_measure = MeasureTextEx(p->fontGeneral, p->g_inputTitle.c_str(), fontSize, fontSpace);
            Vector2 text_coor = {
                inputTitleBase.x + (text_measure.x * 1.1F) + inputTitleBase.height * 0.3F,
                inputTitleBase.y + ((inputTitleBase.height - text_measure.y) / 2) - 0
            };
            DrawTextEx(p->fontGeneral, text, text_coor, fontSize, fontSpace, BLACK);
        }
    }

    {
        std::string text{};
        if (p->g_inputTitle.empty() && !inputBoxActive) {
            text = "Click Here";
        }
        else {
            text = p->g_inputTitle;
        }
        DrawTextCustom(inputTitleBase, text, LEFT, 0.8F, 1.0F, BLACK);
    }
}

Rectangle FlexibleRectangle(Rectangle& BaseRect, float ObjectWidth, float ObjectHeight)
{
    Rectangle flexibleRect{};
    float baseRatio{ BaseRect.width / BaseRect.height };
    float objectRatio{ ObjectWidth / ObjectHeight };

    if (ObjectWidth == ObjectHeight) {
        if (BaseRect.height < BaseRect.width) {
            float h = BaseRect.height;
            float w = h;
            flexibleRect = {
                BaseRect.x + (BaseRect.width - w) / 2,
                BaseRect.y + (BaseRect.height - w) / 2,
                w,
                h
            };
        }
        else {
            float w = BaseRect.width;
            float h = w;
            flexibleRect = {
                BaseRect.x + (BaseRect.width - w) / 2,
                BaseRect.y + (BaseRect.height - w) / 2,
                w,
                h
            };
        }

    }
    else if (ObjectWidth > ObjectHeight) {

        float w = BaseRect.width;
        float h = ObjectHeight / ObjectWidth * BaseRect.width;
        flexibleRect = {
            BaseRect.x + (BaseRect.width - w) / 2,
            BaseRect.y + (BaseRect.height - h) / 2,
            w,
            h
        };
        objectRatio =  flexibleRect.width / flexibleRect.height;

        if (objectRatio < baseRatio) {
            float h = BaseRect.height;
            float w = ObjectWidth / ObjectHeight * BaseRect.height;
            flexibleRect = {
                BaseRect.x + (BaseRect.width - w) / 2,
                BaseRect.y + (BaseRect.height - h) / 2,
                w,
                h
            };
        }


    }
    else {
        float h = BaseRect.height;
        float w = ObjectWidth / ObjectHeight * BaseRect.height;
        flexibleRect = {
            BaseRect.x + (BaseRect.width - w) / 2,
            BaseRect.y + (BaseRect.height - h) / 2,
            w,
            h
        };
        objectRatio = flexibleRect.width / flexibleRect.height;

        if (objectRatio > baseRatio) {
            float w = BaseRect.width;
            float h = ObjectHeight / ObjectWidth * BaseRect.width;
            flexibleRect = {
                BaseRect.x + (BaseRect.width - w) / 2,
                BaseRect.y + (BaseRect.height - h) / 2,
                w,
                h
            };
            objectRatio = flexibleRect.width / flexibleRect.height;
        }
    }

    return flexibleRect;
}

void LoadSetup(int new_width, int new_height)
{
    // TODO: Resizing
    if (p->reload_setup)
    {
        int pixelate_range = p->g_pixelatedRange;
        int new_heightSize = (p->ImageInput.height) / pixelate_range;
        int new_widthSize = int((float(new_heightSize) / new_height) * new_width);

        UnloadTexture(p->texture_input);
        p->texture_input = LoadTextureFromImage(p->ImageInput);

        Image image_process = ImageCopy(p->ImageInput);

        UnloadImage(p->ImageOutput);
        p->ImageOutput = ImageCopy(image_process);

        UnloadImage(image_process);

        // Make it Smaller
        ImageResizeNN(&p->ImageOutput, new_widthSize, new_heightSize);

        int w = p->ImageOutput.width;
        int h = p->ImageOutput.height;

        Color* colorPointer = LoadImageColors(p->ImageOutput);

        std::vector<Color> smallPixelData{};
        for (size_t i = 0; i < h * w; i++) {
            smallPixelData.push_back(colorPointer[i]);
        }
        UnloadImageColors(colorPointer);

        std::cout << "Total Pixels: " << smallPixelData.size() << " -> [W:" << w << " x H:" << h << "]" << std::endl;

        std::vector<std::vector<Color>> matrixSmallImage(h, std::vector<Color>(w));

        for (size_t y = 0; y < h; y++) {
            for (size_t x = 0; x < w; x++) {
                matrixSmallImage[y][x] = smallPixelData[y * w + x];
            }
        }

        p->ImagePixels.clear();
        p->ImagePixels = matrixSmallImage;
        p->ImagePixels.shrink_to_fit();
        matrixSmallImage.clear();
        matrixSmallImage.shrink_to_fit();

        // Make it Normal size Again
        ImageResizeNN(&p->ImageOutput, (int)p->flexibleSize.w, (int)p->flexibleSize.h);

    }

    UnloadTexture(p->texture_output);
    //ImageFormat(&p->ImageOutput, PIXELFORMAT_UNCOMPRESSED_GRAYSCALE);
    p->texture_output = LoadTextureFromImage(p->ImageOutput);
}

void DrawTextCustom(Rectangle& panel, std::string text, int align, float size, float space, const Color color)
{
    float font_size = panel.height * size;
    float font_space = space;
    Vector2 text_measure = MeasureTextEx(p->fontGeneral, text.c_str(), font_size, font_space);
    Vector2 text_coor{};
    float margin = 0.25F;
    if (align == CENTER) {
        text_coor = {
            panel.x + (panel.width - text_measure.x) / 2,
            panel.y + (panel.height - text_measure.y) / 2
        };
    }
    else if (align == LEFT) {
        text_coor = {
            panel.x + panel.height * margin,
            panel.y + (panel.height - text_measure.y) / 2
        };
    }
    else if (align == RIGHT) {
        text_coor = {
            panel.x + panel.width - (text_measure.x + panel.height * margin),
            panel.y + (panel.height - text_measure.y) / 2
        };
    }

    DrawTextEx(p->fontGeneral, text.c_str(), text_coor, font_size, font_space, color);
}

void DrawTextCustom(Rectangle& panel, std::string text, int align, float size, float space, const Font& font, const Color color)
{
    float font_size = panel.height * size;
    float font_space = space;
    Vector2 text_measure = MeasureTextEx(p->fontGeneral, text.c_str(), font_size, font_space);
    Vector2 text_coor{};
    float margin = 0.25F;
    if (align == CENTER) {
        text_coor = {
            panel.x + (panel.width - text_measure.x) / 2,
            panel.y + (panel.height - text_measure.y) / 2
        };
    }
    else if (align == LEFT) {
        text_coor = {
            panel.x + panel.height * margin,
            panel.y + (panel.height - text_measure.y) / 2
        };
    }
    else if (align == RIGHT) {
        text_coor = {
            panel.x + panel.width - (text_measure.x + panel.height * margin),
            panel.y + (panel.height - text_measure.y) / 2
        };
    }

    DrawTextEx(font, text.c_str(), text_coor, font_size, font_space, color);
}

void DrawNotification(Rectangle& panel, std::string text, int align, float size, float space, const Color color, const Color fillColor)
{
    float font_size = panel.height * size;
    float font_space = space;
    Vector2 text_measure = MeasureTextEx(p->fontGeneral, text.c_str(), font_size, font_space);
    Vector2 text_coor{};
    float margin = 0.25F;
    if (align == CENTER) {
        text_coor = {
            panel.x + (panel.width - text_measure.x) / 2,
            panel.y + (panel.height - text_measure.y) / 2
        };
    }
    else if (align == LEFT) {
        text_coor = {
            panel.x + panel.height * margin,
            panel.y + (panel.height - text_measure.y) / 2
        };
    }
    else if (align == RIGHT) {
        text_coor = {
            panel.x + panel.width - (text_measure.x + panel.height * margin),
            panel.y + (panel.height - text_measure.y) / 2
        };
    }
    Rectangle fillRect = {
        panel.x,
        panel.y,
        text_measure.x + (panel.height * 0.3F * 2),
        panel.height
    };
    DrawRectangleRounded(fillRect, 0.2F, 10, fillColor);
    DrawTextEx(p->fontGeneral, text.c_str(), text_coor, font_size, font_space, color);
}

void InitializedFont(void)
{
    p->fontGeneral = LoadFontEx(FONT_LOC_Sofia_Sans_Condensed_MED, 40, 0, 0);
    SetTextureFilter(p->fontGeneral.texture, TEXTURE_FILTER_BILINEAR);

    p->fontNumberLowRes = LoadFontEx(FONT_LOC_Sofia_Sans_Condensed_MED, 40, 0, 0);
    SetTextureFilter(p->fontNumberLowRes.texture, TEXTURE_FILTER_BILINEAR);

    p->fontNumberHighRes = LoadFontEx(FONT_LOC_Sofia_Sans_Condensed_MED, 100, 0, 0);
    SetTextureFilter(p->fontNumberHighRes.texture, TEXTURE_FILTER_BILINEAR);
}

void InitializedIcons(void)
{

    p->TexZEN = LoadTexture(ICON_ZEN);
    SetTextureFilter(p->TexZEN, TEXTURE_FILTER_BILINEAR);

    p->TexExit = LoadTexture(ICON_EXIT);
    SetTextureFilter(p->TexExit, TEXTURE_FILTER_BILINEAR);
}



// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
