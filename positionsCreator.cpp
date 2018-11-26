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
    parameters->add(output.set("Output", {ofPoint()}));

    
    listeners.push(positionX.newListener(this, &positionsCreator::calculateOutput));
    listeners.push(positionY.newListener(this, &positionsCreator::calculateOutput));
}


void positionsCreator::calculateOutput(vector<float> &vf){
    if(positionX->size() == positionY->size()){
        vector<ofPoint> points;
        points.resize(positionX->size());
        for(int i = 0; i < points.size(); i++){
            points[i] = ofPoint(positionX.get()[i], positionY.get()[i]);
        }
        output = points;
    }else if(positionX->size() > positionY->size()){
        vector<ofPoint> points;
        points.resize(positionX->size());
        for(int i = 0; i < points.size(); i++){
            points[i] = ofPoint(positionX.get()[i], positionY.get()[0]);
        }
        output = points;
    }else if(positionX->size() < positionY->size()){
        vector<ofPoint> points;
        points.resize(positionX->size());
        for(int i = 0; i < points.size(); i++){
            points[i] = ofPoint(positionX.get()[0], positionY.get()[i]);
        }
        output = points;
    }
}
