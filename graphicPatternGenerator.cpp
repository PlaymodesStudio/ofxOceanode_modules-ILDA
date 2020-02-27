//
//  graphicPatternGenerator.cpp
//  Espill-ilda
//
//  Created by Eduard Frigola on 21/06/2017.
//
//

#include "graphicPatternGenerator.h"
#include "sharedResources.h"

graphicPatternGenerator::graphicPatternGenerator() : ofxOceanodeNodeModelExternalWindow("Graphic Generator"){
    //setup();
}

void graphicPatternGenerator::setup(){
    parameters->add(positions.set("Positions", {glm::vec3(0.5, 0.5, 1)}));
    parameters->add(positionReplicator.set("Position Replicator", 1, 1, 100));
    lastPositionReplicator = positionReplicator;
    parameters->add(color.set("Color", ofColor::red, ofColor::white, ofColor::black));
    parameters->add(color_red.set("Red", {1}, {0}, {1}));
    parameters->add(color_green.set("Green", {1}, {0}, {1}));
    parameters->add(color_blue.set("Blue", {1}, {0}, {1}));
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
    parameters->add(reloadShader.set("Reload Shader"));
    
    listeners.push(reloadShader.newListener([this](){
        setupShader();
    }));
    
    listeners.push(numVertex.newListener([this](vector<int> &p){
        changedShaderIntParameters.emplace_back(numVertex.getName(), p);
    }));
    
    listeners.push(size.newListener([this](vector<float> &p){
        changedShaderFloatParameters.emplace_back(size.getName(), p);
    }));
    
    parameters->add(output.set("Output", &vbo));
    
    ofAddListener(parameters->parameterChangedE(), this, &graphicPatternGenerator::parameterChangedListener);
    positions = positions;
    
    listeners.push(color_red.newListener(this, &graphicPatternGenerator::rgbChanged));
    listeners.push(color_green.newListener(this, &graphicPatternGenerator::rgbChanged));
    listeners.push(color_blue.newListener(this, &graphicPatternGenerator::rgbChanged));
    
    someParameterChanged = true;
    
    pointDraggingIndex = -1;
    
    setupShader();
    buffer.resize(2);
    
    totalSize = positions.get().size() * positionReplicator;
}

void graphicPatternGenerator::rgbChanged(vector<float> &f){
    color = ofFloatColor(color_red.get()[0], color_green.get()[0], color_blue.get()[0]);
}

vector<ofPath> graphicPatternGenerator::computePolylines(){
    vector<ofPath> paths;
    if(true){
        vector<ofPath> unitPath;
        unitPath.resize(1);
        unitPath[0].setMode(ofPath::POLYLINES);
        int indexMonoline = 0;
        int lastIndexMonoline = 0;
        for(int i = 0 ; i < positions.get().size() * positionReplicator; i++){
            ofFloatColor polyColor = ofFloatColor(getParameterValueForPosition(color_red, i), getParameterValueForPosition(color_green, i), getParameterValueForPosition(color_blue, i));
            if(numVertex.get() != vector<int>(1, 0)){
                unitPath.clear();
                unitPath.resize(1);
            }
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
                            poly.setFillColor(ofFloatColor(polyColor, getParameterValueForPosition(filled, 0)));
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
    return paths;
}

void graphicPatternGenerator::setupShader(){
    //TBOs
    
    //OSCILLATOR SHADER INT
    shaderIntBuffer.allocate();
    shaderFloatBuffer.allocate();
    shaderIntBuffer.bind(GL_TEXTURE_BUFFER);
    shaderFloatBuffer.bind(GL_TEXTURE_BUFFER);
    setShaderParameterDataToTBO();
    shaderIntTexture.allocateAsBufferTexture(shaderIntBuffer, GL_R32UI);
    shaderFloatTexture.allocateAsBufferTexture(shaderFloatBuffer, GL_R32F);

    
    
    // We only need a vertex shader since we are only using the
    // shader to store the modified vertices in a buffer
    ofShader::TransformFeedbackSettings settings;
    settings.shaderFiles[GL_VERTEX_SHADER] = "vert.glsl";
    settings.shaderFiles[GL_GEOMETRY_SHADER] = "geom.glsl";
    settings.bindDefaults = false;
    settings.varyingsToCapture = {"v_position", "v_color"};
    settings.bufferMode = GL_SEPARATE_ATTRIBS;
    
    shader.setup(settings);
    
    shaderIntParametersTextureLocation = sharedResources::getInstance().getNextAvailableShaderTextureLocation();
    shaderFloatParametersTextureLocation = sharedResources::getInstance().getNextAvailableShaderTextureLocation();
    
    shader.begin();
    shader.setUniformTexture("intParameters", shaderIntTexture, shaderIntParametersTextureLocation);
    shader.setUniformTexture("floatParameters", shaderFloatTexture, shaderFloatParametersTextureLocation);
    shader.end();
}

void graphicPatternGenerator::setShaderParameterDataToTBO(){
    vector<int> accumulateParametersShaderIntParameters;
    vector<float> accumulateParametersShaderFloatParameters;
    
    int currentSize = positions.get().size() * positionReplicator;
    for(auto &p : *parameters){
        if(p->type() == typeid(ofParameter<vector<int>>).name()){
            vector<int> tempVec(currentSize, p->cast<vector<int>>().get()[0]);
            accumulateParametersShaderIntParameters.insert(accumulateParametersShaderIntParameters.end(), tempVec.begin(), tempVec.end());
        }else if(p->type() == typeid(ofParameter<vector<float>>).name()){
            vector<float> tempVec(currentSize, p->cast<vector<float>>().get()[0]);
            accumulateParametersShaderFloatParameters.insert(accumulateParametersShaderFloatParameters.end(), tempVec.begin(), tempVec.end());
        }
    }
    
//    vector<int> numVertex_tempVec(positions.get().size() * positionReplicator, numVertex.get()[0]);
//    accumulateParametersShaderIntParameters.insert(accumulateParametersShaderIntParameters.end(), numVertex_tempVec.begin(), numVertex_tempVec.end());
    
    shaderIntBuffer.setData(accumulateParametersShaderIntParameters, GL_STREAM_DRAW);
    shaderFloatBuffer.setData(accumulateParametersShaderFloatParameters, GL_STREAM_DRAW);
}

void graphicPatternGenerator::setShaderFloatParameterDataToTBO(){
    vector<float> accumulateParametersShaderFloatParameters;
    
    vector<float> size_tempVec(positions.get().size() * positionReplicator, size.get()[0]);
    accumulateParametersShaderFloatParameters.insert(accumulateParametersShaderFloatParameters.end(), size_tempVec.begin(), size_tempVec.end());
//    vector<float> indexNumWavesY_tempVec(width, indexNumWaves[1].get()[0]);
//    accumulateParametersOscillatorShaderFloatParameters.insert(accumulateParametersOscillatorShaderFloatParameters.end(), indexNumWavesY_tempVec.begin(), indexNumWavesY_tempVec.end());
//
//    vector<float> indexInvertX_tempVec(height, indexInvert[0].get()[0]);
//    accumulateParametersOscillatorShaderFloatParameters.insert(accumulateParametersOscillatorShaderFloatParameters.end(), indexInvertX_tempVec.begin(), indexInvertX_tempVec.end());
//    vector<float> indexInvertY_tempVec(width, indexInvert[1].get()[0]);
//    accumulateParametersOscillatorShaderFloatParameters.insert(accumulateParametersOscillatorShaderFloatParameters.end(), indexInvertY_tempVec.begin(), indexInvertY_tempVec.end());
//
//    vector<float> indexRandomX_tempVec(height, indexRandom[0].get()[0]);
//    accumulateParametersOscillatorShaderFloatParameters.insert(accumulateParametersOscillatorShaderFloatParameters.end(), indexRandomX_tempVec.begin(), indexRandomX_tempVec.end());
//    vector<float> indexRandomY_tempVec(width, indexRandom[1].get()[0]);
//    accumulateParametersOscillatorShaderFloatParameters.insert(accumulateParametersOscillatorShaderFloatParameters.end(), indexRandomY_tempVec.begin(), indexRandomY_tempVec.end());
//
//    vector<float> indexOffsetX_tempVec(height, indexOffset[0].get()[0]);
//    accumulateParametersOscillatorShaderFloatParameters.insert(accumulateParametersOscillatorShaderFloatParameters.end(), indexOffsetX_tempVec.begin(), indexOffsetX_tempVec.end());
//    vector<float> indexOffsetY_tempVec(width, indexOffset[1].get()[0]);
//    accumulateParametersOscillatorShaderFloatParameters.insert(accumulateParametersOscillatorShaderFloatParameters.end(), indexOffsetY_tempVec.begin(), indexOffsetY_tempVec.end());
//
//    vector<float> indexCombinationX_tempVec(height, indexCombination[0].get()[0]);
//    accumulateParametersOscillatorShaderFloatParameters.insert(accumulateParametersOscillatorShaderFloatParameters.end(), indexCombinationX_tempVec.begin(), indexCombinationX_tempVec.end());
//    vector<float> indexCombinationY_tempVec(width, indexCombination[1].get()[0]);
//    accumulateParametersOscillatorShaderFloatParameters.insert(accumulateParametersOscillatorShaderFloatParameters.end(), indexCombinationY_tempVec.begin(), indexCombinationY_tempVec.end());
//
//    vector<float> phaseOffsetX_tempVec(width, phaseOffset[0].get()[0]);
//    accumulateParametersOscillatorShaderFloatParameters.insert(accumulateParametersOscillatorShaderFloatParameters.end(), phaseOffsetX_tempVec.begin(), phaseOffsetX_tempVec.end());
//    vector<float> phaseOffsetY_tempVec(height, phaseOffset[1].get()[0]);
//    accumulateParametersOscillatorShaderFloatParameters.insert(accumulateParametersOscillatorShaderFloatParameters.end(), phaseOffsetY_tempVec.begin(), phaseOffsetY_tempVec.end());
//
//    vector<float> pulseWidthX_tempVec(width, pulseWidth[0].get()[0]);
//    accumulateParametersOscillatorShaderFloatParameters.insert(accumulateParametersOscillatorShaderFloatParameters.end(), pulseWidthX_tempVec.begin(), pulseWidthX_tempVec.end());
//    vector<float> pulseWidthY_tempVec(height, pulseWidth[1].get()[0]);
//    accumulateParametersOscillatorShaderFloatParameters.insert(accumulateParametersOscillatorShaderFloatParameters.end(), pulseWidthY_tempVec.begin(), pulseWidthY_tempVec.end());
//
//    vector<float> skewX_tempVec(width, skew[0].get()[0]);
//    accumulateParametersOscillatorShaderFloatParameters.insert(accumulateParametersOscillatorShaderFloatParameters.end(), skewX_tempVec.begin(), skewX_tempVec.end());
//    vector<float> skewY_tempVec(height, skew[1].get()[0]);
//    accumulateParametersOscillatorShaderFloatParameters.insert(accumulateParametersOscillatorShaderFloatParameters.end(), skewY_tempVec.begin(), skewY_tempVec.end());
//
//    vector<float> waveformX_tempVec(width, waveform[0].get()[0]);
//    accumulateParametersOscillatorShaderFloatParameters.insert(accumulateParametersOscillatorShaderFloatParameters.end(), waveformX_tempVec.begin(), waveformX_tempVec.end());
//    vector<float> waveformY_tempVec(height, waveform[1].get()[0]);
//    accumulateParametersOscillatorShaderFloatParameters.insert(accumulateParametersOscillatorShaderFloatParameters.end(), waveformY_tempVec.begin(), waveformY_tempVec.end());
    
    shaderFloatBuffer.setData(accumulateParametersShaderFloatParameters, GL_STREAM_DRAW);
}

void graphicPatternGenerator::setParametersInfoMaps(){
//    int dimensionsSum = width+height
    int currentSize = positions.get().size() * positionReplicator;
    int intCount = 0;
    int floatCount = 0;
    for(auto &p : *parameters){
        if(p->type() == typeid(ofParameter<vector<int>>).name()){
            shaderIntParameterNameTBOPositionMap[p->getName()] = currentSize*intCount;
            shaderParameterNameTBOSizeMap[p->getName()] = currentSize;
            intCount++;
        }else if(p->type() == typeid(ofParameter<vector<float>>).name()){
            shaderFloatParameterNameTBOPositionMap[p->getName()] = currentSize*floatCount;
            shaderParameterNameTBOSizeMap[p->getName()] = currentSize;
            floatCount++;
        }
    }
    
//    shaderFloatParameterNameTBOPositionMap[size.getName()] = 0;
//    shaderParameterNameTBOSizeMap[size.getName()] = positions.get().size() * positionReplicator;
//
//    shaderIntParameterNameTBOPositionMap[numVertex.getName()] = 0;
//    shaderParameterNameTBOSizeMap[numVertex.getName()] = positions.get().size() * positionReplicator;
}


void graphicPatternGenerator::computeShader(){
    if(totalSize != positions.get().size() * positionReplicator){
        setParametersInfoMaps();
        setShaderParameterDataToTBO();
//        setShaderFloatParameterDataToTBO();
    }
    totalSize = positions.get().size() * positionReplicator;
    
    for(auto &parameter : changedShaderIntParameters){
        vector<int> &vi = parameter.second;
        int position = shaderIntParameterNameTBOPositionMap[parameter.first];
        int size = shaderParameterNameTBOSizeMap[parameter.first];
        if(vi.size() == size){
            shaderIntBuffer.updateData(position*4, vi);
        }else{
            shaderIntBuffer.updateData(position*4, vector<int>(size, vi[0]));
        }
    }
    changedShaderIntParameters.clear();
    
    for(auto &parameter : changedShaderFloatParameters){
        vector<float> &vf = parameter.second;
        int position = shaderFloatParameterNameTBOPositionMap[parameter.first];
        int size = shaderParameterNameTBOSizeMap[parameter.first];
        if(vf.size() == size){
            shaderFloatBuffer.updateData(position*4, vf);
        }else{
            shaderFloatBuffer.updateData(position*4, vector<float>(size, vf[0]));
        }
    }
    changedShaderFloatParameters.clear();
    
    // allocate enough space for all the vertices in a gpu buffer
    buffer[0].allocate(sizeof(glm::vec3) * positions->size() * 256, GL_STATIC_DRAW);
    buffer[1].allocate(sizeof(glm::vec4) * positions->size() * 256, GL_STATIC_DRAW);
    
    ofMesh sMesh;
    sMesh.setMode(OF_PRIMITIVE_POINTS);
    sMesh.addVertices(positions.get());
    vector<ofFloatColor> colors(positions->size());
    for(int i = 0; i < colors.size(); i++){
        colors[i].r = color_red->size() == colors.size() ? color_red->at(i) : color_red->at(0);
        colors[i].g = color_green->size() == colors.size() ? color_green->at(i) : color_green->at(0);
        colors[i].b = color_blue->size() == colors.size() ? color_blue->at(i) : color_blue->at(0);
    }
    sMesh.addColors(colors);
//    for(int i = 0 ; i < positions.get().size() * positionReplicator; i++){
//        glm::vec3 position;
//        float scaleValue = getParameterValueForPosition(scalePositions, i);
//        position.x = ofMap(scaleValue, 0.0, 1.0, 0.5, positions.get()[fmod(i, positions.get().size())].x);
//        position.y = ofMap(scaleValue, 0.0, 1.0, 0.5, positions.get()[fmod(i, positions.get().size())].y);
//        position.z = 1;
//        sMesh.addVertex(position);
//    }
    //mesh = sMesh;
    
    
    vector<ofShader::TransformFeedbackBaseBinding> transfeedbackbuffers;
    transfeedbackbuffers.emplace_back(buffer[0]);
    transfeedbackbuffers.emplace_back(buffer[1]);
    transfeedbackbuffers[1].index = 1;
    // bind the full buffer using glBindBaseBuffer to default index 0
    // and draw the mesh which will end up stored in our buffer
    shader.beginTransformFeedback(GL_POINTS, transfeedbackbuffers);
    shader.setUniform1i("numPositions", totalSize);
    sMesh.draw();
    shader.endTransformFeedback(transfeedbackbuffers);
    
    
    
    // more fine grained control with range bindings:
    // ofShader::TransformFeedbackRangeBinding binding(buffer);
    // binding.index = 0;
    // binding.offset = 0;
    // binding.size = numVertices * sizeof(glm::vec4) * 2;
    // shader.beginTransformFeedback(GL_POINTS, binding);
    
    // now set the buffer as vertices and colors in the vbo
    // by specifying the stride and offsets
    // since we have position and color both as 4 coordinates:
    // 4 coordinates position at offset 0
    vbo.setVertexBuffer(buffer[0], 3, sizeof(glm::vec3), 0);
    // color at offset sizeof(glm::vec4)
    vbo.setColorBuffer(buffer[1], sizeof(glm::vec4), 0);
}

void graphicPatternGenerator::parameterChangedListener(ofAbstractParameter &parameter){
    someParameterChanged = true;
    for(auto &p : *parameters){
        if(p->type() == typeid(ofParameter<vector<int>>).name()){
            changedShaderIntParameters.emplace_back(p->getName(), p->cast<vector<int>>().get());
        }else if(p->type() == typeid(ofParameter<vector<float>>).name()){
            changedShaderFloatParameters.emplace_back(p->getName(), p->cast<vector<float>>().get());
        }
    }
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
        //ofDrawBitmapString(s, x, y);
    }
}

void graphicPatternGenerator::mousePressed(ofMouseEventArgs &a){
    bool foundPoint = false;
    for(int i = 0; i < positions.get().size() && !foundPoint ; i++){
        auto point = positions.get()[i] * glm::vec3(ofGetWidth(), ofGetHeight(), 1.0);
        if(ofRectangle(point.x - 20, point.y - 20, 40, 40).inside(a)){
            pointDraggingIndex = i;
            foundPoint = true;
        }
    }
    if(ofGetKeyPressed(OF_KEY_SHIFT)){
        if(foundPoint){
            vector<glm::vec3> positionsCopy = positions;
            positionsCopy.erase(positionsCopy.begin() + pointDraggingIndex);
            parameters->get("Positions").cast<vector<glm::vec3>>().set(positionsCopy);
            pointDraggingIndex = -1;
        }else{
            vector<glm::vec3> positionsCopy = positions;
            positionsCopy.push_back(ofPoint(a.x / ofGetWidth(), a.y/ofGetHeight()));
            parameters->get("Positions").cast<vector<glm::vec3>>().set(positionsCopy);
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
            vector<glm::vec3> positionsCopy = positions;
            positionsCopy[pointDraggingIndex] = oldPos - (step/100);
            parameters->get("Positions").cast<vector<glm::vec3>>().set(positionsCopy);
        }else{
            vector<glm::vec3> positionsCopy = positions;
            positionsCopy[pointDraggingIndex] = glm::vec3(a, 1) / glm::vec3(ofGetWidth(), ofGetHeight(), 1);
            parameters->get("Positions").cast<vector<glm::vec3>>().set(positionsCopy);
        }
    }
}

