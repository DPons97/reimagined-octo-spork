#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include "../../Logger.h"
#include <signal.h>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <sstream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <time.h>
#include <chrono>

//#define FRAME_NAME "/home/dpons/Documents/Programming/OctoSpork/Executables/resources/frames/frame"
#define FRAME_NAME "../Client/Executables/resources/cam2/frame"
#define FRAME_FILE "../Client/Executables/resources/curr_frame.txt"
#define FPS 30

using namespace  cv;
using namespace std;

int sockfd;
long int currFrame;
clock_t lastTime = 0;
Logger * mylog;
int blobTresh = 18;
string nextImg();
bool sendImage(Mat image);
void initCurrFrame();
void saveCurrFrame();
bool waitForConfirm();
bool motionDetected(const Mat &fgMask);

void handler (int signal_number) {
    saveCurrFrame();
    delete mylog;
    exit(signal_number);
}

int main(int argc, char *argv[]) {
    mylog = new Logger(string("node_tracker_").append(to_string(currFrame)),true);
    initCurrFrame();
    signal(SIGTERM, handler);
    sockfd = atoi(argv[1]);
    mylog->writeLog(string("New background subtraction job on socket ").append(to_string(sockfd)));
    mylog->writeLog(string("Starting from frame ").append(to_string(currFrame)));

    //create Background Subtractor objects
    Ptr<BackgroundSubtractor> pBackSub;
    pBackSub = createBackgroundSubtractorMOG2(30, 16, -1);

    Mat frame, fgMask;
    Scalar value;
    int count = 0;
    while (true) {
        /*if (currFrame > 1667) { // video stream is over
            //alert nothing was found and no more frames
            write(sockfd, "imgstop",7);
            saveCurrFrame();
            delete mylog;
            return 0;
        }*/
        frame = imread(nextImg().data(), CV_LOAD_IMAGE_COLOR);
        if (frame.empty()){
            write(sockfd, "imgstop",7);
            saveCurrFrame();
            delete mylog;
            return 0;
        }

        //update the background model
        pBackSub->apply(frame, fgMask);



        if (count > 3 && motionDetected(fgMask)) {
            mylog->writeLog(string("detected motion at frame ").append(to_string(currFrame)));
            mylog->writeLog(string("number of elaborated frames: ").append(to_string(count)));
            break;
        }
        count++;
    }
    sendImage(frame);
    saveCurrFrame();
    delete mylog;
    return 0;
}

long int getTimeMs(){
    return std::chrono::duration_cast< std::chrono::milliseconds >(
            std::chrono::system_clock::now().time_since_epoch()).count();
}

string nextImg(){
    double elapsedTime = ((double) (getTimeMs() - lastTime)/1000);
    mylog->writeLog(string("elapsed time: ").append(to_string(elapsedTime)));
    double toSleep = (0.25-elapsedTime);
    //mylog->writeLog(string("toSleep: ").append(to_string(toSleep)));
    if (toSleep > 0 ) {
        usleep(toSleep * 1000000);
        elapsedTime = ((double) (getTimeMs() - lastTime)/1000 );
        //mylog->writeLog(string("ellapsed time: ").append(to_string(ellapsedTime)));
    }
    if(lastTime != 0) currFrame = currFrame + elapsedTime * FPS;
    lastTime = getTimeMs();
    string to_ret = string(FRAME_NAME).append(to_string(currFrame)).append(".jpg");
    mylog->writeLog(to_ret);
    return to_ret;
}

/**
 * Send image to server
 * @param image to send
 * @return True if success
 */
bool sendImage(Mat image){
    vector<uchar> buff;
    char toLog[100];

    //alert image is coming
    write(sockfd, "imgsend",7);
    if (!waitForConfirm()) return false;

    // send width
    char msg[5];
    sprintf(msg, "%d", image.cols);
    sprintf(toLog, "Sending cols: %s\n", msg);
    mylog->writeLog(toLog);

    write(sockfd, msg, 5);
    if (!waitForConfirm()) return false;

    // send height
    sprintf(msg, "%d", image.rows);
    sprintf(toLog,"Sending rows: %s\n", msg);
    mylog->writeLog(toLog);

    write(sockfd, msg, 5);
    if (!waitForConfirm()) return false;

    // send image
    int  imgSize = image.total()*image.elemSize();
    sprintf(toLog,"Sending image of size: %d", imgSize);
    mylog->writeLog(toLog);

    write(sockfd, image.data, imgSize);

    /*
    namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
    imshow( "Display window", image );                   // Show our image inside it.
    waitKey(0);
     */
    return true;
}

void initCurrFrame(){
    FILE *f = fopen(FRAME_FILE, "r");
    long int lastFrame;
    long int last_ms;

    fscanf(f, "%ld %ld", &lastFrame, &last_ms);

    if (last_ms > 0) {
        long int now_ms = getTimeMs();
        currFrame = lastFrame + ((double) (now_ms -last_ms)/1000)*FPS;
    } else currFrame = lastFrame;

    fclose(f);
}

void saveCurrFrame(){
    FILE *f = fopen(FRAME_FILE, "w");
    long int now_ms = getTimeMs();
    fprintf(f, "%ld %ld", currFrame, now_ms);
    mylog->writeLog(string("Saving current frame number ").append(to_string(currFrame)));
    fclose(f);
}


bool waitForConfirm() {
    char cmdBuff[10];
    int n = 0;

    // Wait for response
    do {
        bzero(cmdBuff, 10);
        n = (int) read(sockfd, cmdBuff, 5);
        if (n <= 0) {
            mylog->writeLog("ERROR reading confirmation or server disconnection");
            return false;
        }
    } while (strcmp(cmdBuff, "ready") != 0);
    //mylog->writeLog("Server is ready for next message");
    return true;
}

// Could be done better.
bool motionDetected(const Mat &fgMask){
    Scalar value = mean(fgMask);
    mylog->writeLog(string("mean ").append(to_string(value[0])));
    return value[0] > blobTresh;
}