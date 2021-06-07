#include "ofxTouchEngine.h"
#include "ofLog.h"
#include "ofUtils.h"

void ofxTouchEngine::eventCallbackGlobal(TEInstance * instance, TEEvent event, TEResult result, int64_t start_time_value, int32_t start_time_scale, int64_t end_time_value, int32_t end_time_scale, void * info)
{
	((ofxTouchEngine*)info)->eventCallback(event, result, start_time_value, start_time_scale, end_time_value, end_time_scale);
}

void ofxTouchEngine::linkCallbackGlobal(TEInstance * instance, TELinkEvent event, const char *identifier, void * info)
{
	((ofxTouchEngine*)info)->linkCallback(event, identifier);
}



ofxTouchEngine::ofxTouchEngine()
{
	auto result = TEInstanceCreate(eventCallbackGlobal, linkCallbackGlobal, this, &instance_);
	if(result != TEResultSuccess) {
		ofLogError("ofxTouchEngine") << "failed to create instance";
	}
}

ofxTouchEngine::~ofxTouchEngine()
{
	if(instance_) {
		TERelease(&instance_);
	}
}

void ofxTouchEngine::eventCallback(TEEvent event, TEResult result, int64_t start_time_value, int32_t start_time_scale, int64_t end_time_value, int32_t end_time_scale)
{
	switch(event) {
		case TEEventInstanceDidLoad:
			is_loaded_ = true;
			break;
	}
}
void ofxTouchEngine::linkCallback(TELinkEvent event, const char *identifier)
{
}

bool ofxTouchEngine::load(const std::filesystem::path &filepath)
{
	is_loaded_ = false;
	auto result = TEInstanceLoad(instance_, ofToDataPath(filepath, true).c_str(), TETimeExternal);
	if(result != TEResultSuccess) {
		ofLogError("ofxTouchEngine") << "failed to load a tox file";
		return false;
	}
	result = TEInstanceResume(instance_);
	if(result != TEResultSuccess) {
		ofLogError("ofxTouchEngine") << "failed to resume instance";
		return false;
	}
	return true;
}
