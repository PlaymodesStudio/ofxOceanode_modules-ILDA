//
//  positionsSaver.h
//  Espill-ilda
//
//  Created by Eduard Frigola on 27/06/2017.
//
//

#ifndef positionsSaver_h
#define positionsSaver_h

#include "ofxOceanodeNodeModel.h"

class positionSaver : public ofxOceanodeNodeModel{
public:
    positionSaver() : ofxOceanodeNodeModel("Position Saver") {};
    ~positionSaver(){};
    
    void setup() override;
    
    void presetHasLoaded() override{
        positionToMoveMirrors = positionToMoveMirrors;
        positionToMoveMirrorsAndStaticMirrors = positionToMoveMirrorsAndStaticMirrors;
        positionToStaticMirrors = positionToStaticMirrors;
        positionToCenter = positionToCenter;
    }
    
private:
    
    void savePositionsListener(bool &b);
    void loadPositionsListener(bool &b);
    
    ofParameter<vector<ofPoint>> positionToMoveMirrors;
    ofParameter<vector<ofPoint>> positionToMoveMirrorsAndStaticMirrors;
    ofParameter<vector<ofPoint>> positionToStaticMirrors;
    ofParameter<vector<ofPoint>> positionToCenter;
    ofParameter<bool>   savePositions;
    ofParameter<bool>   loadPositions;
};

#endif /* positionsSaver_h */
