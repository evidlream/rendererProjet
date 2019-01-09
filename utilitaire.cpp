#include "utilitaire.h"
#include "tgaimage.h"
#include <cmath>

utilitaire::utilitaire()
{
}

void utilitaire::drawLine(int x0, int y0,int x1, int y1, TGAImage image, TGAColor color){

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



}
