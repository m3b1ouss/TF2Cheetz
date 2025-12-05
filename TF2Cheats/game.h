#pragma once

#define STATE_AIMBOT 0x01
#define STATE_ESP 0x02
#define STATE_BHOP 0x04
#define STATE_CUSTOMFOV 0x08
#define STATE_THIRDPERSON 0x10
#define STATE_INITIALIZING 0x1000

#include <Windows.h>
#include <cstdint>
#include <string>
#include "mathlib.h"

extern void* CEngineClient;

enum StereoEye_t
{
	STEREO_EYE_MONO = 0,
	STEREO_EYE_LEFT = 1,
	STEREO_EYE_RIGHT = 2,
	STEREO_EYE_MAX = 3,
};

class CViewSetup
{
public:

	__int32 x; //0x0000 
	__int32 m_nUnscaledX; //0x0004 
	__int32 y; //0x0008 
	__int32 m_nUnscaledY; //0x000C 
	__int32 width; //0x0010 
	__int32 m_nUnscaledWidth; //0x0014 
	__int32 height; //0x0018 
	__int32 m_eStereoEye; //0x001C 
	__int32 m_nUnscaledHeight; //0x0020 
	bool m_bOrtho; //0x0024 
	float m_OrthoLeft; //0x0028 
	float m_OrthoTop; //0x002C 
	float m_OrthoRight; //0x0030 
	float m_OrthoBottom; //0x0034 
	float m_fov; //0x0038 
	float m_viewmodel_fov; //0x003C 
	Vector3 origin; //0x0040 
	Vector3 angles; //0x004C 
};


typedef struct player_info_s
{
	// scoreboard information
	char			name[32];
	// local server user ID, unique while server is running
	int				userID;
	// global unique player identifer
	char			guid[33];
	// friends identification number
	uint32_t			friendsID;
	// friends name
	char			friendsName[32];
	// true, if player is a bot controlled by game.dll
	bool			fakeplayer;
	// true if player is the HLTV proxy
	bool			ishltv;
	// custom files CRC for this player
	UINT			customFiles[4];
	// this counter increases each time the server downloaded a new file
	unsigned char	filesDownloaded;
} player_info_t;

typedef void (__thiscall* GetScreenSize)(void* thisPtr, int& w, int& h);
typedef int (__thiscall* GetLPlayer)();
typedef float* (__thiscall* GetMVPMatrix)();
typedef bool (__thiscall* GetPInfo)(void* thisPtr, int entNum, player_info_t* playerInfo);
typedef bool (__thiscall* InGame)();


class entityNode
{
public:
	class entity* entityPtr; //0x0000
	int32_t N0000025A; //0x0008
	char pad_000C[4]; //0x000C
	class entityNode* blink; //0x0010
	class entityNode* flink; //0x0018
}; //Size: 0x0020

class CBaseEntityList
{
public:
	char pad_0000[32]; //0x0000
	class entityNode EntityList[64]; //0x0020
}; //Size: 0x0820

class entity
{
public:
	char pad_0000[224]; //0x0000
	int32_t m_lifeState; //0x00E0
	int32_t m_iHealth; //0x00E4
	char pad_00E8[4]; //0x00E8
	int32_t m_iTeamNumber; //0x00EC
	char pad_00F0[108]; //0x00F0
	float playerHeadOffset; //0x015C
	char pad_0160[202]; //0x0160
	bool m_bDormant; //0x022A
	char pad_022B[269]; //0x022B
	Vector3 playerCoords; //0x0338
	Vector3 playerAngles; //0x0344
	char pad_0350[288]; //0x0350
	int64_t m_fFlags; //0x0470
	char pad_0478[1800]; //0x0478
	Matrix3x4 *playerBoneMatrix; //0x0B80
	char pad_0B88[1424]; //0x0B88
	int32_t m_hMyWeapons[3]; //0x1118
	char pad_1124[180]; //0x1124
	int32_t m_hActiveWeapon; //0x11D8
	char pad_11DC[2520]; //0x11DC
	char m_PlayerClass[8]; //0x1BB4
	char pad_1BBC[588]; //0x1BBC
	int64_t m_iMaxHealth; //0x1E08
	char pad_1E10[356]; //0x1E10
	int32_t m_fShared; //0x1F74

	bool IsDead() 
	{
		return m_lifeState != 2;
	}

	bool IsDormant()
	{
		return m_bDormant;
	}

	int GetHealth() 
	{
		return m_iHealth;
	}

	Vector3 GetPosition()
	{
		if (!IsDormant() && !IsDead())
		{
			return playerCoords;
		}
		return Vector3{0, 0, 0};
	}

	Vector3 GetBonePosition(int boneId)
	{
		if (!IsDormant() && !IsDead())
		{
			return Vector3{ playerBoneMatrix[boneId][0][3], playerBoneMatrix[boneId][1][3], playerBoneMatrix[boneId][2][3] };
		}
		return Vector3{0, 0, 0};
	}

	bool IsScoped() 
	{
		return m_fShared & 0x03;
	}

}; //Size: 0x1E0C

class CTFWeaponInfo
{
public:
	char pad_0000[10]; //0x0000
	char weaponName[32]; //0x000A
};

class CTFWIPtr
{
public:
	CTFWeaponInfo* WIPtr;
};

class TF2Cheat {
public:
	const std::uintptr_t entListOffset = 0x107F8E8;
	const std::uintptr_t dwForceJump = 0x10ED330;
	const std::uintptr_t overrideViewOffset = 0x2421B0;

	std::uintptr_t engineBase;
	std::uintptr_t clientBase;
	std::uintptr_t localPitchAddr;
	std::uintptr_t localYawAddr;

	float localPitch;
	float localYaw;
	entity* localPlayer;
	CBaseEntityList* entityList;
	GetScreenSize oGetScreenSize;
	GetLPlayer oGetLocalPlayer;
	GetMVPMatrix oGetMVPMatrix;
	GetPInfo oGetPlayerInfo;
	InGame oIsInGame;
	float* viewMatrix;
	
	void Init();
	void Update();
	bool IsEntityValid(entity* entPtr);
	void GetWindowSize(int &w, int &h);
	bool WorldToScreen(Vector3 worldPos, Vector2 &screenPos);
	void GetHeadPos(entity *player, Vector3 &headCoords);
	void SetForceJump(int val);
	void SetViewAngles(float newPitch, float newYaw);
	entity* GetLocalPlayer();
	bool GetPlayerInfo(int entId, player_info_t* playerInfo);
	bool IsInGame();
	std::string GetActiveWeaponType();
};