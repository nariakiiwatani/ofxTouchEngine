#pragma once

#include "TEInstance.h"
#include "ofConstants.h"
#include "TouchObject.h"
#include "TETexture.h"
#include "ofTexture.h"
#include <map>
#include <string>
#include <mutex>

class ofxTouchEngine
{
public:
	ofxTouchEngine();
	~ofxTouchEngine();

	bool load(const std::filesystem::path &filepath);
	bool isLoaded() const { return is_loaded_; }

	void update();

	ofTexture getTexture(const std::string &identifier);

private:
	TEInstance *instance_ = nullptr;
	TEOpenGLContext *context_ = nullptr;
	bool is_loaded_ = false;

	struct TextureMap {
		TouchObject<TEOpenGLTexture> te;
		ofTexture of;
	};
	std::map<std::string, TextureMap> textures_;

	std::mutex mutex_;
	std::vector<std::string> pending_textures_;

	void eventCallback(TEEvent event, TEResult result, int64_t start_time_value, int32_t start_time_scale, int64_t end_time_value, int32_t end_time_scale);
	void linkCallback(TELinkEvent event, const std::string &identifier);

	static void eventCallbackGlobal(TEInstance * instance, TEEvent event, TEResult result, int64_t start_time_value, int32_t start_time_scale, int64_t end_time_value, int32_t end_time_scale, void * info);
	static void linkCallbackGlobal(TEInstance * instance, TELinkEvent event, const char *identifier, void * info);

};