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
    parameters->add(positionReplicator.set("Position Replicator", 1, 1, 100));
    lastPositionReplicator = positionReplicator;
    parameters->add(color.set("Color", ofColor::red, ofColor::white, ofColor::black));
    parameters->add(color_red.set("Red", {1}, {0}, {1}));
    parameters->add(color_green.set("Green", {1}, {0}, {1}));
    parameters->add(color_blue.set("Blue", {1}, {0}, {1}));
    parameters->add(svgFile.set("SVG File", ""));
    parameters->add(svgSize.set("SVG Size", "0"));
    parameters->add(numVertex.set("Num Vertex", {1}, {0}, {100}));
    parameters->add(toCenterFigure.set("To Center", {0}, {0}, {1}));
    parameters->add(scalePositions.set("Scale Pos Vec", {1}, {0}, {1.1}));
    parameters->add(opacity.set("Opacity Vec", {1}, {0}, {1}));
    parameters->add(size.set("Size Vec", {0.5}, {0}, {1.5}));
    parameters->add(rotation.set("Rotation Vec", {0}, {0}, {1}));
    parameters->add(jitter.set("Jitter Vec", {0}, {0}, {1}));
    parameters->add(modulationType.set("Modulation type", false));
    parameters->add(pointModulation.set("Point Modulation", {0.5}, {0}, {1}));
    parameters->add(modulationAmount.set("Modulation Amount", {0}, {0}, {1}));
    parameters->add(refollowIn.set("Refollow In Vec", {0}, {0}, {1}));
    parameters->add(endFollow.set("End Follow Vec", {1}, {0}, {1}));
    parameters->add(offsetFollow.set("Offset Follow Vec", {0}, {0}, {1}));
    parameters->add(divisions.set("Divisions Vec", {0}, {0}, {14}));
    parameters->add(divisionSpacing.set("Division Spacing Vec", {0.5}, {0}, {1}));
    parameters->add(width.set("Line Width", {1}, {0}, {10}));
    parameters->add(filled.set("Filled", {0}, {0}, {1}));
    parameters->add(drawOnBlack.set("Draw On Black", true));
    
    parameters->add(polyLinesOut.set("Output", {ofPath()}));
    
    ofAddListener(parameters->parameterChangedE(), this, &graphicPatternGenerator::parameterChangedListener);
    positions = positions;
    
    listeners.push(color_red.newListener(this, &graphicPatternGenerator::rgbChanged));
    listeners.push(color_green.newListener(this, &graphicPatternGenerator::rgbChanged));
    listeners.push(color_blue.newListener(this, &graphicPatternGenerator::rgbChanged));
    
    listeners.push(svgFile.newListener([this](string &s){
        if(s == ""){
            isSvgLoaded = false;
            svgSize = "0";
        }else{
            svg.load(s);
            if(svg.getNumPath() != 0) isSvgLoaded = true;
            svgSize = ofToString(svg.getNumPath());
        }
    }));
    
    someParameterChanged = true;
    
    pointDraggingIndex = -1;
}

void graphicPatternGenerator::rgbChanged(vector<float> &f){
    color = ofFloatColor(color_red.get()[0], color_green.get()[0], color_blue.get()[0]);
}

vector<ofPath> graphicPatternGenerator::computePolylines(){
    vector<ofPath> paths;
    if(true){
        vector<ofPath> unitPath;
        unitPath.resize(1);
        //unitPath[0].setMode(ofPath::POLYLINES);
        int indexMonoline = 0;
        int lastIndexMonoline = 0;
        int numElements = isSvgLoaded ? svg.getNumPath() : positions.get().size();
        for(int i = 0 ; i < numElements * positionReplicator; i++){
            ofFloatColor polyColor = ofFloatColor(getParameterValueForPosition(color_red, i), getParameterValueForPosition(color_green, i), getParameterValueForPosition(color_blue, i));
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
            if(isSvgLoaded){
                unitPath.clear();
                unitPath.resize(1);
                ofPath tempSVGPath = svg.getPathAt(i);
                    tempSVGPath.setPolyWindingMode(OF_POLY_WINDING_ODD);
                    bool newOutlinePath = true;
                    for(auto &outline : tempSVGPath.getOutline()){
                        ofPolyline poly = outline;
                        for(auto &point : poly.getVertices()){
                            point = point / glm::vec3(svg.getWidth(), svg.getHeight(), 1);
                            point -= .5;
                            if(getParameterValueForPosition(rotation, i) != 0)
                                point = glm::rotate(point, getParameterValueForPosition(rotation, i)*2*(float)M_PI, glm::vec3(0,0,1));
                            point *= getParameterValueForPosition(size, i);
                            point += (glm::vec3(positionWithJitter));
                            if(newOutlinePath){
                                unitPath[0].moveTo(point);
                                newOutlinePath = false;
                            }else{
                                unitPath[0].lineTo(point);
                            }
                        }
                    }
                
            }else{
                if(numVertex.get() != vector<int>(1, 0)){
                    unitPath.clear();
                    unitPath.resize(1);
                }
                if(getParameterValueForPosition(numVertex, i) == 0){
                    if(getParameterValueForPosition(opacity, i) != 0){
                        if(unitPath[indexMonoline].getOutline()[0].size() == 0){
                            unitPath[indexMonoline].moveTo(positionWithJitter);
                        }else{
                            unitPath[indexMonoline].lineTo(positionWithJitter);
                        }
                        //                    unitPath[indexMonoline].addVertex(positionWithJitter);
                        lastIndexMonoline = indexMonoline;
                    }
                    else if(lastIndexMonoline == indexMonoline){
                        unitPath.resize(++indexMonoline + 1);
                    }
                }
                else if(getParameterValueForPosition(numVertex, i) == 1){
                    unitPath[0].moveTo(positionWithJitter - ofPoint(0, 0.0001));
                    unitPath[0].lineTo(positionWithJitter + ofPoint(0.0001, 0));
                    unitPath[0].lineTo(positionWithJitter - ofPoint(0.0001, 0));
                    unitPath[0].lineTo(positionWithJitter + ofPoint(0, 0.0001));
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
                        if(lastCreatedVertex != ofPoint(-100, -100) && getParameterValueForPosition(toCenterFigure, i) != 0){
                            ofPoint middleVertex = (newVertex+lastCreatedVertex) / 2;
                            ofPoint toCenterPoint = (middleVertex * (1 - getParameterValueForPosition(toCenterFigure, i))) + (position * getParameterValueForPosition(toCenterFigure, i));
                            if(unitPath[0].getCommands().size() == 0){
                                unitPath[0].moveTo(toCenterPoint);
                            }else{
                                unitPath[0].lineTo(toCenterPoint);
                            }
                        }
                        if(unitPath[0].getCommands().size() == 0){
                            unitPath[0].moveTo(newVertex);
                        }else{
                            unitPath[0].lineTo(newVertex);
                        }
                        lastCreatedVertex = newVertex;
                        if(firstCreatedPoint == ofPoint(-100, -100)){
                            firstCreatedPoint = newVertex;
                        }
                    }
                    if(getParameterValueForPosition(toCenterFigure, i) != 0){
                        ofPoint middleVertex = (newVertex+firstCreatedPoint) / 2;
                        ofPoint toCenterPoint = (middleVertex * (1 - getParameterValueForPosition(toCenterFigure, i))) + (position * getParameterValueForPosition(toCenterFigure, i));
                        if(unitPath[0].getCommands().size() == 0){
                            unitPath[0].moveTo(toCenterPoint);
                        }else{
                            unitPath[0].lineTo(toCenterPoint);
                        }
                    }
                    if(getParameterValueForPosition(numVertex, i) != 2)
                        unitPath[0].close();
                }
            }
            
            if(numVertex.get().size() == 1 && numVertex.get()[0] > 1 && getParameterValueForPosition(modulationAmount, i) != 0){
                vector<ofPolyline> polylines = unitPath[0].getOutline();
                int modulationSize = ((pointModulation.get().size() / positions.get().size()) / positionReplicator) / polylines.size();
                if(modulationSize >= polylines[0].getVertices().size()){
                    unitPath[0].clear();
                    bool newPoly = true;
                    for(int d = 0; d < polylines.size(); d++){
                        ofPolyline polyRes = polylines[d].getResampledByCount(modulationSize);
                        for(int j = 0; j < min(int(polyRes.size()), modulationSize); j++){
                            glm::vec3 poly = polyRes.getVertices()[j];
                            if(!modulationType){ //radial modulation
                                poly = poly + (glm::normalize(poly - toGlm(position)) * ((pointModulation.get()[(((i*polylines.size())+d)*modulationSize) + j]-0.5) * getParameterValueForPosition(modulationAmount, i) * getParameterValueForPosition(size, i)));
                            }else{ //Tangent modulation
                                bool reverse = false;
                                int leftPointIndex = j-1;
                                if(leftPointIndex < 0){
                                    leftPointIndex = polyRes.size()-1;
                                    reverse = true;
                                }
                                
                                int rightPointIndex = j+1;
                                if(rightPointIndex > polyRes.size()-1){
                                    rightPointIndex = 0;
                                    reverse = true;
                                }
                                
                                glm::vec3 leftPoint = polyRes.getVertices()[leftPointIndex];
                                glm::vec3 rightPoint = polyRes.getVertices()[rightPointIndex];
                                
                                glm::vec3 leftToRight = (rightPoint - leftPoint);
                                
                                glm::vec3 perpendicular = glm::vec3(-leftToRight.y, leftToRight.x, 0);
                                if(reverse){
                                    perpendicular = glm::vec3(leftToRight.y, -leftToRight.x, 0);
                                }
                                
                                poly = poly + (glm::normalize(perpendicular) * ((pointModulation.get()[(((i*polylines.size())+d)*modulationSize) + j]-0.5) * getParameterValueForPosition(modulationAmount, i) * getParameterValueForPosition(size, i)));
                            }
                            if(newPoly){
                                unitPath[0].moveTo(poly);
                                newPoly = false;
                            }else{
                                unitPath[0].lineTo(poly);
                            }
                        }
                        if(polyRes.isClosed()){
                            unitPath[0].close();
                        }
                        newPoly = true;
                    }
                }
            }
            int divisionAtIndex = getParameterValueForPosition(divisions, i);
            if(divisionAtIndex != 0 && numVertex.get()[0] > 1){
                for(int ii = 0 ; ii < unitPath.size(); ii++){
                    auto resampledPoly = unitPath[ii].getOutline()[0];
                    if(resampledPoly.getVertices().size() < 400){
                        resampledPoly = resampledPoly.getResampledByCount(400);
                    }
                    vector<glm::vec3> polyVertex = resampledPoly.getVertices();
                    unitPath[ii].clear();
                    int polySize = polyVertex.size();
                    //unitPath.resize(getParameterValueForPosition(divisions, i));
                    bool newPoly = true;
                    for(int d = 0; d < getParameterValueForPosition(divisions, i) ; d++){
                        float increment = 1.0/(float)(((float)getParameterValueForPosition(divisions, i)*2.0)-1);
                        float start = (d * 2 * increment) * getParameterValueForPosition(endFollow, i);
                        float end = start + (increment * getParameterValueForPosition(divisionSpacing, i) * 2 * getParameterValueForPosition(endFollow, i));
                        for(int j = start * polySize; j < end * polySize ; j++){
                            int jj = j + ((getParameterValueForPosition(refollowIn, i) + getParameterValueForPosition(offsetFollow, i)) * polySize);
                            jj = jj % polySize;
                            if(newPoly){
                                unitPath[ii].moveTo(polyVertex[jj]);
                                newPoly = false;
                            }
                            else
                                unitPath[ii].lineTo(polyVertex[jj]);
                        }
                        newPoly = true;
                    }
                }
            }
            if(numVertex.get() != vector<int>(1, 0)){
                if(getParameterValueForPosition(opacity, i) > 0.05 || drawOnBlack){
                    for(auto &poly : unitPath){
                        if(getParameterValueForPosition(numVertex, i) == 0){
                            poly.setColor(polyColor);
                        }else{
                            poly.setColor(ofFloatColor(polyColor, getParameterValueForPosition(opacity, i)));
                        }
                        if(getParameterValueForPosition(filled, i) == 0){
                            poly.setFilled(false);
                        }
                        else{
                            poly.setFilled(true);
                            poly.setFillColor(ofFloatColor(polyColor, getParameterValueForPosition(filled, i)));
                        }
                        poly.setStrokeWidth(getParameterValueForPosition(width, i));
                        paths.push_back(poly);
                    }
                }
            }
        }
        if(numVertex.get() == vector<int>(1, 0)){
            for(auto &poly : unitPath){
                auto color = ofFloatColor(color_red->at(0), color_green->at(0), color_blue->at(0), opacity->at(0));
                poly.setColor(color);
                if(getParameterValueForPosition(filled, 0) == 0){
                    poly.setFilled(false);
                }
                else{
                    poly.setFilled(true);
                    poly.setFillColor(ofFloatColor(color, getParameterValueForPosition(filled, 0)));
                }
                poly.setStrokeWidth(getParameterValueForPosition(width, 0));
                paths.push_back(poly);
            }
        }
    }
    
    
    
    someParameterChanged = false;
    polyLinesOut = paths;
    return paths;
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
