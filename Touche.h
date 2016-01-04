#include <string>
#include "ofApp.h"


#ifndef TOUCHE_H_INCLUDED
#define TOUCHE_H_INCLUDED



class Touche {

private :

    float x;            //Position x
    float y;            //Position y
    string nom;         //Name of the image relative to the button (un .png)
    float hauteur;      //height of the button
    float largeur;      //Width 
    int compteur;       //Counter to set timer
    bool dedans;        //Boolean to know if the mouse is on the button or not

public :

    //Declaration of a sound object
    ofSoundPlayer son;

    ofstream sauvegarde;
    string tempString,recup;
    ifstream lecture;

    Touche(float x, float y, string nom, float hauteur, float largeur);

    string GetNom();
    float GetHauteur();
    float GetLargeur();
    void dessiner (int screenx, int screeny);
    void SetSize (float height,float width); //Useful for  the "update"
    void SetPosition (float posx, float posy);
    void SetNom (string name);
    bool inside (int x1, int y1, int screenx, int screeny);
    void ChangeImage (int x1, int y1, string name, int &a, int z, int screenx, int screeny, string &passe, int lim, bool &parametreCalibrer);
    void Inactivate();

};






#endif // TOUCHE_H_INCLUDED
