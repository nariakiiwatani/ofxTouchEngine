#include "ofxTouchEngine.h"
#include "ofLog.h"
#include "ofUtils.h"
#include "ofAppRunner.h"
#include "ofMath.h"
#include "ofGLUtils.h"

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
	}
}
void ofxTouchEngine::linkCallback(TELinkEvent event, const std::string &identifier)
{
	switch(event) {
		case TELinkEventValueChange: {
			TELinkInfo* link = nullptr;
			auto result = TEInstanceLinkGetInfo(instance_, identifier.c_str(), &link);
			if(result == TEResultSuccess && link->scope == TEScopeOutput) {
				switch (link->type) {
					case TELinkTypeTexture:
						std::lock_guard<std::mutex> lock(mutex_);
						pending_textures_.push_back(identifier);
						break;
				}
			}
		} break;
	}
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

void ofxTouchEngine::update()
{
	TEInstanceStartFrameAtTime(instance_, ofGetElapsedTimef()*1000000, 6000, false);

	decltype(pending_textures_) cpy;
	{
		std::lock_guard<std::mutex> lock(mutex_);
		cpy = pending_textures_;
		pending_textures_.clear();
	}
	for(auto &&name : cpy) {
		TEDXGITexture *texture;
		auto result = TEInstanceLinkGetTextureValue(instance_, name.c_str(), TELinkValueCurrent, &texture);
		if(result == TEResultSuccess && texture != nullptr) {
			auto &&obj = textures_[name];
			if(TEOpenGLContextCreateTexture(context_, texture, obj.te.take()) == TEResultSuccess) {
				obj.of.setUseExternalTextureID(TEOpenGLTextureGetName(obj.te));
				auto &data = obj.of.getTextureData();
				data.textureTarget = TEOpenGLTextureGetTarget(obj.te);
				data.glInternalFormat = TEOpenGLTextureGetInternalFormat(obj.te);
				data.width = TEOpenGLTextureGetWidth(obj.te);
				data.height = TEOpenGLTextureGetHeight(obj.te);
				data.bFlipTexture = !TETextureIsVerticallyFlipped(obj.te);
				if(ofGLSupportsNPOTTextures()){
					data.tex_w = data.width;
					data.tex_h = data.height;
				}
				else {
					data.tex_w = ofNextPow2(data.width);
					data.tex_h = ofNextPow2(data.height);
				}
				data.tex_t = data.width / data.tex_w;
				data.tex_u = data.height / data.tex_h;
			}
			TERelease(&texture);
		}
	}
}

ofTexture ofxTouchEngine::getTexture(const std::string &identifier)
{
	auto found = textures_.find(identifier);
	if(found == end(textures_)) {
		return ofTexture();
	}
	return found->second.of;
}
