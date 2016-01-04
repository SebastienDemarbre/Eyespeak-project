
///////////////////OF///////////////////
#pragma once

#include "ofMain.h"
#include "Touche.h"

///////////////////Acapela///////////////////
/**
#include <stdio.h>
#include <windows.h>
#include "ioBabTts.h"
#include "ifBabTtsDyn.h"
/**/

///////////////////OpenCV///////////////////
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"

#include <iostream>
using namespace cv;


///////////////////ofApp///////////////////
extern int global;
extern int afficher;
extern int switcher;



class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

        void autoCompletion(string mot, vector<string> tabMots, vector<string> &tabMotsAff);
        void drawAutoCompletion();
        void calibrageManuel();
        void nouveauCalibrage();
        void calibrageAuto();




        ofVideoGrabber cam;
        ofImage image;

        //We declare the text we will display 
        ofTrueTypeFont texte, limitos;
        ofTrueTypeFont save;
        //To use a sound
        ofSoundPlayer son;
        //Window size
        int heightWdw;
        int widthWdw;
        //Declaration of an array of Touche (key)
        Touche *tab[90];
        string tabString [90];
        //Integer useful to change pages
        int a;
        //Text passed in parameter to the "drawString" method to display
        string texto;

        int i; //Impact the color of the "on" button
        int compteurRayon; //For automatic calibration
        bool del = false;
        bool espace = false;
        bool boolVideo = true;
        bool mouse = false;
        bool pass, pass1;
        bool active;
        bool saveActive= true;

        int limite, lim;
        ofImage oeil;
        ofImage helper;
        //Save file
        ofstream sauvegarde;
        ifstream lecture;

        //Array of words to display (auto-completion)
        vector <string> tabMotsAff;
        vector <string> tabMots;
        string recup;
        int tailledumot;

        //OfTrueTypeFont for the 5 words to write
        ofTrueTypeFont mot1;
        ofTrueTypeFont mot2;
        ofTrueTypeFont mot3;
        ofTrueTypeFont mot4;
        ofTrueTypeFont mot5;


////////////////////////////////////////////////////////////////////////OpenCV//////////////////////////////////////////////////////////////
        //Mat = a matrix of images
        Mat frame;
        //VideoCapture is a class that allow to use camera
        VideoCapture cap;
        //"Objectif"  is the point that will be determined by Eyetracking applied to the video. "Pointeur" is the "objectif" point put to computer screen dimensions
        cv::Point objectif, pointeur;
        //Know if the calibration have already been done or not
        bool boolCalibrage = false;
        bool parametreCalibrer = false;
        int compteurCalibrage = 0;
        int widthVid = 640;
        int heightVid = 480;
        //Points from the calibration (Top left, top right, bottom left, bottom right and center)
        vector<cv::Point> pointsCalibrage;
        //Rectangle that defines the dimensions of the calibration area
        cv::Rect zoneCalibrage;

////////////////////////////////////////////////////////////////////////Acapela//////////////////////////////////////////////////////////////
        /**
        LPBABTTS myVoice;
        BabTtsError error;
        /**/
};
