#include "game.h"
#include "memrw.h"
#include <string>

void TF2Cheat::Init() {
	engineBase = (std::uintptr_t)GetModuleHandleA("engine.dll");
	clientBase = (std::uintptr_t)GetModuleHandleA("client.dll");
    localPitchAddr = engineBase + 0x53F4E4;
    localYawAddr = localPitchAddr + 4;
	entityList = (CBaseEntityList*)(clientBase + entListOffset);
    oGetScreenSize = (GetScreenSize)(engineBase + 0x70520);
    oGetLocalPlayer = (GetLPlayer)(engineBase + 0x70280);
    oGetMVPMatrix = (GetMVPMatrix)(engineBase + 0x714A0);
    oGetPlayerInfo = (GetPInfo)(engineBase + 0x70390);
    oIsInGame = (InGame)(engineBase + 0x28850);
    CEngineClient = reinterpret_cast<void*>(engineBase + 0x4615A0);
}

bool TF2Cheat::IsEntityValid(entity* entPtr) {
    if (entPtr != nullptr && entPtr != localPlayer)
    {
        if (!entPtr->IsDead() && !entPtr->IsDormant() && entPtr->GetHealth() >= 1 && entPtr->m_iTeamNumber != localPlayer->m_iTeamNumber)
        {
            return true;
        }
    }
	return false;
}

void TF2Cheat::Update() 
{
    localPlayer = GetLocalPlayer();
    viewMatrix = oGetMVPMatrix();
    localPitch = ReadLocalMemory<float>(localPitchAddr);
    localYaw = ReadLocalMemory<float>(localYawAddr);
}

void TF2Cheat::GetWindowSize(int &w, int &h) 
{
    oGetScreenSize(static_cast<void*>(nullptr), w, h);
}

bool TF2Cheat::WorldToScreen(Vector3 worldPos, Vector2& screenPos) 
{
    int width, height;
    GetWindowSize(width, height);

    float w = viewMatrix[12] * worldPos.abscissa + viewMatrix[13] * worldPos.ordinate + viewMatrix[14] * worldPos.applicate + viewMatrix[15];

    bool facing{ (w >= 0.001f) };
    if (!facing) 
    {
        return false;
    }
    float x = viewMatrix[0] * worldPos.abscissa + viewMatrix[1] * worldPos.ordinate + viewMatrix[2] * worldPos.applicate + viewMatrix[3];
    float y = viewMatrix[4] * worldPos.abscissa + viewMatrix[5] * worldPos.ordinate + viewMatrix[6] * worldPos.applicate + viewMatrix[7];
    float nx = x / w;
    float ny = y / w;
    float resX = static_cast<float>(width);
    float resY = static_cast<float>(height);
    screenPos.abscissa = resX * 0.5f * (1.f + nx);
    screenPos.ordinate = resY * 0.5f * (1.f - ny);

    bool visible{ (screenPos.abscissa >= 0 && screenPos.abscissa <= resX) && (screenPos.ordinate >= 0 && screenPos.ordinate <= resY) };
    if (!visible) 
    {
        screenPos.abscissa = -1.f;
        screenPos.ordinate = -1.f;
        return false;
    }
    return true;
}

void TF2Cheat::GetHeadPos(entity *player, Vector3& headCoords)
{
    int headBone{};
    const std::string tfClass = player->m_PlayerClass;
    if (tfClass == "engineer")
    {
        headBone = 8;
    } 
    else if (tfClass == "demo") 
    {
        headBone = 16;
    }
    else 
    {
        headBone = 6;
    }
    headCoords = player->GetBonePosition(headBone);
}

void TF2Cheat::SetForceJump(int val) 
{
    WriteLocalMemory(clientBase + dwForceJump, val);
}

void TF2Cheat::SetViewAngles(float newPitch, float newYaw) 
{
    WriteLocalMemory<float>(localPitchAddr, newPitch);
    WriteLocalMemory<float>(localYawAddr, newYaw);
}

entity* TF2Cheat::GetLocalPlayer()
{
    if (entityList != nullptr) 
    {
        return entityList->EntityList[oGetLocalPlayer() - 1].entityPtr;
    }
    return nullptr;
}

bool TF2Cheat::GetPlayerInfo(int entId, player_info_t *playerInfo)
{
    return oGetPlayerInfo(CEngineClient, entId, playerInfo);
}

bool TF2Cheat::IsInGame() 
{
    return oIsInGame();
}

std::string TF2Cheat::GetActiveWeaponType() 
{
    int entId = localPlayer->m_hActiveWeapon & ((1 << 12) - 1);
    entityNode wpnEntity = entityList->EntityList[--entId];
    CTFWIPtr* wpnInfo = reinterpret_cast<CTFWIPtr*>((std::uintptr_t)wpnEntity.entityPtr + 0xF90);
    if (wpnInfo != nullptr)
    {
        if (wpnInfo->WIPtr != nullptr)
        {
            return static_cast<std::string>(wpnInfo->WIPtr->weaponName);
        }
    }
    return "";
}