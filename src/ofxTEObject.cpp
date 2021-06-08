#include "ofxTEObject.h"

void ofxTEObject::setup(ofxTouchEngine &engine, const std::string &identifier)
{
	engine_ = &engine;
	instance_ = engine.getInstance();
	context_ = engine.getContext();
	identifier_ = identifier;
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
