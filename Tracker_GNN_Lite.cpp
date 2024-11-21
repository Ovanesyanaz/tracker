#include "Filter.h"
#include "ArrayController.h"
#include "MahalDist.h"
#include "Track_GNN.h"
#include "GetMeasVector.h"
#include "ArrayController.h"
#include "GetPredictedMeas.h"
#include <vector>
#include <Eigen/Dense>
#include <limits>
#include "helpfunctions.h"
#include "assignmentoptimal.h"
using Eigen::MatrixXd;
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
    MatrixXd d2;
    MatrixXd detS;
    std::vector<std::vector<MatrixXd>> S;
    // %gating thresholds
    double gatingThresholds;
    // %branch (track hyp) ID counter
    int branchID = 0;
    // %track (confirmed) ID counter
    int trackID = 0;
    std::vector<Track_GNN>tracks;
    // tracks
    // %track array controller
    Tac tac;
    // %initial track score
    double initScore;
    // %track score confirmation/deletion thresholds
    double T1;
    double T2;
    
    //методы
    // инициализация трекера 
    Tracker_GNN_Lite(Filter filter){
        //заполняем массив track_array пустыми траекториями
        for(int i = 0; i < nTrackMax; i++){
            Track_GNN j;
            this->tracks.push_back(j);
        }
        //инициализируем tac
        this->tac = ArrayController();
        //получаем пороги сробирования
        this->gatingThreasholds = GatingThreasholds();
        // d2, detS, S
        MatrixXd d_2(nMeasMax, nTrackMax);
        this->d2 = d_2;
        this->detS = this->d2;
        std::vector<std::vector<MatrixXd>> S_1;
        this->S = S_1;
        //считаем начальный вес
        this->initScore = log(this->Pd * this->betaNT / this->betaFA);
        //пока не сделано !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        this->T1, this->T2 = 1, 2;
    };
    //главный метод - обновление 
    void Update(double t, std::vector<std::vector<double>>meases){
        //кол-во траекторий и измерений
        int nTrk = this->tac.used;
        int nMeas = meases.size();
        std::vector<int> list_of_track = this->tac.list;
        //для каждой существующей траетории делаем предикт
        for (int i : list_of_track){
            this->tracks[i].Predict(t);
        }
        //далее стробирование и присвоение отметок траекториям
        //стробирование
        std::vector<int> cInd; //индексы подтвержденных траекторий
        std::vector<int> iInd; //инициализированных траеткорий
        std::vector<int> tInd; //пробные траектории
        for (int i : list_of_track){
            //если подтвержденный
            if (this->tracks[i].isConfirmed){
                cInd.push_back(i);
            }
            //если инициатор
            else if (this->tracks[i].score == this->initScore){
                iInd.push_back(i);
            }
            //пробные траектории
            else {
                tInd.push_back(i);
            }
        }
        //assigned meas idx
        std::vector<bool> assignedMeasIdx(nMeas);
        //assigned track idx
        std::vector<bool> assignedTrackIdx(nTrk);
        //all assignments (M2TA)       
        std::vector<int> assigments(nMeas);
        //сопоставляем каждую отметку с каждым спрогнозируемым вектором
        if ((nMeas > 0) and (nTrk > 0)){
        //размерность вектора измерений
            int M = 2;
        // для каждой измерения
            for (int i = 0; i < nMeas; i++){
                //находим матрицу R и вектор y
                const auto [y, R] = GetMeasVector(meases[i]);
                //для каждой существующей траекториии
                for (int j = 0; j < nTrk; j++){
                    //индекс рассматриваемого трека
                    int iTr = this->tac.list[j];

                    //вектор и ков матрица траектории
                    MatrixXd x = this->tracks[iTr].vec();
                    MatrixXd P = this->tracks[iTr].cov();

                    //вычисление предугаданного измерения и его матрицы H
                    auto [z, H] = GetPredictedMeas(x);
                    MatrixXd dz = y - z;
                    //обновляем S
                    this->S[i][j] = H * P * H.transpose() + R;
                    //ищем расстояние
                    this->d2(i,j) = MahalDist(dz, S[i][j]);
                    //находим определитель S
                    this->detS(i,j) = S[i][j].determinant();
                }
            }

            // D = this.d2(1:nMeas, 1:nTrk) + log(this.detS(1:nMeas, 1:nTrk)); %log(|S|) is penalty for large covariance
            // D = D - min(D); %cost mat must be non-zero        
            // D(this.d2(1:nMeas, 1:nTrk) > this.gatingThresholds(M)) = inf;  %measurement validation
            MatrixXd D = this->d2 + this->detS.array().log().matrix();
            // вычитание минимальной цены из соответсвующих столбцов
            // присвоение бесконечности 
            for (int i = 0; i < D.cols(); i++){
                double min = 1000000000;
                for (int j = 0; j < D.rows(); j++){
                    if (D(j,i) < min){
                        min = D(j,i);
                    }
                }
                for (int j = 0; j < D.rows(); j++){
                    D(j,i) = D(j,i) - min;
                    if (D(j,i) > this->gatingThresholds(M)){
                        D(j,i) = std::numeric_limits<double>::infinity();
                    }
                }
            }
            //сначала присваиваем отметки в подтвержденные треки
            if (cInd.size() > 0){
                std::pair<MatrixXd, double> assig = assignmentoptimal(getColsMatrix(D, cInd));
                MatrixXd m2ta = std::get<0>(assig);
                for (int i = 0; i < m2ta.rows(); i++){
                    if (m2ta(0,i) > 0){
                        assigments[i] = cInd[m2ta(0,i)];
                        assignedMeasIdx[i] = true;
                        assignedTrackIdx[cInd[m2ta(0, i)]] = true;
                        for (int j; j < D.cols(); j++){
                            D(i,j) = std::numeric_limits<double>::infinity();
                        }
                    }
                }
            }
            //затем в находящиеся на подтверждении
            if (tInd.size() > 0){
                std::pair<MatrixXd, double> assig = assignmentoptimal(getColsMatrix(D, tInd));
                MatrixXd m2ta = std::get<0>(assig);
                for (int i = 0; i < m2ta.rows(); i++){
                    if (m2ta(i,0) > 0){
                        assigments[i] = cInd[m2ta(0,i)];
                        assignedMeasIdx[i] = true;
                        assignedTrackIdx[cInd[m2ta(0, i)]] = true;
                        for (int j; j < D.cols(); j++){
                            D(i,j) = std::numeric_limits<double>::infinity();
                        }
                    }
                }
            }
            //и в последнюю очередь сопоставляем их в траектории с 1 отметкой (инициаторами)
            if (iInd.size() > 0){
                std::pair<MatrixXd, double> assig = assignmentoptimal(getColsMatrix(D, iInd));
                MatrixXd m2ta = std::get<0>(assig);
                for (int i = 0; i < m2ta.rows(); i++){
                    if (m2ta(i,0) > 0){
                        assigments[i] = cInd[m2ta(0,i)];
                        assignedMeasIdx[i] = true;
                        assignedTrackIdx[cInd[m2ta(0, i)]] = true;
                        for (int j; j < D.cols(); j++){
                            D(i,j) = std::numeric_limits<double>::infinity();
                        }
                    }
                }
            }
        //обновляем свойства траекторий, в которые не попали отметки исходя из их статуса
        //не присвоенные метки задают инициаторов
        //удаляем траектории с низким score
        //удаляем траектории с некоторым числом зондирований без присвоения
        //удаляем траектории в которые давно не попадало отметок   
        }
        std::vector<int> ind;
        for (int i = 0; i < assigments.size(); i++){
            if (assigments[i] > 0){
                ind.push_back(i);
            }
        }

        int M = 2;     
        for (int im:ind){
            int it = assigments[im];
            int iTr = this->tac.list[it];
            double dLLR = ScoreIncrement(true, this->Pd, this->betaFA, M, this->detS(im,it), this->d2(im, it));
            this->tracks[iTr].score = this->tracks[iTr].score + dLLR;
            this->tracks[iTr].isConfirmed = (this->tracks[iTr].isConfirmed) || (this->tracks[iTr].score > this->T2);

            if ((this->tracks[iTr].isConfirmed) && !(this->tracks[iTr].trackID > 0)){
                this->trackID = this->trackID + 1;
                this->tracks[iTr].trackID = this->trackID;
            }
            //---------------------------------------------------------------------------------------------
            //не понятная часть
            this->tracks[iTr].measIDHist = Add(this.tracks[iTr].measIDHist, meases[im].id)       
            this->tracks[iTr].nLost = 0;
            this->tracks[iTr] = Update(this.tracks(iTr), meases[im]);
            //---------------------------------------------------------------------------------------------
        }

    //           %NOT ASSIGNED TRACKS --> UPDATE TRACK LOGIC
    //   ind = find(~assignedTrackIdx(:) & ~iIdx(:));  %not assigned tracks inds
        ind.clear();
        //индексы траекторий которым не присвоилась отметка и они не на инициализации
        for (int i : list_of_track){
            if (!assignedTrackIdx[i]){
                for(int j : iInd){
                    bool flag = true;
                    if(i == j){
                        bool flag = true;
                        break;
                    }
                    if (flag){
                        ind.push_back(i);
                    }
                }
            }
        }

        for (int it : ind){
            int iTr = this->tac.list[it];
            int M = 2;
            double dLLR = ScoreIncrement(false, this->Pd, this->betaFA, M);
            this->tracks[iTr].score = this->tracks[iTr].score + dLLR;
            this->tracks[iTr].isConfirmed = (this->tracks[iTr].isConfirmed) || (this->tracks[iTr].score > this->T2);

            this->tracks[iTr].measIDHist = Add(this.tracks[iTr].measIDHist, 0);     
            this->tracks[iTr].nLost = this->tracks[iTr].nLost + 1;
            this->tracks[iTr] = FillHistory(this->tracks[iTr]);
        }
        //инициаторы в котрые не попали отметки удаляем
        ind.clear();
        //индексы траекторий которым не присвоилась отметка и они инициаторы
        for (int i : list_of_track){
            if (!assignedTrackIdx[i]){
                for(int j : iInd){
                    if(i == j){
                        ind.push_back(i);
                        break;
                    }
                }
            }
        }
        //удаляем
        for (int i: ind){
            Deallocate(this->tac.list[i]);
        }
        //не присвоенные отметки создают траектории
        ind.clear();
        //индексы не присвоенных отметок
        for(int i = 0;i < nMeas;i++){
            if (!assignedMeasIdx){
                ind.push_back(i);
            }
        }
        for (int im: ind){
            int iTr = Allocate(this.tac);
            this->branchID = this->branchID + 1;
            this->tracks[iTr].branchID = this->branchID;
            this->tracks[iTr].score = this->initScore;
            this->tracks[iTr].measIDHist = Reset(this->tracks[iTr].measIDHist);
            this->tracks[iTr].measIDHist = Add(this->tracks[iTr].measIDHist, meases[im].id);
            this->tracks[iTr].nLost = 0;
            this->tracks[iTr] = Init(this->tracks[iTr], meases[im]);
        }
        for (int i:this->tac.list){
            //удаление треков с маленьким счетом 
            if (this->tracks[i].score < this->T1 || 
                //с большим количеством пропущенных присвоений
                this->tracks[i].nLost > this->nLostMax ||
                //удаление по прошедшему времени от последнего обновления
                t - this->tracks[i].tUpdate >= this->tLostMax
                ){
                Deallocate(this->tac, i);
            }
        }
    }

};