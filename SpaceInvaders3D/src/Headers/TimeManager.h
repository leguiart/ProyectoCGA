#ifndef _TimeManager_H
#define _TimeManager_H

#include <chrono>
#include <thread>

class TimeManager {
public:

	static TimeManager& Singleton() {
		static TimeManager singleton;

		return singleton;
	}

	double CalculateFrameRate(bool writeToConsole);

	double GetTime();

	void Sleep(int milliseconds);

	double DeltaTime = 0;

	double CurrentTime = 0;

private:

	TimeManager() {
	}
	;
	TimeManager(TimeManager const&);
	TimeManager& operator=(TimeManager const&);
};

#endif
