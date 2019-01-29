//
//  ildaController.hpp
//  Espill-ilda
//
//  Created by Eduard Frigola on 26/06/2017.
//
//

#ifndef ildaController_h
#define ildaController_h

#include "ofMain.h"
#include "ofxEtherdream.h"
#include "ofxOceanodeNodeModelExternalWindow.h"

class ildaController : public ofxOceanodeNodeModelExternalWindow{
public:
    ildaController();
    ~ildaController(){
        if(etherdream != nullptr)
            delete etherdream;
    }
    
    void setup() override;
    
    void draw(){
        ofSetBackgroundColor(0);
        ildaFrame.draw(0, 0, ofGetWidth(), ofGetHeight());
    }
    
    void update(ofEventArgs &a) override{
        setPolylines();
        ildaFrame.update();
        if(etherdream != nullptr)
            etherdream->setPoints(ildaFrame);
    }
    
    void drawInExternalWindow(ofEventArgs &e) override {draw();};
    
    
    void setPolylines(vector<pair<ofPolyline, ofColor>> polys);
    void setPolylines();
    
private:
    void parameterChangedListener(ofAbstractParameter &param);
    
    ofxIlda::Frame ildaFrame;
    
    ofxEtherdream* etherdream;
    
    ofParameter<int>    identifier;
    ofParameter<bool>   clear;
    ofParameter<void>   resetConnection;
    ofParameter<vector<pair<ofPolyline, ofColor>>> in1;
    ofParameter<vector<pair<ofPolyline, ofColor>>> in2;
    ofParameter<vector<pair<ofPolyline, ofColor>>> in3;
    ofParameter<vector<pair<ofPolyline, ofColor>>> in4;
    ofParameter<float>  maxOpacity;
    ofParameter<int> pps;
    ofParameter<bool> capX;
    ofParameter<bool> capY;
    ofParameter<int> blankCount;
    ofParameter<int> endCount;
    ofParameter<bool> flipX;
    ofParameter<bool> flipY;
    ofParameter<int> pointCount;
    ofParameter<int> minimumPointCount;
    //    ofParameter<bool> doSmooth;
    ofParameter<bool> doSpacing;
    ofParameter<int> smoothing;
    ofParameter<float> tolerance;
    ofParameter<ofFloatColor> laserColor;
    ofParameter<float> offsetX;
    ofParameter<float> offsetY;
    ofParameter<float> scaleX;
    ofParameter<float> scaleY;
    ofParameter<glm::vec2> offset;
    ofParameter<glm::vec2> scale;
    
    ofEventListeners listeners;
    
};

#endif /* ildaController_h */
