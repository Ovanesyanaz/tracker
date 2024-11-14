#include "Filter.h"
#include "Tac.h"
class Tracker_GNN_Lite
{
private:
    Filter filter;
    //track array capacity
    int nTrackMax = 50;
    //measurement buffer capacity (in one scan)
    int nMeasMax = 50;
    //spatial density of false detections (in detection state space)
    double betaFA = 0.0001;
    //spatial density of new tracks (tunning parameter)
    double betaNT = 1e-8;
    //probability of detection
    double Pd = 0.9;
    //probability of gating
    double Pg = 0.999;
    //max number of lost detections in a branch
    int nLostMax = 10;
    //max time without meas, seconds
    double tLostMax = 2;
    //false track confirmation probability
    double falseTrackConfirmProb = 1e-6;
    //true track deletion probability
    double trueTrackDeletionProb = 1e-5; //-3
    //gating result containers
    //...
    Tac tac;
    //методы
    // инициализация трекера 
    Tracker_GNN_Lite(Filter filter){
        //применяем фильтр 
        //заполняем фильтр нужными данными, считаем начальный score
        //инициализируем tac - track array controller
    };
    //главный метод - обновление 
    void Update(){
        //принимает t meases
        //для каждой существующей траетории делаем предикт
        //далее стробирование и присвоение отметок траекториям
        //сопоставляем каждую отметку с каждым спрогнозируемым вектором
        //запоминаем нормализованное расстояние между ними и определитель матрицы S
        //далее присвоение
        //сначала присваиваем отметки в подтвержденные треки
        //затем в находящиеся на подтверждении
        //и в последнюю очередь сопоставляем их в траектории с 1 отметкой (инициаторами)
        //обновляем свойства траекторий, в которые не попали отметки исходя из их статуса
        //не присвоенные метки задают инициаторов
        //удаляем траектории с низким score
        //удаляем траектории с некоторым числом зондирований без присвоения
        //удаляем траектории в которые давно не попадало отметок
    }

};