#pragma once

#include "TEInstance.h"
#include "TEGraphicsContext.h"
#include "ofConstants.h"
#include <map>
#include <string>
#include <mutex>
#include <condition_variable>

class ofxTELink;
class ofxTELinkParameter;
class ofxTELinkParameterGroup;
class ofxTELinkInput;
class ofxTELinkOutput;

class ofxTouchEngine
{
public:
	ofxTouchEngine();
	~ofxTouchEngine();

	TEInstance* getInstance() { return instance_; }
	TEOpenGLContext* getContext() { return context_; }

	bool load(const std::filesystem::path &filepath);
	bool isLoaded() const { return is_loaded_; }

	std::shared_ptr<ofxTELinkParameterGroup> useParameterGroup(const std::string &identifier) { return subscribe<ofxTELinkParameterGroup>(identifier); }
	std::shared_ptr<ofxTELinkParameter> useParameter(const std::string &identifier) { return subscribe<ofxTELinkParameter>(identifier); }
	std::shared_ptr<ofxTELinkInput> useInput(const std::string &identifier) { return subscribe<ofxTELinkInput>(identifier); }
	std::shared_ptr<ofxTELinkOutput> useOutput(const std::string &identifier) { return subscribe<ofxTELinkOutput>(identifier); }

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

	template<typename Link> std::shared_ptr<Link> subscribe(const std::string &identifier);
	std::map<std::string, std::weak_ptr<ofxTELink>> subscriber_;

	void eventCallback(TEEvent event, TEResult result, int64_t start_time_value, int32_t start_time_scale, int64_t end_time_value, int32_t end_time_scale);
	void linkCallback(TELinkEvent event, const std::string &identifier);

private:
	static void eventCallbackGlobal(TEInstance * instance, TEEvent event, TEResult result, int64_t start_time_value, int32_t start_time_scale, int64_t end_time_value, int32_t end_time_scale, void * info);
	static void linkCallbackGlobal(TEInstance * instance, TELinkEvent event, const char *identifier, void * info);
};

template std::shared_ptr<ofxTELinkParameterGroup> ofxTouchEngine::subscribe(const std::string&);
template std::shared_ptr<ofxTELinkParameter> ofxTouchEngine::subscribe(const std::string&);
template std::shared_ptr<ofxTELinkInput> ofxTouchEngine::subscribe(const std::string&);
template std::shared_ptr<ofxTELinkOutput> ofxTouchEngine::subscribe(const std::string&);
