
////////ACAPELA\\\\\\\\
/**
#include <stdio.h>
#include <windows.h>
#include "ioBabTts.h"
#include "ifBabTtsDyn.h"
/**/

#include "Touche.h"
#include <string>
#include "ofApp.h"
#include <fstream>

using namespace std;


Touche::Touche(float posx, float posy, string nombouton, float height, float width){
x = posx;
y = posy;
nom = nombouton;
hauteur = height;
largeur = width;
compteur=0;
dedans= false;

}

string Touche::GetNom(){
return nom;
}
float Touche::GetHauteur(){

return hauteur;
}
float Touche::GetLargeur(){
return largeur;
}

void Touche::dessiner(int screenx, int screeny){        //screenx and screeny = size of the window
  ofImage img;                //Declaration of an ofImage object
  img.loadImage(nom+".png");         //Each key(Touche) is characterized by a "name" which is actually the name of his image(blabla.png).
  img.resize(largeur*screenx, hauteur*screeny);       //The  "dessiner"  method is continuously called to the buttons on the screen , it will use the resize function if the user enlarges or reduces the size of the window ( working in relative size )
  img.draw(x*screenx,y*screeny);                      //Then draw the image to the desired position 
}

void Touche::ChangeImage(int x1, int y1, string name, int &a, int z,int screenx, int screeny, string &passe, int lim, bool &parametreCalibrer){

    if (this->inside(x1, y1, screenx, screeny))     //The "ChangeImage" method will allow us to move from one page to another
    
       string tmp;
       tmp=name.insert((name.size()-4),"B");
       this->SetNom(tmp.erase(tmp.size()-4));
    
    
        compteur++;
    
    
        if (compteur == lim)
        {
        compteur=0;
        this->SetNom(nom.erase(nom.size()-1));
    
        switch (z)                                  //the integer "z" represents the button which called the method (tab[z]->ChangeImage)
        {
    
    
    
    
        case 0:                                     
                                                    //If it is the button z=0 (so , A->F)
                                                    //Increment the counter that is on each key
                                                    //When this counter equals 15 , go to page a=1 from a=0
                                                    //That's why there are so many cases (as much as buttons)
    
    
            a=1;
            break;
    
        case 1 :
    
            a=2;
            break;
    
        case 2 :
    
            a=3;
            break;
    
        case 3 :
            a=4;
            break;
    
        case 4 :
            a=0;
    
    
            passe+=nom;
            break;
    
        case 5 :
            a=5;
            break;
    
        case 6 :
            a=6;
            break;
    
        case 7 :
            passe=passe+nom;
            a=0;
            break;
    
        case 8 :
            a=7;
            break;
    
        case 9 :
            a=0;
            passe=passe+nom;
            break;
    
        case 10 :
            a=8;
            break;
    
        case 11 :
            a=9;
            break;
    
        case 12 :
            a=0;
            passe=passe+nom;
            break;
    
        case 13 :
            a=10;
            break;
    
        case 14 :
            a=11;
            break;
    
        case 15 :
            passe=passe+nom;
            a=0;
            break;
    
        case 16 :
            a=0;
            passe=passe+nom;
            break;
    
        case 17 :
            a=12;
            break;
    
        case 18 :
            a=13;
            break;
    
        case 19 :
            a=14;
            break;
    
        case 20 ... 39:
            passe=passe+nom;
            a=0;
            break;
    
        case 40:
            a=0;
            break;
    
        case 41 :
            a=15;
            break;
        case 42 :
            a=20;
            break;
    
        case 43 :
            a=24;
            break;
    
        case 44:
            a=16;
            break;
        case 45:
            a=17;
            break;
        case 46:
            a=18;
            break;
        case 47:
            a=19;
            break;
    
        case 48 ... 57 :
            passe=passe+nom;
            a=15;
            break;
    
        case 58:
            a=21;
            break;
        case 59:
            a=22;
            break;
        case 60:
            a=23;
            break;
    
        case 61:
            a=-1;
            passe=passe+'.';
            break;
    
        case 62:
            a=-1;
            passe = passe + ':';
            break;
    
        case 63:
            a=-1;
            passe = passe + ',';
            break;
    
        case 64:
            a=-1;
            passe = passe + '!';
            break;
    
    
        case 65:
            a=-1;
            passe = passe + '?';
            break;
    
        case 66 ... 69:
            a=-1;
            passe=passe+nom;
            break;
    
        case 70:
            a=-1;
            passe=passe+nom;
            break;
    
        case 71:
            a=-1;
            passe=passe+'"';
            break;
    
        case 72:
            a=-1;
            passe=passe+nom;
            break;
    
    
    
    
    
        case 73:
            a=-1;
            passe=passe+'/';
            break;
    
        case 74:
            switch (global){
                case 1 ... 5 :
                    afficher=global-1;
                    break;
            }
            break;
        case 75:
            a=-1;
    		parametreCalibrer = true;
            break;
        case 76 :
            a=29;
            break;
        case 77 :
            a=30;
            break;
        case 78:
            a=26;
            break;
        case 79 :
            break;
        case 80:
            a=27;
            break;
        case 81:
            a=28;
            break;
        case 82:
        {
            if(passe.length() >= 1){
                passe.erase(passe.length()-1);
            }
    
            son.loadSound("son.mp3");
            son.play();
            /**
    
            LPBABTTS myVoice = BabTTS_Create();
            BabTtsError error;
    
            error = BabTTS_Open(myVoice, "Julie22k_HQ", BABTTS_USEDEFDICT);
    
            if (error != E_BABTTS_NOERROR) {
                cout << "Error while opening voice : " << error << endl;
                BabTTS_Uninit();
                BabTtsUninitDll();
                system("pause");
                return;
            }
            // Il faudrait ne parler que le dernier mot !!!
            BabTTS_Speak(myVoice, (char*)passe.c_str(), BABTTS_ASYNC);
    
            /**/
            break;
    
        }
    
        case 83:
            a = -1;
            break;
        case 84:
            passe += ' ';
            son.loadSound("son.mp3");
            son.play();
            break;
        case 85:
            global++;
    
            if (global>switcher)
                global=0;
            break;
    
        case 86:
    
            a=25;
    
            break;
    
        case 87 :
            lecture.open("Mon_texte.txt");
            while(lecture.good()){
                getline(lecture,recup);
                tempString += recup + "\n";
            }
            lecture.close();
            passe = recup;
            a = -1;
    
            break;
    
    
        case 88 :
    
            // This part will load the text already saved
            lecture.open("Mon_texte.txt");
            while(lecture.good()){
                getline(lecture,recup);
                tempString += recup + "\n";
            }
            lecture.close();
            cout << "Ancien texte = \n" << tempString << endl;
    
            // This part will add new lines in the pre existing text
            sauvegarde.open("Mon_texte.txt");
            sauvegarde << tempString + passe;
            sauvegarde.close();
            cout << "Nouveau texte = \n" << tempString + passe << endl;
    
            passe = "";
    
            a = -1;
            break;
    
        case 89 :
    /**/
            global++;
    
            if (global>switcher)
                global=0;
            break;
    ///////////////////////////ACAPELA\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
    /**
            {
    
                LPBABTTS myVoice = BabTTS_Create();
                BabTtsError error;
    
                error = BabTTS_Open(myVoice, "Julie22k_HQ", BABTTS_USEDEFDICT);
    
                if (error != E_BABTTS_NOERROR) {
                    cout << "Error while opening voice : " << error << endl;
                    BabTTS_Uninit();
                    BabTtsUninitDll();
                    system("pause");
                    return;
                }
    
                BabTTS_Speak(myVoice, (char*)passe.c_str(), BABTTS_ASYNC);
    
                break;
            }
            break;
    /**/
        default:
    
            break;
        }
    }

}

else{
    this->SetNom(name.erase(name.size()-4));

}

}

void Touche::SetNom(string name){
    nom=name;
}

bool Touche::inside (int x1, int y1, int screenx, int screeny)
{

    float x3 = (x+largeur)*screenx;
    float y3= (y+hauteur)*screeny;
    
    if (x1<x3 && x1>(x*screenx) && y1>(y*screeny) && y1<y3)         //Conditions to see if it is within the key , according to the size of the window(screenx, screeny).
    {
        dedans=true;
        return true;
    }

else
{
      dedans=false;
      return false;
}

}

void Touche::SetSize (float height,float width){
      largeur = width;
      hauteur = height;
}
void Touche::SetPosition (float posx, float posy){
      x=posx;
      y=posy;
}

void Touche::Inactivate(){
        if (dedans==false)      //Reset the counter of all the buttons on this page
        compteur=0;
}


