//
//  positionsCreator.cpp
//  Espills_controller
//
//  Created by Eduard Frigola Bagué on 16/08/2018.
//

#include "positionsCreator.h"

positionsCreator::positionsCreator() : ofxOceanodeNodeModel("Positions Creator"){
    
}


void positionsCreator::setup(){
    parameters->add(positionX.set("X", {0.5}, {0}, {1}));
    parameters->add(positionY.set("Y", {0.5}, {0}, {1}));
    parameters->add(positionZ.set("Z", {1}, {0}, {1}));
    parameters->add(output.set("Output", {glm::vec3(0.0)}));

    
    listeners.push(positionX.newListener(this, &positionsCreator::calculateOutput));
    listeners.push(positionY.newListener(this, &positionsCreator::calculateOutput));
}


void positionsCreator::calculateOutput(vector<float> &vf){
    if(positionX->size() == positionY->size()){
        vector<glm::vec3> points;
        points.resize(positionX->size());
        for(int i = 0; i < points.size(); i++){
            points[i] = glm::vec3(positionX.get()[i], positionY.get()[i], positionZ.get()[0]);
        }
        output = points;
    }else if(positionX->size() > positionY->size()){
        vector<glm::vec3> points;
        points.resize(positionX->size());
        for(int i = 0; i < points.size(); i++){
            points[i] = glm::vec3(positionX.get()[i], positionY.get()[0], positionZ.get()[0]);
        }
        output = points;
    }else if(positionX->size() < positionY->size()){
        vector<glm::vec3> points;
        points.resize(positionX->size());
        for(int i = 0; i < points.size(); i++){
            points[i] = glm::vec3(positionX.get()[0], positionY.get()[i], positionZ.get()[0]);
        }
        output = points;
    }
}
