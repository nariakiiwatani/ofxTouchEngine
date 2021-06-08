#pragma once

#include "TEInstance.h"
#include "TEGraphicsContext.h"
#include "ofConstants.h"
#include <map>
#include <string>

class ofxTEObjectInput;
class ofxTEObjectOutput;

class ofxTouchEngine
{
public:
	ofxTouchEngine();
	~ofxTouchEngine();

	TEInstance* getInstance() { return instance_; }
	TEOpenGLContext* getContext() { return context_; }

	bool load(const std::filesystem::path &filepath);
	bool isLoaded() const { return is_loaded_; }

	std::shared_ptr<ofxTEObjectInput> useInput(const std::string &identifier);
	std::shared_ptr<ofxTEObjectOutput> subscribe(const std::string &identifier);

	void update();

private:
	TEInstance *instance_ = nullptr;
	TEOpenGLContext *context_ = nullptr;
	bool is_loaded_ = false;

	std::map<std::string, std::weak_ptr<ofxTEObjectOutput>> subscriber_;

	void eventCallback(TEEvent event, TEResult result, int64_t start_time_value, int32_t start_time_scale, int64_t end_time_value, int32_t end_time_scale);
	void linkCallback(TELinkEvent event, const std::string &identifier);

private:
	static void eventCallbackGlobal(TEInstance * instance, TEEvent event, TEResult result, int64_t start_time_value, int32_t start_time_scale, int64_t end_time_value, int32_t end_time_scale, void * info);
	static void linkCallbackGlobal(TEInstance * instance, TELinkEvent event, const char *identifier, void * info);
};