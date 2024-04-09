#pragma once

#include <windows.h>

namespace Rendering
{
	class Timer
	{
	public:
		Timer();

		double TotalTime()const;
		double DeltaTime()const;

		void Reset(); 
		void Start(); 
		void Stop();  
		void Tick();  

		float NowTotalTime();

	private:
		double mSecondsPerCount;	
		double mDeltaTime;			

		__int64 mBaseTime;			
		__int64 mPausedTime;		
		__int64 mStopTime;			
		__int64 mPrevTime;			
		__int64 mCurrTime;			

		bool mStopped;				
	};
}