#pragma once

#include "TEInstance.h"
#include "TEGraphicsContext.h"
#include "ofConstants.h"
#include <map>
#include <string>
#include <mutex>
#include <condition_variable>

class ofxTEObjectParameter;
class ofxTEObjectParameterGroup;
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

	std::shared_ptr<ofxTEObjectParameterGroup> useParameterGroup(const std::string &identifier);
	std::shared_ptr<ofxTEObjectParameter> useParameter(const std::string &identifier);
	std::shared_ptr<ofxTEObjectInput> useInput(const std::string &identifier);
	std::shared_ptr<ofxTEObjectOutput> useOutput(const std::string &identifier);

	void update();

	void waitForFrame() const {
		std::unique_lock<std::mutex> lock(frame_mutex_);
		cv_.wait(lock, [this] { return !is_frame_busy_; });
	}

private:
	TEInstance *instance_ = nullptr;
	TEOpenGLContext *context_ = nullptr;
	bool is_loaded_ = false;

	bool is_frame_busy_ = false;
	mutable std::mutex frame_mutex_;
	mutable std::condition_variable cv_;
	void setFrameBusy(bool busy) {
		std::lock_guard<std::mutex> lock(frame_mutex_);
		is_frame_busy_ = busy;
		cv_.notify_all();
	}
	bool isFrameBusy() const {
		std::lock_guard<std::mutex> lock(frame_mutex_);
		return is_frame_busy_;
	}

	std::map<std::string, std::weak_ptr<ofxTEObjectParameterGroup>> parameter_group_;
	std::map<std::string, std::weak_ptr<ofxTEObjectParameter>> parameter_;
	std::map<std::string, std::weak_ptr<ofxTEObjectOutput>> output_;

	void eventCallback(TEEvent event, TEResult result, int64_t start_time_value, int32_t start_time_scale, int64_t end_time_value, int32_t end_time_scale);
	void linkCallback(TELinkEvent event, const std::string &identifier);

private:
	static void eventCallbackGlobal(TEInstance * instance, TEEvent event, TEResult result, int64_t start_time_value, int32_t start_time_scale, int64_t end_time_value, int32_t end_time_scale, void * info);
	static void linkCallbackGlobal(TEInstance * instance, TELinkEvent event, const char *identifier, void * info);
};