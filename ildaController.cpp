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
}

void ildaController::setup(){
    etherdream = new ofxEtherdream();
    etherdream->setup();
    etherdream->setPPS(30000);
    
    ildaFrame.params.output.transform.scale = glm::vec2(1,1);
    
    parameters->add(resetConnection.set("Reset Connection", false));
    parameters->add(clear.set("Clear", false));
    parameters->add(in1.set("Polylines In1", {make_pair(ofPolyline(), ofColor())}));
    parameters->add(in2.set("Polylines In2", {make_pair(ofPolyline(), ofColor())}));
    parameters->add(in3.set("Polylines In3", {make_pair(ofPolyline(), ofColor())}));
    parameters->add(in4.set("Polylines In4", {make_pair(ofPolyline(), ofColor())}));
    parameters->add(maxOpacity.set("Max Opacity", 1, 0, 1));
    parameters->add(pps.set("pps", 30000, 500, 60000));
    parameters->add(pointCount.set("Point Count", 300, 0, 2500));
    parameters->add(minimumPointCount.set("Min Point Count", 1000, 0, 2500));
    parameters->add(smoothing.set("Smooth Amount", 0, 0, 10));
    parameters->add(tolerance.set("Tolerance", 0, 0, 1));
    parameters->add(doSpacing.set("Do Spacing", true));
    parameters->add(blankCount.set("Blank Count", 25, 0, 60));
    parameters->add(endCount.set("End Count", 25, 0, 60));
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
    
    setPolysFromIn(in1);
    setPolysFromIn(in2);
    setPolysFromIn(in3);
    setPolysFromIn(in4);
}

void ildaController::parameterChangedListener(ofAbstractParameter &param){
    if(param.getName() == "Clear"){
        in1 = {make_pair(ofPolyline(), ofColor())};
        in2 = {make_pair(ofPolyline(), ofColor())};
        in3 = {make_pair(ofPolyline(), ofColor())};
        in4 = {make_pair(ofPolyline(), ofColor())};

    }
    else if(param.getName() == "Reset Connection"){
        if(resetConnection){
            resetConnection = false;
            etherdream->kill();
            delete etherdream;
            etherdream = new ofxEtherdream();
            etherdream->setup();
            etherdream->setPPS(pps);
        }
    }
    else if(param.getName() == "pps"){
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
    ildaFrame.params.output.color = laserColor.get();
    
    //    ildaFrame.params.output.color.r = (float)redCurve[laserColor.r*255]/255.;
    //    ildaFrame.params.output.color.g = (float)greenCurve[laserColor.g*255]/255.;
    //    ildaFrame.params.output.color.b = (float)blueCurve[laserColor.b*255]/255.;
    ildaFrame.params.output.blankCount = blankCount;
    ildaFrame.params.output.endCount = endCount;
}
