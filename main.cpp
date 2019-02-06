#include "tgaimage.h"
#include "point.h"
#include "cmath"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <stdlib.h>
using namespace std;
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);



struct vector2D{
  float x,y;
};
struct vector3D{
  float x,y,z;
};

struct vector2DI{
  int x,y;
};
struct vector3DI{
  int x,y,z;
};
struct triangle{
  Point3DI p1,p2,p3;
};

//const vector3D lumiere = (0,0,-1);

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
float crossProduct(vector2D v1,vector2D v2){
    return (v1.x*v2.y-v1.y*v2.x );
}

void bary(Point3DI p1, Point3DI p2, Point3DI p3, TGAImage &image,TGAColor color){
  int maxX = max(p1.getX(),max(p2.getX(),p3.getX()));
  int minX = min(p1.getX(),min(p2.getX(),p3.getX()));
  int maxY = max(p1.getY(),max(p2.getY(),p3.getY()));
  int minY = min(p1.getY(),min(p2.getY(),p3.getY()));
  
  vector2D vs1{p2.getX()-(float)p1.getX(),p2.getY()-(float)p1.getY()};
  vector2D vs2{p3.getX()-(float)p1.getX(),p3.getY()-(float)p1.getY()};
  
  for(int x = minX;x<=maxX;x++){
    for(int y = minY;y<=maxY;y++){
      vector2D q{x - (float)p1.getX(), y - (float)p1.getY()};
    
      float s = crossProduct(q, vs2) / crossProduct(vs1, vs2);
      float t = crossProduct(vs1, q) / crossProduct(vs1, vs2);
      
      if ( (s >= 0) && (t >= 0) && (s + t <= 1))
      { /* inside triangle */
	image.set(x, y,color);
      }
    }
  }
}
void dessin(int points1, int points2, int points3,vector< vector< float> >points,TGAImage &image){
  Point3DF lumiere(0,0,-1);
  int x0,x1,x2,y1,y2,y0,z0,z1,z2;
  x0=(points[points1-1][0]+1)*400;
  y0=(points[points1-1][1]+1)*400;
  z0=(points[points1-1][2]+1)*400;
  x1=(points[points2-1][0]+1)*400;
  y1=(points[points2-1][1]+1)*400;
  z1=(points[points2-1][2]+1)*400;
  x2=(points[points3-1][0]+1)*400;
  y2=(points[points3-1][1]+1)*400;
  z2=(points[points3-1][2]+1)*400;
  Point3DI p1 = Point3D<int>(x0,y0,z0);
  Point3DI p2 = Point3D<int>(x1,y1,z1);
  Point3DI p3 = Point3D<int>(x2,y2,z2);
  Point3DF world_coords[3],n;
  world_coords[0] = Point3DF(points[points1-1][0]+1,points[points1-1][1]+1,points[points1-1][2]+1);
  world_coords[1] = Point3DF(points[points2-1][0]+1,points[points2-1][1]+1,points[points2-1][2]+1);
  world_coords[2] = Point3DF(points[points3-1][0]+1,points[points3-1][1]+1,points[points3-1][2]+1);
   n = (world_coords[2]-world_coords[0])^(world_coords[1]-world_coords[0]);
  n.normalize();
  
  float intensite = n*lumiere;
  if(intensite>0){
    bary(p1, p2, p3,image,TGAColor(255*intensite,255*intensite,255*intensite,255));
  }
}


int main(int argc, char** argv) {
        TGAImage image(800, 800, TGAImage::RGB);
	string name = "african_head.obj";

	std::ifstream fichier(name.c_str());


	vector< vector< float> > points;
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
	    v.push_back(atof(ligne.c_str()));
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
        image.write_tga_file("output4.tga");
        return 0;
}
