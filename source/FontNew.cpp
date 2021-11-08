#include "VHud.h"
#include "FontNew.h"
#include "TextureMgr.h"
#include "Utility.h"
#include "HudNew.h"
#include "PadNew.h"
#include "HudColoursNew.h"

#include "CSprite2d.h"
#include "CFont.h"

using namespace plugin;

CFontNew FontNew;

bool CFontNew::bInitialised;
CSprite2d* CFontNew::Sprite[NUM_FONTS];
CFontDetailsNew CFontNew::Details;
char CFontNew::Size[NUM_FONTS][160];
bool CFontNew::bNewLine;
int CFontNew::NumLines;
CSprite2d* CFontNew::PS2Symbol;
CVector CFontNew::PS2SymbolScale;

CSprite2d* CFontNew::ButtonSprite[NUM_BUTTONS];

char* ButtonFileName[] = {
    "none",
    "up",
    "down",
    "left",
    "right",
    "cross",
    "circle",
    "square",
    "triangle",
    "l1",
    "l2",
    "l3",
    "r1",
    "r2",
    "r3",
    "start",
    "select",
    "dud",
    "dlr",
    "thumbl",
    "thumblx",
    "thumblxl",
    "thumblxr",
    "thumbly",
    "thumblyu",
    "thumblyd",
    "thumbr",
    "thumbrx",
    "thumbrxl",
    "thumbrxr",
    "thumbry",
    "thumbryu",
    "thumbryd",
    "pc_up",
    "pc_down",
    "pc_left",
    "pc_right",
    "alt",
    "backspace",
    "caps",
    "del",
    "down",
    "end",
    "enter",
    "esc",
    "f1",
    "f2",
    "f3",
    "f4",
    "f5",
    "f6",
    "f7",
    "f8",
    "f9",
    "f10",
    "f11",
    "f12",
    "home",
    "ins",
    "l",
    "lalt",
    "lctrl",
    "left",
    "lwin",
    "menu",
    "numlock",
    "pause",
    "pgdown",
    "pgup",
    "printscreen",
    "ralt",
    "rctrl",
    "right",
    "rwin",
    "scrolllock",
    "shift",
    "spacebar",
    "up",
};

// Custom GInput action strings
const char* CustomGInputActions[] = {
#if 0
                "VEHICLE_FIREWEAPON",
                "VEHICLE_TURRET_LEFT_RIGHT",
                "VEHICLE_TURRET_UP_DOWN",
                "GO_LEFTRIGHT",
                "GO_UPDOWN",
                "MELEE_ATTACK",
                "BLOW_UP_RC_BUGGY",
#else
                "PED_MOVE",
                "BMX_HANDBRAKE",
                "BMX_BUNNYHOP",
                "CAMERA_LEFT_RIGHT",
                "CAMERA_UP_DOWN",
                "VEHICLE_CHANGE_RADIO_STATION",
                "GO_LEFTRIGHT",
                "GO_UPDOWN",
                "SNATCH_PACKAGE",
                "HYDRA_TARGET",
#endif
};

CFontNew::CFontNew() {

}

void CFontNew::Init() {
    if (bInitialised)
        return;

    for (int i = 0; i < NUM_FONTS; i++) {
        char str[16];
        sprintf(str, "font%d", i + 1);

        Sprite[i] = new CSprite2d();
        Sprite[i]->m_pTexture = CTextureMgr::LoadDDSTextureCB(PLUGIN_PATH("VHud\\fonts"), str);
    }

    char* path = "VHud\\buttons\\xbox";
    for (int i = 0; i < NUM_BUTTONS; i++) {
        ButtonSprite[i] = new CSprite2d();

        if (i > BUTTON_THUMBRYD)
            path = "VHud\\buttons\\pc";

        ButtonSprite[i]->m_pTexture = CTextureMgr::LoadPNGTextureCB(PLUGIN_PATH(path), ButtonFileName[i]);
    }
    ReadValuesFromFile();

    Clear();

    bInitialised = true;
}

void CFontNew::ReadValuesFromFile() {
    std::ifstream file(PLUGIN_PATH("VHud\\data\\fonts.dat"));

    int fontId = 0;
    int index = 0;
    if (file.is_open()) {
        for (std::string line; getline(file, line);) {
            if (!line[0] || line[0] == '#')
                continue;

            char name[64];
            sscanf(line.c_str(), "%s", &name);

            if (strcmp(name, "[FONT_ID]") == 0) {
                index = 0;
                getline(file, line);
                sscanf(line.c_str(), "%d", &fontId);
            }
            else if (strcmp(name, "[PROP]") == 0) {
                for (line; getline(file, line);) {
                    sscanf(line.c_str(), "%s", &name);

                    if (strcmp(name, "[\\PROP]") == 0) {
                        index = 0;
                        break;
                    }

                    int value[16];
                    sscanf(line.c_str(), "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
                        &value[0], &value[1], &value[2], &value[3], &value[4], &value[5], &value[6], &value[7],
                        &value[8], &value[9], &value[10], &value[11], &value[12], &value[13], &value[14], &value[15]);

                    for (int i = 0; i < 16; i++) {
                        Size[fontId][index] = value[i];
                        index++;
                    }
                    if (index > 160)
                        exit(0);
                }
            }
            else if (strcmp(name, "[\\PROP]") == 0) {
                continue;
            }
        }

        file.close();
    }
}

void CFontNew::Clear() {
    SetBackground(false);
    SetGradBackground(false);
    SetBackgroundColor(CRGBA(0, 0, 0, 255));
    SetBackgroundBorder(CRect(0, 0, 0, 0));
    SetClipX(SCREEN_WIDTH);
    SetClipX(-1);
    SetWrapX(SCREEN_WIDTH);
    SetAlignment(ALIGN_LEFT);
    SetFontStyle(FONT_1);
    SetColor(CRGBA(255, 255, 255, 255));
    SetDropShadow(0.0f);
    SetDropColor(CRGBA(0, 0, 0, 255));
    SetScale(1.0f, 1.0f);
    NumLines = 0;
    bNewLine = false;
    PS2Symbol = NULL;
    SetTokenToIgnore(NULL, NULL);
}

void CFontNew::Shutdown() {
    if (!bInitialised)
        return;

    for (int i = 0; i < NUM_FONTS; i++) {
        if (Sprite[i]) {
            Sprite[i]->Delete();
            delete Sprite[i];
        }
    }

    for (int i = 0; i < NUM_BUTTONS; i++) {
        if (ButtonSprite[i]) {
            ButtonSprite[i]->Delete();
            delete ButtonSprite[i];
        }
    }

    bInitialised = false;
}

float CFontNew::GetCharacterSize(char c) {
    return Size[Details.style][c] * Details.scale.x;
}

float CFontNew::GetStringWidth(char* s, bool spaces) {
    float w;

    w = 0.0f;
    for (; (*s != ' ' || spaces) && *s != '\0'; s++) {
        if (*s == '~') {
            s++;
            while (*s != '~') s++;
            s++;
            if (*s == ' ' && !spaces)
                break;
        }
        w += GetCharacterSize(*s - ' ');
    }
    return w;
}

char* CFontNew::GetNextSpace(char* s) {
    for (; *s != ' ' && *s != '\0'; s++)
        if (*s == '~') {
            s++;
            while (*s != '~') s++;
        }

    return s;
}

void CFontNew::SetTokenToIgnore(char t1, char t2) {
    Details.ignoreTokens[0] = t1;
    Details.ignoreTokens[1] = t2;
}

int CFontNew::PrintString(float x, float y, char* s) {
    if (*s != '*') {
        if (Details.background) {
            CRect rect;
            x += Details.backgroundBorder.left;
            y += Details.backgroundBorder.top;
            GetTextRect(&rect, x, y, s);
            if (Details.grad)
                CHudNew::DrawSimpleRectGrad(rect, Details.backgroundColor);
            else
                CSprite2d::DrawRect(rect, Details.backgroundColor);
        }
        return GetNumberLines(true, x, y, s);
    }
}

int CFontNew::GetNumberLines(bool print, float xstart, float ystart, char* s) {
    float x = xstart;
    float y = ystart;
    int n = 1;
    int letterCount = 0;

    if (Details.alignment == ALIGN_CENTER) {
        bool first = true;
        char* start = s;
        char* t = s;
        float length = 0.0f;
        int space = 0;

        if (Details.alignment == ALIGN_CENTER || Details.alignment == ALIGN_RIGHT)
            x = 0.0f;

        while (s) {
            while (s) {
                if (*s == '\0')
                    return n;

                float w = Details.alignment == ALIGN_CENTER ? Details.wrapX : Details.alignment == ALIGN_RIGHT ? xstart - Details.wrapX : xstart + Details.wrapX;

                if (x + GetStringWidth(s) > w && !first && !bNewLine) {
                    float sw = Details.alignment == ALIGN_LEFT || Details.alignment == ALIGN_CENTER ? 0.0f : (Details.wrapX - length) / space;
                    float cx = Details.alignment == ALIGN_CENTER ? xstart - x / 2 : Details.alignment == ALIGN_RIGHT ? xstart - x : xstart;
                    PrintString(print, cx, y, start, s, sw);

                    if (Details.alignment == ALIGN_CENTER || Details.alignment == ALIGN_RIGHT)
                        x = 0.0f;
                    else
                        x = xstart;

                    y += GetHeightScale(Details.scale.y);
                    start = s;

                    length = 0.0f;
                    space = 0;
                    first = true;
                    n++;
                }

                if (bNewLine && !first) {
                    y += GetHeightScale(Details.scale.y);
                    start = s;

                    length = 0.0f;
                    space = 0;
                    first = true;
                    bNewLine = false;
                }

                t = GetNextSpace(s);
                if (t[0] == '\0' || t[0] == ' ' && t[1] == '\0') {
                    break;
                }

                if (!first)
                    space++;

                first = false;
                x += GetStringWidth(s) + GetCharacterSize(*t - ' ');

                length = x;
                s = t + 1;
            }

            if (t[0] == ' ' && t[1] == '\0') {
                t[0] = '\0';
            }

            x += GetStringWidth(s);
            s = t;
            float cx = Details.alignment == ALIGN_CENTER ? xstart - x / 2 : Details.alignment == ALIGN_RIGHT ? xstart - x : xstart;
            PrintString(print, cx, y, start, s, 0.0f);
        }

        bNewLine = false;
        return n;
    }
    else if (Details.alignment == ALIGN_RIGHT) {
        x -= GetStringWidth(s, true);
    }

    for (s; *s != '\0'; s++) {
        if (*s == '~')
            s = ParseToken(s);

        char c;
        c = *s - ' ';

        float f = Details.alignment == ALIGN_CENTER ? Details.wrapX : xstart + Details.wrapX;

        if ((Details.clipXCount == -1 && (x + GetCharacterSize(c) > xstart + Details.clipX)))
            break;
        else if (Details.clipXCount != -1 && letterCount >= Details.clipXCount)
            break;

        if (x + GetStringWidth(s) > f || bNewLine) {
            x = xstart;
            y += 32.0f * Details.scale.y * 0.5f + 2.0f * Details.scale.y;
            bNewLine = false;
            n++;
        }

        if (print)
            PrintChar(x, y, c);

        letterCount++;
        x += GetCharacterSize(c);
    }

    return n;
}

void CFontNew::PrintString(bool print, float x, float y, char* start, char* end, float spwidth) {
    char* s, c;

    float xstart = x;
    for (s = start; s < end; s++) {
        int n = NumLines;

        if (*s == '~')
            s = ParseToken(s);

        if (n != NumLines) {
            x = xstart;
            y += GetHeightScale(Details.scale.y);
        }

        c = *s - ' ';
        float sp = GetCharacterSize(c);

        if (print)
            PrintChar(x, y, c);

        x += sp;

        if (c == 0)
            x += spwidth;
    }
}

char* CFontNew::ParseToken(char* s) {
    char* c = s + 1;
    int a = Details.color.a;

    if (Details.ignoreTokens[0] != *c && Details.ignoreTokens[1] != *c) {
        switch (*c) {
        case '<':
            PS2Symbol = ButtonSprite[BUTTON_LEFT];
            break;
        case '>':
            PS2Symbol = ButtonSprite[BUTTON_RIGHT];
            break;
        case 'A':
        case 'a':
            PS2Symbol = ButtonSprite[BUTTON_L3];
            break;
        case 'B':
        case 'b':
            SetColor(HudColourNew.GetRGB(HUD_COLOUR_BLUE, a));
            break;
        case 'C':
        case 'c':
            PS2Symbol = ButtonSprite[BUTTON_R3];
            break;
        case 'D':
        case 'd':
            PS2Symbol = ButtonSprite[BUTTON_DOWN];
            break;
        case 'G':
        case 'g':
            SetColor(HudColourNew.GetRGB(HUD_COLOUR_GREEN, a));
            break;
        case 'H':
        case 'h':
            SetColor(HudColourNew.GetRGB(HUD_COLOUR_WHITE, a));
            break;
        case 'J':
        case 'j':
            PS2Symbol = ButtonSprite[BUTTON_R1];
            break;
        case 'K':
        case 'k':
            PS2Symbol = ButtonSprite[BUTTON_L1];
            break;
        case 'M':
        case 'm':
            PS2Symbol = ButtonSprite[BUTTON_L2];
            break;
        case 'N':
        case 'n':
            bNewLine = true;
            NumLines++;
            break;
        case 'O':
        case 'o':
            PS2Symbol = ButtonSprite[BUTTON_CIRCLE];
            break;
        case 'P':
        case 'p':
            SetColor(HudColourNew.GetRGB(HUD_COLOUR_PURPLE, a));
            break;
        case 'Q':
        case 'q':
            PS2Symbol = bHasPadInHands ? ButtonSprite[BUTTON_SQUARE] : ButtonSprite[BUTTON_PC_SPACEBAR];
            break;
        case 'R':
        case 'r':
            SetColor(HudColourNew.GetRGB(HUD_COLOUR_RED, a));
            break;
        case 'S':
        case 's':
            SetColor(HudColourNew.GetRGB(HUD_COLOUR_WHITE, a));
            break;
        case 'T':
        case 't':
            PS2Symbol = bHasPadInHands ? ButtonSprite[BUTTON_TRIANGLE] : ButtonSprite[BUTTON_PC_ESC];
            break;
        case 'U':
        case 'u':
            if (c[1] == 'd' || c[1] == 'D') {
                PS2Symbol = ButtonSprite[BUTTON_UPDOWN];
                c++;
            }
            else
                PS2Symbol = ButtonSprite[BUTTON_UP];
            break;
        case 'V':
        case 'v':
            PS2Symbol = ButtonSprite[BUTTON_R2];
            break;
        case 'W':
        case 'w':
            SetColor(HudColourNew.GetRGB(HUD_COLOUR_WHITE, a));
            break;
        case 'X':
        case 'x':
            PS2Symbol = bHasPadInHands ? ButtonSprite[BUTTON_CROSS] : ButtonSprite[BUTTON_PC_ENTER];
            break;
        case 'Y':
        case 'y':
            SetColor(HudColourNew.GetRGB(HUD_COLOUR_YELLOW, a));
            break;
        case 'l':
            SetColor(HudColourNew.GetRGB(HUD_COLOUR_BLACK, a));
            break;
        case '[':
            switch (c[1]) {
            case '~':
                PS2Symbol = ButtonSprite[BUTTON_THUMBL];
                break;
            case 'x':
                PS2Symbol = ButtonSprite[BUTTON_THUMBLX];
                break;
            case 'y':
                PS2Symbol = ButtonSprite[BUTTON_THUMBLY];
                break;
            case '<':
                PS2Symbol = ButtonSprite[BUTTON_THUMBLXL];
                break;
            case '>':
                PS2Symbol = ButtonSprite[BUTTON_THUMBLXR];
                break;
            case 'u':
                PS2Symbol = ButtonSprite[BUTTON_THUMBLYU];
                break;
            case 'd':
                PS2Symbol = ButtonSprite[BUTTON_THUMBLYD];
                break;
            }
            c++;
            break;
        case ']':
            switch (*(++c)) {
            case '~':
                PS2Symbol = ButtonSprite[BUTTON_THUMBR];
                break;
            case 'x':
                PS2Symbol = ButtonSprite[BUTTON_THUMBRX];
                break;
            case 'y':
                PS2Symbol = ButtonSprite[BUTTON_THUMBRY];
                break;
            case '<':
                PS2Symbol = ButtonSprite[BUTTON_THUMBRXL];
                break;
            case '>':
                PS2Symbol = ButtonSprite[BUTTON_THUMBRXR];
                break;
            case 'u':
                PS2Symbol = ButtonSprite[BUTTON_THUMBRYU];
                break;
            case 'd':
                PS2Symbol = ButtonSprite[BUTTON_THUMBRYD];
                break;
            }
            break;
        case '@':
            if (char* nx = c) {
                if (!ParseGInputActions(++c)) {
                    switch (*(++nx)) {
                    case 'E':
                        PS2Symbol = ButtonSprite[BUTTON_PC_ENTER];
                        break;
                    case 'B':
                        PS2Symbol = ButtonSprite[BUTTON_PC_ESC];
                        break;
                    }
                }
                c = nx;
            }
            break;
        case '#':
            if (char* nx = c) {
                switch (*(++nx)) {
                case 'L':
                    PS2Symbol = ButtonSprite[BUTTON_PC_L];
                    break;
                }

                c = nx;
            }
            break;
        }
    }

    while (*c != '~') ++c;

    if (*c)
        return c + 1;

    return c + 2;
}

bool CFontNew::ParseGInputActions(char* s) {
    short mode = CPadNew::GetPad(0)->Mode;
    bool southPaw = false;
    for (int i = 0; i < ARRAY_SIZE(CustomGInputActions); i++) {
        if (!faststrcmp(s, CustomGInputActions[i])) {
            switch (i) {
            case ACTION_PED_MOVE:
                PS2Symbol = southPaw ? ButtonSprite[BUTTON_THUMBR] : ButtonSprite[BUTTON_THUMBL];
                break;
            case ACTION_BMX_HANDBRAKE:
                switch (mode) {
                case 0:
                    PS2Symbol = ButtonSprite[BUTTON_R1];
                    break;
                case 1:
                    PS2Symbol = ButtonSprite[BUTTON_R2];
                    break;
                }
                break;
            case ACTION_BMX_BUNNYHOP:
                switch (mode) {
                case 0:
                    PS2Symbol = ButtonSprite[BUTTON_L1];
                    break;
                case 1:
                    PS2Symbol = ButtonSprite[BUTTON_SQUARE];
                    break;
                }
                break;
            case ACTION_CAMERA_LEFT_RIGHT:
                PS2Symbol = southPaw ? ButtonSprite[BUTTON_THUMBLX] :ButtonSprite[BUTTON_THUMBRX];
                break;
            case ACTION_CAMERA_UP_DOWN:
                PS2Symbol = southPaw ? ButtonSprite[BUTTON_THUMBLY] :ButtonSprite[BUTTON_THUMBRY];
                break;
            case ACTION_VEHICLE_CHANGE_RADIO_STATION:
                switch (mode) {
                case 0:
                    PS2Symbol = ButtonSprite[BUTTON_UPDOWN];
                    break;
                case 1:
                    PS2Symbol = ButtonSprite[BUTTON_LEFTRIGHT];
                    break;
                }
                break;
            case ACTION_GO_LEFTRIGHT:
                PS2Symbol = southPaw ? ButtonSprite[BUTTON_THUMBRX] : ButtonSprite[BUTTON_THUMBLX];
                break;
            case ACTION_GO_UPDOWN:
                PS2Symbol = southPaw ? ButtonSprite[BUTTON_THUMBRY] : ButtonSprite[BUTTON_THUMBLY];
                break;
            case ACTION_SNATCH_PACKAGE:
                switch (mode) {
                case 0:
                    PS2Symbol = ButtonSprite[BUTTON_L1];
                    break;
                case 1:
                    PS2Symbol = ButtonSprite[BUTTON_CIRCLE];
                    break;
                }
                break;
            case ACTION_HYDRA_TARGET:
                switch (mode) {
                case 0:
                    PS2Symbol = ButtonSprite[BUTTON_R1];
                    break;
                case 1:
                    PS2Symbol = ButtonSprite[BUTTON_SQUARE];
                    break;
                }
                break;
            }

            return true;
        }
    }

    return false;
}

void CFontNew::DrawButton(float x, float y, CSprite2d* sprite) {
    if (!sprite)
        return;

    CRect rect;
    
    PS2SymbolScale.x = clamp(sprite->m_pTexture->raster->width, 0, 128);
    PS2SymbolScale.y = clamp(sprite->m_pTexture->raster->height, 0, 128);
    float w = Details.scale.y * (PS2SymbolScale.x / 3);
    float h = Details.scale.y * (PS2SymbolScale.y / 3);
    rect.left = x;
    rect.top = y + SCREEN_COORD(1.0f);
    rect.right = rect.left + (w);
    rect.bottom = rect.top + (h);

    int savedAlpha;
    RwRenderStateGet(rwRENDERSTATEVERTEXALPHAENABLE, &savedAlpha);
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);
    sprite->Draw(rect, CRGBA(255, 255, 255, Details.color.a));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)savedAlpha);
}

void CFontNew::PrintChar(float& x, float& y, char c) {
    float _x = (c % 16);
    float _y = (c / 16);

    float u1 = _x / 16.0f;
    float v1 = _y / 12.8f;
    float u2 = (_x + 1.0f) / 16.0f - 0.003f;
    float v2 = _y / 12.8f;
    float u3 = _x / 16.0f;
    float v3 = (_y + 1.0f) / 12.8f - 0.003f;
    float u4 = (_x + 1.0f) / 16.0f - 0.003f;
    float v4 = (_y + 1.0f) / 12.8f - 0.003f;
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERMIPLINEAR);
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);

    DrawButton(x, y, PS2Symbol);
    x += PS2Symbol ? (Details.scale.y * (PS2SymbolScale.x / 3)) : 0.0f;
    PS2Symbol = NULL;

    // Text shadow
    if (Details.shadow > 0.0f) {
        Sprite[Details.style]->Draw(CRect((x + Details.shadow), (y + Details.shadow), (x + Details.shadow) + (32.0f * Details.scale.x * 1.0f), (y + Details.shadow) + (40.0f * Details.scale.y * 0.5f)), CRGBA(Details.dropColor),
            u1, v1, u2, v2,
            u3, v3, u4, v4);
    }
    else if (Details.outline > 0.0f) { // Text outline
        float outline_x[] = {
            Details.outline,
            -Details.outline,
            0.0f,
            0.0f,
            Details.outline,
            -Details.outline,
            Details.outline,
            -Details.outline,
        };

        float outline_y[] = {
            0.0f,
            0.0f,
            Details.outline,
            -Details.outline,
            Details.outline,
            Details.outline,
            -Details.outline,
            -Details.outline,
        };

        for (int i = 0; i < 8; i++) {
            Sprite[Details.style]->Draw(CRect((x + (outline_x[i])), (y + (outline_y[i])), (x + (outline_x[i])) + (32.0f * Details.scale.x * 1.0f), (y + (outline_y[i])) + (40.0f * Details.scale.y * 0.5f)), CRGBA(Details.dropColor),
                u1, v1, u2, v2,
                u3, v3, u4, v4);
        }
    }

    Sprite[Details.style]->Draw(CRect(x, y, x + (32.0f * Details.scale.x * 1.0f), y + (40.0f * Details.scale.y * 0.5f)), CRGBA(Details.color),
        u1, v1, u2, v2,
        u3, v3, u4, v4);
}

void CFontNew::PrintStringFromBottom(float x, float y, char* s) {
    y -= GetHeightScale(Details.scale.y) * (GetNumberLines(false, x, y, s) - 1);

    PrintString(x, y, s);
}

void PrintCharMap() {
    char* charMap[] = {
        "!\"#$%&'()*+,-./a",
        "0123456789:;<=>?a",
        "@ABCDEFGHIJKLMNOa",
        "PQRSTUVWXYZ[\]^_a",
        "`abcdefghijklmnoa",
        "pqrstuvwxyza",
        "�����������������a",
        "����������������a",
        "����������������a"
    };

    CFontNew::SetBackground(false);
    CFontNew::SetWrapX(SCREEN_WIDTH);
    CFontNew::SetClipX(SCREEN_WIDTH);
    CFontNew::SetClipX(-1);
    CFontNew::SetFontStyle(CFontNew::FONT_1);
    CFontNew::SetAlignment(CFontNew::ALIGN_LEFT);
    CFontNew::SetDropColor(CRGBA(0, 0, 0, 255));
    CFontNew::SetColor(CRGBA(255, 255, 255, 255));
    CFontNew::SetDropShadow(0.0f);
    CFontNew::SetOutline(SCREEN_COORD(2.0f));
    CFontNew::SetScale(SCREEN_MULTIPLIER(2.0f), SCREEN_MULTIPLIER(3.0f));

    float spacing = 0.0f;
    for (int i = 0; i < 8; i++) {
        CFontNew::PrintString(HUD_X(0.0f), SCREEN_COORD(spacing), charMap[i]);
        spacing += SCREEN_COORD(64.0f);
    }
}

float CFontNew::GetHeightScale(float h) {
    return 32.0f * h * 0.5f + 2.0f * h;
}

void CFontNew::GetTextRect(CRect* rect, float xstart, float ystart, char* s) {
    if (Details.alignment == ALIGN_CENTER)
        xstart -= GetStringWidth(s, true) / 2;

    if (Details.alignment == ALIGN_RIGHT)
        xstart -= GetStringWidth(s, true);

    rect->left = xstart - (Details.backgroundBorder.left);
    rect->right = xstart + Details.wrapX + (Details.backgroundBorder.right);

    rect->top = ystart - Details.backgroundBorder.top;
    rect->bottom = ystart + (Details.backgroundBorder.bottom) + ((32.0f * Details.scale.y * 0.5f + 2.0f * Details.scale.y) * GetNumberLines(false, xstart, ystart, s));
}
