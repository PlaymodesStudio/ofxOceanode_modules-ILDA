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
    parameters->add(color_red.set("Red", {1}, {0}, {1}));
    parameters->add(color_green.set("Green", {1}, {0}, {1}));
    parameters->add(color_blue.set("Blue", {1}, {0}, {1}));
    parameters->add(numVertex.set("Num Vertex", {1}, {1}, {100}));
    parameters->add(toCenterFigure.set("To Center", 0, 0, 1));
    parameters->add(scalePositions.set("Scale Pos Vec", {1}, {0}, {1.1}));
    parameters->add(opacity.set("Opacity Vec", {1}, {0}, {1}));
    parameters->add(size.set("Size Vec", {0.5}, {0}, {1.5}));
    parameters->add(rotation.set("Rotation Vec", {0}, {0}, {1}));
    parameters->add(jitter.set("Jitter Vec", {0}, {0}, {1}));
    parameters->add(pointModulation.set("Point Modulation", {0.5}, {0}, {1}));
    parameters->add(modulationAmount.set("Modulation Amount", {0}, {0}, {1}));
    parameters->add(refollowIn.set("Refollow In Vec", {0}, {0}, {1}));
    parameters->add(endFollow.set("End Follow Vec", {1}, {0}, {1}));
    parameters->add(offsetFollow.set("Offset Follow Vec", {0}, {0}, {1}));
    parameters->add(divisions.set("Divisions Vec", {0}, {0}, {14}));
    parameters->add(divisionSpacing.set("Division Spacing Vec", {0.5}, {0}, {1}));
    
    parameters->add(polyLinesOut.set("Output", {make_pair(ofPolyline(), ofColor())}));
    
    ofAddListener(parameters->parameterChangedE(), this, &graphicPatternGenerator::parameterChangedListener);
    positions = positions;
    
    listeners.push(color_red.newListener(this, &graphicPatternGenerator::rgbChanged));
    listeners.push(color_green.newListener(this, &graphicPatternGenerator::rgbChanged));
    listeners.push(color_blue.newListener(this, &graphicPatternGenerator::rgbChanged));
    
    someParameterChanged = true;
    
    pointDraggingIndex = -1;
}

void graphicPatternGenerator::rgbChanged(vector<float> &f){
    color = ofFloatColor(color_red.get()[0], color_green.get()[0], color_blue.get()[0]);
}

vector<pair<ofPolyline, ofColor>> graphicPatternGenerator::computePolylines(){
    vector<pair<ofPolyline, ofColor>> coloredPolylines;
    if(true){
        for(int i = 0 ; i < positions.get().size() * positionReplicator; i++){
            ofFloatColor polyColor = ofFloatColor(getParameterValueForPosition(color_red, i), getParameterValueForPosition(color_green, i), getParameterValueForPosition(color_blue, i));
            vector<ofPolyline> unitPoly;
            unitPoly.resize(1);
            ofPoint position;
            float scaleValue = getParameterValueForPosition(scalePositions, i);
            position.x = ofMap(scaleValue, 0.0, 1.0, 0.5, positions.get()[fmod(i, positions.get().size())].x);
            position.y = ofMap(scaleValue, 0.0, 1.0, 0.5, positions.get()[fmod(i, positions.get().size())].y);
            ofPoint positionWithJitter = position;
            float jitterValue = getParameterValueForPosition(jitter, i);
            if(jitterValue != 0){
                positionWithJitter.x = position.x + ofRandom(-jitterValue*0.05, + jitterValue*0.05);
                positionWithJitter.y = position.y + ofRandom(-jitterValue*0.05, + jitterValue*0.05);
            }
            if(getParameterValueForPosition(numVertex, i) == 1){
                unitPoly[0].addVertex(positionWithJitter - ofPoint(0, 0.0001));
                unitPoly[0].addVertex(positionWithJitter + ofPoint(0.0001, 0));
                unitPoly[0].addVertex(positionWithJitter - ofPoint(0.0001, 0));
                unitPoly[0].addVertex(positionWithJitter + ofPoint(0, 0.0001));
            }else{
                ofPoint firstCreatedPoint = ofPoint(-100, -100);
                ofPoint lastCreatedVertex = ofPoint(-100, -100);
                ofPoint newVertex;
                int counter = 0;
                for(float j = 0 ; j < 1 ; j = j + (1.0/(float)getParameterValueForPosition(numVertex, i))){
                    float jj = j + getParameterValueForPosition(rotation, i);
                    
                    newVertex.x = (sin(jj*2*PI)*getParameterValueForPosition(size, i)/2)+position.x;
                    newVertex.y = (cos(jj*2*PI)*getParameterValueForPosition(size, i)/2)+position.y;
                    if(jitterValue != 0){
                        newVertex.x = newVertex.x + ofRandom(-jitterValue*0.05, + jitterValue*0.05);
                        newVertex.y = newVertex.y + ofRandom(-jitterValue*0.05, + jitterValue*0.05);
                    }
                    if(lastCreatedVertex != ofPoint(-100, -100) && toCenterFigure != 0){
                        ofPoint middleVertex = (newVertex+lastCreatedVertex) / 2;
                        ofPoint toCenterPoint = (middleVertex * (1 - toCenterFigure)) + (position * toCenterFigure);
                        unitPoly[0].addVertex(toCenterPoint);
                    }
                    unitPoly[0].addVertex(newVertex);
                    lastCreatedVertex = newVertex;
                    if(firstCreatedPoint == ofPoint(-100, -100)){
                        firstCreatedPoint = newVertex;
                    }
                }
                if(toCenterFigure != 0){
                    ofPoint middleVertex = (newVertex+firstCreatedPoint) / 2;
                    ofPoint toCenterPoint = (middleVertex * (1 - toCenterFigure)) + (position * toCenterFigure);
                    unitPoly[0].addVertex(toCenterPoint);
                }
                unitPoly[0].close();
            }
            
            
            if(getParameterValueForPosition(divisions, i) != 0){
                unitPoly[0] = unitPoly[0].getResampledByCount(200);
                vector<glm::vec3> polyVertex = unitPoly[0].getVertices();
                unitPoly[0].clear();
                int polySize = polyVertex.size();
                unitPoly.resize(getParameterValueForPosition(divisions, i));
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
                    unitPoly[d] = tempPoly;
//                    coloredPolylines.push_back(make_pair(tempPoly, polyColor * getParameterValueForPosition(opacity, i)));
                }
            }
            if(numVertex.get().size() == 1 && numVertex.get()[0] != 1 && getParameterValueForPosition(modulationAmount, i) != 0){
                int modulationSize = ((pointModulation.get().size() / positions.get().size()) / positionReplicator) / unitPoly.size();
                if(modulationSize >= unitPoly[0].getVertices().size()){
                    for(int d = 0; d < unitPoly.size(); d++){
                        unitPoly[d] = unitPoly[d].getResampledByCount(modulationSize);
                        for(int j = 0; j < modulationSize; j++){
                            auto &poly = unitPoly[d].getVertices()[j];
                            poly = poly + (glm::normalize(poly - toGlm(position)) * ((pointModulation.get()[(((i*unitPoly.size())+d)*modulationSize) + j]-0.5) * getParameterValueForPosition(modulationAmount, i) * getParameterValueForPosition(size, i)));
                        }
                    }
                }
            }
            if(getParameterValueForPosition(opacity, i) > 0.05){
                for(auto &poly : unitPoly){
                    coloredPolylines.push_back(make_pair(poly, polyColor * getParameterValueForPosition(opacity, i)));
                }
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
