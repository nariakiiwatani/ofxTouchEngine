#include "ofxTEObject.h"
#include "ofxTouchEngine.h"
#include "ofGLUtils.h"
#include "ofMath.h"
#include "TEObject.h"

void ofxTELink::setup(ofxTouchEngine &engine, const std::string &identifier)
{
	engine_ = &engine;
	instance_ = engine.getInstance();
	context_ = engine.getContext();
	identifier_ = identifier;
}

void ofxTELink::update()
{
	bool new_frame_arrived = false;
	{
		std::lock_guard<decltype(mtx_)> lock(mtx_);
		std::swap(new_frame_arrived, new_frame_arrived_);
	}
	if((is_frame_new_ = new_frame_arrived)) {
		auto result = TEInstanceLinkGetInfo(instance_, identifier_.c_str(), info_.take());
		if(result != TEResultSuccess) {
			return;
		}
	}
}

void ofxTELink::notifyNewDataArrival()
{
	std::lock_guard<decltype(mtx_)> lock(mtx_);
	new_frame_arrived_ = true;
}

template<>
bool ofxTELink::setValue(const bool &src)
{
	assert(info_.get());
	engine_->waitForFrame();
	TELinkInfo *link = info_.get();
	assert(link->scope == TEScopeInput
		   && link->type == TELinkTypeBoolean
		   && link->domain == TELinkDomainParameter);
	auto result = TEInstanceLinkSetBooleanValue(instance_, identifier_.c_str(), src);
	if(result != TEResultSuccess) {
		return false;
	}
	return true;
}

template<>
bool ofxTELink::setValue(const std::vector<double> &src)
{
	assert(info_.get());
	engine_->waitForFrame();
	TELinkInfo *link = info_.get();
	assert(link->scope == TEScopeInput
		   && link->type == TELinkTypeDouble
		   && link->domain == TELinkDomainParameter);
	auto result = TEInstanceLinkSetDoubleValue(instance_, identifier_.c_str(), src.data(), src.size());
	if(result != TEResultSuccess) {
		return false;
	}
	return true;
}

template<>
bool ofxTELink::setValue(const std::vector<float> &src)
{
	assert(info_.get());
	engine_->waitForFrame();
	TELinkInfo *link = info_.get();
	assert(link->scope == TEScopeInput
		   && link->domain == TELinkDomainParameter);
	switch(link->type) {
		case TELinkTypeFloatBuffer: {
			auto buffer = TouchObject<TEFloatBuffer>::make_take(TEFloatBufferCreate(-1, src.size(), 1, nullptr));
			TEInstanceLinkSetFloatBufferValue(instance_, link->identifier, buffer.get());
		} break;
		case TELinkTypeDouble: {
			std::vector<double> buffer(src.size());
			for(int i = 0; i < src.size(); ++i) {
				buffer[i] = src[i];
			}
			if(!setValue(buffer)) {
				return false;
			}
		} break;
		default:
			assert(false);
	}
	return true;
}

template<>
bool ofxTELink::setValue(const std::string &src)
{
	assert(info_.get());
	engine_->waitForFrame();
	TELinkInfo *link = info_.get();
	assert(link->scope == TEScopeInput
		   && link->type == TELinkTypeString
		   && link->domain == TELinkDomainParameter);
	auto result = TEInstanceLinkSetStringValue(instance_, identifier_.c_str(), src.c_str());
	if(result != TEResultSuccess) {
		return false;
	}
	return true;
}

template<>
bool ofxTELink::setValue(const ofTexture &src)
{
	assert(info_.get());
	engine_->waitForFrame();
	TELinkInfo *link = info_.get();
	assert(link->scope == TEScopeInput
		   && link->type == TELinkTypeTexture
		   && link->domain == TELinkDomainOperator);
	auto &&data = src.getTextureData();
	TouchObject<TETexture> tex;
	tex.take(TEOpenGLTextureCreate(data.textureID, data.textureTarget, data.glInternalFormat, data.tex_w, data.tex_h, data.bFlipTexture, nullptr, nullptr));
	auto result = TEInstanceLinkSetTextureValue(instance_, identifier_.c_str(), tex.get(), context_);
	if(result != TEResultSuccess) {
		return false;
	}
	return true;
}

template<>
bool ofxTELink::getValue(ofTexture &dst, TELinkValue which) const
{
	assert(info_.get());
	engine_->waitForFrame();
	TELinkInfo *link = info_.get();
	assert(link->scope == TEScopeOutput
		   && link->type == TELinkTypeTexture
		   && link->domain == TELinkDomainOperator);
	TouchObject<TEDXGITexture> dxgi_tex;
	auto  result = TEInstanceLinkGetTextureValue(instance_, identifier_.c_str(), which, dxgi_tex.take());
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
bool ofxTELink::getValue(bool &dst, TELinkValue which) const
{
	assert(info_.get());
	engine_->waitForFrame();
	TELinkInfo *link = info_.get();
	assert(link->type == TELinkTypeBoolean);
	auto result = TEInstanceLinkGetBooleanValue(instance_, identifier_.c_str(), which, &dst);
	if(result != TEResultSuccess) {
		return false;
	}
	return true;
}

template<>
bool ofxTELink::getValue(std::vector<double> &dst, TELinkValue which) const
{
	assert(info_.get());
	engine_->waitForFrame();
	TELinkInfo *link = info_.get();
	assert(link->type == TELinkTypeDouble);
	dst.resize(link->count);
	auto result = TEInstanceLinkGetDoubleValue(instance_, identifier_.c_str(), which, dst.data(), link->count);
	if(result != TEResultSuccess) {
		return false;
	}
	return true;
}

template<>
bool ofxTELink::getValue(std::string &dst, TELinkValue which) const
{
	assert(info_.get());
	engine_->waitForFrame();
	TELinkInfo *link = info_.get();
	assert(link->type == TELinkTypeString);
	TouchObject<TEString> buffer;
	auto result = TEInstanceLinkGetStringValue(instance_, identifier_.c_str(), which, buffer.take());
	if(result != TEResultSuccess || buffer.get() == nullptr) {
		return false;
	}
	dst = std::string(buffer.get()->string);
	return true;
}

template<>
bool ofxTELink::getValue(std::vector<float> &dst, TELinkValue which) const
{
	assert(info_.get());
	engine_->waitForFrame();
	TELinkInfo *link = info_.get();
	switch(link->type) {
		case TELinkTypeFloatBuffer: {
			TouchObject<TEFloatBuffer> buffer;
			auto result = TEInstanceLinkGetFloatBufferValue(instance_, identifier_.c_str(), which, buffer.take());
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
		} break;
		case TELinkTypeDouble: {
			std::vector<double> buf;
			if(!getValue(buf, which)) {
				return false;
			}
			dst.resize(buf.size());
			for(int i = 0; i < buf.size(); ++i) {
				dst[i] = buf[i];
			}
		} break;
		default:
			assert(false);
	}
	return true;
}

template<>
bool ofxTELink::getValue(std::vector<std::vector<float>> &dst, TELinkValue which) const
{
	assert(info_.get());
	engine_->waitForFrame();
	TELinkInfo *link = info_.get();
	assert(link->scope == TEScopeOutput
		   && link->type == TELinkTypeFloatBuffer
		   && link->domain == TELinkDomainOperator);
	TouchObject<TEFloatBuffer> buffer;
	auto result = TEInstanceLinkGetFloatBufferValue(instance_, identifier_.c_str(), which, buffer.take());
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

void ofxTELinkParameterGroup::update()
{
	ofxTELink::update();
	getChildren(children_);
}
bool ofxTELinkParameterGroup::getChildren(std::vector<std::string> &dst) const
{
	engine_->waitForFrame();
	TouchObject<TEStringArray> obj;
	auto result = TEInstanceLinkGetChildren(instance_, identifier_.c_str(), obj.take());
	if(result != TEResultSuccess || obj.get() == nullptr) {
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
