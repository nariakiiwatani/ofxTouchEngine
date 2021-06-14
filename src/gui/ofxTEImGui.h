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
	using namespace ImGui;
	auto info = info_.get();
	PushID(info->identifier);
	return []{PopID();};
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
	auto id_scope = ofxTELink::gui();
	bool edited = false;
	auto info = info_.get();
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
			std::vector<double> val, minval, maxval;
			getValue(val);
			getValue(minval, TELinkValueMinimum);
			getValue(maxval, TELinkValueMaximum);
			std::vector<float> fval(val.size());
			for(int i = 0; i < val.size(); ++i) {
				fval[i] = val[i];
			}
			switch(info->intent) {
				case TELinkIntentColorRGBA:
					if((edited = ColorEdit4(info->name, fval.data()))) {
						for(int i = 0; i < val.size(); ++i) {
							val[i] = fval[i];
						}
						setValue(val);
					}
					break;
				case TELinkIntentNotSpecified:
					for(int i = 0; i < val.size(); ++i) {
						PushID(i);
						if((edited |= SliderFloat(info->name, &fval[i], minval[i], maxval[i]))) {
							val[i] = fval[i];
						}
						PopID();
					}
					if(edited) {
						setValue(val);
					}
					break;
			}
		} break;
		default:
			Text("%s", info->name);
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
