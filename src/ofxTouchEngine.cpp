#include "ofxTouchEngine.h"
#include "ofLog.h"

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
	TEResult result = TEInstanceCreate(eventCallbackGlobal, linkCallbackGlobal, this, &instance_);
	if(result != TEResultSuccess) {
		ofLogError("ofxTouchEngine") << "failed to create instance";
	}
}

ofxTouchEngine::~ofxTouchEngine()
{
	TERelease(&instance_);
}

void ofxTouchEngine::eventCallback(TEEvent event, TEResult result, int64_t start_time_value, int32_t start_time_scale, int64_t end_time_value, int32_t end_time_scale)
{
}
void ofxTouchEngine::linkCallback(TELinkEvent event, const char *identifier)
{
}
