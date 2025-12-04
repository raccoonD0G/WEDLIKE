// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RankingRowWidget.generated.h"

/**
 * 
 */
UCLASS()
class EASYGAMEKIT_API URankingRowWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

public:
	void Init(const FString& InPlayerName, int32 InScore, float Duration = 0.0f);

	FORCEINLINE int32 GetScore() const { return CurrentScore; }

	void SetRankNumText(int32 InNum);

	void SetRankIconImage(const FSlateBrush& Brush);

	void SetRankFrame(const FSlateBrush& Brush);
	void SetPlayerName(const FString& PlayerName);

	void SetScore(int32 Score);

	UFUNCTION(BlueprintNativeEvent)
	void Highlight();

	void RefreshWidget();

	void SetShowRankIcon(bool bInShowRankIcon);

	void SetShowFrame(bool bInShowFrame);

private:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* RankNumText;

	UPROPERTY(meta = (BindWidget))
	class UImage* RankIconImage;

	UPROPERTY(meta = (BindWidget))
	class UImage* RankFrameImage;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PlayerNameText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ScoreText;

	UPROPERTY(VisibleAnywhere)
	int32 CurrentScore;

	UPROPERTY(EditAnywhere)
	uint8 bShowFrame : 1;

	UPROPERTY(EditAnywhere)
	uint8 bShowRankIcon : 1;

	UPROPERTY(EditAnywhere)
	FSlateBrush RankIcon;

	UPROPERTY(EditAnywhere)
	FAnchors RowAchors = FAnchors(0.5f, 0.5f);

	UPROPERTY(EditAnywhere)
	float OffsetLeft = 0.f;

	UPROPERTY(EditAnywhere)
	float OffsetTop = 0.f;

	UPROPERTY(EditAnywhere)
	float OffsetRight = 0.f;

	UPROPERTY(EditAnywhere)
	float OffsetBottom = 0.f;

// Move Section
public:
	void StartMoveTo(const FMargin& InOffset, float InDuration);
	void UpdateMove();

private:
	FMargin MoveStartOffset;

	FMargin MoveTargetOffset;

	float MoveElapsed = 0.f;

	float MoveDuration = 0.25f;

	FTimerHandle MoveTimer;

// Score Animation Section
private:
	FTimerHandle ScoreTimerHandle;
	int32 TargetScore = 0;
	float AnimationDuration = 0.0f;
	float AnimationTimeElapsed = 0.0f;
	const float TimerInterval = 0.033f;

	// 타이머에 의해 주기적으로 호출될 함수
	void UpdateScoreAnimation();

public:
	void SetScoreDelayed(int32 Score, float Duration);
};
