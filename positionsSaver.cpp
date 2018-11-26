//
//  positionsSaver.cpp
//  Espill-ilda
//
//  Created by Eduard Frigola on 27/06/2017.
//
//

#include "positionsSaver.h"

void positionSaver::setup(){
    parameters->add(savePositions.set("Save Positions", false));
    parameters->add(loadPositions.set("Load Positions", false));
    parameters->add(positionToMoveMirrors.set("Mov Mirrors", {ofPoint()}));
    parameters->add(positionToMoveMirrorsAndStaticMirrors.set("Mov and Static Mirrors", {ofPoint()}));
    parameters->add(positionToStaticMirrors.set("Static Mirrors", {ofPoint()}));
    parameters->add(positionToCenter.set("Center", {ofPoint()}));
    
    savePositions.addListener(this, &positionSaver::savePositionsListener);
    loadPositions.addListener(this, &positionSaver::loadPositionsListener);
    
}

void positionSaver::savePositionsListener(bool &b){
    if(b){
        savePositions = false;
        ofJson json;
        auto addPointsFromPointsVec = [](ofJson &json, ofParameter<vector<ofPoint>> positions){
            for(int i = 0; i < positions.get().size() ; i++){
                auto position = positions.get()[i];
                json[positions.getEscapedName()]["Point"+ofToString(i)] = ofToString(position.x) + "_" + ofToString(position.y);
            }
        };

        addPointsFromPointsVec(json, positionToMoveMirrors);
        addPointsFromPointsVec(json, positionToMoveMirrorsAndStaticMirrors);
        addPointsFromPointsVec(json, positionToStaticMirrors);
        addPointsFromPointsVec(json, positionToCenter);


        ofSavePrettyJson("Positions.json", json);
    }
}

void positionSaver::loadPositionsListener(bool &b){
    if(b){
        ofJson json;
        json = ofLoadJson("Positions.json");
        if(json.empty()){
            return;
        }

        auto setPointsFromXml = [](ofJson &json, ofParameter<vector<ofPoint>> positions){
            if(json.count(positions.getEscapedName()) > 0){
                int i = 0;
                vector<ofPoint> newPositions;
                for (ofJson::iterator it = json[positions.getEscapedName()].begin(); it != json[positions.getEscapedName()].end(); ++it) {
                    vector<string> stringPoints = ofSplitString(it.value(), "_");
                    newPositions.push_back(ofPoint(ofToFloat(stringPoints[0]), ofToFloat(stringPoints[1])));
                    i++;
                }
                positions.set(newPositions);
            }
        };


            setPointsFromXml(json, positionToMoveMirrors);
            setPointsFromXml(json, positionToMoveMirrorsAndStaticMirrors);
            setPointsFromXml(json, positionToStaticMirrors);
            setPointsFromXml(json, positionToCenter);

        loadPositions = false;
//        parameters->get("Mov Mirrors").cast<vector<ofPoint>>() = positionToMoveMirrors;
//        parameters->get("Mov and Static Mirrors").cast<vector<ofPoint>>() = positionToMoveMirrorsAndStaticMirrors;
//        parameters->get("Static Mirrors").cast<vector<ofPoint>>() = positionToStaticMirrors;
//        parameters->get("Center").cast<vector<ofPoint>>() = positionToCenter;
    }
}

