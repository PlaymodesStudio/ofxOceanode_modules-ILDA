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
    
    addOutputParameterToGroupAndInfo(identifier.set("Identifier", 0, 0, 1));
    listeners.push(identifier.newListener([&](int &i){
        etherdream = new ofxEtherdream();
        etherdream->setup(true, i);
        etherdream->setPPS(pps);
    }));
    parameters->add(resetConnection.set("Reset Connection"));
    listeners.push(resetConnection.newListener([&](){
        //etherdream->kill();
        etherdream->resetup(true);
//        delete etherdream;
//        etherdream = new ofxEtherdream();
//        etherdream->setup(true, identifier);
//        etherdream->setPPS(pps);
    }));
    parameters->add(clear.set("Clear", false));
    parameters->add(in1.set("Polylines In1", {make_pair(ofPolyline(), ofColor())}));
    parameters->add(in2.set("Polylines In2", {make_pair(ofPolyline(), ofColor())}));
    parameters->add(in3.set("Polylines In3", {make_pair(ofPolyline(), ofColor())}));
    parameters->add(in4.set("Polylines In4", {make_pair(ofPolyline(), ofColor())}));
    parameters->add(maxOpacity.set("Max Opacity", 1, 0, 1));
    addParameterToGroupAndInfo(pps.set("pps", 30000, 500, 100000));//.isSavePreset = false;
    addParameterToGroupAndInfo(pointCount.set("Point Count", 300, 0, 3500));//.isSavePreset = false;
    addParameterToGroupAndInfo(minimumPointCount.set("Min Point Count", 1000, 0, 2500));//.isSavePreset = false;
    addParameterToGroupAndInfo(smoothing.set("Smooth Amount", 0, 0, 10));//.isSavePreset = false;
    addParameterToGroupAndInfo(tolerance.set("Tolerance", 0, 0, 1));//.isSavePreset = false;
    addParameterToGroupAndInfo(doSpacing.set("Do Spacing", true));//.isSavePreset = false;
    addParameterToGroupAndInfo(blankCount.set("Blank Count", 25, 0, 60));//.isSavePreset = false;
    addParameterToGroupAndInfo(endCount.set("End Count", 25, 0, 60));//.isSavePreset = false;
    addParameterToGroupAndInfo(flipX.set("Flip X", false));
    addParameterToGroupAndInfo(flipY.set("Flip Y", false));
//    parameters->add(offset.set("Offset", ofPoint(0,0), ofPoint(-1, 1), ofPoint(-1, 1)));
//    parameters->add(scale.set("Scale", ofPoint(1, 1), ofPoint(0,0), ofPoint(1, 1)));
    
    ofAddListener(parameters->parameterChangedE(), this, &ildaController::parameterChangedListener);
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
    auto setPolysFromIn = [this](ofParameter<vector<pair<ofPolyline, ofColor>>> pairVec){
        for(auto poly : pairVec.get()){
            ildaFrame.params.output.color = poly.second * maxOpacity;
            ildaFrame.addPoly(poly.first);
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
        in1 = {make_pair(ofPolyline(), ofColor())};
        in2 = {make_pair(ofPolyline(), ofColor())};
        in3 = {make_pair(ofPolyline(), ofColor())};
        in4 = {make_pair(ofPolyline(), ofColor())};

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
    
    ildaFrame.params.output.transform.scale = scale.get();
    ildaFrame.params.output.transform.offset = offset.get();
    ildaFrame.params.output.transform.doFlipX = flipX;
    ildaFrame.params.output.transform.doFlipY = flipY;
    ildaFrame.params.output.doCapX = capX;
    ildaFrame.params.output.doCapY = capY;
    //ildaFrame.params.output.color = laserColor.get();
    
    //    ildaFrame.params.output.color.r = (float)redCurve[laserColor.r*255]/255.;
    //    ildaFrame.params.output.color.g = (float)greenCurve[laserColor.g*255]/255.;
    //    ildaFrame.params.output.color.b = (float)blueCurve[laserColor.b*255]/255.;
    ildaFrame.params.output.blankCount = blankCount;
    ildaFrame.params.output.endCount = endCount;
}
