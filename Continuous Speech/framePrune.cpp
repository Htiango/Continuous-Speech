#include "framePrune.h"

double EnergyPerSample(SAMPLE *audioframe, long framesToCalc)
{
	double sum = 0;
	double decibel = 0;
	double pointValue = 0; // the value of each point of the frame

	for (int i = 0; i < framesToCalc; i++) {
		pointValue = *audioframe;
		sum += pow(pointValue, 2);
		audioframe++;
	}

	decibel = 10 * log10(sum);

	return decibel;
}

bool classifyFrame(SAMPLE *audioframe, long framesToCalc, double& backgroundOf10f, double& levelStart)
{
	bool isSpeech = false;
	double current = EnergyPerSample(audioframe, framesToCalc);
	double background = backgroundOf10f / FRAME_TO_BACKGROUND;

	levelStart = ((levelStart * FORGET_FACTOR) + current) / (FORGET_FACTOR + 1);

	//    if (flag) {
	//        cout << "backgroundORIRIDIDID " << background << endl;
	//    }


	if (current < background) {
		background = current;
	}
	else {
		background += (current - background) * ADJUSTMENT;
	}

	if (levelStart < background) {
		levelStart = background;
	}
	if (levelStart - background > THRESHOLD_F) {
		isSpeech = true;
	}
	//    if (flag) {
	//        cout << "current: " << current << endl;
	//        cout << "level: " << levelStart << endl;
	//        cout << "background: " << background << endl;
	//    }
	return isSpeech;
}


int pruneFrameFromStart(short* dataWave, int& numSamples) {
	double backgroundOf10f = 0;
	double levelStart = 0;
	int start = 0;
	bool startFlag = true;
	int continueSpeakTimef = 0;

	for (int i = FRAME_IGNORE_S * SAMPLE_PER_FRAME; i < numSamples; i += SAMPLE_PER_FRAME) {
		if (i < (FRAME_IGNORE_S + FRAME_TO_BACKGROUND) * SAMPLE_PER_FRAME) {
			double current = EnergyPerSample(dataWave + i, SAMPLE_PER_FRAME);
			backgroundOf10f += current;
		}
		else {
			if (classifyFrame(dataWave + i, SAMPLE_PER_FRAME, backgroundOf10f, levelStart)) {
				if (startFlag) {
					continueSpeakTimef++;
				}
			}
			else {
				if (startFlag) {
					continueSpeakTimef = 0;
				}
			}

		}
		if (continueSpeakTimef > SPEAKTHRESHOLD && startFlag) {
			start = i - SPEAKTHRESHOLD * SAMPLE_PER_FRAME;
			startFlag = false;
			break;
		}
	}
	cout << "START   " << start << endl;
	return start;
}

int pruneFrameFromEnd(short* dataWave, int& numSamples) {
	double backgroundOf10f = 0;
	double levelEnd = 0;
	int end = 0;
	bool endFlag = true;

	int continueSpeakTimef = 0;

	for (int i = numSamples - FRAME_IGNORE_E * SAMPLE_PER_FRAME; i > 0; i -= SAMPLE_PER_FRAME) {
		if (i > numSamples - (FRAME_IGNORE_E + FRAME_TO_BACKGROUND) * SAMPLE_PER_FRAME) {
			double current = EnergyPerSample(dataWave + i, SAMPLE_PER_FRAME);
			backgroundOf10f += current;
		}
		else {
			if (classifyFrame(dataWave + i, SAMPLE_PER_FRAME, backgroundOf10f, levelEnd)) {
				if (endFlag) {
					continueSpeakTimef++;
				}
			}
			else {
				if (endFlag) {
					continueSpeakTimef = 0;
				}
			}
		}
		if (continueSpeakTimef > SILENCETHRESHOLD && endFlag) {
			end = i + SILENCETHRESHOLD * SAMPLE_PER_FRAME;
			endFlag = false;
			break;
		}
	}
	cout << "END   " << end << endl;
	return end;
}

