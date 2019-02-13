#include "tgaimage.h"
#include "point.h"
#include "cmath"
#include "matrix.h"
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
const int taille = 800;
int const depth = 255;
Point3DF lumiere(0,-1,-1);
Point3DF camera(0,0,3);
Point3DF eye(3,1,3);
Point3DF center(0,0,0);

struct vector2D{
  float x,y;
};
struct vector3D{
  float x,y,z;
};

struct vector2DI{
  int x,y;
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

Point3DF matrixToPoint(Matrix m){
  return Point3DF(m[0][0]/m[3][0],m[1][0]/m[3][0],m[2][0]/m[3][0]);
}

Matrix pointToMatrix(Point3DF p){
  Matrix m(4,1);
  m[0][0] = p.getX();
  m[1][0] = p.getY();
  m[2][0] = p.getZ();
  m[3][0] = 1.f;
  return m;
}

Matrix viewport(int x, int y, int w, int h) {
    Matrix m = Matrix::identity(4);
    m[0][3] = x+w/2.f;
    m[1][3] = y+h/2.f;
    m[2][3] = depth/2.f;

    m[0][0] = w/2.f;
    m[1][1] = h/2.f;
    m[2][2] = depth/2.f;
    return m;
}

Matrix lookat(Point3DF eye, Point3DF center, Point3DF up) {	
    Point3DF z = (eye-center).normalize();
    Point3DF x = (up^z).normalize();
    Point3DF y = (z^x).normalize();
    Matrix res = Matrix::identity(4);
    res[0][0] = x.getX();
    res[0][1] = x.getY();
    res[0][2] = x.getZ();
    res[1][0] = y.getX();
    res[1][1] = y.getY();
    res[1][2] = y.getZ();
    res[2][0] = z.getX();
    res[2][1] = z.getY();
    res[2][2] = z.getZ();
    res[0][3] = -center.getX();
    res[1][3] = -center.getY();
    res[2][3] = -center.getZ();

    return res;
    
}
vector<std::string> split(const std::string &chaine, char delimiteur)
{
  vector<string> elements;
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
Point3DF barycentre(Point3DF p1,Point3DF p2,Point3DF p3,Point3DF p){
  Point3DF t1(p3.getX()-p1.getX(),p2.getX()-p1.getX(),p1.getX()-p.getX());
  Point3DF t2(p3.getY()-p1.getY(),p2.getY()-p1.getY(),p1.getY()-p.getY());

  Point3DF bary = t1^(t2);
  float comp = std::abs(bary.getZ());
  if(comp < 1){

     return Point3DF(-1,1,1);
  }
  return Point3DF(1.f-(bary.getX()+bary.getY())/bary.getZ(),
                  bary.getY()/bary.getZ(),
		  bary.getX()/bary.getZ());
}
void bary(Point3DF p1, Point3DF p2, Point3DF p3,float *z_buffer,Point3DF pt1,Point3DF pt2,Point3DF pt3,float* intensite, TGAImage &image, TGAImage &texture){
  int maxX = max(p1.getX(),max(p2.getX(),p3.getX()));
  int minX = min(p1.getX(),min(p2.getX(),p3.getX()));
  int maxY = max(p1.getY(),max(p2.getY(),p3.getY()));
  int minY = min(p1.getY(),min(p2.getY(),p3.getY()));

  Point3DF t;
  float z=0;
  float textureX, textureY,intens;
  for(float x = minX ; x <= maxX ; x++){
    for(float y = minY; y<= maxY ;y++){
      t = Point3DF(x,y,z);
      Point3DF bar = barycentre(p1,p2,p3,t);
      if(bar.getX()<0 || bar.getY() <0 || bar.getZ()<0) continue;
      z = 0;
      z = (p1.getZ()*bar.getX())+(p2.getZ()*bar.getY())+(p3.getZ()*bar.getZ());
      if(z_buffer[int(x+y*taille)] < z ){
        z_buffer[int(x+y*taille)] = z ;

        textureX = pt1.getX()*bar.getX() + pt2.getX()*bar.getY() + pt3.getX()*bar.getZ();
        textureY = pt1.getY()*bar.getX() + pt2.getY()*bar.getY() + pt3.getY()*bar.getZ();
	intens = intensite[0]*bar.getX()+intensite[1]*bar.getY()+intensite[2]*bar.getZ();
        TGAColor color = texture.get(textureX * texture.get_width() , textureY * texture.get_height());
	
        image.set(x,y,color*(-intens));


      }
    }
}
}


void dessin(int points1, int points2, int points3,vector< vector< float> >points,vector< Point3DF> textureVt,Point3DI texturef,vector< Point3DF> intensiteVn,Point3DI intensitef,float * z_buffer,TGAImage &image,TGAImage &texture){
  
  int x0,x1,x2,y1,y2,y0,z0,z1,z2;
  Matrix Projection = Matrix::identity(4);
  Matrix ViewPort   = viewport(taille/8, taille/8, taille*3/4, taille*3/4);
  Projection[3][2] = -1.f/camera.getZ();
  Point3DF screen_coords[3];
  x0=(points[points1-1][0]+1)*taille/2;
  y0=(points[points1-1][1]+1)*taille/2;
  z0=(points[points1-1][2]+1)*taille/2;
  x1=(points[points2-1][0]+1)*taille/2;
  y1=(points[points2-1][1]+1)*taille/2;
  z1=(points[points2-1][2]+1)*taille/2;
  x2=(points[points3-1][0]+1)*taille/2;
  y2=(points[points3-1][1]+1)*taille/2;
  z2=(points[points3-1][2]+1)*taille/2;
  Point3DF p1 = Point3D<float>(x0,y0,z0);
  Point3DF p2 = Point3D<float>(x1,y1,z1);
  Point3DF p3 = Point3D<float>(x2,y2,z2);
  Point3DF world_coords[3],n;
  world_coords[0] = Point3DF(points[points1-1][0],points[points1-1][1],points[points1-1][2]);
  world_coords[1] = Point3DF(points[points2-1][0],points[points2-1][1],points[points2-1][2]);
  world_coords[2] = Point3DF(points[points3-1][0],points[points3-1][1],points[points3-1][2]);
  
  
  for (int j=0; j<3; j++) {
      screen_coords[j] = matrixToPoint(ViewPort*Projection*lookat(eye,center,Point3DF(0,1,0))*pointToMatrix(world_coords[j]));
      
  }
   n = (world_coords[2]-world_coords[0])^(world_coords[1]-world_coords[0]);
  n.normalize();
  float intensite [3];
  intensite[0] = lumiere * intensiteVn[intensitef.getX()-1];
  intensite[1] = lumiere * intensiteVn[intensitef.getY()-1];
  intensite[2] = lumiere * intensiteVn[intensitef.getZ()-1];
  
  Point3DF pt1 = (textureVt[texturef.getX()-1]);
  Point3DF pt2 = (textureVt[texturef.getY()-1]);
  Point3DF pt3 = (textureVt[texturef.getZ()-1]);


  bary(screen_coords[0],screen_coords[1],screen_coords[2],z_buffer,pt1,pt2,pt3,intensite,image,texture);
}


int main(int argc, char** argv) {
        TGAImage image(taille,taille , TGAImage::RGB);
	string name = "african_head.obj";
	//string name = "diablo3_pose.obj";

	TGAImage texture;
	texture.read_tga_file("african_head_diffuse.tga");
	//texture.read_tga_file("diablo3_pose_diffuse.tga");

	texture.flip_vertically();
	float * z_buffer = new float[taille*taille];

	std::ifstream fichier(name.c_str());


	int x,y,z;
	float xt,yt,zt;
	vector< vector< float> > points;
	vector<  Point3DF> textureVt;
	vector<  Point3DF> intensiteVn;
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
	    while( ligne != "vt"){
		fichier >> ligne;
	    }
	    while( ligne == "vt"){

	    fichier >> ligne;
	    xt= atof(ligne.c_str());
	    fichier >> ligne;
	    yt=atof(ligne.c_str());
	    fichier >> ligne;
	    zt= atof(ligne.c_str());
	    fichier >> ligne;
	    Point3DF v(xt,yt,zt);
	    textureVt.push_back(v);
	  }
	   while( ligne != "vn"){
		fichier >> ligne;
	    }
	    while( ligne == "vn"){

	    fichier >> ligne;
	    xt= atof(ligne.c_str());
	    fichier >> ligne;
	    yt=atof(ligne.c_str());
	    fichier >> ligne;
	    zt= atof(ligne.c_str());
	    fichier >> ligne;
	    Point3DF v(xt,yt,zt);
	    intensiteVn.push_back(v);
	  }
	    while( ligne != "f"){
		fichier >> ligne;
	    }
	    while( ligne == "f"){
	      int points1,xvn,yvn,zvn;
	      int points2;
	      int points3;
	      vector<string> v;
	      fichier >> ligne;
	      v=split(ligne,'/');
	      points1= atoi(v[0].c_str());
	      x=atoi(v[1].c_str());
	      xvn=atoi(v[2].c_str());
	      fichier >> ligne;
	      v=split(ligne,'/');
	      points2= atoi(v[0].c_str());
	      y=atoi(v[1].c_str());
	      yvn=atoi(v[2].c_str());
	      fichier >> ligne;
	      v=split(ligne,'/');
	      points3= atoi(v[0].c_str());
	      z=atoi(v[1].c_str());
	      zvn=atoi(v[2].c_str());

	      Point3DI texturef(x,y,z);
	      Point3DI intensitef(xvn,yvn,zvn);
	      dessin(points1,points2,points3,points,textureVt,texturef,intensiteVn,intensitef,z_buffer,image,texture);
	      fichier >> ligne;
	    }

	    fichier.close();


	} else
	  std::cerr << "Impossible d'ouvrir le fichier !" << std::endl;




	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
        image.write_tga_file("output8.tga");
        return 0;
}
