// Copyright (c) Jong-il Hong 2015. All rights reserved.

#include "CoolSummer.h"
#include "CSGameInstance.h"

const FString UCSGameInstance::DefaultPlayerFile(TEXT("Player.fsd"));

const FName UCSGameInstance::LobbyLevelName(TEXT("/Game/CoolSummer/Maps/Lobby"));
const FName UCSGameInstance::MissionLevelNames[] =
{
	TEXT("/Game/CoolSummer/Maps/Level0"), 
	TEXT("/Game/CoolSummer/Maps/Level1"),
	TEXT("/Game/CoolSummer/Maps/Level2"),
	TEXT("/Game/CoolSummer/Maps/Level3")
};

const uint16 UCSGameInstance::MaxLevel = 10;
const uint16 UCSGameInstance::ExperienceTable[MaxLevel] =
{
	500, 1000, 3000, 3000, 3000, 
	5000, 5000, 5000, 5000, 10000
};
const int32 UCSGameInstance::MaxHPTable[MaxLevel] =
{
	500, 1000, 1500, 2000, 3000, 
	4000, 5000, 6000, 8000, 10000
};
const int32 UCSGameInstance::MaxTemperatureTable[MaxLevel] =
{
	100, 200, 300, 400, 500, 
	600, 700, 800, 900, 1000
};

FName UCSGameInstance::GetLobbyLevelName()
{
	return LobbyLevelName;
}

FName UCSGameInstance::GetMissionLevelName(const ECSPlayerMission Mission)
{
	check(ECSPlayerMission::Count > Mission);
	return MissionLevelNames[(int32)Mission];
}

int32 UCSGameInstance::GetMaxExperience(const int32 Level)
{
	check(Level <= MaxLevel);
	return static_cast<int32>(ExperienceTable[Level - 1]);
}
int32 UCSGameInstance::GetMaxHP(const int32 Level)
{
	check(Level <= MaxLevel);
	return MaxHPTable[Level - 1];
}
int32 UCSGameInstance::GetMaxTemperature(const int32 Level)
{
	check(Level <= MaxLevel);
	return MaxTemperatureTable[Level - 1];
}

UCSGameInstance::UCSGameInstance(const FObjectInitializer& Initializer)
	: Super(Initializer)
{
	LobbyOpenReason = ECSLobbyOpenReason::StartGame;
}

void UCSGameInstance::Init()
{
	LoadDefaultPlayer();
}

void UCSGameInstance::Shutdown()
{
	SaveDefaultPlayer();
}

void UCSGameInstance::LoadDefaultPlayer()
{
	PlayerData = SCSPlayerData(FPaths::GameDir() + DefaultPlayerFile);
	if (!PlayerData.IsValidMagic())
	{
		PlayerData = SCSPlayerData();
	}
}
void UCSGameInstance::SaveDefaultPlayer()
{
	if (PlayerData.IsValidMagic())
	{
		PlayerData.SaveToFile(FPaths::GameDir() + DefaultPlayerFile);
	}
}

bool UCSGameInstance::IsPlayerStoryWatched(const ECSPlayerStory Story) const
{
	return PlayerData.IsStoryWatched(static_cast<int32>(Story));
}
void UCSGameInstance::SetIsPlayerStoryWatched(const ECSPlayerStory Story, const bool Set)
{
	PlayerData.SetStroyWatched(static_cast<int32>(Story), Set);
}

ECSPlayerAbility UCSGameInstance::GetPlayerAbility()
{
	return (ECSPlayerAbility)PlayerData.Ability;
}
ECSPlayerMission UCSGameInstance::GetPlayerLastMission()
{
	return (ECSPlayerMission)PlayerData.LastMission;
}

void UCSGameInstance::SetPlayerAbility(const ECSPlayerAbility Ability)
{
	PlayerData.Ability = (int32)Ability;
}
void UCSGameInstance::SetPlayerLastMission(const ECSPlayerMission Level)
{
	PlayerData.LastMission = (int32)Level;
}

bool UCSGameInstance::IsPlayerAbilityEnabled(const ECSPlayerAbility Index) const
{
	return PlayerData.IsAbilityEnabled((int32)Index);
}
bool UCSGameInstance::IsPlayerMissionEnabled(const ECSPlayerMission Index) const
{
	return PlayerData.IsMissionEnabled((int32)Index);
}

void UCSGameInstance::SetPlayerAbilityEnabled(const ECSPlayerAbility Index, const bool Set)
{
	PlayerData.SetAbilityEnabled((int32)Index, Set);
}
void UCSGameInstance::SetPlayerMissionEnabled(const ECSPlayerMission Index, const bool Set)
{
	PlayerData.SetMissionEnabled((int32)Index, Set);
}

int32 UCSGameInstance::GetPlayerLevel()
{
	return static_cast<int32>(PlayerData.Level);
}
int32 UCSGameInstance::GetPlayerExperience()
{
	return static_cast<int32>(PlayerData.Experience);
}

int32 UCSGameInstance::GivePlayerExperience(int32 ToGive, bool& bLevelChanged)
{
	if (PlayerData.Level >= MaxLevel)
	{
		bLevelChanged = false;
		return 0;
	}
	else
	{
		const uint16 StartLevel = PlayerData.Level;
		int32 Used = 0;
		while (0 < ToGive && PlayerData.Level < MaxLevel)
		{
			const uint16 CurrentMax = ExperienceTable[PlayerData.Level - 1];
			const uint16 Remain = CurrentMax - PlayerData.Experience;
			if (ToGive < Remain)
			{
				Used += ToGive;
				PlayerData.Experience += ToGive;
				ToGive = 0;
			}
			else if (ToGive == Remain)
			{
				Used += ToGive;
				PlayerData.Level++;
				PlayerData.Experience = 0;
				ToGive = 0;
			}
			else
			{
				Used += Remain;
				PlayerData.Level++;
				PlayerData.Experience = 0;
				ToGive -= Remain;
			}
		}
		bLevelChanged = StartLevel != PlayerData.Level;
		return Used;
	}
}
