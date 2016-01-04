#include "ofApp.h"
#include "Touche.h"

#include <vector>
#include <iostream>
#include <string>
#include <fstream>


using namespace std;
using namespace cv;


//calculates the binarization threshold according to the brightness of the image
int calculerSeuil(Mat &image){

	static int counterGray = 5 ;
    static int counterPixel;
    static int delayFrames = 0;

    if (delayFrames%10 == 0) { //the threshold is recalculated every 10 frames that which reduces the computation time and increases the fluidity of detection
        counterGray = 0;
        counterPixel = 0;
        //calcul de la lumiosité moyenne
        for (int i = 0; i < image.rows ; i++){
            for (int j = 0 ; j < image.cols ; j++){
                if ((int)image.at<uchar>(i,j) < 255)
                    counterGray += (int)image.at<uchar>(i, j);
                counterPixel++;
            }
        }
        counterGray /= counterPixel;
    }
    delayFrames ++;
	return counterGray*2.5;
}
//calculate the largest convex contour from a binary image
vector<cv::Point> convex_Hull(Mat &frame){
    vector<vector<cv::Point> > contours;
    vector<Vec4i> hierarchy;
    findContours(frame, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );

    vector<vector<cv::Point> > hull(contours.size());
    frame.setTo(0);

    size_t size_max = 0, max = 0;

    for( unsigned i = 0; i < contours.size(); i++ ) {
        convexHull( Mat(contours[i]), hull[i], false ); //calcul des cv::Points du contours
        if (hull[i].size() > size_max) { //Permet de sélectionner le contours le plus grand
            max = i;
            size_max = hull[i].size();
        }
    }
    return (contours[max]);
}
//Calculate a Region Of Interest
void ROI(Mat& image){
	Mat mask = image.clone();
	Mat imBis = image.clone();
	mask.setTo(0);
	imBis.setTo(255);

	int x = image.cols/4,
		y = image.rows/4,
		width = image.cols/2,
		height = image.rows/2;
	cv::Rect box(x, y, width, height);


	rectangle(mask, box, 255, CV_FILLED, 8, 0);
	rectangle(imBis, box, 0, CV_FILLED, 8, 0);

	bitwise_and(image, mask, image);	//Apply the filter
	image = image + imBis;				//Put the image on a white background
}
//calculate the coordinates of the eye on basis of an image
cv::Point calculCoordonnees (Mat &image){
    Mat gray;

	//We use grayscale because it is easier to handle
	cvtColor(image, gray, COLOR_BGR2GRAY);
	//Definition of a Region Of Interest
	ROI(gray);
	//Applying a filter on the image will blur it and make the ward area uniform
	medianBlur(gray, gray, 21);
	//  Go to black and white from Grayscale for the user clearly appears in black on a white background
	threshold(gray, gray, calculerSeuil(gray), 255,0);
	//detection of the contours of the ward(pupil) via a convex contours
	vector<cv::Point> contours = convex_Hull(gray);

	//If there is less than 5 points, it's impossible to have an ellipsis --> impossible to have a point -->return the coordinates of the center of the image
	if( contours.size() > 5) {
		//passed an ellipsis through the points of the contours that were detected
		cv::RotatedRect box = fitEllipse(contours);
		return box.center;
	} else {
		static int counter = 0;
		counter++;
		if (counter >= 5){
                cout << "Error : Eye not detected !" << endl;
                counter =0;
		}
		return cv::Point(0,0);
	}

}
//return a calibration area
cv::Rect calibrer(vector<cv::Point> pointsCalibrage){
	return boundingRect(pointsCalibrage);
}

//calculate the position of the cv::Pointer given calibration, display area and cv::Point in the calibration area
cv::Point pointer(cv::Rect zoneCalibration, cv::Rect zoneAffichage, cv::Point objectif){
//cv::Rectangle représentant la zone de calibrage dans l'image filmée; cv::Rectangle représentant la zone d'affichage sur l'écran; position de l'oeil sur l'image filmée
    float x1 = (float)zoneCalibration.x;
    float x2 = (float)zoneCalibration.x + zoneCalibration.width;
    float y1 = (float)zoneCalibration.y;
    float y2 = (float)zoneCalibration.y + zoneCalibration.height;
    float x = (float)objectif.x;
    float y = (float)objectif.y;

    float w1 = (float)zoneAffichage.x;
    float w2 = (float)zoneAffichage.x + (float)zoneAffichage.width;
    float z1 = (float)zoneAffichage.y;
    float z2 = (float)zoneAffichage.y + (float)zoneAffichage.height;

    float w = w1 + (w2-w1)*(x-x1)/(x2-x1);
    float z = z1 + (z2-z1)*(y-y1)/(y2-y1);


    if(w > w2)
        w = w2 - 1;
    else if(w < w1)
        w = w1 + 1;
    if(z > z2)
        z = z2 - 1;
    else if(z < z1)
        z = z1 + 1;

    return cv::Point((int)w,(int)z);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int global = 0;
int afficher = -1;
int switcher = 5;


void ofApp::setup(){

    //Search all available cameras
    //If no one is found, just quit the application
    vector<ofVideoDevice> devices = cam.listDevices();
    int id;
    for(id = 0; id < devices.size(); id++){
		cout << devices[id].id << ": " << devices[id].deviceName;
        if( devices[id].bAvailable ){
            cout << endl;
        }else{
            cout << " - unavailable " << endl;
            if (id == devices.size() - 1)
                ofApp::exit();
        }

	}
	id--;
    cam.setDeviceID(id);
    cam.setDesiredFrameRate(50);
    cam.initGrabber(widthVid, heightVid);
    ofSetFrameRate(50);
    pointeur = cv::Point(0,0);


    //////////////////////////Interface parameters////////////////////////
    lim = 7;
    limite = lim;
    a = -1;
    pass = true;
    pass1 = true;
    active = true;
    i = 0;
    tailledumot = -2;
    texto=" ";
    compteurRayon = 100;

    for (int j = 0; j < 5 ; j++)
        tabMotsAff.push_back(" ");

    //Setting the song
    son.loadSound("son.mp3");

    //Setting the text font
    texte.loadFont("arial.ttf", 23);
    limitos.loadFont ("arial.ttf", 70);
    save.loadFont("arial.ttf", 23);

    mot1.loadFont("arial.ttf", 23);
    mot2.loadFont("arial.ttf", 23);
    mot3.loadFont("arial.ttf", 23);
    mot4.loadFont("arial.ttf", 23);
    mot5.loadFont("arial.ttf", 23);

    //We fill in the string array with the name of the pictures of each button, that will be useful to change it when we hover the button

    tabString[0]="af.png";
    tabString[1]="gm.png";
    tabString[2]="ns.png";
    tabString[3]="tz.png";
    tabString[4]="a.png";
    tabString[5]="bc.png";
    tabString[6]="df.png";
    tabString[7]="e.png";
    tabString[8]="gh.png";
    tabString[9]="i.png";

    tabString[10]="jk.png";
    tabString[11]="ml.png";
    tabString[12]="n.png";
    tabString[13]="op.png";
    tabString[14]="qr.png";
    tabString[15]="s.png";
    tabString[16]="t.png";
    tabString[17]="uv.png";
    tabString[18]="wx.png";
    tabString[19]="yz.png";

    tabString[20]="b.png";
    tabString[21]="c.png";
    tabString[22]="d.png";
    tabString[23]="f.png";
    tabString[24]="g.png";
    tabString[25]="h.png";
    tabString[26]="j.png";
    tabString[27]="k.png";
    tabString[28]="m.png";
    tabString[29]="l.png";

    tabString[30]="o.png";
    tabString[31]="p.png";
    tabString[32]="q.png";
    tabString[33]="r.png";
    tabString[34]="u.png";
    tabString[35]="v.png";
    tabString[36]="w.png";
    tabString[37]="x.png";
    tabString[38]="y.png";
    tabString[39]="z.png";

    tabString[40]="alphabet.png";
    tabString[41]="chiffres.png";
    tabString[42]="caractères.png";
    tabString[43]="options.png";
    tabString[44]="01.png";
    tabString[45]="23.png";
    tabString[46]="45.png";
    tabString[47]="6789.png";
    tabString[48]="0.png";
    tabString[49]="1.png";

    tabString[50]="2.png";
    tabString[51]="3.png";
    tabString[52]="4.png";
    tabString[53]="5.png";
    tabString[54]="6.png";
    tabString[55]="7.png";
    tabString[56]="8.png";
    tabString[57]="9.png";
    tabString[58]="pct1.png";
    tabString[59]="pct2.png";

    tabString[60]="pct3.png";
    tabString[61]="pct4.png";
    tabString[62]="pct11.png";
    tabString[63]="pct12.png";
    tabString[64]="pct13.png";
    tabString[65]="pct14.png";
    tabString[66]="(.png";
    tabString[67]=").png";
    tabString[68]="-.png";
    tabString[69]="_.png";

    tabString[70]="@.png";
    tabString[71]="guillemet.png";
    tabString[72]="'.png";
    tabString[73]="backslash.png";
    tabString[74]="txt.png";
    tabString[75]="Calibrage.png";
    tabString[76]="Afficher.png";
    tabString[77]="Aide.png";
    tabString[78]="paramètres.png";

    tabString[80]="plus.png";
    tabString[81]="moins.png";
    tabString[82]="blanche.png";
    tabString[83]="blanche.png";
    tabString[84]="blanche.png";
    tabString[85]="blanche.png";
    tabString[86]="blanche.png";
    tabString[87]="Ouvrir.png";
    tabString[88]="Sauvegarder.png";
    tabString[89]="blanche.png";


    //Definition of an array of buttons    (posx, posy, nombouton, height, width)
    tab [0] = new Touche (0.01, 0.01, "af", 0.4, 0.49);
    tab [1] = new Touche (0.5, 0.01, "gm", 0.4, 0.49);
    tab [2] = new Touche (0.01, 0.6, "ns", 0.39, 0.49);
    tab [3] = new Touche (0.5, 0.6, "tz", 0.39, 0.49);
    tab [4] = new Touche (0.01, 0.01, "a", 0.4, 0.49);
    tab [5] = new Touche (0.5, 0.01, "bc", 0.4, 0.49);
    tab [6] = new Touche (0.01, 0.6, "df", 0.39, 0.49);
    tab [7] = new Touche (0.5, 0.6, "e", 0.39, 0.49);
    tab [8] = new Touche (0.01, 0.01, "gh", 0.4, 0.49);
    tab [9] = new Touche (0.5, 0.01, "i", 0.4, 0.49 );

    tab [10] = new Touche (0.01, 0.6, "jk", 0.39, 0.49);
    tab [11] = new Touche (0.5, 0.6, "ml", 0.39, 0.49);
    tab [12] = new Touche (0.01, 0.01, "n", 0.4, 0.49);
    tab [13] = new Touche (0.5, 0.01, "op", 0.4, 0.49 );
    tab [14] = new Touche (0.01, 0.6, "qr", 0.39, 0.49);
    tab [15] = new Touche (0.5, 0.6, "s", 0.39, 0.49);
    tab [16] = new Touche (0.01, 0.01, "t", 0.4, 0.49);
    tab [17] = new Touche (0.5, 0.01, "uv", 0.4, 0.49);
    tab [18] = new Touche (0.01, 0.6, "wx", 0.39, 0.49);
    tab [19] = new Touche (0.5, 0.6, "yz", 0.39, 0.49);

    tab [20] = new Touche (0.01, 0.01, "b", 0.4, 0.98);
    tab [21] = new Touche (0.01, 0.6, "c", 0.39, 0.98);
    tab [22] = new Touche (0.01, 0.01, "d", 0.4, 0.98);
    tab [23] = new Touche (0.01, 0.6, "f", 0.39, 0.98);
    tab [24] = new Touche (0.01, 0.01, "g", 0.4, 0.98);
    tab [25] = new Touche (0.01, 0.6, "h", 0.39, 0.98);
    tab [26] = new Touche (0.01, 0.01, "j", 0.4, 0.98);
    tab [27] = new Touche (0.01, 0.6, "k", 0.39, 0.98);
    tab [28] = new Touche (0.01, 0.01, "m", 0.4, 0.98);
    tab [29] = new Touche (0.01, 0.6, "l", 0.39, 0.98);

    tab [30] = new Touche (0.01, 0.01, "o", 0.4, 0.98);
    tab [31] = new Touche (0.01, 0.6, "p", 0.39, 0.98);
    tab [32] = new Touche (0.01, 0.01, "q", 0.4, 0.98);
    tab [33] = new Touche (0.01, 0.6, "r", 0.39, 0.98);
    tab [34] = new Touche (0.01, 0.01, "u", 0.4, 0.98);
    tab [35] = new Touche (0.01, 0.6, "v", 0.39, 0.98);
    tab [36] = new Touche (0.01, 0.01, "w", 0.4, 0.98);
    tab [37] = new Touche (0.01, 0.6, "x", 0.39, 0.98);
    tab [38] = new Touche (0.01, 0.01, "y", 0.4, 0.98);
    tab [39] = new Touche (0.01, 0.6, "z", 0.39, 0.98);

    tab [40] = new Touche (0.01,0.01, "alphabet", 0.4, 0.49);
    tab [41] = new Touche (0.5,0.01, "chiffres", 0.4, 0.49);
    tab [42] = new Touche (0.01,0.6, "caractères", 0.39, 0.49);
    tab [43] = new Touche (0.5,0.6, "options", 0.39, 0.49);
    tab [44] = new Touche (0.01,0.01, "01", 0.4, 0.49);
    tab [45] = new Touche (0.5,0.01, "23", 0.4, 0.49);
    tab [46] = new Touche (0.01,0.6, "45", 0.39, 0.49);
    tab [47] = new Touche (0.5,0.6, "6789", 0.39, 0.49);
    tab [48] = new Touche (0.01,0.01, "0", 0.4, 0.98);
    tab [49] = new Touche (0.01,0.6, "1", 0.39, 0.98);

    tab [50] = new Touche (0.01,0.01, "2", 0.4, 0.98);
    tab [51] = new Touche (0.01,0.6, "3", 0.39, 0.98);
    tab [52] = new Touche (0.01,0.01, "4", 0.4, 0.98);
    tab [53] = new Touche (0.01,0.6, "5", 0.39, 0.98);
    tab [54] = new Touche (0.01,0.01, "6", 0.4, 0.49);
    tab [55] = new Touche (0.5,0.01, "7", 0.4, 0.49);
    tab [56] = new Touche (0.01,0.6, "8", 0.39, 0.49);
    tab [57] = new Touche (0.5,0.6, "9", 0.39, 0.49);
    tab [58] = new Touche (0.01,0.01, "pct1", 0.4, 0.49);
    tab [59] = new Touche (0.5,0.01, "pct2", 0.4, 0.49);

    tab [60] = new Touche (0.01,0.6, "pct3", 0.39, 0.49);
    tab [61] = new Touche (0.5,0.6, "pct4", 0.39, 0.49);
    tab [62] = new Touche (0.01,0.01, "pct11", 0.4, 0.49);
    tab [63] = new Touche (0.5,0.01, "pct12", 0.4, 0.49);
    tab [64] = new Touche (0.01,0.6, "pct13", 0.39, 0.49);
    tab [65] = new Touche (0.5,0.6, "pct14", 0.39, 0.49);
    tab [66] = new Touche (0.01,0.01, "(", 0.4, 0.49);
    tab [67] = new Touche (0.5,0.01, ")", 0.4, 0.49);
    tab [68] = new Touche (0.01,0.6, "-", 0.39, 0.49);
    tab [69] = new Touche (0.5,0.6, "slashbas", 0.39, 0.49);

    tab [70] = new Touche (0.01,0.01, "@", 0.4, 0.49);
    tab [71] = new Touche (0.5,0.01, "guillemet", 0.4, 0.49);
    tab [72] = new Touche (0.01,0.6, "'", 0.39, 0.49);
    tab [73] = new Touche (0.5,0.6, "backslash", 0.39, 0.49);
    tab [74] = new Touche (0.01, 0.4, "txt", 0.2, 0.98);
    tab [75] = new Touche (0.01,0.01, "Calibrage", 0.4, 0.49);
    tab [76] = new Touche (0.5,0.01, "Afficher", 0.4, 0.49);
    tab [77] = new Touche (0.01, 0.6, "Aide", 0.39, 0.49);
    tab [78] = new Touche (0.5,0.6, "paramètres", 0.39, 0.49);
    tab [79] = new Touche (0.25, 0.25, "On", 0.5, 0.5);

    tab [80] = new Touche (0.01,0.01, "plus", 0.4, 0.98);
    tab [81] = new Touche (0.01,0.6, "moins", 0.39, 0.98);
    tab [82] = new Touche (0.01,0, "blanche", 0.01, 0.98); //delete
    tab [83] = new Touche (0,0.01, "blanche", 0.98, 0.01); //Accueil
    tab [84] = new Touche (0.01,0.99, "blanche", 0.01, 0.98); //espace
    tab [85] = new Touche (0.99,0.01, "blanche", 0.49, 0.01); //Proposition de mots
    tab [86] = new Touche (0, 0, "blanche", 0.01, 0.01);
    tab [87] = new Touche (0.01, 0.01, "Ouvrir", 0.4, 0.98);
    tab [88] = new Touche (0.01, 0.6, "Sauvegarder", 0.39, 0.98);
    tab [89] = new Touche (0.99,0.50, "blanche", 0.49, 0.01); //parler le texte

    oeil.loadImage("oeil.png");
    helper.loadImage("aider.png");
    //////////////////////////////Auto-completion initialisation//////////////////////////////
    lecture.open("Liste_mots.txt");
    while(lecture.good()){
        getline(lecture,recup);
        tabMots.push_back(recup);
    }
    lecture.close();
//////////////////////////////////////////Acapela/////////////////////////////////////////////////////////////
    /**
    myVoice = BabTTS_Create();
    error = BabTTS_Open(myVoice, "Julie22k_HQ", BABTTS_USEDEFDICT);
    if (error != E_BABTTS_NOERROR) {
        cout << "Error while opening voice : " << error << endl;
        BabTTS_Uninit();
        BabTtsUninitDll();
        system("pause");
        ofApp::exit();
    }
    /**/

 }
//--------------------------------------------------------------
void ofApp::update(){

    cv::Rect zoneAffichage = cv::Rect(0,0,widthWdw,heightWdw);

    //Each value of the "a" parameter defines on which page we are

//In all cases
    if (a != 25 && a!=30) {
        for (int z = 82; z < 87; z++){
            tab[z]->ChangeImage(pointeur.x, pointeur.y, tabString[z], a, z, widthWdw, heightWdw, texto, lim, parametreCalibrer);
            tab[z]->Inactivate();
        }
        int z = 89;
        tab[z]->ChangeImage(pointeur.x, pointeur.y, tabString[z], a, z, widthWdw, heightWdw, texto, lim, parametreCalibrer);
        tab[z]->Inactivate();
    }

    if (a==30){
            tab[83]->ChangeImage(pointeur.x, pointeur.y, tabString[83], a, 83, widthWdw, heightWdw, texto, lim, parametreCalibrer);
            tab[83]->Inactivate();
    }

    if (a!=30){
        tab[74]->ChangeImage(pointeur.x, pointeur.y, tabString[74], a, 74, widthWdw, heightWdw, texto, lim, parametreCalibrer);
        tab[74]->Inactivate();
    }



    switch (a) {

		case -1:
			for (int z=40; z<44; z++){
				tab[z]->ChangeImage(pointeur.x, pointeur.y, tabString[z], a, z, widthWdw, heightWdw, texto, lim, parametreCalibrer);
				tab[z]->Inactivate();
			}
			if (a==25)
					active=false;
			break;

		case 0:            
									//Every button from the home page(a=0) will call the "ChangeImage" and "Inactivate" methods (See the "Touche" class)
									//We did the same thing if we are on another page. We have 14 pages, that's why there are 14 cases

			for (int z=0; z<4; z++){
				tab[z]->ChangeImage(pointeur.x, pointeur.y, tabString[z], a, z, widthWdw, heightWdw, texto, lim, parametreCalibrer);
				tab[z]->Inactivate();
			}
			break;

		case 1:

			for (int z=4 ; z<8; z++){
				tab[z]->ChangeImage(pointeur.x, pointeur.y, tabString[z], a, z, widthWdw, heightWdw,texto, lim, parametreCalibrer);
				tab[z]->Inactivate();
			}
			break;

		case 2:
			for (int z=8 ; z<12; z++){
				tab[z]->ChangeImage(pointeur.x, pointeur.y, tabString[z], a, z, widthWdw, heightWdw,texto, lim, parametreCalibrer);
				tab[z]->Inactivate();
			}
			break;

		case 3:
			for (int z=12 ; z<16; z++){
				tab[z]->ChangeImage(pointeur.x, pointeur.y, tabString[z], a, z, widthWdw, heightWdw,texto, lim, parametreCalibrer);
				tab[z]->Inactivate();
			}
			break;

		case 4:
			for (int z=16 ; z<20; z++){
				tab[z]->ChangeImage(pointeur.x, pointeur.y, tabString[z], a, z, widthWdw, heightWdw,texto, lim, parametreCalibrer);
				tab[z]->Inactivate();
			}
			break;

		case 5:
			for (int z=20 ; z<22; z++){
				tab[z]->ChangeImage(pointeur.x, pointeur.y, tabString[z], a, z, widthWdw, heightWdw,texto, lim, parametreCalibrer);
				tab[z]->Inactivate();
			}
			break;

		case 6:
			for (int z=22 ; z<24; z++){
				tab[z]->ChangeImage(pointeur.x, pointeur.y, tabString[z], a, z, widthWdw, heightWdw,texto, lim, parametreCalibrer);
				tab[z]->Inactivate();
			}
			break;

		case 7:
			for (int z=24 ; z<26; z++){
				tab[z]->ChangeImage(pointeur.x, pointeur.y, tabString[z], a, z, widthWdw, heightWdw,texto, lim, parametreCalibrer);
				tab[z]->Inactivate();
			}
			break;

		case 8:
			for (int z=26 ; z<28; z++){
				tab[z]->ChangeImage(pointeur.x, pointeur.y, tabString[z], a, z, widthWdw, heightWdw,texto, lim, parametreCalibrer);
				tab[z]->Inactivate();
			}
			break;

		case 9:
			for (int z=28 ; z<30; z++){
				tab[z]->ChangeImage(pointeur.x, pointeur.y, tabString[z], a, z, widthWdw, heightWdw,texto, lim, parametreCalibrer);
				tab[z]->Inactivate();
			}
			break;

		case 10:
			for (int z=30 ; z<32; z++){
				tab[z]->ChangeImage(pointeur.x, pointeur.y, tabString[z], a, z, widthWdw, heightWdw,texto, lim, parametreCalibrer);
				tab[z]->Inactivate();
			}
			break;

		case 11:
			for (int z=32 ; z<34; z++){
				tab[z]->ChangeImage(pointeur.x, pointeur.y, tabString[z], a, z, widthWdw, heightWdw,texto, lim, parametreCalibrer);
				tab[z]->Inactivate();
			}
			break;

		case 12:
			for (int z=34 ; z<36; z++){
				tab[z]->ChangeImage(pointeur.x, pointeur.y, tabString[z], a, z, widthWdw, heightWdw,texto, lim, parametreCalibrer);
				tab[z]->Inactivate();
			}
			break;

		case 13:
			for (int z=36 ; z<38; z++){
				tab[z]->ChangeImage(pointeur.x, pointeur.y, tabString[z], a, z, widthWdw, heightWdw,texto, lim, parametreCalibrer);
				tab[z]->Inactivate();
			}
			break;

		case 14:
			for (int z=38 ; z<40; z++){
				tab[z]->ChangeImage(pointeur.x, pointeur.y, tabString[z], a, z, widthWdw, heightWdw,texto, lim, parametreCalibrer);
				tab[z]->Inactivate();
			}
			break;

		case 15 :
			for (int z=44 ; z<48; z++){
				tab[z]->ChangeImage(pointeur.x, pointeur.y, tabString[z], a, z, widthWdw, heightWdw,texto, lim, parametreCalibrer);
				tab[z]->Inactivate();
			}
			break;

		case 16:
			for (int z=48 ; z<50; z++){
				tab[z]->ChangeImage(pointeur.x, pointeur.y, tabString[z], a, z, widthWdw, heightWdw,texto, lim, parametreCalibrer);
				tab[z]->Inactivate();
			}
				break;

		case 17:
			for (int z=50 ; z<52; z++){
				tab[z]->ChangeImage(pointeur.x, pointeur.y, tabString[z], a, z, widthWdw, heightWdw,texto, lim, parametreCalibrer);
				tab[z]->Inactivate();
			}
				break;

		case 18:
			for (int z=52 ; z<54; z++){
				tab[z]->ChangeImage(pointeur.x, pointeur.y, tabString[z], a, z, widthWdw, heightWdw,texto, lim, parametreCalibrer);
				tab[z]->Inactivate();
			}
				break;

		case 19:
			for (int z=54 ; z<58; z++){
				tab[z]->ChangeImage(pointeur.x, pointeur.y, tabString[z], a, z, widthWdw, heightWdw,texto, lim, parametreCalibrer);
				tab[z]->Inactivate();
			}
			break;

		case 20:
			for (int z=58; z<62; z++){
				tab[z]->ChangeImage(pointeur.x, pointeur.y, tabString[z], a, z, widthWdw, heightWdw,texto, lim, parametreCalibrer);
				tab[z]->Inactivate();
			}
			break;

		case 21:
			for (int z=62; z<66; z++){
				tab[z]->ChangeImage(pointeur.x, pointeur.y, tabString[z], a, z, widthWdw, heightWdw,texto, lim, parametreCalibrer);
				tab[z]->Inactivate();
			}
			break;

		case 22:
			for (int z=66; z<70; z++){
				tab[z]->ChangeImage(pointeur.x, pointeur.y, tabString[z], a, z, widthWdw, heightWdw,texto, lim, parametreCalibrer);
				tab[z]->Inactivate();
			}
			break;

		case 23:
			for (int z=70; z<74; z++){
				tab[z]->ChangeImage(pointeur.x, pointeur.y, tabString[z], a, z, widthWdw, heightWdw,texto, lim, parametreCalibrer);
				tab[z]->Inactivate();
			}
			break;

		case 24:
			for (int z=75; z<79; z++) {
				tab[z]->ChangeImage(pointeur.x, pointeur.y, tabString[z], a, z, widthWdw, heightWdw,texto, lim, parametreCalibrer);
				tab[z]->Inactivate();
			}

        break;

		case 26:
			for (int z=80; z<82; z++) {
				tab[z]->ChangeImage(pointeur.x, pointeur.y, tabString[z], a, z, widthWdw, heightWdw,texto, lim, parametreCalibrer);
				tab[z]->Inactivate();
				cout<<limite<<endl;
				if (a==27){
					limite++;
				}
				else if (a==28 && limite>5){
					limite--;
				}
				a=26;
			}

			lim = limite;
			break;

        case 29 :
            for (int z=87; z<89; z++){
				tab[z]->ChangeImage(pointeur.x, pointeur.y, tabString[z], a, z, widthWdw, heightWdw,texto, lim, parametreCalibrer);
				tab[z]->Inactivate();
			}

        break;

    }


    if (afficher != -1){
        global = 0;
        for (int y = texto.length()-1; y >= 0; y--){
            if (texto[y] == ' '){
                texto.erase(y+1, texto.length()-1);
                texto = texto + tabMotsAff[afficher] + " ";
                afficher = -1;
                break;
            }
        }
    }

	//////////////////////////////////////////////OpenCV////////////////////////////////////////////////
	//charge l'image de la vidéo dans "frame"
    cam.update();
    if(cam.isFrameNew()){
        image.setFromPixels(cam.getPixelsRef());
        image.mirror(true,false);
    }
    frame = Mat(cam.getHeight(), cam.getWidth(), CV_8UC3, cam.getPixels());
    
    //vertically invert the image 
    //theoretically, we shoud also horizontaly invert the image so that if the user looks left, the eye (front filmed) in the video also goes left
    //Virtually, we did not do it because the camera was upside so the only think we had to do was to vertically invert the image
    cv::flip(frame, frame, 0); //flip around x axis


    //calculate the coordinate point from the filmed image 
    objectif = calculCoordonnees(frame);

    if(!boolCalibrage){
        //If calibration already done, convert the coordinates of the center of the eye in pointer coordinates
        pointeur = pointer(zoneCalibrage, zoneAffichage, objectif);
    } else {
        pointeur = cv::Point(0,0);
        }
    if(mouse){
            pointeur = cv::Point(mouseX,mouseY);
    }
    if(parametreCalibrer){
        nouveauCalibrage();
    }

}
//--------------------------------------------------------------
void ofApp::draw(){


    heightWdw = ofGetWindowHeight(); 
    widthWdw = ofGetWindowWidth();

    //We use it to be sure that nothing will be drawn over the help button
    tab [74]->dessiner(widthWdw, heightWdw);

    //Home page drawing
	if (active == true) {



		switch (a){


		case -1:
			for (int i=40; i<44;i++){
				tab[i]->dessiner(widthWdw, heightWdw);
			}
			break;

		case 0:         
						// The 4 buttons on the home page calls the method "dessiner"
						//This method will draw the picture relative to each button depending on which page we are
						//That's why there are 14 cases

			for (int i=0; i<4;i++){
				tab[i]->dessiner(widthWdw, heightWdw);
			}
			break;

		case 1:
			for (int i=4; i<8;i++){
				tab[i]->dessiner(widthWdw, heightWdw);
			}
			break;

		case 2:

			for (int i=8; i<12;i++){
				tab[i]->dessiner(widthWdw, heightWdw);
			}
			break;

		case 3:
			for (int i=12 ; i<16; i++){
				tab[i]->dessiner(widthWdw, heightWdw);
			}
			break;

		case 4 :
			for (int i=16; i<20; i++){
				tab[i]->dessiner(widthWdw, heightWdw);
			}
			break;

		case 5:
			for (int i=20; i<22;i++){
				tab[i]->dessiner(widthWdw, heightWdw);
			}
			break;

		case 6:
			for (int i=22; i<24;i++){
				tab[i]->dessiner(widthWdw, heightWdw);
			}
			break;

		case 7:
			for (int i=24; i<26;i++){
				tab[i]->dessiner(widthWdw, heightWdw);
			}
			break;

		case 8:
			for (int i=26; i<28;i++){
				tab[i]->dessiner(widthWdw, heightWdw);
			}
			break;

		case 9:
			for (int i=28; i<30;i++){
				tab[i]->dessiner(widthWdw, heightWdw);
			}
			break;

		case 10:
			for (int i=30; i<32;i++){
				tab[i]->dessiner(widthWdw, heightWdw);
			}
			break;

		case 11:
			for (int i=32; i<34;i++){
				tab[i]->dessiner(widthWdw, heightWdw);
			}
			break;

		case 12:
			for (int i=34; i<36;i++){
				tab[i]->dessiner(widthWdw, heightWdw);
			}
			break;

		case 13:
			for (int i=36; i<38;i++){
				tab[i]->dessiner(widthWdw, heightWdw);
			}
			break;

		case 14:
			for (int i=38; i<40;i++){
				tab[i]->dessiner(widthWdw, heightWdw);
			}
			break;
		case 15 :
			for (int i=44; i<48;i++){
				tab[i]->dessiner(widthWdw, heightWdw);
			}
			break;

		case 16:
			for (int i=48; i<50;i++){
				tab[i]->dessiner(widthWdw, heightWdw);
			}
			break;

		case 17:
			for (int i=50; i<52;i++){
				tab[i]->dessiner(widthWdw, heightWdw);
			}
			break;

		case 18:
			for (int i=52; i<54;i++){
				tab[i]->dessiner(widthWdw, heightWdw);
			}
			break;

		case 19:
			for (int i=54; i<58;i++){
				tab[i]->dessiner(widthWdw, heightWdw);
			}
			break;

		case 20:
			for (int i=58; i<62;i++){
				tab[i]->dessiner(widthWdw, heightWdw);
			}
			break;

		case 21:
			for (int i=62; i<66;i++){
				tab[i]->dessiner(widthWdw, heightWdw);
			}
			break;

		case 22:
			for (int i=66; i<70;i++){
				tab[i]->dessiner(widthWdw, heightWdw);
			}
			break;

		case 23:
			for (int i=70; i<74;i++){
				tab[i]->dessiner(widthWdw, heightWdw);
			}
			break;

		case 24 :
			for (int i=75; i<79;i++){
				tab[i]->dessiner(widthWdw, heightWdw);
			}
			break;

        case 25  :

            break;

		case 26:
			for (int i=80; i<82;i++){
				tab[i]->dessiner(widthWdw, heightWdw);
			}
			break;

        case 29:
			for (int i=87; i<89;i++){
				tab[i]->dessiner(widthWdw, heightWdw);
			}
			break;
        case 30 :

            helper.resize(widthWdw,heightWdw);
            helper.draw(0, 0);

            break;
		default :
			break;

		}


		if (a!=26 && a!=25 && a!=30){
			ofSetColor(0);
			texte.drawString(texto, widthWdw*0.05,heightWdw*0.45);
			ofSetColor(255);

			if (880<texte.stringWidth(texto) && texte.stringWidth(texto)<900){
				if (pass==true){
					texto=texto + "\n";
					pass=false;
				}
            }
				if (900 < texte.stringWidth(texto) && texte.stringWidth(texto) < 920 && texte.stringHeight(texto) > 30){
					if (pass1 == true){
						texto=texto + "\n";
						pass1=false;
					}
				}
            if (920 < texte.stringWidth(texto)){
                string tempString, recup;

                // This part will load the text already saved
                lecture.open("Mon_texte.txt");
                while(lecture.good()){
                    getline(lecture,recup);
                    tempString += recup + "\n";
                }
                lecture.close();
                //cout << "Ancien texte = \n" << tempString << endl;

                // This part will add new lines in the pre existing text
                sauvegarde.open("Mon_texte.txt");
                sauvegarde << tempString + texto;
                sauvegarde.close();
                cout << "Nouveau texte = \n" << tempString << endl;

                texto = "";
            }

		}
        if (a == 26){
            ofSetColor(0);
            stringstream limitess;
            limitess<<limite;
            limitos.drawString("L'attente est de : " + limitess.str(), widthWdw*0.035,heightWdw*0.56);
            ofSetColor(255);
            cout << " _____________________________" << endl;
        }
	}

    if (a == 25){
        ofBackground(i+55);
        tab[79]->dessiner(widthWdw,heightWdw);
        if (tab[79]->inside(pointeur.x, pointeur.y, widthWdw, heightWdw)){
            cout<<i<<endl;
            i += 8;
            if (i >= 200){
                active=true;
                i = 0;
                ofBackground(200);
                a = -1;
            }
        }
        else
            i=0;
    }

// Auto-completion of words method
    if(tailledumot!=texto.length()){
        tailledumot=texto.length();
        autoCompletion(texto,tabMots,tabMotsAff);
    }
//Display suggested words
    if (a != 30 && a!=25) {
        static int globalBis = 0;

        switch (global){

            case 0 :

                ofSetColor(127);
                mot1.drawString(tabMotsAff[0], widthWdw*0.035,heightWdw*0.58);
                mot2.drawString(tabMotsAff[1], widthWdw*0.235,heightWdw*0.58);
                mot3.drawString(tabMotsAff[2], widthWdw*0.435,heightWdw*0.58);
                mot4.drawString(tabMotsAff[3], widthWdw*0.635,heightWdw*0.58);
                mot5.drawString(tabMotsAff[4], widthWdw*0.835,heightWdw*0.58);

                mot1.loadFont("arial.ttf", 23);
                mot2.loadFont("arial.ttf", 23);
                mot3.loadFont("arial.ttf", 23);
                mot4.loadFont("arial.ttf", 23);
                mot5.loadFont("arial.ttf", 23);

                break;


            case 1 :

                ofSetColor (0);
                mot1.loadFont("arial.ttf", 32);
                mot1.drawString(tabMotsAff[0], widthWdw*0.035,heightWdw*0.58);

                ofSetColor(127);
                mot2.drawString(tabMotsAff[1], widthWdw*0.235,heightWdw*0.58);
                mot3.drawString(tabMotsAff[2], widthWdw*0.435,heightWdw*0.58);
                mot4.drawString(tabMotsAff[3], widthWdw*0.635,heightWdw*0.58);
                mot5.drawString(tabMotsAff[4], widthWdw*0.835,heightWdw*0.58);
                mot5.loadFont("arial.ttf", 23);

                break;

            case 2 :

                ofSetColor (0);
                mot2.loadFont("arial.ttf", 32);
                mot2.drawString(tabMotsAff[1], widthWdw*0.235,heightWdw*0.58);

                ofSetColor(127);
                mot1.drawString(tabMotsAff[0], widthWdw*0.035,heightWdw*0.58);
                mot3.drawString(tabMotsAff[2], widthWdw*0.435,heightWdw*0.58);
                mot4.drawString(tabMotsAff[3], widthWdw*0.635,heightWdw*0.58);
                mot5.drawString(tabMotsAff[4], widthWdw*0.835,heightWdw*0.58);
                mot1.loadFont("arial.ttf", 23);

                break;

            case 3 :

                ofSetColor(0);
                mot3.loadFont("arial.ttf", 32);
                mot3.drawString(tabMotsAff[2], widthWdw*0.435,heightWdw*0.58);

                ofSetColor(127);
                mot2.loadFont("arial.ttf", 23);
                mot1.drawString(tabMotsAff[0], widthWdw*0.035,heightWdw*0.58);
                mot2.drawString(tabMotsAff[1], widthWdw*0.235,heightWdw*0.58);
                mot4.drawString(tabMotsAff[3], widthWdw*0.635,heightWdw*0.58);
                mot5.drawString(tabMotsAff[4], widthWdw*0.835,heightWdw*0.58);

                break;

            case 4 :

                ofSetColor(0);
                mot4.loadFont("arial.ttf", 32);
                mot4.drawString(tabMotsAff[3], widthWdw*0.635,heightWdw*0.58);

                ofSetColor(127);
                mot3.loadFont("arial.ttf", 23);
                mot1.drawString(tabMotsAff[0], widthWdw*0.035,heightWdw*0.58);
                mot2.drawString(tabMotsAff[1], widthWdw*0.235,heightWdw*0.58);
                mot3.drawString(tabMotsAff[2], widthWdw*0.435,heightWdw*0.58);
                mot5.drawString(tabMotsAff[4], widthWdw*0.835,heightWdw*0.58);

                break;

            case 5 :

                ofSetColor(0);
                mot5.loadFont("arial.ttf", 32);
                mot5.drawString(tabMotsAff[4], widthWdw*0.835,heightWdw*0.58);

                ofSetColor(127);
                mot4.loadFont("arial.ttf", 23);
                mot1.drawString(tabMotsAff[0], widthWdw*0.035,heightWdw*0.58);
                mot2.drawString(tabMotsAff[1], widthWdw*0.235,heightWdw*0.58);
                mot3.drawString(tabMotsAff[2], widthWdw*0.435,heightWdw*0.58);
                mot4.drawString(tabMotsAff[3], widthWdw*0.635,heightWdw*0.58);

                break;
        }

        ofSetColor(255);

        if (globalBis != global){
            /**
            BabTTS_Speak(myVoice, (char*)tabMotsAff[global-1].c_str(), BABTTS_ASYNC);
            /**/
            globalBis = global;
        }
    }


    //////////////////////////////Displaying video///////////////////////
    if (boolVideo){
        image.draw(widthWdw/2 - widthVid/2, heightWdw/2 - heightVid/2);
        ofCircle(widthWdw/2 - widthVid/2 + objectif.x, heightWdw/2 - heightVid/2 + objectif.y, 7);
        ofSetColor(255,0,0);
        for(int counter = 0 ; counter < compteurCalibrage ; counter++){
            ofCircle(widthWdw/2 - widthVid/2 + pointsCalibrage[counter].x, heightWdw/2 - heightVid/2 + pointsCalibrage[counter].y, 5);
        }
        ofSetColor(255,255,255);
    }
    ////////////////////////////Displaying pointer///////////////////
    if (!boolCalibrage){
        oeil.resize(widthWdw*0.05, heightWdw*0.03);
        oeil.draw(pointeur.x-(oeil.width)/2, pointeur.y-(oeil.height)/2);
    }
    //////////////////////////Automatic calibration/////////////////////
    if (boolCalibrage){
        calibrageAuto();
    }
}
//--------------------------------------------------------------
void ofApp::nouveauCalibrage(){
    parametreCalibrer = false;
    pointeur = cv::Point(0,0);

    cout << "Nouveau calibrage" << endl;
    pointsCalibrage.clear();
    boolCalibrage = true;
    boolVideo = true;
    compteurCalibrage = 0;
    compteurRayon = 100;

    /**
    string message = "Veuillez regarder successivement les 4 points, encerclés de rouge. Le calibrage va commencer dans. 3. 2. 1.";
    BabTTS_Speak(myVoice, (char*)message.c_str(), BABTTS_SYNC);
    /**/

}
void ofApp::calibrageManuel(){
    //Récupère la coordonnée de l'oeil et la stocke dans un vecteur.
    pointsCalibrage.push_back(objectif);
    compteurCalibrage ++;
    cout << "calibrage " << pointsCalibrage[compteurCalibrage].x << ";" << pointsCalibrage[compteurCalibrage].y << endl;
    if (compteurCalibrage >= 4){//si on a tous les points, il faut calculer la zone délimitée et dire que le calibrage est fini (=false)
        boolCalibrage = false;
        boolVideo = false;
        zoneCalibrage = calibrer(pointsCalibrage);
    }
    compteurRayon = 100;

    /**
    stringstream msg;
    msg << compteurCalibrage;
    string message = msg.str();
    if (compteurCalibrage > 1)
        message += " points ont étés placés.";
    else
        message += " point a été placé.";
    if (compteurCalibrage >= 4)
        message += " Calibrage terminé.";
    BabTTS_Speak(myVoice, (char*)message.c_str(), BABTTS_ASYNC);
    /**/
}
void ofApp::calibrageAuto(){

    compteurRayon *= 0.8;

    ofSetColor(255,0,0);
    ofNoFill();
    switch (compteurCalibrage){
    case 0 :
        ofCircle(0,0,compteurRayon*sqrt(widthWdw^2 + heightWdw^2)/2);
        ofFill();
        ofCircle(1,1,10);
        break;
    case 1 :
        ofCircle(widthWdw,0,compteurRayon*sqrt(widthWdw^2 + heightWdw^2)/2);
        ofFill();
        ofCircle(widthWdw-1,1,10);
        break;
    case 2 :
        ofCircle(0,heightWdw,compteurRayon*sqrt(widthWdw^2 + heightWdw^2)/2);
        ofFill();
        ofCircle(1,heightWdw-1,10);
        break;
    case 3 :
        ofCircle(widthWdw,heightWdw,compteurRayon*sqrt(widthWdw^2 + heightWdw^2)/2);
        ofFill();
        ofCircle(widthWdw-1,heightWdw-1,10);
        break;
    default : break;
    }
    ofSetColor(255,255,255);


    if (compteurRayon <= 0){
        calibrageManuel();
    }
}
void ofApp::autoCompletion(string mot, vector<string> tabMots, vector<string> &tabMotsAff){

    char space;
    int taille = 0;
    switcher = 0;
    mot= ' ' + mot;

    for(int j = 0 ; j < tabMotsAff.size() ; j++){
        tabMotsAff[j]=' ';
    }


    while (space != ' ') {
        taille++;
        space = mot[mot.length()-taille];
    }
    //taille est la taille du dernier mot (en train de s'écrire)
    if(mot[mot.length() - taille]==' '){
        for(int i = 0; i < tabMots.size() ; i++){
            for(int z = taille - 1 ; z >= 1 ; z--){
                if(mot[mot.length()-taille+z] != tabMots[i][z-1]){
                    tabMots[i]='0';
                    break;
                }
            }
        }
    }

    for(int i = 0 ; i < tabMots.size() ; i++){
        if(tabMots[i].compare("0")!=0){
            tabMotsAff[switcher] = tabMots[i];
            switcher++;
            if (switcher == tabMotsAff.size()) {
                global = 0;
                break;
            }
        }
    }

}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == OF_KEY_UP && boolCalibrage){
        calibrageManuel();
    }

    if (key == OF_KEY_DOWN){
        nouveauCalibrage();
    }

    if (key == OF_KEY_LEFT){
        boolVideo = false;
    }

    if (key == OF_KEY_RIGHT){
        boolVideo = true;
    }

    if (key == ' '){
        mouse = !mouse;
        pointeur = cv::Point(0,0);
        /**
        string message;
        if(mouse)
            message = "Mode souris activé";
        else
            message = "Détection oculaire activée";
        BabTTS_Speak(myVoice, (char*)message.c_str(), BABTTS_ASYNC);
        /**/

    }

    if (key == 'q' || key == 'Q'){
        a = -1;
    }
}



