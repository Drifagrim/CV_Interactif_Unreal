// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GI_CV_Interactif.generated.h"

/**
 * 
 */
UCLASS()
class CV_INTERACTIF_UE_API UGI_CV_Interactif : public UGameInstance
{
	GENERATED_BODY()

private : 

    virtual void            Init(void) override;

    TSharedPtr<FJsonObject> LoadProgress(void) const;

    FString                 GetSaveFilePath(void) const;

    void                    SaveProgress(TSharedPtr<FJsonObject> JsonObject) const;

public :

    UFUNCTION(BlueprintCallable, Category = "UE To Unity")
    void                    MarkStepCompletedAndSave(FString StepCompleted, FString NextStep);
	
    UFUNCTION(BlueprintCallable, Category = "UE To Unity")
    void                    LaunchUnity();
};
