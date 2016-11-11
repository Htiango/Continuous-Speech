#include "featureExtractionOld.h"

int frameNumber = 0;

double backgroundTotal = 0;
double startLevel = 0;
double endLevel = 0;

double EnergyPerSampleInDecibelf(SAMPLE *audioframe, long framesToCalc)
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

bool classifyStartFramef(SAMPLE *audioframe, long framesToCalc)
{
	bool isSpeech = false;
	double current = EnergyPerSampleInDecibelf(audioframe, framesToCalc);
	double background = backgroundTotal / FRAME_TO_BACKGROUND_O;

	startLevel = ((startLevel * FORGET_FACTOR_O) + current) / (FORGET_FACTOR_O + 1);

	//    if (flag) {
	//        cout << "backgroundORIRIDIDID " << background << endl;
	//    }


	if (current < background) {
		background = current;
	}
	else {
		background += (current - background) * ADJUSTMENT_O;
	}

	if (startLevel < background) {
		startLevel = background;
	}
	if (startLevel - background > THRESHOLD_F_O) {
		isSpeech = true;
	}
	//    if (flag) {
	//        cout << "current: " << current << endl;
	//        cout << "level: " << levelStart << endl;
	//        cout << "background: " << background << endl;
	//    }
	return isSpeech;
}


bool classifyEndFramef(SAMPLE *audioframe, long framesToCalc)
{
	bool isSpeech = false;
	double current = EnergyPerSampleInDecibelf(audioframe, framesToCalc);
	double background = backgroundTotal / FRAME_TO_BACKGROUND_O;

	endLevel = ((endLevel * FORGET_FACTOR_O) + current) / (FORGET_FACTOR_O + 1);

	if (current < background) {
		background = current;
	}
	else {
		background += (current - background) * ADJUSTMENT_O;
	}

	if (endLevel < background) {
		endLevel = background;
	}
	if (endLevel - background > THRESHOLD_F_O) {
		isSpeech = true;
	}

	return isSpeech;
}


int pruneFrame(short* dataWave, int& numSamples) {
	int start = 0;
	int end = 0;
	bool startFlag = true;
	bool endFlag = true;

	int continueSpeakTimef = 0;
	int continueSilenceTimef = 0;

	for (int i = FRAME_IGNORE_O * SAMPLE_PER_FRAME_O; i < numSamples; i += SAMPLE_PER_FRAME_O) {
		if (i < (FRAME_IGNORE_O + FRAME_TO_BACKGROUND_O) * SAMPLE_PER_FRAME_O) {
			double current = EnergyPerSampleInDecibelf(dataWave + i, SAMPLE_PER_FRAME_O);
			backgroundTotal += current;
		}
		else {
			if (classifyStartFramef(dataWave + i, SAMPLE_PER_FRAME_O)) {
				if (startFlag) {
					continueSpeakTimef++;
				}
			}
			else {
				if (startFlag) {
					continueSpeakTimef = 0;
				}
			}
			if (classifyEndFramef(dataWave + numSamples - i + (FRAME_IGNORE_O + FRAME_TO_BACKGROUND_O - 1) * SAMPLE_PER_FRAME_O, SAMPLE_PER_FRAME_O)) {
				if (endFlag) {
					continueSilenceTimef++;
				}
			}
			else {
				if (endFlag) {
					continueSilenceTimef = 0;
				}
			}

		}

		if (continueSpeakTimef > SPEAKTHRESHOLD_O && startFlag) {
			start = i - SPEAKTHRESHOLD_O * SAMPLE_PER_FRAME_O;
			startFlag = false;
		}
		if (continueSilenceTimef > SILENCETHRESHOLD_F_O && endFlag) {
			end = numSamples - i + (FRAME_IGNORE_O + FRAME_TO_BACKGROUND_O + SILENCETHRESHOLD_F_O) * SAMPLE_PER_FRAME_O;
			endFlag = false;
		}
	}

	numSamples = end - start;
	//    dataWave += start;
	cout << "END    " << end << endl;
	cout << "START   " << start << endl;
	return start;
}

/**

*  do the pre-emphasize   s'[n] = s[n] - alpha * s[n-1]
*
*  @param waveData    input original wave data
*  @param factor      the factor alpha in the formula
*
*  @return            waveData after pre-emphasized
*/
double* preemphasized(short* waveData, double factor, int sampleNum)
{
	//    long sampleNum = sizeof(waveData);

	double* waveDataAfter = new double[sampleNum];

	//    cout << waveData[138008];

	waveDataAfter[0] = waveData[0];

	for (int i = 1; i < sampleNum; i++) {
		waveDataAfter[i] = waveData[i] - waveData[i - 1] * factor;
	}

	return waveDataAfter;
}

/**
*  construct a hamming window
*
*  @param hamWin     hamming window function
*  @param hamWinSize hamming window size
*/
void hamming(double *hamWin, int hamWinSize)
{
	for (int i = 0; i < hamWinSize; i++)
	{
		hamWin[i] = (double)(0.54 - 0.46 * cos(2 * PI * (double)i / ((double)hamWinSize - 1)));
	}
}

/**
*  change the sample data into frame data
*
*  @param waveData          sample data
*  @param numPerframe       the sample point of each frame
*  @param actualNumPerFrame the actual point after zero-padding
*
*  @return  2-d frames, each line represents several points in the frame
*/
double** getFrame(double* waveData, int numPerframe, int actualNumPerFrame, int sampleNum)
{
	//    long sampleNum = sizeof(waveData);
	//    long frameNum;
	double *hamWin = new double[numPerframe];
	hamming(hamWin, numPerframe);

	int uniqueFrameNum = numPerframe / 2;

	if ((sampleNum - uniqueFrameNum) % uniqueFrameNum == 0) {
		frameNumber = (sampleNum - uniqueFrameNum) / uniqueFrameNum;
	}
	else {
		frameNumber = (sampleNum - uniqueFrameNum) / uniqueFrameNum;
	}
	double** frameData = new double*[frameNumber];

	for (int i = 0; i < frameNumber; i++) {
		frameData[i] = new double[actualNumPerFrame];
	}
	//    frameData[0][0] = 1;
	for (int i = 0; i < frameNumber; i++) {
		for (int j = 0; j < actualNumPerFrame; j++) {
			if ((j >= numPerframe) || ((j + i * uniqueFrameNum) >= sampleNum)) {
				frameData[i][j] = 0;
			}
			else frameData[i][j] = waveData[j + i * uniqueFrameNum] * hamWin[j];
		}
	}
	return frameData;
}



/**
*  for each frame, do the fft and compute the energy for the first half
*
*  @param frameData         sample point in each frame
*  @param actualNumPerFrame the length of the frame after zero-padding
*
*  @return discrete frequency power spectrum
*/
double* getFftEnergy(double* frameData, int actualNumPerFrame) {

	double* frameEnergy = new double[actualNumPerFrame / 2 + 1];
	//    Complex *frameDataComplex = new Complex(actualNumPerFrame);
	Complex *frameDataComplex = new Complex[actualNumPerFrame];

	for (int i = 0; i < actualNumPerFrame; i++) {
		frameDataComplex[i] = frameData[i] * 1.0;
	}

	CArray data(frameDataComplex, actualNumPerFrame);

	fft(data);

	for (int i = 0; i < (actualNumPerFrame / 2 + 1); i++) {
		if (abs(data[i]) <= 1) {
			frameEnergy[i] = 0;
		}
		else {
			frameEnergy[i] = pow(abs(data[i]), 2);
		}
	}
	return frameEnergy;
}


/**
*  get the mel-fre power spectrum
*
*  @param fftSampleEnergy discrete frequency power spectrum after fft(the first half)
*
*  @return mel-fre power spectrum
*/
double* getMelEnergy(double* fftSampleEnergy) {
	int filterNum = MEL_POINT_O;
	double* melEnergy = new double[filterNum];
	int frameSize = ACTUAL_SAMPLE_PER_FRAME_O / 2 + 1;
	int sampleRate = SAMPLE_RATE_O;
	double freMax = sampleRate / 2;
	double melFreMax = 1125 * log(1 + freMax / 700);

	double melGap = melFreMax / (filterNum + 1);   // mel-fre gap

	double* melEdge = new double[filterNum + 2];   // each mel-fre edge
	double* freEdge = new double[filterNum + 2];   // each actual-fre edge
	double* freEdgePoint = new double[filterNum + 2];    // each actual-fre edge point

	for (int i = 0; i < filterNum + 2; i++) {
		melEdge[i] = melGap * i;
		freEdge[i] = 700 * (exp(melEdge[i] / 1125) - 1);
		freEdgePoint[i] = (frameSize * freEdge[i] / freMax);
		//        cout << i <<" =" << freEdgePoint[i]<<endl;
	}

	delete[] melEdge;
	//    delete [] freEdge;

	for (int i = 0; i < filterNum; i++) {
		melEnergy[i] = 0;
	}

	for (int i = 1; i <= filterNum; i++) {
		double temp = 0;

		//        cout << "i-1= " << freEdgePoint[i-1] << endl;
		//        cout << "i = " <<  freEdgePoint[i]<< endl;

		for (int j = 0; j < frameSize; j++) {

			if (j < freEdgePoint[i - 1]) {
				temp = 0;
			}
			else if (j >= freEdgePoint[i - 1] && j <= freEdgePoint[i] && (freEdgePoint[i] != freEdgePoint[i - 1])) {
				temp = (j - freEdgePoint[i - 1]) / (freEdgePoint[i] - freEdgePoint[i - 1]) * 1.0;
			}
			else if (j >= freEdgePoint[i] && j <= freEdgePoint[i + 1] && (freEdgePoint[i + 1] != freEdgePoint[i])) {
				temp = (freEdgePoint[i + 1] - j) / (freEdgePoint[i + 1] - freEdgePoint[i]) * 1.0;
			}
			else if (j > freEdgePoint[i + 1]) {
				temp = 0;
			}
			melEnergy[i - 1] += fftSampleEnergy[j] * temp;
			//            if (temp != 0)
			//                cout << "temp = " << temp <<endl;
		}

	}

	return melEnergy;
}


/**
*  do the log for the mel power spectrum
*
*  @param melEnergy mel-fre power spectrum
*
*  @return log mel-fre power spectrum
*/
double* getMelLogEnergy(double* melEnergy) {
	int filterNum = MEL_POINT_O;
	double* melLogEnergy = new double[filterNum];
	for (int i = 0; i < filterNum; i++) {
		if (melEnergy[i] <= 1) {
			melLogEnergy[i] = 0;
		}
		else
			melLogEnergy[i] = log(melEnergy[i]);
	}

	return melLogEnergy;
}

/**
*  do the DCT for each frame.
*
*  @param melLogEnergy        the log mel-fre power spectrum of a frame
*
*  @return         the 13-D DCT result
*/
double* getDCT(double* melLogEnergy) {
	int filterNum = MEL_POINT_O;
	int dctDimension = DCT_DIMENSION_O;

	double* dctResult = new double[dctDimension];

	for (int i = 0; i < dctDimension; i++) {
		dctResult[i] = 0;
	}

	for (int i = 0; i < dctDimension; i++) {
		for (int j = 0; j < filterNum; j++) {
			dctResult[i] += melLogEnergy[j] * cos(PI * i / (2 * filterNum) *  (2 * j + 1));
		}
		//        dctResult[i] = dctResult[i] * sqrt(2.0 / filterNum);
	}
	return dctResult;
}

/**
*  do the norm (sub average and divide variance)
*
*  @param dctResult        the 13 * frameNum DCT result
*/
void getNormalizedDCT(double** dctResult) {
	double averageValue;
	double varianceValue;

	// do the norm
	for (int k = 0; k < DCT_DIMENSION_O; k++) {
		averageValue = 0;
		varianceValue = 0;
		// compute the average of this dimension
		for (int i = 0; i < frameNumber; i++) {
			averageValue += dctResult[i][k];
		}
		averageValue = averageValue / frameNumber;
		// minus the average from the dimension
		for (int i = 0; i < frameNumber; i++) {
			dctResult[i][k] -= averageValue;
			varianceValue += pow(dctResult[i][k], 2);
		}
		varianceValue = varianceValue / frameNumber;
		varianceValue = sqrt(varianceValue);
		// divide the variance from the dimension
		for (int i = 0; i < frameNumber; i++) {
			dctResult[i][k] /= varianceValue;
		}
	}
}


/**
*  entend the feature into 39-D
*
*  @param dctResult        the 13 * frameNum DCT result
*  @param normDCT          the vector of each frame, each with 39-D normed feature, it should be initially null
*/
void DCTNorm(double** dctResult, vector<vector<double>>& normDCT) {

	// put normed dct value into vector
	for (int i = 0; i < frameNumber; i++) {
		vector<double> dimension(DCT_DIMENSION_O * 3);
		for (int k = 0; k < DCT_DIMENSION_O * 3; k++) {
			if (k < DCT_DIMENSION_O) {
				dimension[k] = dctResult[i][k];
			}
			else if (k < DCT_DIMENSION_O * 2) {
				if (i == (frameNumber - 1)) {
					dimension[k] = dctResult[i][k - DCT_DIMENSION_O] - dctResult[i - 1][k - DCT_DIMENSION_O];
				}
				else if (i == 0) {
					//                    cout << "front = " << dctResult[i + 1][k - DCT_DIMENSION]  << endl;
					//                    cout << "now = " << dctResult[i ][k - DCT_DIMENSION] << endl;
					dimension[k] = dctResult[i + 1][k - DCT_DIMENSION_O] - dctResult[i][k - DCT_DIMENSION_O];

				}
				else {
					dimension[k] = dctResult[i + 1][k - DCT_DIMENSION_O] - dctResult[i - 1][k - DCT_DIMENSION_O];
				}
			}
			else {
				if (i == 0) {
					dimension[k] = (dctResult[i + 2][k - 2 * DCT_DIMENSION_O] - dctResult[i][k - 2 * DCT_DIMENSION_O]) - (dctResult[i + 1][k - 2 * DCT_DIMENSION_O] - dctResult[i][k - 2 * DCT_DIMENSION_O]);
				}
				else if (i == (frameNumber - 1)) {
					dimension[k] = (dctResult[i][k - 2 * DCT_DIMENSION_O] - dctResult[i - 1][k - 2 * DCT_DIMENSION_O]) - (dctResult[i][k - 2 * DCT_DIMENSION_O] - dctResult[i - 2][k - 2 * DCT_DIMENSION_O]);
				}
				else if (i == 1) {
					dimension[k] = (dctResult[i + 2][k - 2 * DCT_DIMENSION_O] - dctResult[i][k - 2 * DCT_DIMENSION_O]) - (dctResult[i][k - 2 * DCT_DIMENSION_O] - dctResult[i - 1][k - 2 * DCT_DIMENSION_O]);
				}
				else if (i == (frameNumber - 2)) {
					dimension[k] = (dctResult[i + 1][k - 2 * DCT_DIMENSION_O] - dctResult[i][k - 2 * DCT_DIMENSION_O]) - (dctResult[i][k - 2 * DCT_DIMENSION_O] - dctResult[i - 2][k - 2 * DCT_DIMENSION_O]);
				}
				else {
					dimension[k] = (dctResult[i + 2][k - 2 * DCT_DIMENSION_O] - dctResult[i][k - 2 * DCT_DIMENSION_O]) - (dctResult[i][k - 2 * DCT_DIMENSION_O] - dctResult[i - 2][k - 2 * DCT_DIMENSION_O]);
				}
			}
		}
		normDCT.push_back(dimension);
	}
}




void featureExtractionTwoOld(vector<vector<double>>& normDCT, string& wav, string& filePath) {

	frameNumber = 0;


	short* dataWave;    // store the original wave data
	double* waveDataAfter; // store the wave data after pre-emphasize
	double** frameData;   // store the wave data in each frame
	int numSample;
	int sampleRate;

	const char *wavFile = wav.c_str();

	// read in the wave data
	dataWave = ReadWavFile(wavFile, &numSample, &sampleRate);
	cout << "numSample " << numSample << endl;


	int start = pruneFrameFromStart(dataWave, numSample);
	int end = pruneFrameFromEnd(dataWave, numSample);
	dataWave += start;
	numSample = end - start;
	cout << "numSample " << numSample << endl;


	//    cout << dataWave;

	// do the preemphasize
	waveDataAfter = preemphasized(dataWave, PREEMPHASIZED_FACTOR_O, numSample);
	// divide the wave data into frame (Here represent as 2-D)
	frameData = getFrame(waveDataAfter, SAMPLE_PER_FRAME, ACTUAL_SAMPLE_PER_FRAME_O, numSample);
	// get the frame number
	//    frameNum = sizeof(frameData);

	// do the fft and get each frame's power spectrum
	// get the mel-fre power spectrum, do the log and DCT
	double** frameEnergy = new double*[frameNumber];
	double** melEnergy = new double*[frameNumber];
	double** melLogEnergy = new double*[frameNumber];
	double** frameDCT = new double*[frameNumber];

	for (int i = 0; i < frameNumber; i++) {

		frameEnergy[i] = getFftEnergy(frameData[i], ACTUAL_SAMPLE_PER_FRAME_O);
		melEnergy[i] = getMelEnergy(frameEnergy[i]);
		melLogEnergy[i] = getMelLogEnergy(melEnergy[i]);
		frameDCT[i] = getDCT(melLogEnergy[i]);
	}

	ofstream fileResult(filePath + "result.txt");

	//	delete[] dataWave;
	//	delete[] waveDataAfter;


	getNormalizedDCT(frameDCT);

	cout << "frameNumber =  " << frameNumber << endl;

	DCTNorm(frameDCT, normDCT);

	for (int i = 0; i < frameNumber; i++) {

		for (int j = 0; j < DCT_DIMENSION_O * 3; j++) {
			fileResult << normDCT[i][j] << " ";
		}
		fileResult << endl;
	}


}

void featureExtractionOld(vector<vector<double>>& normDCT, string& wav, string& filePath) {

	frameNumber = 0;
	backgroundTotal = 0;
	startLevel = 0;
	endLevel = 0;



	short* dataWave;    // store the original wave data
	double* waveDataAfter; // store the wave data after pre-emphasize
	double** frameData;   // store the wave data in each frame
	int numSample;
	int sampleRate;

	const char *wavFile = wav.c_str();

	// read in the wave data
	dataWave = ReadWavFile(wavFile, &numSample, &sampleRate);
	cout << "numSample " << numSample << endl;


	short start = pruneFrame(dataWave, numSample);
	dataWave += start;

	cout << "numSample " << numSample << endl;


	//    cout << dataWave;

	// do the preemphasize
	waveDataAfter = preemphasized(dataWave, PREEMPHASIZED_FACTOR_O, numSample);
	// divide the wave data into frame (Here represent as 2-D)
	frameData = getFrame(waveDataAfter, SAMPLE_PER_FRAME, ACTUAL_SAMPLE_PER_FRAME_O, numSample);
	// get the frame number
	//    frameNum = sizeof(frameData);

	// do the fft and get each frame's power spectrum
	// get the mel-fre power spectrum, do the log and DCT
	double** frameEnergy = new double*[frameNumber];
	double** melEnergy = new double*[frameNumber];
	double** melLogEnergy = new double*[frameNumber];
	double** frameDCT = new double*[frameNumber];

	for (int i = 0; i < frameNumber; i++) {

		frameEnergy[i] = getFftEnergy(frameData[i], ACTUAL_SAMPLE_PER_FRAME_O);
		melEnergy[i] = getMelEnergy(frameEnergy[i]);
		melLogEnergy[i] = getMelLogEnergy(melEnergy[i]);
		frameDCT[i] = getDCT(melLogEnergy[i]);
	}

	ofstream fileResult(filePath + "result.txt");

	//	delete[] dataWave;
	//	delete[] waveDataAfter;

	getNormalizedDCT(frameDCT);

	cout << "frameNum =  " << frameNumber << endl;

	DCTNorm(frameDCT, normDCT);
	for (int i = 0; i < frameNumber; i++) {

		for (int j = 0; j < DCT_DIMENSION_O * 3; j++) {
			fileResult << normDCT[i][j] << " ";
		}
		fileResult << endl;
	}

}

