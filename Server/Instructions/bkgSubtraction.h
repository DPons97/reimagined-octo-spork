//
// Created by dpons on 5/19/19.
//

#ifndef OCTOSPORK_BKGSUBTRACTION_H
#define OCTOSPORK_BKGSUBTRACTION_H


#include "Instruction.h"

#include "../darknetCPP/DarknetCalculator.h"

#include <opencv2/core/core.hpp>
#include <opencv2/core/hal/interface.h>
#include <opencv2/highgui/highgui.hpp>

class bkgSubtraction : public Instruction {

public:
    bkgSubtraction(const string &name, const map<int, int> &instructions);

    void start(int nodeSocket, int nodePort) override;

private:
    void backgroundSubtraction(vector<int> toTrack);

    bool getAnswerImg(int bkgSocket, cv::Mat& outMat) const;

};


#endif //OCTOSPORK_BKGSUBTRACTION_H
