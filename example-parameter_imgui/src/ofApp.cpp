#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	tox_.load("text.tox");
	page_ = tox_.useParameterGroup("pg/Text");

	gui_.setup();
}

//--------------------------------------------------------------
void ofApp::update(){
	if(tox_.isLoaded()) {
		tox_.update();
		page_->update();
		if(page_->isFrameNew()) {
			auto children = page_->getChildren();
			parameter_.clear();
			for(auto &&c : children) {
				parameter_.push_back(tox_.useParameter(c));
			}
		}
		for(auto &&p : parameter_) {
			p->update();
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	using namespace ImGui;
	gui_.begin();

	if(Begin(page_->getIdentifier().c_str())) {
		for(auto &&p : parameter_) {
			Text("%s", p->getIdentifier().c_str());
		}
	}
	End();

	gui_.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
