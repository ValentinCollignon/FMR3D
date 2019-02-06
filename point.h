#ifndef POINT_H
#define POINT_H

template<typename T > class Point3D{
  public:
    Point3D(T i,T j, T k){x =i;y=j;z=k;}
    Point3D(T i,T j){x =i;y=j;z=0;}
    T getX(){return x;}
    T getY(){return y;}
    T getZ(){return z;}
    float norm(){
            return sqrt(x*x+y*y+z*z);
        }
        Point3D<T> & normalize(T l = 1){
            *this = (*this)*(l/norm());
            return *this;
        }
        void afficher(std::ostream &flux)const{
            flux << x  <<" , "<< y << " , "<< z;
        }
  private :
    T x,y,z;
  
  
};

/*
*Point3D
*
*/

/*redefinition des operande*/
template <typename T>
bool operator==(Point3D<T> const& a, Point3D<T> const& b) {
    return a.getX() == b.getX() && a.getY() == b.getY() && a.getZ()==b.getZ();
}
template <typename T>
bool operator!=(Point3D<T> const& a, Point3D<T> const& b) {return !(a==b);}
template <typename T>
Point3D<T> operator+(Point3D<T> const& a, Point3D<T> const& b) {
    Point3D<T> result(a.getX()+b.getX(),a.getY()+b.getY(),a.getZ()+b.getZ());
    return result;
}
template <typename T>
Point3D<T> operator-(Point3D<T> const& a, Point3D<T> const& b){
    Point3D<T> result(a.getX()-b.getX(),a.getY()-b.getY(),a.getZ()-b.getZ());
    return result;
}
template <typename T>
std::ostream &operator<<(std::ostream &flux, Point3D<T> const& point){
    point.afficher(flux);
    return flux;
}
template <typename T>
Point3D<T> operator*(Point3D<T> p ,float f) {
    Point3D<T> result(p.getX()*f , p.getY()*f , p.getZ()*f);
    return result;
}
template <typename T> T operator*(const Point3D<T> a,const Point3D<T> b){
    return a.getX()*b.getX() + a.getY()*b.getY() + a.getZ()*b.getZ();
}

//template <typename T> operator^(const Point3D<T> a , const Point3D<T> b){
template <typename T> Point3D<T> operator ^(const Point3D<T> a,const Point3D<T> b){
    Point3D<T> pp(a.getY()*b.getZ()-a.getZ()*b.getY() , a.getZ()*b.getX()-a.getX()*b.getZ() , a.getX()*b.getY()-a.getY()*b.getX());
    return pp;
}

typedef Point3D<int> Point3DI;
typedef Point3D<float> Point3DF;

#endif