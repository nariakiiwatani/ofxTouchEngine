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

static ofxTEGuiReturn makeIDScope(const char *id) {
	using namespace ImGui;
	PushID(id);
	return []{PopID();};
}

inline ofxTEGuiReturn ofxTELink::gui()
{
	auto id_scope = makeIDScope(info_.get()->identifier);
	ImGui::Text("%s", info_.get()->name);
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
	using namespace ImGui;
	auto info = info_.get();
	auto id_scope = makeIDScope(info->identifier);
	bool edited = false;
	switch(info->type) {
		case TELinkTypeBoolean: {
			bool val;
			getValue(val);
			if((edited = Checkbox(info->name, &val))) {
				setValue(val);
			}
		} break;
		case TELinkTypeString: {
			std::string val;
			getValue(val);
			char buf[256];
			strncpy(buf, val.c_str(), 255);
			if((edited = InputText(info->name, buf, 255))) {
				setValue(std::string(buf));
			}
		} break;
		case TELinkTypeDouble: {
			std::vector<float> val, minval, maxval;
			getValue(val);
			switch(info->intent) {
				case TELinkIntentColorRGBA:
					if((edited = ColorEdit4(info->name, val.data()))) {
						setValue(val);
					}
					break;
				case TELinkIntentNotSpecified:
					getValue(minval, TELinkValueMinimum);
					getValue(maxval, TELinkValueMaximum);
					for(int i = 0; i < val.size(); ++i) {
						PushID(i);
						edited |= SliderFloat(info->name, &val[i], minval[i], maxval[i]);
						PopID();
					}
					if(edited) {
						setValue(val);
					}
					break;
			}
		} break;
		default:
			ofxTELink::gui();
			break;
	}
	return {edited};
}

inline ofxTEGuiReturn ofxTELinkParameterGroup::gui()
{
	using namespace ImGui;
	auto info = info_.get();
	PushID(info->identifier);
	return {
		ImGui::Begin(info->name),
		[]{
			ImGui::End();
			PopID();
		}
	};
}
