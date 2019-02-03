#include "tgaimage.h"
#include "utilitaire.h"
#include <vector>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <utility>

using namespace std;

struct Point {
  int x;
  int y;
  int z;
  float Rx;
  float Ry;
  float Rz;
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
const int profondeur = 1000;
std::vector<Point> positions;
std::vector<Triangle> triangles;
float zBuffer[largeur][hauteur];



//dessin ligne simple
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

//dessin ligne avec utilisation du zbuffer
void lineZBuffer(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color, int z) {

	bool steep = false;
	if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	}
	if (x0 > x1) {
		std::swap(x0, x1);
		std::swap(y0, y1);
	}
	int dx = x1 - x0;
	int dy = y1 - y0;
	int derror2 = std::abs(dy) * 2;
	int error2 = 0;
	int y = y0;
	for (int x = x0; x <= x1; x++) {
		if (steep) {
			if (zBuffer[y][x] < z) {
				zBuffer[y][x] = z;
				image.set(y, x, color);
			}
		}
		else {
			if (zBuffer[x][y] < z) {
				zBuffer[x][y] = z;
				image.set(x, y, color);
			}
		}
		error2 += derror2;
		if (error2 > dx) {
			y += (y1 > y0 ? 1 : -1);
			error2 -= dx * 2;
		}
	}
}

//explosion d'une chaine de caractère
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

//lecture du fichier et recupératio ndes informations
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
    float a,b,c;
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
            std::istringstream streamC(ex[3]);
            streamC >> c;

            p = Point();

            p.Rx = a;
            p.Ry = b;
            p.Rz = c;

            a = a*(largeur/2)+(largeur/2);
            b = b*(hauteur/2)+(hauteur/2);
            c = c*(profondeur/2)+(profondeur/2);

            p.x = a;
            p.y = b;
            p.z = c;
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

//colorisation d'un triangle
void colorTriangle(TGAImage &image, TGAColor tga,int i){

    Triangle t;
    t = triangles[i];
    float pente1, pente2, x1, x2;
    int tmp;
    //on range les sommets dans l'ordre : axe y
    if(t.a.y < t.b.y) std::swap(t.a,t.b);
    if(t.a.y < t.c.y) std::swap(t.a,t.c);
    if(t.b.y < t.c.y) std::swap(t.b,t.c);

    //calcule de la pente gauche et drotie du triangle
    tmp = (t.b.y - t.a.y);
    if(tmp != 0)
        pente1 = (t.b.x - t.a.x) / (float)tmp;
    else pente1 = 0;

    tmp = (t.c.y - t.a.y);
    if(tmp != 0)
        pente2 = (t.c.x - t.a.x) / (float)tmp;
    else pente2 = 0;

    //position de depart x : point le plus haut du triangle
    x1 = t.a.x;
    x2 = x1;

    //on prend le point avec le y le plus haut comme depart et on va jusqu'au y du point en dessous
    for (int h = t.a.y; h >= t.b.y; h--) {
        lineZBuffer((int)x1,h,(int)x2,h,image,tga,t.a.z);
        x1 -= pente1;
        x2 -= pente2;
    }



    //on recalcul la pente 1
    tmp = (t.c.y - t.b.y);
    if (tmp != 0)
        pente1 = (t.c.x - t.b.x) / (float)tmp;
    else pente1 = 0;
    x1 = t.c.x;
    x2 = x1;
    //on garde la pente2 : deja calculé correspont au coté le plus long du triangle
    for (int h = t.c.y; h < t.b.y; h++) {
        lineZBuffer((int)x1, h, (int)x2, h, image, tga,t.a.z);
        x1 += pente1;
        x2 += pente2;
    }
}

//colorisation de tout les triangle avec backface culling
void remplissageTriangle(TGAImage &image){
    Triangle t;
    TGAColor tga;
    float tmp;
    float light_dir[3] = {0,0,1};
    for(int i =0;i < triangles.size();i++){
        t = triangles[i];
        //tga = TGAColor(rand() % 256,rand() % 256,rand() % 256,255);

        float vector1[3];
        float vector2[3];
        float normale[3];

        //x
        vector1[0] = (t.b.x - t.a.x);
        vector2[0] = (t.c.x - t.a.x);

        //y
        vector1[1] = (t.b.y - t.a.y);
        vector2[1] = (t.c.y - t.a.y);

        //z
        vector1[2] = (t.b.z - t.a.z);
        vector2[2] = (t.c.z - t.a.z);

        //creation de la normale
        normale[0] = vector1[1] * vector2[2] - vector1[2] * vector2[1];
        normale[1] = vector2[2] * vector1[0] - vector2[0] * vector1[2];
        normale[2] = vector1[0] * vector2[1] - vector1[1] * vector2[0];

        //normalisation
        float norme  = std::sqrt(normale[0]*normale[0]+normale[1]*normale[1]+normale[2]*normale[2]);
        normale[0] = normale[0]/norme;
        normale[1] = normale[1]/norme;
        normale[2] = normale[2]/norme;

        tmp = 0;
        for (int m=0; m<3; m++)
             tmp += light_dir[m]*normale[m];

        if(tmp > 0)
            colorTriangle(image,TGAColor(255*tmp,255*tmp,255*tmp,255),i);
    }


}

//affichage des sommets des triangles
void affichagePoint(TGAImage &image){
    //affichage des points
    for(int i =0;i < positions.size();i++){
        image.set(positions[i].x,positions[i].y,white);
    }
}

//affichages des arrètes des triangles
void affichageLignes(TGAImage &image){
    //dessin des lignes
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
	for (int i = 0; i < largeur;i++) {
		for (int j = 0; j < hauteur;j++) {
			zBuffer[i][j] = std::numeric_limits<int>::min();
		}
	}
    TGAImage image(largeur, hauteur, TGAImage::RGB);

    lectureFichier("../african_head.obj",image);

    remplissageTriangle(image);

    image.flip_vertically();
    image.write_tga_file("output.tga");
    return 0;

}
