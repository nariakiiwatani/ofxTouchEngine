#pragma once

#include "ofTexture.h"
#include <string>
#include <mutex>
#include "TouchObject.h"
#include "TEInstance.h"
#include "TEGraphicsContext.h"

class ofxTouchEngine;

class ofxTEObject
{
public:
	void setup(ofxTouchEngine &engine, const std::string &identifier);
	const std::string& getIdentifier() const { return identifier_; }

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

extern template void ofxTEObjectInput::setValue(const std::string&);
extern template void ofxTEObjectInput::setValue(const ofTexture&);

class ofxTEObjectOutput : public ofxTEObject
{
friend class ofxTouchEngine;
public:
	virtual void update() override;
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

extern template bool ofxTEObjectOutput::decodeTo(ofTexture&) const;
extern template bool ofxTEObjectOutput::decodeTo(std::vector<float>&) const;
extern template bool ofxTEObjectOutput::decodeTo(std::vector<std::vector<float>>&) const;


class ofxTEObjectParameterGroup : public ofxTEObjectOutput
{
public:
	void update() override;
	const std::vector<std::string>& getChildren() const { return children_; }
private:
	bool decodeTo(std::vector<std::string> &dst) const;
	std::vector<std::string> children_;
};
class ofxTEObjectParameter : public ofxTEObjectInput, public ofxTEObjectOutput
{
public:
	using ofxTEObject::setup;
	using ofxTEObjectOutput::update;
	using ofxTEObject::getIdentifier;
};