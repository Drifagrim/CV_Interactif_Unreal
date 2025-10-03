// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/GI_CV_Interactif.h"

#include "Kismet/GameplayStatics.h"

void UGI_CV_Interactif::Init(void)
{
    Super::Init();

    TSharedPtr<FJsonObject> Progress = LoadProgress();

    FString Step = "fps"; // Default value

    // Try to read and store the current step from the JSon save file
    if (Progress.IsValid())
    {
        Step = Progress->GetStringField("step");

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green,
                                             FString::Printf(TEXT("Step read at the beginning : %s"), *Step));
        }
    }
    else
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red,
                                             FString::Printf(TEXT("Progress.json invalid, fallback on FPS")));
        }
    }

    // For TPS current step need to wait to load the map
    // Because Init finish before it can load the map
    if (Step == "tps")
    {
        // Wait 0.1s to load the level
        FTimerHandle TimerHandle;
        FTimerDelegate TimerDelegate;

        TimerDelegate.BindLambda([this]()
            {
                UGameplayStatics::OpenLevel(this, FName("TPS_Map"));
            });

        GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 0.1f, false);
    }
}

TSharedPtr<FJsonObject> UGI_CV_Interactif::LoadProgress(void) const
{
    FString JsonString;
    FString Path = GetSaveFilePath();

    if (FFileHelper::LoadFileToString(JsonString, *Path))
    {
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            return JsonObject;
        }
        else
        {
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red,
                                                 FString::Printf(TEXT("Fail to deserialize JSon : %s"), *JsonString));
            }
        }
    }
    else
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, 
                                             FString::Printf(TEXT("Impossible to load %s"), *Path));
        }
    }

    return nullptr;
}

FString UGI_CV_Interactif::GetSaveFilePath(void) const
{
    return FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::ProjectDir(), 
                                             TEXT("../../../Save/progress.json")));;
}

void UGI_CV_Interactif::SaveProgress(TSharedPtr<FJsonObject> JsonObject) const
{
    FString OutputString;

    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);

    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
    FFileHelper::SaveStringToFile(OutputString, *GetSaveFilePath());
}

void UGI_CV_Interactif::MarkStepCompletedAndSave(FString StepCompleted, FString NextStep)
{
    TSharedPtr<FJsonObject> Progress = LoadProgress();

    if (!Progress.IsValid())
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red,
                                             FString(TEXT("[MarkStepCompletedAndSave] progress is invalid")));
        }
    }

    Progress->SetBoolField(StepCompleted + "_completed", true);
    Progress->SetStringField("step", NextStep);

    SaveProgress(Progress);
}

void UGI_CV_Interactif::LaunchUnity()
{
    // Store the path of the unity executable
    FString UnityExePath = FPaths::ConvertRelativePathToFull(
                                FPaths::Combine(FPaths::LaunchDir(),
                                TEXT("../../CV_Interactif_Unity_Build/CV_Interactif_Unity.exe")));

    // Check if it the path exists
    if (FPaths::FileExists(UnityExePath))
    {
        // If it exists, run the executable
        // Third parameter set to true means unreal is not closed when unity is opened
        FProcHandle ProcHandle = FPlatformProcess::CreateProc(*UnityExePath, nullptr, true, false, false, nullptr, 0, nullptr, nullptr);
    }
    else
    {
        // Error path doesn't exists
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red,
                                             FString::Printf(TEXT("Unity build not found at : %s"), *UnityExePath));
        }
    }
}
