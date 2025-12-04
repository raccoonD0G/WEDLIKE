// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/LobbyWidget.h"
#include "Components/WidgetSwitcher.h"

void ULobbyWidget::OpenSetting()
{
    if (ensureMsgf(WidgetSwitcher != nullptr, TEXT("WidgetSwitcher is null on %s"), *GetName()))
    {
        const int32 Index = 1;
        WidgetSwitcher->SetActiveWidgetIndex(Index);
    }
}