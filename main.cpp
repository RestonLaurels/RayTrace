#include "mainwindow.h"
#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QFile>
#include <QTextStream>

#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <cstring>
# define PI           3.14159265358979323846

int width = 800;
int height = 600;
int roomleangh=700;//y
int roomwidth=800; //x
int roomheight=600;//z
using namespace std;

struct color{
    int R;
    int G;
    int B;
};

struct lightning{
    int x=320;
    int y=250;
    int z=460;
};

struct object{//параметры объекта
    int x;  //
    int y;  //местоположение в мировом пространстве
    int z;  //
    int R;  //радиус шара
    //struct color color;
    int Red;
    int Green;
    int Blue;
};
struct viewer{//параметры наблюдателя
    // напротив какого пикселя находится наблюдатель
    int x=400;
    int z=300;
    int y=-500;//расстояние до изображения
};


float minim(float t1, float t2){
    if(t1>t2) t1 = t2;
    return t1;
}
float maxim(float t1, float t2){
    if(t2>t1) t1 = t2;
    return t1;
}

 float descr (object sphere,float Xo,float Yo,float Zo,float a,float b,float c){
    float D; float X=Xo-sphere.x; float Y=Yo-sphere.y; float Z=Zo-sphere.z;
    D=4*((X*a+Y*b+Z*c)*(X*a+Y*b+Z*c)-(a*a+b*b+c*c)*(X*X+Y*Y+Z*Z-(sphere.R)*(sphere.R)));
    return D;
}
 float intersect (float D,object sphere,float Xo, float Yo, float Zo, float a,float b,float c){//находим t для всех систем уравнений
    float t,t1,t2;
    t1=(pow(D, 0.5 )-(2*(Xo-sphere.x)*a+2*(Yo-sphere.y)*b+2*(Zo-sphere.z)*c))/(2*(a*a+b*b+c*c));
    t2=(-(pow(D, 0.5 ))-(2*(Xo-sphere.x)*a+2*(Yo-sphere.y)*b+2*(Zo-sphere.z)*c))/(2*(a*a+b*b+c*c));
    t=minim(t1,t2);
    return t;
 }


void Power_pix (float ox,float oy,float oz,object *sphere, viewer camera,lightning light,int n,QRgb *rgb){//n возможно здесь больше истины(+1)
    /*здесь имеем координаты начальной точки луча, данные по всем сферам, данные по свету, и размер комнат
    */
    int pulse=0;
    float a,b,c,t,D;
    float tmin;
    int i;
    int numberspher;
    //oy=0;
    a=(ox-(camera.x));//задаем вектор направления
    b=(oy-(camera.y));
    c=(oz-(camera.z));
    //далее найдем, с какой из сфер нашлойсь пересечение и найдём минимальный t, чтобы определить точку пересечение

    for (i=0 ; i<n ; i++){
        if ((D=descr(sphere[i],ox,oy,oz,a,b,c))>=0){ //если имеется пересечение с сферой
            t = intersect (D,sphere[i],ox,oy,oz,a,b,c);//расчитываем t для определения точки пересечения
            if (pulse == 0){//если до этого не был расчитан минимальный t,то минимальный t это расчитанный
                tmin = t;
                pulse =1;
                numberspher=i;
            }
            else {
                if (t<tmin){
                    tmin = t;
                    numberspher=i;
                }
            }
            *rgb=qRgb(sphere[numberspher].Red,sphere[numberspher].Green,sphere[numberspher].Blue);
        }
    }
    return;
}

int main(int argc, char *argv[])
{
    int i=0;
    QApplication a(argc, argv);
    object sphere[10];
    viewer camera;
    lightning light;

    QImage img(width,height,QImage::Format_RGB32);
    QRgb rgb;
    QFile file("D:/cond.txt");

    file.open(QIODevice::ReadOnly);
    QTextStream stream(&file);

    while (!stream.atEnd()) { //радиус координаты, цвет сферы
        QByteArray ba = file.readLine(); // читаем первую строку
        QList<QByteArray> baList = ba.split(' '); // делим строку на кусочки... в качестве разделителя используем пробел

        sphere[i].R=atoi(baList[0]);
        sphere[i].x=atoi(baList[1]);
        sphere[i].y=atoi(baList[2]);
        sphere[i].z=atoi(baList[3]);
        sphere[i].Red=atoi(baList[4]);
        sphere[i].Green=atoi(baList[5]);
        sphere[i].Blue=atoi(baList[6]);
        i++;
    }
    file.close(); // закрываем файл

    for (int z=0; z<height; z++) {
      for (int x=0; x<width; x++) {
        rgb = qRgb(0,0,0);
        Power_pix(x,0,z,sphere,camera,light,i,&rgb);
        img.setPixel(x,z,rgb);
        rgb = qRgb(0,0,0);
      }
    }
    img.save("D:/mynumber.jpg","JPG");
    return a.exec();
}
