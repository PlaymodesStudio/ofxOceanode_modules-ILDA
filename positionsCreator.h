//
//  positionsCreator.h
//  Espills_controller
//
//  Created by Eduard Frigola Bagué on 16/08/2018.
//

#ifndef positionsCreator_h
#define positionsCreator_h

#include "ofxOceanodeNodeModel.h"

class positionsCreator : public ofxOceanodeNodeModel{
public:
    positionsCreator();
    ~positionsCreator(){};
    
    void setup() override;
private:
    ofEventListeners listeners;
    
    void calculateOutput(vector<float> &vf);
    
    ofParameter<vector<float>> positionX;
    ofParameter<vector<float>> positionY;
    ofParameter<vector<float>> positionZ;
    ofParameter<vector<glm::vec3>> output;
};

#endif /* positionsCreator_h */
