#include "tgaimage.h"
#include <QCoreApplication>
#include "utilitaire.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <utility>

using namespace std;

struct Point {
  int x;
  int y;
};
struct Triangle {
  Point a;
  Point b;
  Point c;
} ;

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const int largeur = 1000;
const int hauteur = 1000;
std::vector<Point> positions;
std::vector<Triangle> triangles;




void drawLine(int x0, int y0,int x1, int y1, TGAImage &image, TGAColor color){

    bool steep = false;
       if (std::abs(x0-x1)<std::abs(y0-y1)) {
           std::swap(x0, y0);
           std::swap(x1, y1);
           steep = true;
       }
       if (x0>x1) {
           std::swap(x0, x1);
           std::swap(y0, y1);
       }
       int dx = x1-x0;
       int dy = y1-y0;
       int derror2 = std::abs(dy)*2;
       int error2 = 0;
       int y = y0;
       for (int x=x0; x<=x1; x++) {
           if (steep) {
               image.set(y, x, color);
           } else {
               image.set(x, y, color);
           }
           error2 += derror2;
           if (error2 > dx) {
               y += (y1>y0?1:-1);
               error2 -= dx*2;
           }
       }
}

vector<Point> getLine(int x0, int y0,int x1, int y1){

    vector<Point> res;
    Point p;
    bool steep = false;
       if (std::abs(x0-x1)<std::abs(y0-y1)) {
           std::swap(x0, y0);
           std::swap(x1, y1);
           steep = true;
       }
       if (x0>x1) {
           std::swap(x0, x1);
           std::swap(y0, y1);
       }
       int dx = x1-x0;
       int dy = y1-y0;
       int derror2 = std::abs(dy)*2;
       int error2 = 0;
       int y = y0;
       for (int x=x0; x<=x1; x++) {
           if (steep) {
               p.x = y;
               p.y = x;
           } else {
               p.x = x;
               p.y = y;
           }
           res.push_back(p);
           error2 += derror2;
           if (error2 > dx) {
               y += (y1>y0?1:-1);
               error2 -= dx*2;
           }
       }

       return res;
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

    //declarations
    std::string tmp;
    std::vector<std::string> ex;
    float a,b;
    int t1,t2,t3;
    Point p;
    Triangle t;

    //pour chaque ligne du fichier
    while (!file.eof()) {
        std::getline(file,tmp);

        //recuperation des points
        if(tmp[0] == 'v' && tmp[1] == ' '){
            ex = explode(tmp,' ');

            //recuperation des valeurs
            std::istringstream streamA(ex[1]);
            streamA >> a;
            std::istringstream streamB(ex[2]);
            streamB >> b;

            a = a*(largeur/2)+(largeur/2);
            b = b*(hauteur/2)+(hauteur/2);

            p = Point();
            p.x = a;
            p.y = b;
            positions.push_back(p);
        }
        //recuperations des triangles a tracer
        if(tmp[0] == 'f' && tmp[1] == ' '){
            ex = explode(tmp,' ');
            std::istringstream streamT1(explode(ex[1],'/')[0]);
            streamT1 >> t1;
            std::istringstream streamT2(explode(ex[2],'/')[0]);
            streamT2 >> t2;
            std::istringstream streamT3(explode(ex[3],'/')[0]);
            streamT3 >> t3;

            t = Triangle();
            t.a = positions[t1-1];
            t.b = positions[t2-1];
            t.c = positions[t3-1];
            triangles.push_back(t);
        }
    }
    file.close();
}

void remplissageTriangle(TGAImage &image){
    Triangle t;
    TGAColor tga;
    vector<Point> p,p2,p3;
    int base,hauteur;
    for(int i =0;i < triangles.size();i++){
        t = triangles[i];
        tga = TGAColor(rand() % 256,rand() % 256,rand() % 256,255);
        if(t.a.y < t.b.y) std::swap(t.a,t.b);
        if(t.a.y < t.c.y) std::swap(t.a,t.c);
        if(t.b.y < t.c.y) std::swap(t.b,t.c);

        p = getLine(t.a.x, t.a.y,t.c.x, t.c.y);
        p2 = getLine(t.a.x, t.a.y,t.b.x, t.b.y);
        p3 = getLine(t.b.x, t.b.y,t.c.x, t.c.y);
        drawLine(t.a.x,t.a.y,t.c.x,t.c.y,image,tga);
        drawLine(t.a.x,t.a.y,t.b.x,t.b.y,image,tga);
        drawLine(t.b.x,t.b.y,t.c.x,t.c.y,image,tga);

        int o = 0;
        int m = 0;
        int temporaire = 0;
        while(m < p2.size()){
            if(p[o].y == p2[m].y){
                drawLine(p[o].x,p[o].y,p2[m].x,p2[m].y,image,tga);
                temporaire = o;
                /*while(temporaire == o)*/
                    o++;
            }
            m++;

        }
        m = 0;
        while(m < p3.size()){
            if(p[o].y == p3[m].y){
                drawLine(p[o].x,p[o].y,p3[m].x,p3[m].y,image,tga);
                /*while(temporaire == o)
                    o++;*/
                o++;
            }
            m++;
        }

    }


}

void affichagePoint(TGAImage &image){
    //affichage des points
    for(int i =0;i < positions.size();i++){
        image.set(positions[i].x,positions[i].y,white);
    }
}

void affichageLignes(TGAImage &image){
    //dessin des lignes
    int d,b,c;
    Triangle t;
    for(int i = 0;i < triangles.size();i++){
        t = triangles[i];
        drawLine(t.a.x,t.a.y,t.b.x,t.b.y,image,white);
        drawLine(t.a.x,t.a.y,t.c.x,t.c.y,image,white);
        drawLine(t.b.x,t.b.y,t.c.x,t.c.y,image,white);
    }
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    TGAImage image(largeur, hauteur, TGAImage::RGB);

    lectureFichier("../african_head.obj",image);

    remplissageTriangle(image);

    image.flip_vertically();
    image.write_tga_file("output.tga");
    return 0;

}
