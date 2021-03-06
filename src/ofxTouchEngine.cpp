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
	//TouchObject<TELinkInfo> link;
	//auto result = TEInstanceLinkGetInfo(instance_, identifier.c_str(), link.take());
	//if(result != TEResultSuccess) {
	//	ofLogError("ofxTouchEngine") << "failed to get Link info: " << identifier;
	//	return;
	//}
	std::weak_ptr<ofxTELink> subscriber;
	auto found = subscriber_.find(identifier);
	if(found != end(subscriber_)) {
		subscriber = found->second;
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

template<typename Link>
std::shared_ptr<Link> ofxTouchEngine::subscribe(const std::string &identifier) {
	auto found = subscriber_.find(identifier);
	if(found != end(subscriber_) && !found->second.expired()) {
		auto ret = std::dynamic_pointer_cast<Link>(found->second.lock());
		if(!ret) {
			ofLogWarning("ofxTouchEngine") << "this identifier has subscribed as different type of link: " << identifier;
		}
		return ret;
	}
	auto ret = make_shared<Link>();
	ret->setup(*this, identifier);
	subscriber_[identifier] = ret;
	if(isLoaded()) {
		ret->notifyNewDataArrival();
		ret->update();
	}
	return ret;
}

void ofxTouchEngine::update()
{
	setFrameBusy(true);
	auto result = TEInstanceStartFrameAtTime(instance_, ofGetElapsedTimef()*1000000, 6000, false);
	if(result != TEResultSuccess) {
		setFrameBusy(false);
	}
	
	for(auto it = begin(subscriber_); it != end(subscriber_);) {
		if(it->second.expired()) {
			it = subscriber_.erase(it);
		}
		else {
			it->second.lock()->update();
			++it;
		}
	}
}

