#ifndef __effectAudioProcessing_h__
#define __effectAudioProcessing_h__

#include "effectMain.h"

class effectAudioProcessing :
    public effectMain
{
    void process(float** inputs, float** outputs, long sampleFrames);
    void processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames);
};

#endif
