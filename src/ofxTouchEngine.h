#pragma once

#include "TEInstance.h"

class ofxTouchEngine
{
public:
	ofxTouchEngine();
	~ofxTouchEngine();

private:
	TEInstance *instance_;

	void eventCallback(TEEvent event, TEResult result, int64_t start_time_value, int32_t start_time_scale, int64_t end_time_value, int32_t end_time_scale);
	void linkCallback(TELinkEvent event, const char *identifier);

	static void eventCallbackGlobal(TEInstance * instance, TEEvent event, TEResult result, int64_t start_time_value, int32_t start_time_scale, int64_t end_time_value, int32_t end_time_scale, void * info);
	static void linkCallbackGlobal(TEInstance * instance, TELinkEvent event, const char *identifier, void * info);

};