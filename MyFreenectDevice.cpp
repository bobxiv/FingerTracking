#include "MyFreenectDevice.h"
#include <cmath>
using namespace std;

MyFreenectDevice::MyFreenectDevice(freenect_context *_ctx, int _index) 
	: 
	Freenect::FreenectDevice(_ctx, _index),
	m_buffer_depth1(freenect_find_video_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_VIDEO_RGB).bytes),
	m_new_depth_frame(false) {	}

MyFreenectDevice::~MyFreenectDevice() {
	m_buffer_depth1.clear();
}

// Do not call directly even in child
void MyFreenectDevice::VideoCallback(void* _rgb, uint32_t timestamp) {};

// Do not call directly even in child
void MyFreenectDevice::DepthCallback(void* _depth, uint32_t timestamp) {
	Mutex::ScopedLock lock(m_depth_mutex);
	uint16_t* depth = static_cast<uint16_t*>(_depth);
	std::copy(depth, depth+(w*h), m_buffer_depth1.begin());
	m_new_depth_frame = true;
}

bool MyFreenectDevice::getRGB(std::vector<uint8_t> &buffer) {
	return false;
}

bool MyFreenectDevice::getDepth(double depth[h][w] ) {
	Mutex::ScopedLock lock(m_depth_mutex);
	for(int i=0; i<h; ++i) {
		for(int j=0; j<w; ++j) {
			if( m_buffer_depth1[i*640+j] != 2047 )
				//Mapeo todo a +- 0 y 1. ademas tiro ls primeros 300 valores que no puede capturar
				depth[i][w-1-j] = double(m_buffer_depth1[i*w+j] - 300) / 1747.0;
			else
				//Si no puede medir la profundidad asigno -1
				depth[i][w-1-j] = -1;
		}
	}
//	FiltroPasaBajo(depth);
	m_new_depth_frame = false;
	return true;
}
