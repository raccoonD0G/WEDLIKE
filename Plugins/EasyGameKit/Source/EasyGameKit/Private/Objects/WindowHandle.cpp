// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/WindowHandle.h"
#include "Blueprint/UserWidget.h"

void UWindowHandle::BeginDestroy()
{
    Super::BeginDestroy();

    TSharedPtr<SWindow> PinnedWindow = WindowRef.Pin();
    if (PinnedWindow.IsValid())
    {
        PinnedWindow->RequestDestroyWindow();
    }
}

void UWindowHandle::SetWindowContent(UUserWidget* WidgetToDisplay)
{
    if (!IsValid(WidgetToDisplay))
    {
        return;
    }

    if (!WindowRef.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("Target Window is no longer valid (Closed?)"));
        return;
    }

    if (WidgetToDisplay->IsInViewport())
    {
        WidgetToDisplay->RemoveFromParent();
    }

    TSharedPtr<SWindow> ActualWindow = WindowRef.Pin();

    if (ActualWindow.IsValid())
    {
        ActualWindow->SetContent(WidgetToDisplay->TakeWidget());
    }
}