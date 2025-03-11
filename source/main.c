#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <3ds.h>
#include <citro2d.h>

C2D_TextBuf g_staticBuf;
C2D_Text g_staticText[1];
C2D_Text g_bottomText;
C2D_Font font[1];

// White, Black, Blue, Green, Red, Cyan, Magenta, Yellow
u32 colors[] = {0xFFFFFFFF, 0xFF000000, 0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFF00, 0xFFFF00FF, 0xFF00FFFF};

int colorIndex = 0;
bool showText = true;

static u32 getTextColor(u32 bgColor)
{
    if (bgColor == 0xFF000000) // Bg is black
    {
        return 0xFFFFFFFF; // White text
    }
    else
    {
        return 0xFF000000; // Black text
    }
}

static void sceneInit(void)
{
    g_staticBuf  = C2D_TextBufNew(4096); // support up to 4096 glyphs in the buffer
    font[0] = C2D_FontLoadSystem(CFG_REGION_USA); // Load the default font

    // Parse the bottom screen text
    C2D_TextFontParse(&g_bottomText, font[0], g_staticBuf, "Classyham's Pixel Checker\nA - Next\nB - Previous\nSelect - Toggle Text\nStart - Close");

    // Optimize the text strings
    C2D_TextOptimize(&g_bottomText);
}

static void sceneRenderBottom()
{
    if (showText)
    {
        // Draw bottom screen text centered
        float textHeight =  (30*5.0f)/2.0f; // Thic sucks but it's the simplist way
        float posX = (320.0f ) / 2.0f;
        float posY = (240.0f - textHeight) / 2.0f; // Adjusted to use text height
        C2D_DrawText(&g_bottomText, C2D_AlignCenter | C2D_WithColor, posX, posY, 0.5f, 0.5, 0.5, getTextColor(colors[colorIndex]));
    }
}

static void sceneExit(void)
{
    // Delete the text buffers
    C2D_TextBufDelete(g_staticBuf);
    C2D_FontFree(font[0]);
}

int main()
{
    romfsInit();
    cfguInit(); // Allow C2D_FontLoadSystem to work
    // Initialize the libs
    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();

    // Create screens
    C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    C3D_RenderTarget* bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

    // Initialize the scene
    sceneInit();

    // Main loop
    while (aptMainLoop())
    {
        hidScanInput();

        // Respond to user input
        u32 kDown = hidKeysDown();
        if (kDown & KEY_START)
            break; // break in order to return to hbmenu
        if (kDown & KEY_A)
            colorIndex = (colorIndex + 1) % 8; // Cycle forward through colors
        if (kDown & KEY_B)
            colorIndex = (colorIndex - 1 + 8) % 8; // Cycle backward through colors
        if (kDown & KEY_SELECT)
            showText = !showText; // Toggle text rendering

        // Render the scene
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C2D_TargetClear(top, colors[colorIndex]);
        C2D_SceneBegin(top);

        C2D_TargetClear(bottom, colors[colorIndex]);
        C2D_SceneBegin(bottom);
        sceneRenderBottom();
        C3D_FrameEnd(0);
    }

    // Deinitialize the scene
    sceneExit();

    // Deinitialize the libs
    C2D_Fini();
    C3D_Fini();
    romfsExit();
    cfguExit();
    gfxExit();
    return 0;
}
