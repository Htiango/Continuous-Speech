# Continuous-Speech

## Telephone number recognition system. 
Can recognize either a 7-digit number or a 4-digit number. Record 25 valid telephone numbers randomly and report recognition accuracy.

We use a matrix in below s a state machine to build the telephone number recognition system. In this matrix, each row represent the number in relative position. According to the requirement, we always avoid number 0 and 1 in the first digit when there are 7 digits in the telephone number. We use a traceMatrix to keep trace of the path in order to do the backtracing afterwards. We do a DTW here to get to the final state and then backtracing to get the minimum path and all the number in each digit, which are stored in vector resultPhone.

![](https://oh1ulkf4j.qnssl.com/Pictured.png)

### Result
Run 25 templates with several 7-digit number and the other 4-digit number. Use levenshtein distance to calculate the edit distance to get the word recognition accuracy. For project 1 and project 3, we both get the same word accuracy 30.5732%. The result picture is shown as:

![](https://oh1ulkf4j.qnssl.com/Picture7.png)

## Recognize unrestricted digit strings. 
Using loopback as well as backpointing.

And then record each digit sequence as a continuous recording and use them as training sets and recognize continuous digit sequences.

When we recognize unrestricted digit number, we use the model as following to represent. Using the method called back pointing to do so. We use a matrix to record where current point comes from (minimal cost point’s position) and whether current state comes from previous last state or last but one state, which means when we doing DTW for input frame and template, current point can come from previous column state (first state of every digit can come from previous last state and last but one state, second state of every digit can come from previous last state). After DTW, we just search the matrix which is called backTable, once find a point comes from previous state (loop back), that means this point is at the start frame of new digit, so we can find which digit the previous point belongs to, then that digit is the previous digit which we should recognize. Every time we find a digit, we just put it into a stack, for that we search from the last frame, then we can pop the stack to get the right order recognized digit.

![](https://oh1ulkf4j.qnssl.com/Pictureda.png)

When we do the digit recognition from continuous speech, the flow chart is represented as:

![](https://oh1ulkf4j.qnssl.com/Picture3.png)


We first record several continuous speech as the templates. Then we initialize the HMM for all digits using the isolated recordings in project 6.1. Here we adapt the methods used in project 6.1, regarding the templates firstly as the input and using state machine to separate the continuous. And then do the segmental k-mean to really separate the template and get the HMM of the relative digit. The process is seen as :

![](https://oh1ulkf4j.qnssl.com/Picture4.png)

![](https://oh1ulkf4j.qnssl.com/Picture5.png)

### Result
We record several continuous speech several times. Train to get the separate digit number. Use the separate digit number as the new isolate templates. And use the previous 25 templates as the testing set. Here we can see that when using state machine, we can get a quite good accuracy of 78.98%. And if recognize unrestricted digit string, the accuracy is also 61.78%. These are all the word level accuracy.

![](https://oh1ulkf4j.qnssl.com/Picture8.png)

