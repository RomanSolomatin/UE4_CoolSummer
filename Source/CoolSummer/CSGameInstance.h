// Copyright (c) Jong-il Hong 2015. All rights reserved.

#pragma once

#include "Engine/GameInstance.h"
#include <fstream>
#include "CSGameInstance.generated.h"

struct SCSPlayerData
{
public:
	FORCEINLINE static bool Load(std::istream& Stream, SCSPlayerData& OutResult)
	{
		Stream.seekg(0, std::ios_base::end);
		const size_t Size = Stream.tellg();
		Stream.seekg(0, std::ios_base::beg);
		if (sizeof(SCSPlayerData) == Size)
		{
			Stream.read((char*)&OutResult, sizeof(SCSPlayerData));
			return true;
		}
		return false;
	}
	FORCEINLINE static bool Save(std::ostream& Stream, const SCSPlayerData& ToSave)
	{
		Stream.seekp(0, std::ios_base::beg);
		Stream.write((char*)&ToSave, sizeof(SCSPlayerData));
		return true;
	}

	FORCEINLINE SCSPlayerData()
		: Magic0('J'), Magic1('I'), 
		WatchedStories(0),
		LastMission(0), Ability(0),
		EnabledAbilities(1), EnabledMissions(1),
		Level(1), Experience(0)
	{
	}
	FORCEINLINE explicit SCSPlayerData(const FString& LoadFile)
	{
		std::ifstream Stream = std::ifstream(*LoadFile, std::ios_base::binary);
		if (!Load(Stream, *this))
		{
			Magic0 = Magic1 = int8('\0');
		}
	}

	FORCEINLINE bool IsValidMagic() const
	{
		return Magic0 == int8('J') && Magic1 == int8('I');
	}

	FORCEINLINE bool SaveToFile(const FString& File)
	{
		std::ofstream Stream = std::ofstream(*File, std::ios_base::binary | std::ios_base::trunc);
		return Save(Stream, *this);
	}

	FORCEINLINE bool IsStoryWatched(const int32 Index) const
	{
		return 0 != (WatchedStories & (0x01 << Index));
	}

	FORCEINLINE bool IsAbilityEnabled(const int32 Index) const
	{
		return 0 != (EnabledAbilities & (0x01 << Index));
	}
	FORCEINLINE bool IsMissionEnabled(const int32 Index) const
	{
		return 0 != (EnabledMissions & (0x01 << Index));
	}

	FORCEINLINE void SetStroyWatched(const int32 Index, const bool Set)
	{
		if (Set)
		{
			WatchedStories |= (0x01 << Index);
		}
		else
		{
			WatchedStories &= ~(0x01 << Index);
		}
	}

	FORCEINLINE void SetAbilityEnabled(const int32 Index, const bool Set)
	{
		if (Set)
		{
			EnabledAbilities |= (0x01 << Index);
		}
		else
		{
			EnabledAbilities &= ~(0x01 << Index);
		}
	}
	FORCEINLINE void SetMissionEnabled(const int32 Index, const bool Set)
	{
		if (Set)
		{
			EnabledMissions |= (0x01 << Index);
		}
		else
		{
			EnabledMissions &= ~(0x01 << Index);
		}
	}

	int8 Magic0, Magic1;
	int8 WatchedStories;
	int8 Ability;
	int16 EnabledAbilities;
	int16 LastMission;
	int32 EnabledMissions;
	uint16 Level;
	uint16 Experience;
};

UENUM(BlueprintType)
enum class ECSPlayerAbility
{
	None, Defensive, Offensive, Stealth, Count
};

UENUM(BlueprintType)
enum class ECSPlayerMission
{
	Mission1, Mission2, Mission3, Mission4, Count
};

UENUM(BlueprintType)
enum class ECSLobbyOpenReason
{
	StartGame, UserRequested, MissionCompleted, MissionFailed, GameFin
};

UENUM(BlueprintType)
enum class ECSPlayerStory
{
	Prologue, Misstion1, Misstion2, Misstion3, Epilogue, Count
};


/**
 * 
 */
UCLASS()
class COOLSUMMER_API UCSGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintPure)
		static FName GetLobbyLevelName();
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintPure)
		static FName GetMissionLevelName(const ECSPlayerMission Mission);
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintPure)
		static int32 GetMaxExperience(const int32 Level);
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintPure)
		static int32 GetMaxHP(const int32 Level);
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintPure)
		static int32 GetMaxTemperature(const int32 Level);

	FORCEINLINE static bool CheckPlayerAbility(const ECSPlayerAbility Check, UWorld* World)
	{
		UCSGameInstance* Instance = Cast<UCSGameInstance>(UGameplayStatics::GetGameInstance(nullptr != World ? World : GWorld));
		if (nullptr != Instance)
		{
			return Instance->GetPlayerAbility() == Check;
		}
		return false;
	}
	FORCEINLINE static UCSGameInstance* Get(UWorld* World)
	{
		return Cast<UCSGameInstance>(UGameplayStatics::GetGameInstance(nullptr != World ? World : GWorld));
	}

	UCSGameInstance(const FObjectInitializer& Initializer);

	virtual void Init() override;
	virtual void Shutdown() override;
	
	UFUNCTION(Category = "CS", BlueprintCallable) void LoadDefaultPlayer();
	UFUNCTION(Category = "CS", BlueprintCallable) void SaveDefaultPlayer();

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintPure) bool IsPlayerStoryWatched(const ECSPlayerStory Story) const;
	UFUNCTION(Category = "CS", BlueprintCallable) void SetIsPlayerStoryWatched(const ECSPlayerStory Story, const bool Set);

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintPure) ECSPlayerAbility GetPlayerAbility();
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintPure) ECSPlayerMission GetPlayerLastMission();

	UFUNCTION(Category = "CS", BlueprintCallable) void SetPlayerAbility(const ECSPlayerAbility Ability);
	UFUNCTION(Category = "CS", BlueprintCallable) void SetPlayerLastMission(const ECSPlayerMission Mission);

	UFUNCTION(Category = "CS", BlueprintCallable) bool IsPlayerAbilityEnabled(const ECSPlayerAbility Index) const;
	UFUNCTION(Category = "CS", BlueprintCallable) bool IsPlayerMissionEnabled(const ECSPlayerMission Index) const;

	UFUNCTION(Category = "CS", BlueprintCallable) void SetPlayerAbilityEnabled(const ECSPlayerAbility Index, const bool Set);
	UFUNCTION(Category = "CS", BlueprintCallable) void SetPlayerMissionEnabled(const ECSPlayerMission Index, const bool Set);

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintPure) int32 GetPlayerLevel();
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintPure) int32 GetPlayerExperience();

	UFUNCTION(Category = "CS", BlueprintCallable) int32 GivePlayerExperience(int32 ToGive, bool& bLevelChanged);

	FORCEINLINE SCSPlayerData& GetPlayerData()
	{
		return PlayerData;
	}

protected:
	SCSPlayerData PlayerData;

	UPROPERTY(Category = "CS", BlueprintReadWrite) TEnumAsByte<ECSLobbyOpenReason> LobbyOpenReason;

	static const FString DefaultPlayerFile;

	static const FName LobbyLevelName;
	static const FName MissionLevelNames[];

	static const uint16 MaxLevel;
	static const uint16 ExperienceTable[];	
	static const int32 MaxHPTable[];
	static const int32 MaxTemperatureTable[];
};
