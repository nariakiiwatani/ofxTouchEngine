#pragma once

#include "ofxTEObject.h"
#include "imgui.h"
#include <functional>

struct ofxTEGuiReturn {
	ofxTEGuiReturn(){}
	ofxTEGuiReturn(bool result):result(result){}
	ofxTEGuiReturn(std::function<void()> deleter):deleter(deleter){}
	ofxTEGuiReturn(bool result, std::function<void()> deleter)
		:result(result), deleter(deleter){}
	~ofxTEGuiReturn() {
		deleter();
	}
	operator bool() const { return result; }
	std::function<void()> deleter = []{};
	bool result = false;
};

inline ofxTEGuiReturn ofxTELink::gui()
{
	ImGui::Text("%s", info_.get()->identifier);
	return {};
}

inline ofxTEGuiReturn ofxTELinkInput::gui()
{
	return ofxTELink::gui();
}

inline ofxTEGuiReturn ofxTELinkOutput::gui()
{
	return ofxTELink::gui();
}

inline ofxTEGuiReturn ofxTELinkParameter::gui()
{
	return ofxTELink::gui();
}

inline ofxTEGuiReturn ofxTELinkParameterGroup::gui()
{
	return ofxTEGuiReturn(ImGui::Begin(info_.get()->identifier), []{ImGui::End();});
}
