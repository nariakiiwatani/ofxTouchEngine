#pragma once

#include "ofxTouchEngine.h"
#include "ofTexture.h"
#include <string>
#include <mutex>
#include "TouchObject.h"
#include "TEInstance.h"
#include "TEGraphicsContext.h"
#include "ofGLUtils.h"
#include "ofMath.h"
#include "TEObject.h"

class ofxTEObject
{
public:
	void setup(ofxTouchEngine &engine, const std::string &identifier);

	virtual void update(){};

protected:
	ofxTouchEngine *engine_ = nullptr;
	TEInstance *instance_ = nullptr;
	TEOpenGLContext *context_= nullptr;
	std::string identifier_;
};

class ofxTEObjectInput : public ofxTEObject
{
public:
	template<typename Src>
	void setValue(const Src &src);
};

template<>
inline void ofxTEObjectInput::setValue(const std::string &src)
{
	engine_->waitForFrame();
	TouchObject<TELinkInfo> link;
	auto result = TEInstanceLinkGetInfo(instance_, identifier_.c_str(), link.take());
	if(result != TEResultSuccess) {
		return;
	}
	assert(link.get()->scope == TEScopeInput
		   && link.get()->type == TELinkTypeString
		   && link.get()->domain == TELinkDomainParameter);
	result = TEInstanceLinkSetStringValue(instance_, identifier_.c_str(), src.c_str());
	if(result != TEResultSuccess) {
		return;
	}
}

class ofxTEObjectOutput : public ofxTEObject
{
friend class ofxTouchEngine;
public:
	void update() override;
	bool isFrameNew() const { return is_frame_new_; }

	template<typename Dst>
	bool decodeTo(Dst &dst) const;
private:
	mutable TouchObject<TEObject> object_;
	std::mutex mtx_;
	bool new_frame_arrived_ = false;
	bool is_frame_new_ = false;

	void notifyNewDataArrival();
};

template<>
inline bool ofxTEObjectOutput::decodeTo(ofTexture &dst) const
{
	engine_->waitForFrame();
	TouchObject<TELinkInfo> link;
	auto result = TEInstanceLinkGetInfo(instance_, identifier_.c_str(), link.take());
	if(result != TEResultSuccess) {
		return false;
	}
	assert(link.get()->scope == TEScopeOutput
		   && link.get()->type == TELinkTypeTexture
		   && link.get()->domain == TELinkDomainOperator);
	TouchObject<TEDXGITexture> dxgi_tex;
	result = TEInstanceLinkGetTextureValue(instance_, identifier_.c_str(), TELinkValueCurrent, dxgi_tex.take());
	if(result != TEResultSuccess || dxgi_tex.get() == nullptr) {
		return false;
	}
	TEOpenGLTexture *te_tex = nullptr;
	if(TEOpenGLContextCreateTexture(context_, dxgi_tex.get(), &te_tex) != TEResultSuccess) {
		return false;
	}
	object_.take(te_tex);
	dst.setUseExternalTextureID(TEOpenGLTextureGetName(te_tex));
	auto &data = dst.getTextureData();
	data.textureTarget = TEOpenGLTextureGetTarget(te_tex);
	data.glInternalFormat = TEOpenGLTextureGetInternalFormat(te_tex);
	data.width = TEOpenGLTextureGetWidth(te_tex);
	data.height = TEOpenGLTextureGetHeight(te_tex);
	data.bFlipTexture = !TETextureIsVerticallyFlipped(te_tex);
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
	return true;
}
