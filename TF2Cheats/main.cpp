#define WIN32_LEAN_AND_MEAN
#define _USE_MATH_DEFINES

#include <iostream>
#include <thread>
#include <vector>
#include <conio.h>
#include "memrw.h"
#include "game.h"
#include "dx.h"
#include "drawing.h"
#include "MinHook.h"
#pragma comment (lib, "libMinHook.x64.lib")

namespace Config
{
    int customFov = 130;
    float aimbotFov = 20;
    float thirdPersonAngleOffset = 0.f;
    float thirdPersonCamDist = 125.f;
    float thirdPersonCamHeight = 50.f;
}

typedef void(__stdcall* EndScene)(LPDIRECT3DDEVICE9 pDevice);
typedef void(__thiscall* OverrideView)(void* thisPtr, CViewSetup* viewSetup);
typedef int(__thiscall* CAM_IsThirdPerson)();

void* d3d9Device[119];
EndScene oEndScene = nullptr;
OverrideView oOverrideView = nullptr;
CAM_IsThirdPerson oIsThirdPerson = nullptr;
extern LPDIRECT3DDEVICE9 pDevice = nullptr;
extern ID3DXFont* Font = nullptr;
extern void* CEngineClient = nullptr;
TF2Cheat* Intruder{};
int GLOBALSTATE{};

const D3DCOLOR BASECOL = D3DCOLOR_ARGB(255, 255, 12, 12);
const D3DCOLOR COL1 = D3DCOLOR_ARGB(255, 39, 191, 44);
const D3DCOLOR COL2 = D3DCOLOR_ARGB(255, 0, 128, 255);
const D3DCOLOR TEXTCOL = D3DCOLOR_ARGB(255, 225, 225, 225);

int APIENTRY hkIsThirdPerson() {
    return 1;
}

void APIENTRY hkOverrideView(void* thisPtr, CViewSetup *viewSetup) 
{
    Vector3 newCamPos = viewSetup->origin;
    if (!(Intruder->localPlayer->IsScoped())) 
    {   
        viewSetup->m_fov = 90;
        if (GLOBALSTATE & STATE_CUSTOMFOV) 
        {
            viewSetup->m_fov = Config::customFov;
        }
        if (GLOBALSTATE & STATE_THIRDPERSON)
        {
            float rad = ((Intruder->localYaw - 90.f) + Config::thirdPersonAngleOffset) * (M_PI / 180.f);
            newCamPos.ordinate -= Config::thirdPersonCamDist * cosf(rad);
            newCamPos.abscissa += Config::thirdPersonCamDist * sinf(rad);
            newCamPos.applicate += Config::thirdPersonCamHeight;
            viewSetup->origin = newCamPos;
        }
    } 
    else 
    {
        viewSetup->m_fov = 20; 
    }
    oOverrideView(thisPtr, viewSetup); // Вызов оригинала
}

void APIENTRY hkEndScene(LPDIRECT3DDEVICE9 oPDevice) 
{
    if (!pDevice)
        pDevice = oPDevice;

    if (!Font)
        InitFont(10); 
    
    int c{};
    player_info_t playerInfo{};
    Vector2 screenBottom{}, screenHead{}, tmpScreen1{}, tmpScreen2{};
    Vector3 tmpPos{};
    float healthBarScale{};
    CBaseEntityList* baseEntityList = Intruder->entityList;
    for (auto ent : baseEntityList->EntityList)
    {
        c++;
        if (!Intruder->IsEntityValid(ent.entityPtr)) 
            continue;

        Vector3 enemyPos = ent.entityPtr->GetPosition();
        Intruder->GetHeadPos(ent.entityPtr, tmpPos);
        Intruder->WorldToScreen(tmpPos, screenHead);
        if (Intruder->WorldToScreen(enemyPos, screenBottom))
        {
            healthBarScale = static_cast<float>(ent.entityPtr->GetHealth()) / static_cast<float>(ent.entityPtr->m_iMaxHealth);
            screenBottom.abscissa = screenHead.abscissa;
            screenHead.ordinate -= 8.f;  
            Draw2DBox(screenHead, screenBottom, 1, BASECOL, tmpScreen1, tmpScreen2);
            tmpScreen1.abscissa -= 4.f;
            if (healthBarScale <= 1)
            {
                DrawHBar(tmpScreen1, (tmpScreen2.ordinate - tmpScreen1.ordinate) * healthBarScale, 2, COL1);
            }
            else
            {
                DrawHBar(tmpScreen1, tmpScreen2.ordinate - tmpScreen1.ordinate, 2, COL2);
            }
            tmpScreen1.abscissa -= 10.f;
            screenBottom.ordinate += 10.f;
            DrawDXText(tmpScreen1, std::to_string(ent.entityPtr->GetHealth()), 15, TEXTCOL);
            DrawDXText(screenBottom, ent.entityPtr->m_PlayerClass, 15, TEXTCOL);
            if (Intruder->GetPlayerInfo(c, &playerInfo))
            {
                screenHead.ordinate -= 10.f;
                DrawDXText(screenHead, playerInfo.name, 15, TEXTCOL);
            }
        }
    }
    oEndScene(pDevice);
}

bool InitHooks() {
    void* endSceneAddr{};
    void* overrideView{}; 
    void* isThirdPerson{};
    MH_Initialize();
    if (GetDXDevice(d3d9Device, sizeof(d3d9Device)))
    {
        endSceneAddr = reinterpret_cast<void*>(d3d9Device[42]);
        overrideView = (OverrideView)(Intruder->clientBase + Intruder->overrideViewOffset);
        isThirdPerson = (CAM_IsThirdPerson)(Intruder->clientBase + 0x2B95D0);
        MH_CreateHook(endSceneAddr, hkEndScene, reinterpret_cast<void**>(&oEndScene));
        MH_CreateHook(overrideView, hkOverrideView, reinterpret_cast<void**>(&oOverrideView));
        MH_CreateHook(isThirdPerson, hkIsThirdPerson, reinterpret_cast<void**>(&oIsThirdPerson));
        return true;
    }
    return false;
}

void ESPFunc() noexcept 
{
    void* endSceneAddr = reinterpret_cast<void*>(d3d9Device[42]);
    MH_EnableHook(endSceneAddr);
    while (true)
    {
        if (!(GLOBALSTATE & STATE_ESP))
        {
            MH_DisableHook(endSceneAddr);
            break;
        }
        Sleep(5);
    }
}

void AimbotFunc() noexcept
{
    bool aimEnabled = false;
    Vector3 tmpVec{}, localPlayerPos{};
    while (GLOBALSTATE & STATE_AIMBOT)
    {
        if (GetAsyncKeyState(0x51) & 1) // Q
        {
            aimEnabled = !aimEnabled;
        }

        if (!aimEnabled) 
        {
            continue;
        }

        int count = 1;
        int playerAimLocked = 0;
        std::string weaponType{};
        for (auto ent : Intruder->entityList->EntityList)
        {
            if (Intruder->IsEntityValid(ent.entityPtr) && !Intruder->localPlayer->IsDead())
            {
                float targetPitch, targetYaw;
                weaponType = Intruder->GetActiveWeaponType();
                if (weaponType == "tf_weapon_sniperrifle" && Intruder->localPlayer->IsScoped())
                {
                    Intruder->GetHeadPos(ent.entityPtr, tmpVec);
                }
                else 
                {
                    tmpVec = ent.entityPtr->GetBonePosition(2);
                }
                localPlayerPos = Intruder->localPlayer->GetPosition();
                localPlayerPos.applicate += Intruder->localPlayer->playerHeadOffset;
                localPlayerPos.subtract(tmpVec);
                targetPitch = atan2(localPlayerPos.applicate, sqrt(pow(localPlayerPos.abscissa, 2.0f) + pow(localPlayerPos.ordinate, 2.0f))) * (180.0f / (float)M_PI);
                targetYaw = atan2(localPlayerPos.ordinate, localPlayerPos.abscissa) * (180.0f / (float)M_PI);
                (targetYaw < 0) ? targetYaw += 180.0f : targetYaw -= 180.0f;

                if ((abs(Intruder->localPlayer->playerAngles.abscissa - targetPitch) <= 3 && abs(Intruder->localPlayer->playerAngles.ordinate - targetYaw) <= 3 || playerAimLocked == count))
                {
                    Intruder->SetViewAngles(targetPitch, targetYaw);
                    playerAimLocked = count;
                }
            }
            count++;
        }
        Sleep(5);
    }
}

void BhopFunc() noexcept 
{
    while (GLOBALSTATE & STATE_BHOP)
    {
        if (Intruder->localPlayer->IsDead())
            continue;

        if (!GetAsyncKeyState(VK_SPACE))
            continue;

        (Intruder->localPlayer->m_fFlags & (1 << 0)) ? Intruder->SetForceJump(6) : Intruder->SetForceJump(4);
        Sleep(1);
    }
}

void ThirdPersonFunc() noexcept
{
    void* isThirdPerson = (CAM_IsThirdPerson)(Intruder->clientBase + 0x2B95D0);
    MH_EnableHook(isThirdPerson);
    while (true)
    {
        if (!(GLOBALSTATE & STATE_THIRDPERSON))
        {
            MH_DisableHook(isThirdPerson);
            break;
        }
        Sleep(5);
    }
}

void InitMain(const HMODULE hModule) 
{
    GLOBALSTATE |= STATE_INITIALIZING;
    Intruder = new TF2Cheat();
    Intruder->Init();
    void* overrideViewAddr = (OverrideView)(Intruder->clientBase + Intruder->overrideViewOffset);
    
    if(!InitHooks()) 
    {
        FreeLibraryAndExitThread(hModule, 0);
    }

    do
    {
        if (Intruder->IsInGame())
        {

            if (GLOBALSTATE & STATE_INITIALIZING) 
            {
                MH_EnableHook(overrideViewAddr);
                GLOBALSTATE ^= STATE_INITIALIZING;
            }
            Intruder->Update();
            Sleep(5);
            
            if (GetAsyncKeyState(VK_F4) & 1)
            {
                if (!(GLOBALSTATE & STATE_AIMBOT))
                {
                    GLOBALSTATE |= STATE_AIMBOT;
                    CloseHandle(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AimbotFunc, hModule, 0, NULL));
                }
                else
                {
                    GLOBALSTATE ^= STATE_AIMBOT;
                }
            }

            if (GetAsyncKeyState(VK_F5) & 1)
            {
                if (!(GLOBALSTATE & STATE_CUSTOMFOV))
                {
                    GLOBALSTATE |= STATE_CUSTOMFOV;
                }
                else 
                {
                    GLOBALSTATE ^= STATE_CUSTOMFOV;
                }
            }

            if (GetAsyncKeyState(VK_F6) & 1)
            {
                if (!(GLOBALSTATE & STATE_ESP))
                {
                    GLOBALSTATE |= STATE_ESP;
                    CloseHandle(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ESPFunc, 0, 0, NULL));
                }
                else
                {
                    GLOBALSTATE ^= STATE_ESP;
                }
            }

            if (GetAsyncKeyState(VK_F7) & 1)
            {
                if (!(GLOBALSTATE & STATE_BHOP))
                {
                    GLOBALSTATE |= STATE_BHOP;
                    CloseHandle(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)BhopFunc, 0, 0, NULL));
                }
                else
                {
                    GLOBALSTATE ^= STATE_BHOP;
                }
            }

            if (GetAsyncKeyState(VK_F8) & 1)
            {
                if (!(GLOBALSTATE & STATE_THIRDPERSON))
                {
                    GLOBALSTATE |= STATE_THIRDPERSON;
                    CloseHandle(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThirdPersonFunc, 0, 0, NULL));
                }
                else
                {
                    GLOBALSTATE ^= STATE_THIRDPERSON;
                }
            }

        }
        else 
        {
            GLOBALSTATE ^= GLOBALSTATE;
            Sleep(500);
        }
    } while (!GetAsyncKeyState(VK_END) & 1);

    if (GLOBALSTATE) {
        GLOBALSTATE ^= GLOBALSTATE;
        Sleep(500);
    }

    MH_Uninitialize();
    //fclose(f);
    //FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);
}


BOOL __stdcall DllMain(const HMODULE hModule, const DWORD fdwReason, const LPVOID lpReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        const HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)InitMain, hModule, 0, NULL);
        if (hThread)
        {
            CloseHandle(hThread);
        }
    }
    return TRUE;
}