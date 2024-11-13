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
        
    }

};