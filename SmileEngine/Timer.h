#ifndef __TIMER_H__
#define __TIMER_H__

#include "SmileSetup.h"
#include "SDL\include\SDL.h"

class Timer
{
public:

	// Constructor
	Timer(bool start = true);

	void Start();
	void Stop();
	void Resume(); 
	Uint32 Read();
	Uint32 ReadSec();

	bool IsRunning() const { return running; }; 
private:

	bool	running = false;
	Uint32	started_at;
	Uint32	stopped_at;

	friend class SmileGameTimeManager; 
};

#endif //__TIMER_H__