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


int width = 1400;
int height = 900;
int roomleangh=2000;//y
int roomwidth=1400; //x
int roomheight=900;//z
int MAXrecurs=6;
float Ks=0.7;
float p=200;

using namespace std;

struct color{
    int R;
    int G;
    int B;
};

struct lightning{
    int x;
    int y;
    int z;
    float Il;//интенсивность света
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
    float Kd;
};
struct viewer{//параметры наблюдателя
    // напротив какого пикселя находится наблюдатель
    int x=700;
    int z=450;
    int y=-1400;//расстояние до изображения
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
    t1=(-(2*(Xo-sphere.x)*a+2*(Yo-sphere.y)*b+2*(Zo-sphere.z)*c)+qPow(D,0.5))/(2*(a*a+b*b+c*c));
    t2=(-(2*(Xo-sphere.x)*a+2*(Yo-sphere.y)*b+2*(Zo-sphere.z)*c)-qPow(D,0.5))/(2*(a*a+b*b+c*c));
    t=minim(t1,t2);
    return t;
 }
             // x,y,z- это точка, из которой проводится луч, a,b,c-это векторы направлений    количество сфер изначальная сфера

                                                                  //к-во сфер, номер сферы, из которой происходит отбор
 float peres(float ox,float oy, float oz,float a,float b, float c, object *sphere,int n,int f){
  float D,t;
  for (int i=0 ; i<=n ; i++){
      if (((D=descr(sphere[i],ox,oy,oz,a,b,c))>=0)){//&&(t!=0)) { //проверяем на пересечение с всеми сферами
          t = intersect (D,sphere[i],ox,oy,oz,a,b,c);//расчитываем t для определения точки пересечения
          if (t>(-0.01))
          {//если пересечение найдено не в начальной точке
            return 1;// между источником света и нач. точкой есть объект
          }
      }
  }
  return 0;
}


 // Получаем координаты откуда идет луч, направление луча, массив сфер, источник света, количество сфер, сфера, которую мы исключаем(из нее и идет луч), указатель на цвет

 int intensity (float ox, float oy, float oz,float a, float b,float c,object *sphere,lightning *light,int n_sphere,int n_light, int f,float *IotrR,float *IotrG, float *IotrB,viewer camera, int nrecurs){//функция для определения интенсивности цветов
     int pulse=0,numberspher;
     int l;
     int i;
     float Cosa=0,Cosb=0;
     float D,t,tmin;
     float normalx,normaly,normalz;
     float IaR,IaG,IaB;
     float Ir,Ig,Ib;
     float Xinters,Yinters,Zinters;
     float af,bf,cf;
     float Xotr,Yotr,Zotr;

     //переменные цвета от отражения
     float IotrRi=0,IotrGi=0,IotrBi=0;

     float ind;
     //переменные для света
     float l_otr_x,l_otr_y,l_otr_z;
     float x_to_view,y_to_view,z_to_view;
     float a_to_light,b_to_light,c_to_light,x_to_light,y_to_light,z_to_light;
     float a_light,b_light,c_light;

     float Shadow= 0;
     float Bliks = 0;

     if (nrecurs > MAXrecurs) return 0;//мы слишком далеко зашли (IotrRi и другие остаются =0)

     for (i=0 ; i<n_sphere ; i++){
         if ((D=descr(sphere[i],ox,oy,oz,a,b,c))>=0 && (i!=f)){ //если имеется пересечение с сферой и эта сфера не является сферой из которой и идет пересечение то
            t = intersect (D,sphere[i],ox,oy,oz,a,b,c);//расчитываем t для определения точки пересечения
            if (t>=0)
            {
             if (pulse == 0){      //если до этого не был расчитан минимальный t,то минимальный t это расчитанный
                 tmin = t;
                 pulse = 1;
                 numberspher=i;//сфера с которой всё таки состоялось пересечение
             }
             else {
                 if (t<tmin){
                     tmin = t;
                     numberspher = i;
                 }
             }
            }
         }
     }

     if (pulse==0) return 0;//если пересечения ни с 1ой сферой не было (луч ушел в пустоту) возвращаемся (IotrRi и другие остаются =0)

     Xinters= ox + a * tmin; //
     Yinters= oy + b * tmin; //  точка пересечения с объектом
     Zinters= oz + c * tmin; //


     normalx=(Xinters - sphere[numberspher].x);//
     normaly=(Yinters - sphere[numberspher].y);//   векторы нормали с направлением
     normalz=(Zinters - sphere[numberspher].z);//

     //задаем новое направление

     Xotr=2*normalx*(normalx*(ox-Xinters)+normaly*(oy-Yinters)+normalz*(oz-Zinters))/(normalx*normalx+normaly*normaly+normalz*normalz)-(ox-Xinters);
     Yotr=2*normaly*(normalx*(ox-Xinters)+normaly*(oy-Yinters)+normalz*(oz-Zinters))/(normalx*normalx+normaly*normaly+normalz*normalz)-(oy-Yinters);
     Zotr=2*normalz*(normalx*(ox-Xinters)+normaly*(oy-Yinters)+normalz*(oz-Zinters))/(normalx*normalx+normaly*normaly+normalz*normalz)-(oz-Zinters);

     a=Xotr;
     b=Yotr;
     c=Zotr;

     af=a/(qPow((a*a+b*b+c*c),0.5));//нормированные
     bf=b/(qPow((a*a+b*b+c*c),0.5));
     cf=c/(qPow((a*a+b*b+c*c),0.5));

     ox=Xinters;
     oy=Yinters;
     oz=Zinters;

     f=numberspher; //запоминаем сферу с которой было пересечение

     tmin=intensity(ox,oy,oz,af,bf,cf,sphere,light,n_sphere,n_light,f,&IotrRi,&IotrGi,&IotrBi,camera,nrecurs+1);

     for (int j=0; j<n_light; j++){
        //раздел с определением теней и бликов
        a_to_light=(light[j].x-Xinters); b_to_light=(light[j].y-Yinters); c_to_light=(light[j].z-Zinters);

         a_light=(light[j].x - Xinters) / (pow((a_to_light * a_to_light + b_to_light * b_to_light + c_to_light * c_to_light),0.5));     b_light=(light[j].y - Yinters) / (pow((a_to_light * a_to_light + b_to_light * b_to_light + c_to_light * c_to_light),0.5));     c_light=(light[j].z - Zinters) / (pow((a_to_light * a_to_light + b_to_light * b_to_light + c_to_light * c_to_light),0.5));

         x_to_light=Xinters; y_to_light=Yinters;  z_to_light=Zinters;

        //теперь необходимо определить, есть-ли что-то перед нашим источником света

        ind=peres(x_to_light, y_to_light, z_to_light, a_light, b_light, c_light, sphere, n_sphere, f);

        if (ind==1){//если пересечение было и не в той-же точке откуда выходил => точка в тени
            Cosa=0;
            Cosb=0;
        }
        else{
             //косинус между найденной нормалью в точке и прямой к источнику света
            Cosa=(a_light * normalx + b_light * normaly + c_light * normalz)/(qPow(((a_light * a_light + b_light * b_light + c_light * c_light) * (normalx * normalx + normaly * normaly + normalz * normalz)),0.5));
            //необходимо расчитать отраженный луч света от поверхности
            //это и есть коэф отражения
            l_otr_x=2*normalx*(normalx*(light[j].x-Xinters)+normaly*(light[j].y-Yinters)+normalz*(light[j].z-Zinters))/(normalx*normalx+normaly*normaly+normalz*normalz)-(light[j].x-Xinters);
            l_otr_y=2*normaly*(normalx*(light[j].x-Xinters)+normaly*(light[j].y-Yinters)+normalz*(light[j].z-Zinters))/(normalx*normalx+normaly*normaly+normalz*normalz)-(light[j].y-Xinters);
            l_otr_z=2*normalz*(normalx*(light[j].x-Xinters)+normaly*(light[j].y-Yinters)+normalz*(light[j].z-Zinters))/(normalx*normalx+normaly*normaly+normalz*normalz)-(light[j].z-Xinters);

            x_to_view=camera.x-Xinters;
            y_to_view=camera.y-Yinters;
            z_to_view=camera.z-Zinters;
            Cosb=(l_otr_x * x_to_view + l_otr_y * y_to_view + l_otr_z * z_to_view)/(qPow(((l_otr_x * l_otr_x + l_otr_y * l_otr_y + l_otr_z * l_otr_z) * (x_to_view * x_to_view + y_to_view * y_to_view + z_to_view * z_to_view)),0.5));
        }
        Shadow = Shadow + Cosa * light[j].Il;
        Bliks = Bliks + (pow(Cosb,p) * light[j].Il);
     }
     //конец раздела, посвященного теням и бликам//здесь мы поличили Shadow и Bliks


     //общая интенсивность по отдельному цвету здесь будет равна

     IaR=(sphere[numberspher].Red) / 255;//фоновая интенсивность объекта
     IaG=(sphere[numberspher].Green) / 255;//
     IaB=(sphere[numberspher].Blue) / 255;

     //фон_интенс + тени + блики + отражение
     Ir=IaR * sphere[f].Kd + sphere[f].Kd * Shadow + Ks * Bliks + (1-sphere[f].Kd) * IotrRi;
     Ig=IaG * sphere[f].Kd + sphere[f].Kd * Shadow + Ks * Bliks + (1-sphere[f].Kd) * IotrGi;
     Ib=IaB * sphere[f].Kd + sphere[f].Kd * Shadow + Ks * Bliks + (1-sphere[f].Kd) * IotrBi;

     if (Ir>=1) Ir=1;
     if (Ig>=1) Ig=1;
     if (Ib>=1) Ib=1;
     *IotrR=Ir;
     *IotrG=Ig;
     *IotrB=Ib;

     return 1;
}



void Power_pix (float ox,float oy,float oz,object *sphere, viewer camera,lightning *light,int n_sphere,int n_light, QRgb *rgb){//n возможно здесь больше истины(+1)
    /*здесь имеем координаты начальной точки луча, данные по всем сферам, данные по свету, и размер комнат
    */

    float tmin;
    float a,b,c;
    float af,bf,cf;
    int f=-1;
    int l;
    int nrecurs=1;
    float IotrR=0,IotrG=0,IotrB=0;

    a=(ox-(camera.x));//задаем вектор направления
    b=(oy-(camera.y));
    c=(oz-(camera.z));

    af=a/(qPow((a*a+b*b+c*c),0.5));//нормировка
    bf=b/(qPow((a*a+b*b+c*c),0.5));
    cf=c/(qPow((a*a+b*b+c*c),0.5));
    // координаты откуда пускается луч его напр, количество источников света, сфер, сфера,из которой идет луч, ссылка на интенсивность красного,зеленого,голубого, данные наблюдателя и номер нынешней рекурсии
    l=intensity(ox,oy,oz,af,bf,cf,sphere,light,n_sphere,n_light,f,&IotrR,&IotrG,&IotrB,camera,nrecurs);

    *rgb=qRgb(int(IotrR*255),int(IotrG*255),int(IotrB*255));

    return;
}


int main(int argc, char *argv[]){
    int n_light;
    int n_sphere;
    QApplication a(argc, argv);
    object sphere[10];
    viewer camera;
    lightning light[10];

    QImage img(width,height,QImage::Format_RGB32);
    QRgb rgb;
    QFile file("D:/cond.txt");


    file.open(QIODevice::ReadOnly);
    QTextStream stream(&file);

    QByteArray qw = file.readLine(); // читаем первую строку
    n_light = atoi(qw);

    for (int j=0;j<n_light;j++){ //координаты, интенсивность света
        QByteArray ba = file.readLine(); // читаем первую строку
        QList<QByteArray> baList = ba.split(' '); // делим строку на кусочки... в качестве разделителя используем пробел

        light[j].x=atoi(baList[0]);
        light[j].y=atoi(baList[1]);
        light[j].z=atoi(baList[2]);
        light[j].Il=atoi(baList[3]);
        light[j].Il=(light[j].Il)/10;
        //j++;
    }

    QByteArray ty = file.readLine();
    n_sphere = atoi(ty);

    for (int i=0; i<n_sphere; i++){ //радиус координаты, цвет сферы
        QByteArray ce = file.readLine(); // читаем первую строку
        QList<QByteArray> ceList = ce.split(' '); // делим строку на кусочки... в качестве разделителя используем пробел

        sphere[i].R=atoi(ceList[0]);
        sphere[i].x=atoi(ceList[1]);
        sphere[i].y=atoi(ceList[2]);
        sphere[i].z=atoi(ceList[3]);
        sphere[i].Red=atoi(ceList[4]);
        sphere[i].Green=atoi(ceList[5]);
        sphere[i].Blue=atoi(ceList[6]);
        sphere[i].Kd=(atoi(ceList[7]));
        sphere[i].Kd=(sphere[i].Kd)/10;
    }
    file.close(); // закрываем файл

    for (int z=0; z<=height; z++) {
      for (int x=0; x<=width; x++) {
        rgb = qRgb(0,0,0);
        Power_pix(x,0,z,sphere,camera,light,n_sphere,n_light,&rgb);
        img.setPixel(x,z,rgb);
        rgb = qRgb(0,0,0);
      }
    }
    img.save("D:/mynumber.jpg","JPG");
    return a.exec();
}
