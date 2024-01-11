// Fill out your copyright notice in the Description page of Project Settings.


#include "Rest/RestSubsystem.h"
#include <Http.h>
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

void URestSubsystem::CallURL(const FString& URL, EVaRestRequestVerb Verb, const FString& Body, const FRestCallDelegate& Delegate)
{
    FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &URestSubsystem::OnResponseReceived);
	this->Callback = Delegate;
    //Request->OnProcessRequestComplete().
    Request->SetURL(URL);

	// Set verb
	switch (Verb)
	{
	case EVaRestRequestVerb::GET:
		Request->SetVerb(TEXT("GET"));
		break;

	case EVaRestRequestVerb::POST:
		Request->SetVerb(TEXT("POST"));
		Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
		Request->SetContentAsString(Body);
		break;

	/*case EVaRestRequestVerb::PUT:
		Request->SetVerb(TEXT("PUT"));
		break;

	case EVaRestRequestVerb::DEL:
		Request->SetVerb(TEXT("DELETE"));
		break;*/

	default:
		break;
	}
    
    Request->ProcessRequest();
}

void URestSubsystem::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	//TSharedPtr<FJsonObject> ResponseObj;
	//TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	//FJsonSerializer::Deserialize(Reader, ResponseObj);
	this->Callback.ExecuteIfBound(Response->GetContentAsString());
}
