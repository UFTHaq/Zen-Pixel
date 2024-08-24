// ZenPixel.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Title  : ZEN PIXEL
// Author : UFTHaq
// Date   : 09/07/2024
//

#include <iostream>
#include <vector>
#include <string>

#include <raylib.h>
#include "ZenPixel.h"

#define FONT_LOC_Sofia_Sans_Condensed_MED       {"resources/Fonts/Sofia_Sans_Condensed/static/SofiaSansCondensed-Medium.ttf"}
#define ICON_ZEN                                {"resources/Icons/Zen.png"}
#define ICON_EXIT                               {"resources/Icons/Exit.png"}

struct RectSize {
    float w{ 1600 };    // IF i put 1920, or fullscreen, then it will be broken, not transparent anymore, and the FLAG_WINDOW_FULLSCREEN do the same.
    float h{ 910 };
};

struct ImageSize {
    float w{};
    float h{};
};

struct Plug {
    std::string AppTitle{ "ZEN PIXEL" };
    RectSize Screen{};
    Vector2 mousePosition{};
    //Color ColorLayer1    { 40,40,40, 90 };
    Color ColorLayer1    { BLANK };
    Color ColorLayer2    { 25,33,50,255 };
    Color ColorTitleBar  { 18,24,36,255 };
    Font font{};
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
    bool reload_setup{ true };
    ImageSize flexibleSize{};
    std::vector<std::vector<Color>> ImagePixels{};
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
void DrawTextCustom(Rectangle& panel, std::string& text, int align, float size, float space, const Color color);
void LoadSetup(int new_width, int new_height);
void UpdateDraw();

int main()
{
    SetConfigFlags(FLAG_WINDOW_ALWAYS_RUN);
    //SetConfigFlags(FLAG_WINDOW_TOPMOST);
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    SetConfigFlags(FLAG_WINDOW_TRANSPARENT);
    InitWindow(p->Screen.w, p->Screen.h, p->AppTitle.c_str());
    SetTargetFPS(60);

    InitializedFont();
    InitializedIcons();

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(p->ColorLayer1);

        UpdateDraw();

        EndDrawing();

        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyDown(KEY_S) && IsKeyPressed(KEY_C)) {
            Image SS = LoadImageFromScreen();
            ExportImage(SS, "SS.png");

            ImageCrop(&SS, p->flexible_panel_crop);
            ExportImage(SS, "SS-normal.png");

            Image SS_resize = ImageCopy(SS);
            ImageResize(&SS_resize, int(p->flexible_panel_crop.width * 2), int(p->flexible_panel_crop.height * 2));
            ExportImage(SS_resize, "SS_resize.png");

            Image SS_resizeNN = ImageCopy(SS);
            ImageResize(&SS_resizeNN, int(p->flexible_panel_crop.width * 2), int(p->flexible_panel_crop.height * 2));
            ExportImage(SS_resizeNN, "SS_resizeNN.png");
        }
    }
    CloseWindow();
}

void UpdateDraw()
{
    p->mousePosition = GetMousePosition();

    RectSize BaseApp{ 1500, 900 };
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
        DrawTextCustom(TitleBarRect, text, CENTER, 0.7F, 1.0F,WHITE);

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
            if (CheckCollisionPointRec(p->mousePosition, dest))
            {
                color = RED;
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    CloseWindow();
                }
            }
            else {
                color = LIGHTGRAY;
            }

            DrawRectangleRounded(dest, 0.2F, 10, color);
            DrawTexturePro(p->TexExit, source, dest, {}, 0, WHITE);
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

        RectSize header{ PanelBase.width, PanelBase.height * 0.05F };
        Rectangle PanelHeader{
            PanelBase.x,
            PanelBase.y,
            header.w,
            header.h,
        };
        DrawRectangleLinesEx(PanelHeader, 0.5F, WHITE);

        // HEADER
        {
            std::string text{ "DRAG DROP YOUR IMAGE" };
            DrawTextCustom(PanelHeader, text, CENTER, 0.8F, 1.0F, WHITE);
        }

        RectSize space{ header.w, PanelBase.height * 0.020F };
        RectSize section1{ header.w, PanelBase.height * 0.7F };
        Rectangle PanelSection1{
            PanelHeader.x,
            PanelHeader.y + PanelHeader.height + space.h,
            section1.w,
            section1.h,
        };
        DrawRectangleLinesEx(PanelSection1, 0.5F, WHITE);

        RectSize footer{ header.w, PanelBase.height * (1.0F - 0.05F - (0.020F * 2) - 0.7F) };
        Rectangle PanelFooter{
            PanelSection1.x,
            PanelSection1.y + PanelSection1.height + space.h,
            footer.w,
            footer.h,
        };
        DrawRectangleLinesEx(PanelFooter, 0.5F, WHITE);


        // SECTION 1
        {
            Rectangle& panel = PanelSection1;
            float space = 0.01F * panel.width;
            Rectangle rect_l{
                panel.x,
                panel.y,
                panel.width / 2 - space,
                panel.height
            };
            DrawRectangleLinesEx(rect_l, 0.5F, WHITE);

            Rectangle rect_r{
                rect_l.x + rect_l.width + space + space,
                rect_l.y,
                rect_l.width,
                rect_l.height
            };
            DrawRectangleLinesEx(rect_r, 0.5F, WHITE);

            // SECTION 1 LEFT
            {
                float name_space = p->LabelSize;
                Rectangle panel_input_image_base{
                    rect_l.x,
                    rect_l.y,
                    rect_l.width,
                    rect_l.height - name_space
                };
                DrawRectangleLinesEx(panel_input_image_base, 0.5F, WHITE);

                Rectangle panel_input_image_name{
                    panel_input_image_base.x,
                    panel_input_image_base.y + panel_input_image_base.height,
                    panel_input_image_base.width,
                    name_space
                };
                DrawRectangleLinesEx(panel_input_image_name, 0.5F, WHITE);

                {
                    // INPUT IMAGE TEXT
                    std::string text = "INPUT IMAGE";
                    DrawTextCustom(panel_input_image_name, text, CENTER, 0.9F, 1.0F, WHITE);
                }

                if (IsFileDropped()) {
                    FilePathList dropped_file = LoadDroppedFiles();

                    const char* c_file_path = dropped_file.paths[0];
                    std::string cpp_file_path = std::string(c_file_path);

                    if (IsFileExtension(c_file_path, ".png")) // I dont know why jpg doesnt work.
                    {
                        p->ImageInput = LoadImage(c_file_path);
                        ImageSize imageOldSize = { (float)p->ImageInput.width, (float)p->ImageInput.height };

                        //int new_height = 40;
                        int new_height = p->ImageInput.height;
                        int new_width = int((float(new_height) / imageOldSize.h) * imageOldSize.w);
                        std::cout << "height: " << new_height << "\nwidth: " << new_width << std::endl;
                        ImageSize imageNewSize = { (float)new_width, (float)new_height };
                        p->flexibleSize = imageNewSize;

                        if (imageNewSize.w > imageNewSize.h) {
                            float w = panel_input_image_base.width;
                            float h = imageNewSize.h / imageNewSize.w * panel_input_image_base.width;
                            p->flexible_panel_input = {
                                panel_input_image_base.x + (panel_input_image_base.width - w) / 2,
                                panel_input_image_base.y + (panel_input_image_base.height - h) / 2,
                                w,
                                h
                            };
                        }
                        else {
                            float h = panel_input_image_base.height;
                            float w = imageNewSize.w / imageNewSize.h * panel_input_image_base.height;
                            p->flexible_panel_input = {
                                panel_input_image_base.x + (panel_input_image_base.width - w) / 2,
                                panel_input_image_base.y + (panel_input_image_base.height - h) / 2,
                                w,
                                h
                            };
                        }
                    }
                    // I dont know why jpg doesnt work.
                    else if (IsFileExtension(c_file_path, "jpg") || IsFileExtension(c_file_path, "jpeg")) {

                    }

                    p->reload_setup = true;

                    UnloadDroppedFiles(dropped_file);
                }

                if (p->reload_setup) {
                    LoadSetup((int)p->flexibleSize.w, (int)p->flexibleSize.h);
                    p->reload_setup = false;
                }

                if (p->texture_input.height != 0) {
                    // Draw input
                    {
                        Rectangle source{
                            0,0,(float)p->texture_input.width, (float)p->texture_input.height
                        };
                        Rectangle dest{ p->flexible_panel_input };
                        DrawTexturePro(p->texture_input, source, dest, { 0,0 }, 0, WHITE);
                    }
                }

                DrawRectangleLinesEx(p->flexible_panel_input, 0.5F, WHITE);

                if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S)) {
                    ExportImage(p->ImageOutput, "output.png");
                }

            }

            // SECTION 1 RIGHT
            {
                float name_space = p->LabelSize;
                Rectangle panel_output_image_base{
                    rect_r.x,
                    rect_r.y,
                    rect_r.width,
                    rect_r.height - name_space
                };
                DrawRectangleLinesEx(panel_output_image_base, 0.5F, WHITE);

                Rectangle panel_output_image_name{
                    panel_output_image_base.x,
                    panel_output_image_base.y + panel_output_image_base.height,
                    panel_output_image_base.width,
                    name_space
                };
                DrawRectangleLinesEx(panel_output_image_name, 0.5F, WHITE);

                {
                    // INPUT IMAGE TEXT
                    std::string text = "OUTPUT IMAGE";
                    DrawTextCustom(panel_output_image_name, text, CENTER, 0.9F, 1.0F, WHITE);
                }

                p->flexible_panel_output = {
                    panel_output_image_base.x + (panel_output_image_base.width - p->flexible_panel_input.width) / 2,
                    p->flexible_panel_input.y,
                    p->flexible_panel_input.width,
                    p->flexible_panel_input.height
                };

                p->flexible_panel_crop = {
                    p->flexible_panel_output.x,
                    p->flexible_panel_output.y,
                    p->flexible_panel_output.width + 1,
                    p->flexible_panel_output.height + 1,
                };

                if (p->texture_input.height != 0) {
                    // Draw output
                    {
                        Rectangle source{
                            0,0,(float)p->texture_output.width, (float)p->texture_output.height
                        };
                        Rectangle dest{ p->flexible_panel_output };
                        //DrawTexturePro(p->texture_output, source, dest, { 0,0 }, 0, WHITE);
                    }
                }

                DrawRectangleLinesEx(p->flexible_panel_output, 0.5F, WHITE);

                float pad = 2;
                Rectangle pixelDrawArea = {
                    p->flexible_panel_output.x + (pad * 1),
                    p->flexible_panel_output.y + (pad * 1),
                    p->flexible_panel_output.width - (pad * 2),
                    p->flexible_panel_output.height - (pad * 2),
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

                            float pad = 0.75F;
                            Rectangle pixel = {
                                tiles.x + (pad * 1),
                                tiles.y + (pad * 1),
                                tiles.width - (pad * 2),
                                tiles.height - (pad * 2),
                            };

                            Color colorTile = p->ImagePixels[y][x];
                            DrawRectangleRec(pixel, colorTile);
                            //DrawRectangleRounded(pixel, 0.6F, 10, colorTile);

                            float luminance = 0.2126f * colorTile.r + 0.7152f * colorTile.g + 0.0722f * colorTile.b;

                            //Color textColor = (luminance > 140) ? BLACK : WHITE;
                            Color textColor = (luminance > 128) ? BLACK : WHITE;

                            int w = p->ImagePixels[y].size();
                            int number = y * w + x;

                            std::string text = std::to_string(number);
                            DrawTextCustom(pixel, text, CENTER, 0.75F, -0.5F, textColor);
                            
                        }
                    }

                }

            }

        }



    }


}

void LoadSetup(int new_width, int new_height)
{
    // TODO: Resizing
    if (p->reload_setup)
    {
        int pixelate_range = 25;
        int new_heightSize = (p->ImageInput.height) / pixelate_range;
        int new_widthSize = int((float(new_heightSize) / new_height) * new_width);

        p->texture_input = LoadTextureFromImage(p->ImageInput);

        Image image_process = ImageCopy(p->ImageInput);

        p->ImageOutput = ImageCopy(image_process);

        // TODO:
        // Pixelation will use rectangle and padding
        // Steps:
        // 1. Make image small to the wanted pixel size. and store the image matrix size.
        // 2. Take data color  from it, put in vector.
        // 3. Make vector matrix Color.
        // 4. Make the matrix rectangle in output rectangle.
        // 5. ez pz.
        //

        // Make it Smaller
        ImageResizeNN(&p->ImageOutput, new_widthSize, new_heightSize);

        int w = p->ImageOutput.width;
        int h = p->ImageOutput.height;

        Color* colorPointer = LoadImageColors(p->ImageOutput);

        std::vector<Color> smallPixelData{};
        for (size_t i = 0; i < h * w; i++) {
            smallPixelData.push_back(colorPointer[i]);
        }

        std::cout << smallPixelData.size() << std::endl;

        for (auto& i : smallPixelData) {
            //std::cout << (uint16_t)i.r << " ";
        }

        std::vector<std::vector<Color>> matrixSmallImage(h, std::vector<Color>(w));

        for (size_t y = 0; y < h; y++) {
            for (size_t x = 0; x < w; x++) {
                matrixSmallImage[y][x] = smallPixelData[y * w + x];
            }
        }

        p->ImagePixels = matrixSmallImage;

        // Make it Normal size Again
        ImageResizeNN(&p->ImageOutput, p->flexibleSize.w, p->flexibleSize.h);

    }

    //ImageFormat(&p->ImageOutput, PIXELFORMAT_UNCOMPRESSED_GRAYSCALE);
    p->texture_output = LoadTextureFromImage(p->ImageOutput);
}

void DrawTextCustom(Rectangle& panel, std::string& text, int align, float size, float space, const Color color)
{
    float font_size = panel.height * size;
    float font_space = space;
    Vector2 text_measure = MeasureTextEx(p->font, text.c_str(), font_size, font_space);
    Vector2 text_coor{};
    if (align == CENTER) {
        text_coor = {
            panel.x + (panel.width - text_measure.x) / 2,
            panel.y + (panel.height - text_measure.y) / 2
        };
    }
    else if (align == LEFT) {
        text_coor = {
            panel.x + panel.height * 0.3F,
            panel.y + (panel.height - text_measure.y) / 2
        };
    }
    else if (align == RIGHT) {
        text_coor = {
            panel.x + panel.width - (text_measure.x + panel.height * 0.3F),
            panel.y + (panel.height - text_measure.y) / 2
        };
    }
    Rectangle fillRect = {
        panel.x,
        panel.y,
        text_measure.x + (panel.height * 0.3F * 2),
        panel.height
    };
    DrawTextEx(p->font, text.c_str(), text_coor, font_size, font_space, color);
}

void InitializedFont(void)
{
    p->font = LoadFontEx(FONT_LOC_Sofia_Sans_Condensed_MED, 40, 0, 0);
    SetTextureFilter(p->font.texture, TEXTURE_FILTER_BILINEAR);
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
