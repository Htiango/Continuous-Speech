# Continuous-Speech

## Telephone number recognition system. 
Can recognize either a 7-digit number or a 4-digit number. Record 25 valid telephone numbers randomly and report recognition accuracy.

We use a matrix in below s a state machine to build the telephone number recognition system. In this matrix, each row represent the number in relative position. According to the requirement, we always avoid number 0 and 1 in the first digit when there are 7 digits in the telephone number. We use a traceMatrix to keep trace of the path in order to do the backtracing afterwards. We do a DTW here to get to the final state and then backtracing to get the minimum path and all the number in each digit, which are stored in vector resultPhone.

![](https://oh1ulkf4j.qnssl.com/Pictured.png)

## Recognize unrestricted digit strings. 
Using loopback as well as backpointing.

And then record each digit sequence as a continuous recording and use them as training sets and recognize continuous digit sequences.

When we recognize unrestricted digit number, we use the model as following to represent. Using the method called back pointing to do so. We use a matrix to record where current point comes from (minimal cost point’s position) and whether current state comes from previous last state or last but one state, which means when we doing DTW for input frame and template, current point can come from previous column state (first state of every digit can come from previous last state and last but one state, second state of every digit can come from previous last state). After DTW, we just search the matrix which is called backTable, once find a point comes from previous state (loop back), that means this point is at the start frame of new digit, so we can find which digit the previous point belongs to, then that digit is the previous digit which we should recognize. Every time we find a digit, we just put it into a stack, for that we search from the last frame, then we can pop the stack to get the right order recognized digit.

![](https://oh1ulkf4j.qnssl.com/Pictureda.png)
