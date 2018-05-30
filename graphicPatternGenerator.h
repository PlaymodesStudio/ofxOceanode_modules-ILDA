//
//  graphicPatternGenerator.h
//  Espill-ilda
//
//  Created by Eduard Frigola on 21/06/2017.
//
//

#ifndef graphicPatternGenerator_h
#define graphicPatternGenerator_h

#include "ofMain.h"
#include "ofxOceanodeNodeModelExternalWindow.h"

class graphicPatternGenerator : public ofxOceanodeNodeModelExternalWindow{
public:
    graphicPatternGenerator();
    ~graphicPatternGenerator(){};
    
    void setup() override;
    
    void update(ofEventArgs &a) override {computePolylines();};
    
    void draw();
    void drawInExternalWindow(ofEventArgs &e) override {draw();};
    
    void mousePressed(ofMouseEventArgs &a) override;
    void mouseReleased(ofMouseEventArgs &a) override;
    void mouseDragged(ofMouseEventArgs &a) override;
    
    vector<pair<ofPolyline, ofColor>> computePolylines();
    
    ofParameterGroup* getGroup(){return parameters;};
    
private:
    void parameterChangedListener(ofAbstractParameter &parameter);
    int pointDraggingIndex;
    
    bool someParameterChanged;
    
    template <typename T>
    T getParameterValueForPosition(ofParameter<vector<T>> &param, int index){
        if(param.get().size() == 1 || param.get().size() <= index){
            return param.get()[0];
        }
        else{
            return param.get()[index];
        }
    }
    
    
    ofParameter<vector<ofPoint>> positions;
    ofParameter<int>    positionReplicator;
    ofParameter<ofColor> color;
    ofParameter<bool> toCenterFigure;
    
    ofParameter<vector<int>>   numVertex;
    ofParameter<vector<float>> scalePositions;
    ofParameter<vector<float>> opacity;
    ofParameter<vector<float>> size;
    ofParameter<vector<float>> rotation;
    ofParameter<vector<float>> jitter;
    ofParameter<vector<float>>  refollowIn;
    ofParameter<vector<float>>  endFollow;
    ofParameter<vector<float>>  offsetFollow;
    ofParameter<vector<int>>  divisions;
    ofParameter<vector<float>>  divisionSpacing;
    
    int lastPositionReplicator;
    ofParameter<vector<pair<ofPolyline, ofColor>>> polyLinesOut;
};

#endif /* graphicPatternGenerator_h */
