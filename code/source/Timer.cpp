#include "Timer.h"

namespace Rendering
{
	Timer::Timer() // ��ʱ�����캯��
		: mSecondsPerCount(0.0), mDeltaTime(-1.0), mBaseTime(0),
		mPausedTime(0), mPrevTime(0), mCurrTime(0), mStopped(false)
	{
		// �����б����֡���mDeltaTime��Ϊ-1��ʾ�����֡������Ϊ��ֵӦ��ͨ������Tick��������õ�

		__int64 countsPerSec;
		QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec); // ��ȡ���ܼ�ʱ����Ƶ��
		mSecondsPerCount = 1.0 / (double)countsPerSec;  // ����ÿ����������������
	}

	// Returns the total time elapsed since Reset() was called, NOT counting any
	// time when the clock is stopped.
	double Timer::TotalTime()const // ��ȡ��������¼����ʱ��
	{
		// If we are stopped, do not count the time that has passed since we stopped.
		// Moreover, if we previously already had a pause, the distance 
		// mStopTime - mBaseTime includes paused time, which we do not want to count.
		// To correct this, we can subtract the paused time from mStopTime:  
		//
		//                     |<--paused time-->|
		// ----*---------------*-----------------*------------*------------*------> time
		//  mBaseTime       mStopTime        startTime     mStopTime    mCurrTime

		// ��������Ǽ�����ֹͣ���� ��������ʱ�� = ����֡���� - ���������� * ÿ������������
		//                                       = ��mCurrTime - mBaseTime) * mSecondsPerCount
		// ������Ǽ�ʱ��ֹͣ����   ��ʱ����ʱ�� =  (��֡���� - �������� - ֹͣ�ڼ����) * ÿ������������
		//                                       =  ((mCurrTime-mPausedTime)-mBaseTime)*mSecondsPerCount
		// ����else�д����ʾ��ʱ����ʱδֹͣ�����¼����ʱ�����������ʽ
		// �����ʱ��ʱ���Ѿ�ֹͣ����������ʹ�ô�֡���� ��Ϊ��ʱ������ʱ�䣬��Ӧ��ʹ��ֹͣʱ����
		// ������if�д��룬��ʾ��¼��ʱ��Ĺ�ʽ���ǣ�((mStopTime - mPausedTime)-mBaseTime)*mSecondsPerCount

		if (mStopped)
		{
			return (float)(((mStopTime - mPausedTime) - mBaseTime) * mSecondsPerCount);
		}

		// The distance mCurrTime - mBaseTime includes paused time,
		// which we do not want to count.  To correct this, we can subtract 
		// the paused time from mCurrTime:  
		//
		//  (mCurrTime - mPausedTime) - mBaseTime 
		//
		//                     |<--paused time-->|
		// ----*---------------*-----------------*------------*------> time
		//  mBaseTime       mStopTime        startTime     mCurrTime

		else // ���������δֹͣ
		{
			return (float)(((mCurrTime - mPausedTime) - mBaseTime) * mSecondsPerCount);
		}
	}

	double Timer::DeltaTime()const	// ��ȡ֡���
	{
		return mDeltaTime;
	}

	void Timer::Reset() // ���ü�ʱ��
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime); // ��ȡ����ʱ�ļ���

		mBaseTime = currTime; // �������������� = ����������ʱ�ļ���ֵ
		mPrevTime = currTime; // ����������һ֡���� ������һ֡����֡���ʱ�õ��������ֵΪ��ʱ����
		mStopTime = 0;		  // ֹͣ����ֵΪ0
		mStopped = false;	  // ��ʼ������δֹͣ
	}

	void Timer::Start()   // ��ʼ��ʱ��������ʱ����ͣʱ���������ü�ʱ����
	{
		__int64 startTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&startTime);


		// Accumulate the time elapsed between stop and start pairs.
		//
		//                     |<-------d------->|
		// ----*---------------*-----------------*------------> time
		//  mBaseTime       mStopTime        startTime     

		if (mStopped) // ���������ֹͣ��
		{
			// ����ֹͣ�ڼ�ļ��� = ���¿���ʱ���� - ֹͣʱ����
			// ���ڼ��������ܶ��ֹͣ�Ϳ�ʼ��mPausedTime��¼���Ǽ�ʱ����ֹͣ�ڼ�ļ�������˴˴���"+="
			mPausedTime += (startTime - mStopTime);

			mPrevTime = startTime;	// ��¼��һ֡������������һ֡����֡���ʱ�õ�
			mStopTime = 0;			// ��ֹͣʱ��������Ϊ0
			mStopped = false;		// ���ü�ʱ��״̬Ϊδֹͣ
		}
	}

	void Timer::Stop() // ֹͣ��ʱ��
	{
		if (!mStopped)
		{
			__int64 currTime;
			QueryPerformanceCounter((LARGE_INTEGER*)&currTime); // ��ȡ��ʱ����

			mStopTime = currTime;	// ��¼��ʱ���ڴ�ʱ����ֹͣ
			mStopped = true;		// ����ʱ��״̬��Ϊ��ֹͣ
		}
	}

	void Timer::Tick()	// ÿ֡ʱ���ø��¼�ʱ��
	{
		if (mStopped)		// ���Ѿ�ֹͣ��ʱ�򲻸����κ����ݣ�������֡���Ϊ0��
		{
			mDeltaTime = 0.0;
			return;
		}

		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);    // ��ȡ��֡����
		mCurrTime = currTime;	// ʹ��currTime��¼��֡����

		// Time difference between this frame and the previous.
		// ֡������� = ����֡���� - ��֡������ * ÿ����������������
		mDeltaTime = (mCurrTime - mPrevTime) * mSecondsPerCount;

		// Prepare for next frame.
		// ������֡����Ϊ��֡��������Ϊ��֡����Ѽ�����ϣ���������һ֡����һ֡����֡������Ϊ��֡������
		mPrevTime = mCurrTime;

		// Force nonnegative.  The DXSDK's CDXUTTimer mentions that if the 
		// processor goes into a power save mode or we get shuffled to another
		// processor, then mDeltaTime can be negative.
		// �������������ʡ��ģʽ������˴���������֡���mDeltaTime����Ϊ������˴˴�ǿ����Ǹ�
		if (mDeltaTime < 0.0)
		{
			mDeltaTime = 0.0;
		}
	}

	float Timer::NowTotalTime()
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);    // 获取此帧计数

		return (float)(((currTime - mPausedTime) - mBaseTime) * mSecondsPerCount);
	}
}