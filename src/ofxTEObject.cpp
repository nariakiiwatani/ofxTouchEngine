#include "ofxTEObject.h"

void ofxTEObject::setup(ofxTouchEngine &engine, const std::string &identifier)
{
	instance_ = engine.getInstance();
	context_ = engine.getContext();
	identifier_ = identifier;
}
void ofxTEObject::update()
{
	bool new_frame_arrived = false;
	{
		std::lock_guard<decltype(mtx_)> lock(mtx_);
		std::swap(new_frame_arrived, new_frame_arrived_);
	}
	is_frame_new_ = new_frame_arrived;
}

void ofxTEObject::notifyNewDataArrival()
{
	std::lock_guard<decltype(mtx_)> lock(mtx_);
	new_frame_arrived_ = true;
}
