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
    addParameter(positionX.set("X", {0.5}, {0}, {1}));
    addParameter(positionY.set("Y", {0.5}, {0}, {1}));
    addParameter(close.set("Close", false));
    addParameter(output.set("Output", {ofPoint()}));

    
    listeners.push(positionX.newListener(this, &positionsCreator::calculateOutput));
    listeners.push(positionY.newListener(this, &positionsCreator::calculateOutput));
}


void positionsCreator::calculateOutput(vector<float> &vf){
    if(positionX->size() == positionY->size()){
        vector<ofPoint> points;
        points.resize(close ? positionX->size() + 1 : positionX->size());
        for(int i = 0; i < points.size(); i++){
            points[i] = ofPoint(positionX.get()[i], positionY.get()[i]);
        }
        if(close) points.back() = points.front();
        output = points;
    }else if(positionX->size() > positionY->size()){
        vector<ofPoint> points;
        points.resize(close ? positionX->size() + 1 : positionX->size());
        for(int i = 0; i < points.size(); i++){
            points[i] = ofPoint(positionX.get()[i], positionY.get()[0]);
        }
        if(close) points.back() = points.front();
        output = points;
    }else if(positionX->size() < positionY->size()){
        vector<ofPoint> points;
        points.resize(close ? positionY->size() + 1 : positionY->size());
        for(int i = 0; i < points.size(); i++){
            points[i] = ofPoint(positionX.get()[0], positionY.get()[i]);
        }
        if(close) points.back() = points.front();
        output = points;
    }
}
