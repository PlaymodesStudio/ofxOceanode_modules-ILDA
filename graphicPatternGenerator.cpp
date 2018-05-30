//
//  graphicPatternGenerator.cpp
//  Espill-ilda
//
//  Created by Eduard Frigola on 21/06/2017.
//
//

#include "graphicPatternGenerator.h"

graphicPatternGenerator::graphicPatternGenerator() : ofxOceanodeNodeModelExternalWindow("Graphic Generator"){
    //setup();
}

void graphicPatternGenerator::setup(){
    parameters->add(positions.set("Positions", {ofPoint(0.25, 0.25), ofPoint(0.25, 0.75), ofPoint(0.75, 0.25), ofPoint(0.75, 0.75)}));
    parameters->add(positionReplicator.set("Position Replicator", 1, 1, 10));
    lastPositionReplicator = positionReplicator;
    parameters->add(color.set("Color", ofColor::red, ofColor::white, ofColor::black));
    parameters->add(toCenterFigure.set("To Center", false));
    
    parameters->add(numVertex.set("Num Vertex", {1}, {1}, {100}));
    parameters->add(scalePositions.set("Scale Pos Vec", {1}, {0}, {1.1}));
    parameters->add(opacity.set("Opacity Vec", {1}, {0}, {1}));
    parameters->add(size.set("Size Vec", {0.5}, {0}, {1}));
    parameters->add(rotation.set("Rotation Vec", {0}, {0}, {1}));
    parameters->add(jitter.set("Jitter Vec", {0}, {0}, {1}));
    parameters->add(refollowIn.set("Refollow In Vec", {0}, {0}, {1}));
    parameters->add(endFollow.set("End Follow Vec", {1}, {0}, {1}));
    parameters->add(offsetFollow.set("Offset Follow Vec", {0}, {0}, {1}));
    parameters->add(divisions.set("Divisions Vec", {0}, {0}, {14}));
    parameters->add(divisionSpacing.set("Division Spacing Vec", {0.5}, {0}, {1}));
    
    
    
    parameters->add(polyLinesOut.set("Output", {make_pair(ofPolyline(), ofColor())}));
    
    ofAddListener(parameters->parameterChangedE(), this, &graphicPatternGenerator::parameterChangedListener);
    positions = positions;
    
    someParameterChanged = true;
    
    pointDraggingIndex = -1;
}

vector<pair<ofPolyline, ofColor>> graphicPatternGenerator::computePolylines(){
    vector<pair<ofPolyline, ofColor>> coloredPolylines;
    if(true){
        for(int i = 0 ; i < positions.get().size() * positionReplicator; i++){
            ofPolyline unitPoly;
            ofPoint position;
            float scaleValue = getParameterValueForPosition(scalePositions, i);
            position.x = ofMap(scaleValue, 0.0, 1.0, 0.5, positions.get()[floor(i / positionReplicator)].x);
            position.y = ofMap(scaleValue, 0.0, 1.0, 0.5, positions.get()[floor(i / positionReplicator)].y);
            ofPoint positionWithJitter = position;
            float jitterValue = getParameterValueForPosition(jitter, i);
            if(jitterValue != 0){
                positionWithJitter.x = position.x + ofRandom(-jitterValue*0.05, + jitterValue*0.05);
                positionWithJitter.y = position.y + ofRandom(-jitterValue*0.05, + jitterValue*0.05);
            }
            if(getParameterValueForPosition(numVertex, i) == 1){
                unitPoly.addVertex(positionWithJitter - ofPoint(0, 0.0001));
                unitPoly.addVertex(positionWithJitter + ofPoint(0.0001, 0));
                unitPoly.addVertex(positionWithJitter - ofPoint(0.0001, 0));
                unitPoly.addVertex(positionWithJitter + ofPoint(0, 0.0001));
            }else{
                for(float j = 0 ; j < 1 ; j = j + (1.0/(float)getParameterValueForPosition(numVertex, i))){
                    float jj = j + getParameterValueForPosition(rotation, i);
                    ofPoint newVertex;
                    newVertex.x = (sin(jj*2*PI)*getParameterValueForPosition(size, i)/2)+position.x;
                    newVertex.y = (cos(jj*2*PI)*getParameterValueForPosition(size, i)/2)+position.y;
                    //float jitterValue = getParameterValueForPosition(jitter, i);
                    if(jitterValue != 0){
                        newVertex.x = newVertex.x + ofRandom(-jitterValue*0.05, + jitterValue*0.05);
                        newVertex.y = newVertex.y + ofRandom(-jitterValue*0.05, + jitterValue*0.05);
                    }
                    unitPoly.addVertex(newVertex);
                    if(toCenterFigure){
                        unitPoly.addVertex(positionWithJitter);
                    }
                }
                unitPoly.close();
                unitPoly = unitPoly.getResampledByCount(200);
            }
            
            
            if(getParameterValueForPosition(divisions, i) != 0){
                vector<glm::vec3> polyVertex = unitPoly.getVertices();
                unitPoly.clear();
                int polySize = polyVertex.size();
                for(int d = 0; d < getParameterValueForPosition(divisions, i) ; d++){
                    ofPolyline tempPoly;
                    float increment = 1.0/(float)(((float)getParameterValueForPosition(divisions, i)*2.0)-1);
                    float start = (d * 2 * increment) * getParameterValueForPosition(endFollow, i);
                    float end = start + (increment * getParameterValueForPosition(divisionSpacing, i) * 2 * getParameterValueForPosition(endFollow, i));
                    for(int j = start * polySize; j < end * polySize ; j++){
                        int jj = j + ((getParameterValueForPosition(refollowIn, i) + getParameterValueForPosition(offsetFollow, i)) * polySize);
                        jj = jj % polySize;
                        tempPoly.addVertex(polyVertex[jj]);
                    }
                    coloredPolylines.push_back(make_pair(tempPoly, color.get() * getParameterValueForPosition(opacity, i)));
                }
            }else{
                coloredPolylines.push_back(make_pair(unitPoly, color.get() * getParameterValueForPosition(opacity, i)));
            }
        }
    }
    someParameterChanged = false;
    polyLinesOut = coloredPolylines;
    return coloredPolylines;
}

void graphicPatternGenerator::parameterChangedListener(ofAbstractParameter &parameter){
    someParameterChanged = true;

}


void graphicPatternGenerator::draw(){
    ofBackground(0);
    ofSetColor(255);
    
    for(int i = 0 ; i < positions.get().size(); i++){
        auto point = positions.get()[i];
        ofSetColor(255);
        int x = point.x * ofGetWidth();
        int y = point.y * ofGetHeight();
        ofDrawCircle(x, y, 20);
        ofSetColor(255,0,0);
        string s = ofToString(i);
        ofDrawBitmapString(s, x, y);
    }
}

void graphicPatternGenerator::mousePressed(ofMouseEventArgs &a){
    bool foundPoint = false;
    for(int i = 0; i < positions.get().size() && !foundPoint ; i++){
        auto point = positions.get()[i] * ofPoint(ofGetWidth(), ofGetHeight());
        if(ofRectangle(point.x - 20, point.y - 20, 40, 40).inside(a)){
            pointDraggingIndex = i;
            foundPoint = true;
        }
    }
    if(ofGetKeyPressed(OF_KEY_SHIFT)){
        if(foundPoint){
            vector<ofPoint> positionsCopy = positions;
            positionsCopy.erase(positionsCopy.begin() + pointDraggingIndex);
            parameters->get("Positions").cast<vector<ofPoint>>().set(positionsCopy);
            pointDraggingIndex = -1;
        }else{
            vector<ofPoint> positionsCopy = positions;
            positionsCopy.push_back(ofPoint(a.x / ofGetWidth(), a.y/ofGetHeight()));
            parameters->get("Positions").cast<vector<ofPoint>>().set(positionsCopy);
        }
    }
}

void graphicPatternGenerator::mouseReleased(ofMouseEventArgs &a){
    pointDraggingIndex = -1;
}

void graphicPatternGenerator::mouseDragged(ofMouseEventArgs &a){
    if(pointDraggingIndex != -1){
        if(ofGetKeyPressed(OF_KEY_ALT)){
            ofPoint oldPos = positions.get()[pointDraggingIndex];
            ofPoint step = oldPos - (a / ofPoint((float)ofGetWidth(), (float)ofGetHeight()));
            vector<ofPoint> positionsCopy = positions;
            positionsCopy[pointDraggingIndex] = oldPos - (step/100);
            parameters->get("Positions").cast<vector<ofPoint>>().set(positionsCopy);
        }else{
            vector<ofPoint> positionsCopy = positions;
            positionsCopy[pointDraggingIndex] = a / ofPoint(ofGetWidth(), ofGetHeight());
            parameters->get("Positions").cast<vector<ofPoint>>().set(positionsCopy);
        }
    }
}
