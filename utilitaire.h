#ifndef UTILITAIRE_H
#define UTILITAIRE_H

#include "tgaimage.h"

class utilitaire
{
public:
    utilitaire();
    void drawLine(int x0, int y0,int x1, int y1, TGAImage image, TGAColor color);
};

#endif // UTILITAIRE_H
