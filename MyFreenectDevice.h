#ifndef MYFREENECTDEVICE_H
#define MYFREENECTDEVICE_H
#include "libfreenect.hpp"
#include <vector>
#include "Extras.h"
using namespace std;

class Mutex {
public:
	Mutex() {
		pthread_mutex_init( &m_mutex, NULL );
	}
	void lock() {
		pthread_mutex_lock( &m_mutex );
	}
	void unlock() {
		pthread_mutex_unlock( &m_mutex );
		
	}
	class ScopedLock
	{
		Mutex & _mutex;
	public:
		ScopedLock(Mutex & mutex)
		: _mutex(mutex)
		{
			_mutex.lock();
		}
		~ScopedLock()
		{
			_mutex.unlock();
		}
	};
private:
	pthread_mutex_t m_mutex;
};

class MyFreenectDevice : public Freenect::FreenectDevice {
private:
	std::vector<uint16_t> m_buffer_depth1;
	Mutex m_depth_mutex;
	bool m_new_depth_frame;
public:
	MyFreenectDevice(freenect_context *_ctx, int _index);
	~MyFreenectDevice();
	// Do not call directly even in child
	void VideoCallback(void* _rgb, uint32_t timestamp);
	void DepthCallback(void* _depth, uint32_t timestamp);
	
	bool getRGB(std::vector<uint8_t> &buffer);
	bool getDepth(double depth[h][w]);
	
	bool HayNuevoFrame() {
		return m_new_depth_frame;
	}
};


#endif

