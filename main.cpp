/**
#include <stdio.h>
#include <windows.h>
#include "ioBabTts.h"
#define _BABTTSDYN_IMPL_
#include "ifBabTtsDyn.h"
#undef _BABTTSDYN_IMPL_
/**/

#include <iostream>
using namespace std;

#include "ofMain.h"
#include "ofApp.h"
#include "Touche.h"



//========================================================================
int main( ){
////////////////////////////////////////ACAPELA////////////////////////////////////////
/**
    if (BabTtsInitDllEx("C:/Acapela") == NULL){
        cout << "Error: Could not load AcaTTS\n" ;
        return -1;
    }

    if (!BabTTS_Init()){
        cout << "Error : Could not initialize AcaTTS\n" ;
        BabTtsUninitDll();
        return -2;
    }
/**/
////////////////////////////////////////OF////////////////////////////////////////
	ofSetupOpenGL(1024,768,OF_WINDOW);
	ofRunApp(new ofApp());

////////////////////////////////////////Close Acapela/////////////////////////////////////////////
/**
    BabTTS_Uninit();
    BabTtsUninitDll();
/**/

    return 0;
}

