#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	touch_.load("blobTrack.tox");
	src_image_ = touch_.useInput("op/in1");
	result_ = touch_.useOutput("op/out1");
	fbo_.allocate(640, 640);
}

//--------------------------------------------------------------
void ofApp::update(){
	if(touch_.isLoaded()) {
		touch_.update();
		fbo_.begin();
		ofClear(0);
		ofDrawCircle(ofGetMouseX(), ofGetMouseY(), 100);
		fbo_.end();
		src_image_->setValue(fbo_.getTexture());
		if(result_->isFrameNew()) {
			result_->getValue(info_);
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	fbo_.draw(0,0);
	if(info_.size() > 0) {
		int num_obj = info_[0];
		ofPushStyle();
		for(int i = 0; i < num_obj; ++i) {
			ofRectangle rect(info_[i*4+2], info_[i*4+3], info_[i*4+4], info_[i*4+5]);
			rect.x -= rect.width/2.f;
			rect.y -= rect.height/2.f;
			rect.x *= fbo_.getWidth();
			rect.y *= fbo_.getHeight();
			rect.width *= fbo_.getWidth();
			rect.height *= fbo_.getHeight();
			ofDrawBitmapStringHighlight(ofToString((int)info_[i*4+1]), rect.position);
			ofNoFill();
			ofSetColor(ofColor::yellow);
			ofDrawRectangle(rect);
		}
		ofPopStyle();
	}
	ofDrawBitmapStringHighlight(touch_.isLoaded() ? "loaded." : "loading...", 10, 10);
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
