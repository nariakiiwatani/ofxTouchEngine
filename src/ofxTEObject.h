#pragma once

#include "ofTexture.h"
#include <string>
#include <mutex>
#include "TouchObject.h"
#include "TEInstance.h"
#include "TEGraphicsContext.h"

class ofxTouchEngine;

extern struct ofxTEGuiReturn;

class ofxTELink
{
	friend class ofxTouchEngine;
public:
	void setup(ofxTouchEngine &engine, const std::string &identifier);
	const std::string& getIdentifier() const { return identifier_; }

	virtual void update();
	bool isFrameNew() const { return is_frame_new_; }

	ofxTEGuiReturn gui();

protected:
	ofxTouchEngine *engine_ = nullptr;
	TEInstance *instance_ = nullptr;
	TEOpenGLContext *context_= nullptr;
	std::string identifier_;

	TouchObject<TELinkInfo> info_;

	std::mutex mtx_;
	bool new_frame_arrived_ = false;
	bool is_frame_new_ = false;

	void notifyNewDataArrival();

	template<typename Src>
	bool setValue(const Src &src);
	template<typename Dst>
	bool getValue(Dst &dst) const;
	mutable TouchObject<TEObject> object_;// cache object for texture decoding. wanna remove...
};

class ofxTELinkInput : public ofxTELink
{
public:
	using ofxTELink::setValue;
	ofxTEGuiReturn gui();
};


class ofxTELinkOutput : public ofxTELink
{
public:
	using ofxTELink::getValue;
	ofxTEGuiReturn gui();
};

class ofxTELinkParameterGroup : public ofxTELink
{
public:
	void update() override;
	const std::vector<std::string>& getChildren() const { return children_; }
	ofxTEGuiReturn gui();
private:
	bool getChildren(std::vector<std::string> &dst) const;
	std::vector<std::string> children_;
};
class ofxTELinkParameter : public ofxTELink
{
public:
	using ofxTELink::setValue;
	ofxTEGuiReturn gui();
};


extern template bool ofxTELink::setValue(const std::string&);
extern template bool ofxTELink::setValue(const ofTexture&);

extern template bool ofxTELink::getValue(ofTexture&) const;
extern template bool ofxTELink::getValue(std::vector<float>&) const;
extern template bool ofxTELink::getValue(std::vector<std::vector<float>>&) const;
