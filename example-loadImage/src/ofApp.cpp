#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	touch_.load("moviefilein.tox");
}

//--------------------------------------------------------------
void ofApp::update(){
	if(touch_.isLoaded()) {
		touch_.update();
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	if(touch_.isLoaded()) {
		touch_.getTexture("op/out1").draw(0, 0);
	}
	ofDrawBitmapStringHighlight(touch_.isLoaded() ? "test.tox loaded from openFrameworks" : "not loaded", 10, 10);
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
