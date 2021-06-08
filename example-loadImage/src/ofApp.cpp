#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	touch_.load("moviefilein.tox");
	filename_ = touch_.useInput("pn/File");
	subscriber_ = touch_.subscribe("op/out1");
}

//--------------------------------------------------------------
void ofApp::update(){
	if(touch_.isLoaded()) {
		touch_.update();
		subscriber_->update();
		if(subscriber_->isFrameNew()) {
			subscriber_->decodeTo(tex_);
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	if(tex_.isAllocated()) {
		ofRectangle rect(0,0,tex_.getWidth(),tex_.getHeight());
		rect.scaleTo(ofGetCurrentViewport());
		tex_.draw(rect);
	}
	ofDrawBitmapStringHighlight(touch_.isLoaded() ? "D&D any image or movie file into this window to display" : "loading...", 10, 10);
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
	filename_->setValue(dragInfo.files[0]);
}
