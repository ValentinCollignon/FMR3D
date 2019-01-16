#include "tgaimage.h"
#include "cmath"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
using namespace std;
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
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

void triangle(int x0, int y0, int x1, int y1, int x2, int y2, TGAImage &image, TGAColor color) {


    if (y0>y1) {
            std::swap(x0, x1);
            std::swap(y0, y1);
    }
    if (y0>y2) {
            std::swap(x0, x2);
            std::swap(y0, y2);
    }
    if (y1>y2) {
            std::swap(x1, x2);
            std::swap(y1, y2);
    }
    int total_height = y2-y0;
    for (int y=y0; y<=y1; y++) {
        int segment_height = y1-y0+1;
        float alpha = (float)(y-y0)/total_height;
        float beta  = (float)(y-y0)/segment_height; // be careful with divisions by zero
        int xA = x0 + (x2-x0)*alpha;
        int xB = x0 + (x1-x0)*beta;
        int yA = y0 + (y2-y0)*alpha;
        int yB = y0 + (y1-y0)*beta;
        if (xA>xB) {
                std::swap(xA,xB);
                 std::swap(yA,yB);
        }
        for (int j=xA; j<=xB; j++) {
            image.set(j, y, color); // attention, due to int casts y0+i != A.y
        }
    }
    for (int y=y1; y<=y2; y++) {
        int segment_height =  y2-y1+1;
        float alpha = (float)(y-y0)/total_height;
        float beta  = (float)(y-y1)/segment_height; // be careful with divisions by zero
        int xA = x0 + (x2-x0)*alpha;
        int xB = x1 + (x2-x1)*beta;
        int yA = y0 + (y2-y0)*alpha;
        int yB = y1 + (y2-y1)*beta;
       if (xA>xB) {
                std::swap(xA,xB);
                 std::swap(yA,yB);
        }
        for (int j=xA; j<=xB; j++) {
            image.set(j, y, color); // attention, due to int casts y0+i != A.y
        }
    }
}
std::vector<std::string> split(const std::string &chaine, char delimiteur)
{
  std::vector<std::string> elements;
  std::stringstream ss(chaine);
  std::string sousChaine;
  while (getline(ss, sousChaine, delimiteur))
  {
  elements.push_back(sousChaine);
  }
  return elements;
}

void dessin(int points1, int points2, int points3,vector< vector< float> >points,TGAImage &image){
  int x0,x1,x2,y1,y2,y0;
  x0=(points[points1-1][0]+1)*400;
  y0=(points[points1-1][1]+1)*400;
  x1=(points[points2-1][0]+1)*400;
  y1=(points[points2-1][1]+1)*400;
  x2=(points[points3-1][0]+1)*400;
  y2=(points[points3-1][1]+1)*400;
  line(x0,y0,x1,y1,image,white);
  line(x1,y1,x2,y2,image,white);
  line(x0,y0,x2,y2,image,white);
  triangle( x0,y0,x1,y1,x2,y2,image,white);
}

int main(int argc, char** argv) {
        TGAImage image(800, 800, TGAImage::RGB);
	string name = "african_head.obj";

	std::ifstream fichier(name.c_str());


	vector< vector< float> > points;
   /* triangle(10,70,50,160,70,80,image,red);
    triangle(180,50,150,1,70,180,image,white);
    triangle(180,150,120,160,130,180,image,red);*/
	if(fichier) {
	  std::string ligne;
	  fichier >> ligne;
	  while( ligne == "v"){
	    vector<float> v;
	    fichier >> ligne;
	    v.push_back(atof(ligne.c_str()));
	    fichier >> ligne;
	    v.push_back(atof(ligne.c_str()));
	    fichier >> ligne;
	    fichier >> ligne;
	    points.push_back(v);
	  }
	    int x;
	    int y;
	    int s =points.size();
	    for(int i = 0 ; i<s;i++){
	      x=(points[i][0]+1)*400;
	      y=(points[i][1]+1)*400;
	      image.set(x, y, white);
	    }

	    while( ligne != "f"){
		fichier >> ligne;
	    }
	    while( ligne == "f"){
	      int points1;
	      int points2;
	      int points3;
	      vector<string> v;
	      fichier >> ligne;
	      v=split(ligne,'/');
	      points1= atoi(v[0].c_str());
	      fichier >> ligne;
	      v=split(ligne,'/');
	      points2= atoi(v[0].c_str());
	      fichier >> ligne;
	      v=split(ligne,'/');
	      points3= atoi(v[0].c_str());
	      dessin(points1,points2,points3,points,image);
	      fichier >> ligne;
	    }

	    fichier.close();


	} else
	  std::cerr << "Impossible d'ouvrir le fichier !" << std::endl;




	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
        image.write_tga_file("output3.tga");
        return 0;
}
