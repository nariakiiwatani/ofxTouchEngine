#include "ofxTEObject.h"
#include "ofxTouchEngine.h"
#include "ofGLUtils.h"
#include "ofMath.h"
#include "TEObject.h"

void ofxTEObject::setup(ofxTouchEngine &engine, const std::string &identifier)
{
	engine_ = &engine;
	instance_ = engine.getInstance();
	context_ = engine.getContext();
	identifier_ = identifier;
}


template<>
void ofxTEObjectInput::setValue(const std::string &src)
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

template<>
void ofxTEObjectInput::setValue(const ofTexture &src)
{
	engine_->waitForFrame();
	TouchObject<TELinkInfo> link;
	auto result = TEInstanceLinkGetInfo(instance_, identifier_.c_str(), link.take());
	if(result != TEResultSuccess) {
		return;
	}
	assert(link.get()->scope == TEScopeInput
		   && link.get()->type == TELinkTypeTexture
		   && link.get()->domain == TELinkDomainOperator);
	auto &&data = src.getTextureData();
	TouchObject<TETexture> tex;
	tex.take(TEOpenGLTextureCreate(data.textureID, data.textureTarget, data.glInternalFormat, data.tex_w, data.tex_h, data.bFlipTexture, nullptr, nullptr));
	result = TEInstanceLinkSetTextureValue(instance_, identifier_.c_str(), tex.get(), context_);
	if(result != TEResultSuccess) {
		return;
	}
}


void ofxTEObjectOutput::update()
{
	bool new_frame_arrived = false;
	{
		std::lock_guard<decltype(mtx_)> lock(mtx_);
		std::swap(new_frame_arrived, new_frame_arrived_);
	}
	is_frame_new_ = new_frame_arrived;
}

void ofxTEObjectOutput::notifyNewDataArrival()
{
	std::lock_guard<decltype(mtx_)> lock(mtx_);
	new_frame_arrived_ = true;
}


template<>
bool ofxTEObjectOutput::decodeTo(ofTexture &dst) const
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

template<>
bool ofxTEObjectOutput::decodeTo(std::vector<float> &dst) const
{
	engine_->waitForFrame();
	TouchObject<TELinkInfo> link;
	auto result = TEInstanceLinkGetInfo(instance_, identifier_.c_str(), link.take());
	if(result != TEResultSuccess) {
		return false;
	}
	assert(link.get()->scope == TEScopeOutput
		   && link.get()->type == TELinkTypeFloatBuffer
		   && link.get()->domain == TELinkDomainOperator);
	TouchObject<TEFloatBuffer> buffer;
	TEObject *obj;
	result = TEInstanceLinkGetFloatBufferValue(instance_, identifier_.c_str(), TELinkValueCurrent, (TEFloatBuffer**)(&obj));
	if(result != TEResultSuccess || buffer.get() == nullptr) {
		return false;
	}
	auto buf = buffer.get();
	auto num_channels = TEFloatBufferGetChannelCount(buf);
	auto num_samples = TEFloatBufferGetCapacity(buf);
	assert(num_channels == 1 || num_samples == 1);
	dst.resize(num_channels * num_samples);
	auto value = TEFloatBufferGetValues(buf);
	for(auto ch = 0; ch < num_channels; ++ch) {
		for(auto sample = 0; sample < num_samples; ++sample) {
			dst[ch*num_samples+sample] = value[ch][sample];
		}
	}
	return true;
}

template<>
bool ofxTEObjectOutput::decodeTo(std::vector<std::vector<float>> &dst) const
{
	engine_->waitForFrame();
	TouchObject<TELinkInfo> link;
	auto result = TEInstanceLinkGetInfo(instance_, identifier_.c_str(), link.take());
	if(result != TEResultSuccess) {
		return false;
	}
	assert(link.get()->scope == TEScopeOutput
		   && link.get()->type == TELinkTypeFloatBuffer
		   && link.get()->domain == TELinkDomainOperator);
	TouchObject<TEFloatBuffer> buffer;
	result = TEInstanceLinkGetFloatBufferValue(instance_, identifier_.c_str(), TELinkValueCurrent, buffer.take());
	if(result != TEResultSuccess || buffer.get() == nullptr) {
		return false;
	}
	auto buf = buffer.get();
	auto num_channels = TEFloatBufferGetChannelCount(buf);
	auto num_samples = TEFloatBufferGetCapacity(buf);
	dst.resize(num_channels);
	auto value = TEFloatBufferGetValues(buf);
	for(auto ch = 0; ch < num_channels; ++ch) {
		dst[ch].assign(value[ch], value[ch]+num_samples);
	}
	return true;
}

void ofxTEObjectParameterGroup::update()
{
	ofxTEObjectOutput::update();
	if(isFrameNew()) {
		decodeTo(children_);
	}
}
bool ofxTEObjectParameterGroup::decodeTo(std::vector<std::string> &dst) const
{
	TouchObject<TEStringArray> obj;
	auto result = TEInstanceLinkGetChildren(instance_, identifier_.c_str(), obj.take());
	if(result != TEResultSuccess && obj.get() != nullptr) {
		return false;
	}
	auto names = obj.get();
	dst.clear();
	dst.reserve(names->count);
	for(int i = 0; i < names->count; ++i) {
		dst.emplace_back(names->strings[i]);
	}
	return true;
}
