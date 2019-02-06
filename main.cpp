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
};
struct Triangle {
  Point a;
  Point b;
  Point c;

  Point texA;
  Point texB;
  Point texC;

} ;

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const int largeur = 1024;
const int hauteur = 1024;
const int profondeur = 1024;
std::vector<Point> positions;
std::vector<Point> posTex;
std::vector<Triangle> triangles;
float zBuffer[largeur][hauteur];

TGAImage texture;
float ratioLargeur;
float ratioHauteur;



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
void lineZBuffer(int x0, int y0, int x1, int y1, TGAImage &image, int z) {

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
                image.set(y, x, red);
			}
		}
		else {
			if (zBuffer[x][y] < z) {
				zBuffer[x][y] = z;
                image.set(x, y, red);
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

	std::vector<std::string> exp;

    file.open(name.c_str());
    if (!file){
        cout << "Pas de fichier\n";
        exit(1);
    }

    //declarations
    std::string tmp;
    std::vector<std::string> ex;
    float a,b,c;
    int t1,t2,t3,tex1,tex2,tex3;
    Point p;
    Triangle t;

    //pour chaque ligne du fichier
    while (!file.eof()) {
        std::getline(file,tmp);

        //recuperation des points
        if(tmp[0] == 'v' && (tmp[1] == ' ' || tmp[1] == 't')){
            ex = explode(tmp,' ');
            std::istringstream stream;
            if(tmp[1] == ' ')
                //recuperation des valeurs
                stream.str(ex[1]);
            else stream.str(ex[2]);
            stream >> a;
            stream.str(ex[2]);
            stream.seekg(0);
            stream >> b;
            stream.str(ex[3]);
			stream.seekg(0);
            stream >> c;

            p = Point();

            a = a*(largeur/2)+(largeur/2);
            b = b*(hauteur/2)+(hauteur/2);
            c = c*(profondeur/2)+(profondeur/2);

            p.x = a;
            p.y = b;
            p.z = c;

            if(tmp[1] == 't')
				posTex.push_back(p);
			else positions.push_back(p);

        }
        //recuperations des triangles a tracer
        if(tmp[0] == 'f' && tmp[1] == ' '){
            ex = explode(tmp,' ');
			exp = explode(ex[1], '/');

			//point a
            std::istringstream stream(exp[0]);
            stream >> t1;
			stream.str(exp[1]);
			stream.seekg(0);
			stream >> tex1;

			//point b
			exp = explode(ex[2], '/');
			stream.str(exp[0]);
			stream.seekg(0);
            stream >> t2;
			stream.str(exp[1]);
			stream.seekg(0);
			stream >> tex2;

			//point c
			exp = explode(ex[3], '/');
            stream.str(exp[0]);
			stream.seekg(0);
            stream >> t3;
			stream.str(exp[1]);
			stream.seekg(0);
			stream >> tex3;

			//creation du triangle
            t = Triangle();
            t.a = positions[t1-1];
            t.b = positions[t2-1];
            t.c = positions[t3-1];

			t.texA = posTex[tex1 - 1];
			t.texB = posTex[tex2 - 1];
			t.texC = posTex[tex3 - 1];

            triangles.push_back(t);
        }
    }
    file.close();
}

Point soustractionPoint(Point a, Point b){
    Point res;
    res.x = b.x - a.x;
    res.y = b.y - a.y;
    res.z = b.z - a.z;
    return res;
}

//colorisation d'un triangle
void colorTriangle(TGAImage &image, int i,float intensity){

    //Cramer's rule
    Triangle t = triangles[i];
    Point p1,p2,p3,p,tex;
    TGAColor color;

    p.z = t.a.z;

    float div,alpha,beta,gamma;

    int minX = std::min(std::min(t.a.x,t.b.x),t.c.x);
    int maxX = std::max(std::max(t.a.x,t.b.x),t.c.x);

    int minY = std::min(std::min(t.a.y,t.b.y),t.c.y);
    int maxY = std::max(std::max(t.a.y,t.b.y),t.c.y);
    for(int i = minX;i < maxX;i++){
        p.x = i;
        for(int j = minY; j < maxY;j++){
            p.y = j;
            p1 = soustractionPoint(t.b,t.a);
            p2 = soustractionPoint(t.c,t.a);
            p3 = soustractionPoint(p,t.a);

            div = p1.x * p2.y - p2.x * p1.y;

            if(div != 0){
                beta = (p1.x * p3.y - p3.x * p1.y)/div;
                alpha = (p3.x * p2.y - p2.x * p3.y)/div;
            }
            else{
                beta = -1;
                alpha = -1;
            }

            gamma = 1.0f - alpha - beta;
            if(gamma >= -.001 && alpha >= -.001 && beta >= -.001){
                if(p.z > zBuffer[p.x][p.y]){
                    zBuffer[p.x][p.y] = p.z;
                    tex.x = alpha * t.texA.x + beta * t.texB.x + gamma * t.texC.x;
                    tex.y = alpha * t.texA.y + beta * t.texB.y + gamma * t.texC.y;
                    //cout << tex.x << "\n";
                    color = texture.get(tex.x,tex.y);
                    image.set(p.x,p.y,color.operator *(intensity));
                }
            }
        }

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
            colorTriangle(image,i,tmp);
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
            zBuffer[i][j] = -50000;
		}
	}
    TGAImage image(largeur, hauteur, TGAImage::RGB);

	texture.read_tga_file("../african_head_diffuse.tga");
    //texture.flip_vertically();

    lectureFichier("../african_head.obj",image);
	//lecture de la texture

	ratioLargeur = texture.get_width() / (float)largeur;
	ratioHauteur = texture.get_height() / (float)hauteur;
	remplissageTriangle(image);
    //remplissageTriangle(image);

    image.flip_vertically();
    image.write_tga_file("output.tga");

    return 0;

}
