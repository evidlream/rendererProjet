#include "tgaimage.h"
#include <QCoreApplication>
#include "utilitaire.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);

void drawLine(int x0, int y0,int x1, int y1, TGAImage &image, TGAColor color){

    bool vertical = false;


    if(std::abs(x0-x1) < std::abs(y0-y1)){
        std::swap(x0,y0);
        std::swap(x1,y1);
        vertical = true;
    }

    if(x0 > x1){
        std::swap(x1,x0);
        std::swap(y1,y0);
    }

    for(int x = x0; x < x1;x++){
        int t = (x-x0)/(float)(x1-x0);
        int y = y0*(1.-t)+y1*t;

        if(vertical){
            image.set(y,x,color);
        }
        else{
            image.set(x,y,color);
        }
    }

//correctino a faire : y mal affiché

}


std::vector<std::string> explode(std::string const & s, char delimitation)
{
    std::vector<std::string> res; // resultat
    std::istringstream iss(s);

    for (std::string token; std::getline(iss, token, delimitation); )
    {
        res.push_back(std::move(token));
    }

    return res;
}

void lectureFichier(std::string name, TGAImage &image){

    ifstream file;

    file.open(name.c_str());
    if (!file){
        cout << "Pas de fichier\n";
        exit(1);
    }

    string tmp;
    std::vector<std::string> explode;
    while (!file.eof()) {
        std::getline(file,tmp);

        if(tmp[0] == 'v' && tmp[1] == ' '){
            //explode = explode(tmp,' ');
            //image.set(std::stoi(explode.at(1)),std::stoi(explode.at(2)),red);
        }
            //image.set(x,y,red);
    }
    file.close();
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    TGAImage image(100, 100, TGAImage::RGB);

    lectureFichier("../test.obj",image);

    //image.set(52, 41, red);
    drawLine(40,20,100,80,image,red);
    image.flip_vertically();
    image.write_tga_file("output.tga");
    return 0;

}
