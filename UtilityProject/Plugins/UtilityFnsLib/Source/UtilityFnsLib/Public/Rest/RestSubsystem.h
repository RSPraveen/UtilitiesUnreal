// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Http.h"
#include <Http.h>
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "RestSubsystem.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FRestCallDelegate, FString, Response);

UENUM(BlueprintType)
enum class EVaRestRequestVerb : uint8
{
	GET,
	POST,
	/*PUT,
	DEL UMETA(DisplayName = "DELETE")*/
};

/**
 * 
 */
UCLASS()
class UTILITYFNSLIB_API URestSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	/** Easy way to process http requests */
	UFUNCTION(BlueprintCallable, Category = "VaRest|Utility")
	void CallURL(const FString& URL, EVaRestRequestVerb Verb, const FString& Body, const FRestCallDelegate& Callback_local);
	
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);


	UPROPERTY()
	FRestCallDelegate Callback;

};
