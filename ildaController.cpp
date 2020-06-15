//
//  ildaController.cpp
//  Espill-ilda
//
//  Created by Eduard Frigola on 26/06/2017.
//
//

#include "ildaController.h"
#include "ofxOceanodeContainer.h"

ildaController::ildaController() : ofxOceanodeNodeModelExternalWindow("Ilda Controller"){
    //setup();
    etherdream = nullptr;
}

void ildaController::setup(){
    ildaFrame.params.output.transform.scale = glm::vec2(1,1);
    
    addOutputParameter(identifier.set("Identifier", 0, 0, 1), ofxOceanodeParameterFlags_DisableSavePreset);
    listeners.push(identifier.newListener([&](int &i){
        etherdream = new ofxEtherdream();
        etherdream->setup(true, i);
        etherdream->setPPS(pps);
    }));
    addParameter(resetConnection.set("Reset Connection"));
    listeners.push(resetConnection.newListener([&](){
        //etherdream->kill();
        etherdream->resetup(true);
//        delete etherdream;
//        etherdream = new ofxEtherdream();
//        etherdream->setup(true, identifier);
//        etherdream->setPPS(pps);
    }));
    addParameter(clear.set("Clear", false));
    addParameter(in1.set("Polylines In1", {ofPath()}));
    addParameter(in2.set("Polylines In2", {ofPath()}));
    addParameter(in3.set("Polylines In3", {ofPath()}));
    addParameter(in4.set("Polylines In4", {ofPath()}));
    addParameter(maxOpacity.set("Max Opacity", 1, 0, 1), ofxOceanodeParameterFlags_DisableSavePreset);
    addParameter(pps.set("pps", 30000, 500, 100000));//.isSavePreset = false;
    addParameter(pointCount.set("Point Count", 300, 0, 3500));//.isSavePreset = false;
    addParameter(minimumPointCount.set("Min Point Count", 1000, 0, 2500));//.isSavePreset = false;
    addParameter(smoothing.set("Smooth Amount", 0, 0, 10));//.isSavePreset = false;
    addParameter(tolerance.set("Tolerance", 0, 0, 1));//.isSavePreset = false;
    addParameter(doSpacing.set("Do Spacing", true));//.isSavePreset = false;
    addParameter(blankCount.set("Blank Count", 25, 0, 60));//.isSavePreset = false;
    addParameter(endCount.set("End Count", 25, 0, 60));//.isSavePreset = false;
    addParameter(flipX.set("Flip X", false));
    addParameter(flipY.set("Flip Y", false));
    addParameter(offsetX.set("Offset X", 0, -1, 1), ofxOceanodeParameterFlags_DisableSavePreset);
    addParameter(offsetY.set("Offset Y", 0, -1, 1), ofxOceanodeParameterFlags_DisableSavePreset);
    addParameter(scaleX.set("Scale X", 1, 0, 2), ofxOceanodeParameterFlags_DisableSavePreset);
    addParameter(scaleY.set("Scale Y", 1, 0, 2), ofxOceanodeParameterFlags_DisableSavePreset);
//    addParameter(offset.set("Offset", ofPoint(0,0), ofPoint(-1, 1), ofPoint(-1, 1)));
//    addParameter(scale.set("Scale", ofPoint(1, 1), ofPoint(0,0), ofPoint(1, 1)));
    
    ofAddListener(getParameterGroup().parameterChangedE(), this, &ildaController::parameterChangedListener);
//    etherdream = new ofxEtherdream();
//    etherdream->setup(true, 0);
//    etherdream->setPPS(pps);
    
}

void ildaController::setPolylines(vector<pair<ofPolyline, ofColor>> polys){
    ildaFrame.clear();
    for(auto poly : polys){
        ildaFrame.params.output.color = poly.second * maxOpacity;
        ildaFrame.addPoly(poly.first);
    }
}

void ildaController::setPolylines(){
    ildaFrame.clear();
    auto setPolysFromIn = [this](ofParameter<vector<ofPath>> pairVec){
        for(auto path : pairVec.get()){
            for(auto poly : path.getOutline()){
                ildaFrame.params.output.color = ofFloatColor(path.getStrokeColor()) * ((float)path.getStrokeColor().a/255.0) * maxOpacity;// * ((path.getStrokeColor().a * maxOpacity)/255.0);
                ildaFrame.addPoly(poly);
            }
        }
    };
    if(in1->size() == 0 && in2->size() == 0 && in3->size() == 0 && in4->size() == 0){
        ildaFrame.clear();
    }
    else{
        setPolysFromIn(in1);
        setPolysFromIn(in2);
        setPolysFromIn(in3);
        setPolysFromIn(in4);
    }
}

void ildaController::parameterChangedListener(ofAbstractParameter &param){
    if(param.getName() == "Clear"){
        in1 = {ofPath()};
        in2 = {ofPath()};
        in3 = {ofPath()};
        in4 = {ofPath()};
    }
    else if(param.getName() == "pps"){
        if(etherdream != nullptr)
            etherdream->setPPS(param.cast<int>());
    }
    ildaFrame.polyProcessor.params.targetPointCount=pointCount-(blankCount+endCount)*ildaFrame.getPolys().size()*2;
    if (pointCount != 0 && ildaFrame.polyProcessor.params.targetPointCount < 0)
        ildaFrame.polyProcessor.params.targetPointCount = 10;
    else if (pointCount == 0)
        ildaFrame.polyProcessor.params.targetPointCount = 0;
    
    ildaFrame.polyProcessor.params.smoothAmount=smoothing;
    ildaFrame.polyProcessor.params.optimizeTolerance=tolerance;
    ildaFrame.polyProcessor.params.spacing=doSpacing;
    
//    ildaFrame.params.output.transform.scale = scale.get();
//    ildaFrame.params.output.transform.offset = offset.get();
    ildaFrame.params.output.transform.scale = glm::vec2(scaleX, scaleY);
    ildaFrame.params.output.transform.offset = glm::vec2(offsetX, offsetY);

    ildaFrame.params.output.transform.doFlipX = !flipX;
    ildaFrame.params.output.transform.doFlipY = !flipY;
    ildaFrame.params.output.doCapX = capX;
    ildaFrame.params.output.doCapY = capY;
    //ildaFrame.params.output.color = laserColor.get();
    
    //    ildaFrame.params.output.color.r = (float)redCurve[laserColor.r*255]/255.;
    //    ildaFrame.params.output.color.g = (float)greenCurve[laserColor.g*255]/255.;
    //    ildaFrame.params.output.color.b = (float)blueCurve[laserColor.b*255]/255.;
    ildaFrame.params.output.blankCount = blankCount;
    ildaFrame.params.output.endCount = endCount;
}
