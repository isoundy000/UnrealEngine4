// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	StatsMisc.h: misc timers and such
=============================================================================*/
#pragma once


#if STATS
/**
 * Utility class to capture time passed in seconds, adding delta time to passed
 * in variable.
 */
class FScopeSecondsCounter
{
public:
	/** Ctor, capturing start time. */
	FScopeSecondsCounter( double& InSeconds )
	:	StartTime(FPlatformTime::Seconds())
	,	Seconds(InSeconds)
	{}
	/** Dtor, updating seconds with time delta. */
	~FScopeSecondsCounter()
	{
		Seconds += FPlatformTime::Seconds() - StartTime;
	}
private:
	/** Start time, captured in ctor. */
	double StartTime;
	/** Time variable to update. */
	double& Seconds;
};

/** Macro for updating a seconds counter without creating new scope. */
#define SCOPE_SECONDS_COUNTER(Seconds) \
	FScopeSecondsCounter SecondsCount_##Seconds(Seconds);

#else
#define SCOPE_SECONDS_COUNTER(Seconds)
#endif 

/** Key contains total time, value contains total count. */
typedef TKeyValuePair<double, uint32> FTotalTimeAndCount;

/**
 *	Utility class to log time passed in seconds, adding cumulative stats to passed in variable. 
 */
struct FScopeLogTime
{
	/**
	 * Initialization constructor.
	 *
	 * @param InName - String that will be displayed in the log
	 * @param InGlobal - Pointer to the variable that holds the cumulative stats
	 *
	 */
	FScopeLogTime( const TCHAR* InName, FTotalTimeAndCount* InGlobal = nullptr );

	/** Destructor. */
	~FScopeLogTime();

protected:
	const double StartTime;
	const FString Name;
	FTotalTimeAndCount* Global;
};

#define SCOPE_LOG_TIME(Name,Global) \
	FScopeLogTime ScopeLogTime(Name,Global);