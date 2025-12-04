// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RankingBoardWidget.generated.h"

/**
 * 
 */
UCLASS()
class EASYGAMEKIT_API URankingBoardWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* RankingCanvas;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class URankingRowWidget> RankingRowWidgetClass;

// Rows Section
public:
	class URankingRowWidget* AddRankingRowAnimated(const FString& InPlayerName, int32 InScore, float ScoreDuration = 0.0f);

	class URankingRowWidget* AddRankingRowDirect(const FString& InPlayerName, int32 InScore, float ScoreDuration = 0.0f);

	void HighlightRow(int32 Index);

	bool GetIndexOfRow(class URankingRowWidget* RowWidget, int32& OutIndex) const;

private:
	UPROPERTY(EditAnywhere)
	float RowMargin = 100.f;

	UPROPERTY(EditAnywhere)
	float SlideInStartX = -600.f;

	UPROPERTY(EditAnywhere)
	float MoveDuration = 0.25f;

	UPROPERTY(EditAnywhere)
	float SlideDuration = 0.25f;

	UPROPERTY(EditAnywhere)
	int32 RowCount = 0;

	UPROPERTY(EditAnywhere)
	int32 ShowFrameRawCount = 3;

	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<class URankingRowWidget>> Rows;

	UPROPERTY(EditAnywhere)
	TArray<FSlateBrush> RankIcons;

	void SetRankIcon(URankingRowWidget* InRow);

	void SetRankFrame(URankingRowWidget* InRow);

	void SetRankNumTexts();
};
