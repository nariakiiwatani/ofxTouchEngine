#pragma once

#include "TEInstance.h"
#include "ofConstants.h"

class ofxTouchEngine
{
public:
	ofxTouchEngine();
	~ofxTouchEngine();

	bool load(const std::filesystem::path &filepath);
	bool isLoaded() const { return is_loaded_; }

private:
	TEInstance *instance_ = nullptr;
	bool is_loaded_ = false;

	void eventCallback(TEEvent event, TEResult result, int64_t start_time_value, int32_t start_time_scale, int64_t end_time_value, int32_t end_time_scale);
	void linkCallback(TELinkEvent event, const char *identifier);

	static void eventCallbackGlobal(TEInstance * instance, TEEvent event, TEResult result, int64_t start_time_value, int32_t start_time_scale, int64_t end_time_value, int32_t end_time_scale, void * info);
	static void linkCallbackGlobal(TEInstance * instance, TELinkEvent event, const char *identifier, void * info);

};