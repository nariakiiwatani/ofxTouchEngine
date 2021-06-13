#pragma once

#include "ofTexture.h"
#include <string>
#include <mutex>
#include "TouchObject.h"
#include "TEInstance.h"
#include "TEGraphicsContext.h"

class ofxTouchEngine;

class ofxTELink
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

class ofxTELinkInput : public ofxTELink
{
public:
	template<typename Src>
	void setValue(const Src &src);
};

extern template void ofxTELinkInput::setValue(const std::string&);
extern template void ofxTELinkInput::setValue(const ofTexture&);

class ofxTELinkOutput : public ofxTELink
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

extern template bool ofxTELinkOutput::decodeTo(ofTexture&) const;
extern template bool ofxTELinkOutput::decodeTo(std::vector<float>&) const;
extern template bool ofxTELinkOutput::decodeTo(std::vector<std::vector<float>>&) const;


class ofxTELinkParameterGroup : public ofxTELinkOutput
{
public:
	void update() override;
	const std::vector<std::string>& getChildren() const { return children_; }
private:
	bool decodeTo(std::vector<std::string> &dst) const;
	std::vector<std::string> children_;
};
class ofxTELinkParameter : public ofxTELinkInput, public ofxTELinkOutput
{
public:
	using ofxTELink::setup;
	using ofxTELinkOutput::update;
	using ofxTELink::getIdentifier;
};