#include "mainwindow.h"
#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QFile>
#include <QTextStream>
#include <QtCore/qmath.h>

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
int MAXrecurs=10;

using namespace std;

struct color{
    int R;
    int G;
    int B;
};

struct lightning{
    int x=100;
    int y=100;
    int z=100;
    float Il=1;//интенсивность света
};

struct object{//параметры объекта
    int x;  //
    int y;  //местоположение в мировом пространстве
    int z;  //
    int R;  //радиус шара
    //struct color color;
    float Red; //фоновая интенсивность для красного
    float Green;//фоновая интенсивность для зеленого
    float Blue;//фоновая интенсивность для голубого
    float Kd;//какая реакция у данной поверхности на свет (поглощение, отражение и т.д.)
};
struct viewer{//параметры наблюдателя
    // напротив какого пикселя находится наблюдатель
    int x=400;
    int z=300;
    int y=-300;//расстояние до изображения
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
             // x,y,z- это точка, из которой проводится луч, a,b,c-это векторы направлений    количество сфер изначальная сфера

                                                                  //к-во сфер, номер сферы, из которой происходит отбор
 float peres(float ox,float oy, float oz,float a,float b, float c, object *sphere,int n, int f){
  float tmin,t,D;
  int pulse=0;
  for (int i=0 ; i<n ; i++){
      if ((D=descr(sphere[i],ox,oy,oz,a,b,c))>=0){ //если имеется пересечение с любой сферой
          t = intersect (D,sphere[i],ox,oy,oz,a,b,c);//расчитываем t для определения точки пересечения
          if (t!=0){//если пересечение найдено не в этой же точке
            if (pulse == 0){      //если до этого не был расчитан минимальный t,то минимальный t это расчитанный
              tmin = t;
              pulse =1;
              //сфера с которой всё таки состоялось пересечение
            }
            else {
              if (t<tmin){
                  tmin = t;
              }
            }
          }

      }
  }
  if (pulse==0) return 0;//пересечений не было
  return 1;//пересечение было
}


 // Получаем координаты откуда идет луч, направление луча, массив сфер, источник света, количество сфер, сфера, которую мы исключаем(из нее и идет луч), указатель на цвет

 float intensity (float ox, float oy, float oz,float a, float b,float c,object *sphere,lightning light,int n,int f,QRgb *rgb){//функция для определения интенсивности цветов
     int pulse=0,numberspher;
     int i;
     float Cosa;
     float D,t,tmin;
     float normalx,normaly,normalz;
     float IaR,IaG,IaB;
     float Ir,Ig,Ib;
     float Xinters,Yinters,Zinters;
     float a_to_light,b_to_light,c_to_light,x_to_light,y_to_light,z_to_light;
     float Xotr,Yotr,Zotr;

     for (i=0 ; i<n ; i++){
         if ((D=descr(sphere[i],ox,oy,oz,a,b,c))>=0 && (i!=f)){ //если имеется пересечение с сферой и эта сфера не является сферой из которой и идет пересечение то
             t = intersect (D,sphere[i],ox,oy,oz,a,b,c);//расчитываем t для определения точки пересечения
             if (pulse == 0){      //если до этого не был расчитан минимальный t,то минимальный t это расчитанный
                 tmin = t;
                 pulse =1;
                 numberspher=i;//сфера с которой всё таки состоялось пересечение
             }
             else {
                 if (t<tmin){
                     tmin = t;
                     numberspher=i;
                 }
             }

         }
     }
     if (pulse==0) return 0;//если пересечения ни с 1ой сферой не было возвращаемся


     Xinters=ox+a*tmin; //
     Yinters=oy+b*tmin; //  точка пересечения с объектом
     Zinters=oz+c*tmin; //


     normalx=(Xinters-sphere[numberspher].x);//
     normaly=(Yinters-sphere[numberspher].y);//   векторы нормали с направлением
     normalz=(Zinters-sphere[numberspher].z);//
     //задаем новое направление
     /*
     Xotr=2*normalx*(normalx*(ox-Xinters)+normaly*(oy-Yinters)+normalz*(oz-Zinters))/(normalx*2+normaly*2+normalz*2)-(ox-Xinters);
     Yotr=2*normaly*(normalx*(ox-Xinters)+normaly*(oy-Yinters)+normalz*(oz-Zinters))/(normalx*2+normaly*2+normalz*2)-(oy-Yinters);
     Zotr=2*normalz*(normalx*(ox-Xinters)+normaly*(oy-Yinters)+normalz*(oz-Zinters))/(normalx*2+normaly*2+normalz*2)-(oz-Zinters);
     a=Xotr-Xinters;
     b=Yotr-Yinters;
     c=Zotr-Zinters;
     ox=Xinters;
     oy=Yinters;
     oz=Zinters;
     */
     f=numberspher;
     a_to_light=light.x-Xinters;
     b_to_light=light.y-Yinters;
     c_to_light=light.z-Zinters;
     x_to_light=Xinters;
     y_to_light=Yinters;
     z_to_light=Zinters;
     //теперь необходимо определить, если что-то перед нашим источником света(исключив при этом сферу, с которой начата проверка)
     //если что-то есть, то Ili=0;- для данной точки на сфере
     if (peres(x_to_light,y_to_light,z_to_light,a_to_light,b_to_light,c_to_light,sphere,n,f)){//если пересечение было (точка в тени)
         Cosa=0;
         //light.Il=0;
     }
     else{//косинус между найденной нормалью и прямой к источнику света
         Cosa=(a_to_light*normalx+b_to_light*normaly+c_to_light*normalz)/((a_to_light*a_to_light+b_to_light*b_to_light+c_to_light*c_to_light)*(normalx*normalx+normaly*normaly+normalz*normalz));

     }
     //общая интенсивность по отдельному цвету здесь будет равна

     IaR=(sphere[numberspher].Red)/255;//таким образом находим фоновую интенсивность объекта
     IaG=(sphere[numberspher].Green)/255;//по красному зеленому и голубому для объекта
     IaB=(sphere[numberspher].Blue)/255;

     //Ir=IaR*sphere[f].Kd+light.Il*sphere[f].Kd*Cosa;
     //Ig=IaG*sphere[f].Kd+light.Il*sphere[f].Kd*Cosa;
     //Ib=IaB*sphere[f].Kd+light.Il*sphere[f].Kd*Cosa;
     //Ir=1;
     //Ig=1;
     //Ib=1;
     Ir=IaR*sphere[f].Kd+light.Il*sphere[f].Kd*Cosa;
     Ig=IaG*sphere[f].Kd+light.Il*sphere[f].Kd*Cosa;
     Ib=IaB*sphere[f].Kd+light.Il*sphere[f].Kd*Cosa;
     //
     //
     // Ib=light.Il*sphere[f].Kd;

     //Ir=IaR*sphere[f].Kd+light.Il*sphere[f].Kd;
     //Ig=IaG*sphere[f].Kd+light.Il*sphere[f].Kd;
     //IaB*sphere[f].Kd+light.Il*sphere[f].Kd;

     if (Ir>=1) Ir=1;
     if (Ig>=1) Ig=1;
     if (Ib>=1) Ib=1;
     *rgb=qRgb(int(Ir*255),int(Ig*255),int(Ib*255));

     return 0;
 }



void Power_pix (float ox,float oy,float oz,object *sphere, viewer camera,lightning light,int n,QRgb *rgb){//n возможно здесь больше истины(+1)
    /*здесь имеем координаты начальной точки луча, данные по всем сферам, данные по свету, и размер комнат
    */

    float tmin;
    float a,b,c;
    int f=-1;

    QRgb rgbi=(0,0,0);

    a=(ox-(camera.x));//задаем вектор направления
    b=(oy-(camera.y));
    c=(oz-(camera.z));
    tmin=intensity(ox,oy,oz,a,b,c,sphere,light,n,f,&rgbi);
    *rgb=rgbi;
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
        sphere[i].Kd=(atoi(baList[7]));
        sphere[i].Kd=(sphere[i].Kd)/10;
        i++;
    }
    file.close(); // закрываем файл

    for (int z=0; z<=height; z++) {
      for (int x=0; x<=width; x++) {
        rgb = qRgb(0,0,0);
        Power_pix(x,0,z,sphere,camera,light,i,&rgb);
        img.setPixel(x,z,rgb);
        rgb = qRgb(0,0,0);
      }
    }
    img.save("D:/mynumber.jpg","JPG");
    return a.exec();
}
