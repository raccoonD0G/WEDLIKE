// Fill out your copyright notice in the Description page of Project Settings.


#include "Utilities/WindowHelper.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/SWindow.h"
#include "Objects/WindowHandle.h"
#include "Blueprint/UserWidget.h"

UWindowHandle* UWindowHelper::OpenNewWindow(FString WindowTitle, int32 Width, int32 Height)
{
    TSharedRef<SWindow> NewWindow = SNew(SWindow)
        .Title(FText::FromString(WindowTitle))
        .ClientSize(FVector2D(Width, Height))
        .SupportsMinimize(true)
        .SupportsMaximize(true)
        .SizingRule(ESizingRule::UserSized)
        .UseOSWindowBorder(true)
        .AutoCenter(EAutoCenter::PreferredWorkArea)
        .CreateTitleBar(true);

    FSlateApplication::Get().AddWindow(NewWindow, true);

    UWindowHandle* NewHandle = NewObject<UWindowHandle>();
    NewHandle->WindowRef = NewWindow;

    return NewHandle;
}