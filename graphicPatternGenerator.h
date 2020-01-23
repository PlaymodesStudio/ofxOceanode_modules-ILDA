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
    
    virtual void presetSave(ofJson &json) override{
        string positionsStr;
        for(ofPoint p : positions.get()){
            positionsStr += ofToString(p) + "_|_";
        }
        positionsStr.erase(positionsStr.end()-3, positionsStr.end());
        json["Position_points"] = positionsStr;
    }
    
    virtual void presetRecallBeforeSettingParameters(ofJson &json) override{
        opacity = vector<float>(1, 0);
        if(json.count("Position_points") == 1){
            vector<string> positionsStrVec = ofSplitString(json["Position_points"], "_|_");
            vector<glm::vec3> newPositions;
            newPositions.resize(positionsStrVec.size());
            for(int i = 0; i < newPositions.size(); i++){
                newPositions[i] = ofFromString<glm::vec3>(positionsStrVec[i]);
            }
            positions = newPositions;
        }
    }
    
    void update(ofEventArgs &a) override {
        //polylinesout = computePolylines();
        computeShader();
        output = output;
    };
    
    void draw();
    void drawInExternalWindow(ofEventArgs &e) override {draw();};
    
    void mousePressed(ofMouseEventArgs &a) override;
    void mouseReleased(ofMouseEventArgs &a) override;
    void mouseDragged(ofMouseEventArgs &a) override;
    
    vector<ofPath> computePolylines();
    void setupShader();
    void computeShader();
        
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
    
    void rgbChanged(vector<float> &f);
    
    ofEventListeners listeners;
    
    ofParameter<vector<glm::vec3>> positions;
    ofParameter<int>    positionReplicator;
    ofParameter<ofColor> color;
    ofParameter<vector<float>> color_red;
    ofParameter<vector<float>> color_green;
    ofParameter<vector<float>> color_blue;
    ofParameter<vector<float>> toCenterFigure;
    
    ofParameter<vector<int>>   numVertex;
    ofParameter<vector<float>> scalePositions;
    ofParameter<vector<float>> opacity;
    ofParameter<vector<float>> size;
    ofParameter<vector<float>> rotation;
    ofParameter<vector<float>> jitter;
    ofParameter<bool> modulationType;
    ofParameter<vector<float>> pointModulation;
    ofParameter<vector<float>> modulationAmount;
    ofParameter<vector<float>>  refollowIn;
    ofParameter<vector<float>>  endFollow;
    ofParameter<vector<float>>  offsetFollow;
    ofParameter<vector<int>>  divisions;
    ofParameter<vector<float>>  divisionSpacing;
    ofParameter<vector<float>> width;
    ofParameter<vector<float>> filled;
    ofParameter<bool> drawOnBlack;
    ofParameter<void> reloadShader;
    
    int lastPositionReplicator;
    ofParameter<ofVbo*> output;
    
    ofVbo vbo;
    ofShader shader;
    vector<ofBufferObject> buffer;
};

#endif /* graphicPatternGenerator_h */
