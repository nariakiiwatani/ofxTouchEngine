#include "ofxTouchEngine.h"
#include "ofLog.h"
#include "ofUtils.h"
#include "ofAppRunner.h"
#include "ofxTEObject.h"

using namespace std;

void ofxTouchEngine::eventCallbackGlobal(TEInstance * instance, TEEvent event, TEResult result, int64_t start_time_value, int32_t start_time_scale, int64_t end_time_value, int32_t end_time_scale, void * info)
{
	((ofxTouchEngine*)info)->eventCallback(event, result, start_time_value, start_time_scale, end_time_value, end_time_scale);
}

void ofxTouchEngine::linkCallbackGlobal(TEInstance * instance, TELinkEvent event, const char *identifier, void * info)
{
	((ofxTouchEngine*)info)->linkCallback(event, std::string(identifier));
}



ofxTouchEngine::ofxTouchEngine()
{
	auto result = TEInstanceCreate(eventCallbackGlobal, linkCallbackGlobal, this, &instance_);
	if(result != TEResultSuccess) {
		ofLogError("ofxTouchEngine") << "failed to create instance";
	}
	result = TEOpenGLContextCreate(GetDC(ofGetWin32Window()), ofGetWGLContext(), &context_);
	if(result != TEResultSuccess) {
		ofLogError("ofxTouchEngine") << "failed to create context";
	}
	result = TEInstanceAssociateGraphicsContext(instance_, context_);
	if(result != TEResultSuccess) {
		ofLogError("ofxTouchEngine") << "failed to associate context";
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
		case TEEventFrameDidFinish:
			setFrameBusy(false);
			break;
	}
}
void ofxTouchEngine::linkCallback(TELinkEvent event, const std::string &identifier)
{
	TouchObject<TELinkInfo> link;
	auto result = TEInstanceLinkGetInfo(instance_, identifier.c_str(), link.take());
	//if(result != TEResultSuccess) {
	//	ofLogError("ofxTouchEngine") << "failed to get Link info: " << identifier;
	//	return;
	//}
	std::weak_ptr<ofxTEObjectOutput> subscriber;
	if(subscriber.expired()) {
		auto found = output_.find(identifier);
		if(found != end(output_)) {
			subscriber = found->second;
		}
	}
	if(subscriber.expired()) {
		auto found = parameter_.find(identifier);
		if(found != end(parameter_)) {
			subscriber = found->second;
		}
	}
	if(subscriber.expired()) {
		auto found = parameter_group_.find(identifier);
		if(found != end(parameter_group_)) {
			subscriber = found->second;
		}
	}
	if(subscriber.expired()) {
		return;
	}
	subscriber.lock()->notifyNewDataArrival();
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

std::shared_ptr<ofxTEObjectInput> ofxTouchEngine::useInput(const std::string &identifier)
{
	auto ret = make_shared<ofxTEObjectInput>();
	ret->setup(*this, identifier);
	return ret;
}

std::shared_ptr<ofxTEObjectParameterGroup> ofxTouchEngine::useParameterGroup(const std::string &identifier)
{
	auto found = parameter_group_.find(identifier);
	if(found != end(parameter_group_) && !found->second.expired()) {
		return found->second.lock();
	}
	auto ret = make_shared<ofxTEObjectParameterGroup>();
	ret->setup(*this, identifier);
	parameter_group_[identifier] = ret;
	return ret;
}

std::shared_ptr<ofxTEObjectParameter> ofxTouchEngine::useParameter(const std::string &identifier)
{
	auto found = parameter_.find(identifier);
	if(found != end(parameter_) && !found->second.expired()) {
		return found->second.lock();
	}
	auto ret = make_shared<ofxTEObjectParameter>();
	ret->setup(*this, identifier);
	parameter_[identifier] = ret;
	return ret;
}

std::shared_ptr<ofxTEObjectOutput> ofxTouchEngine::useOutput(const std::string &identifier)
{
	auto found = output_.find(identifier);
	if(found != end(output_) && !found->second.expired()) {
		return found->second.lock();
	}
	auto ret = make_shared<ofxTEObjectOutput>();
	ret->setup(*this, identifier);
	output_[identifier] = ret;
	return ret;
}


void ofxTouchEngine::update()
{
	setFrameBusy(true);
	auto result = TEInstanceStartFrameAtTime(instance_, ofGetElapsedTimef()*1000000, 6000, false);
	if(result != TEResultSuccess) {
		setFrameBusy(false);
	}
	
	for(auto it = begin(output_); it != end(output_);) {
		it = it->second.expired() ? output_.erase(it) : ++it;
	}
	for(auto it = begin(parameter_); it != end(parameter_);) {
		it = it->second.expired() ? parameter_.erase(it) : ++it;
	}
	for(auto it = begin(parameter_group_); it != end(parameter_group_);) {
		it = it->second.expired() ? parameter_group_.erase(it) : ++it;
	}
}

