#include "Filter.h"
#include "ArrayController.h"
#include "Source.h"
#include "Track_GNN.h"
#include "ArrayController.h"
#include <vector>
#include <Eigen/Dense>
#include <limits>
#include "helpfunctions.h"
#include "AssignmentOptimal.h"
#include "GatingThresholds.h"
using Eigen::MatrixXd;

class Tracker_GNN_Lite {
public:
    Filter filter;
    // track array capacity
    int nTrackMax = 10;
    // measurement buffer capacity (in one scan)
    int nMeasMax = 10;
    // spatial density of false detections (in detection state space)
    double betaFA = 0.0001;
    // spatial density of new tracks (tunning parameter)
    double betaNT = 0.00001;
    // probability of detection
    double Pd = 0.9;
    // probability of gating
    double Pg = 0.999;
    // max number of lost detections in a branch
    int nLostMax = 10;
    // max time without meas, seconds
    double tLostMax = 2;
    // false track confirmation probability
    double falseTrackConfirmProb = 1e-6;
    // true track deletion probability
    double trueTrackDeletionProb = 1e-5; //-3
    // gating result containers
    MatrixXd d2;
    MatrixXd detS;
    std::vector<std::vector<MatrixXd> > S;
    // %gating thresholds
    double gatingThresholds;
    // %branch (track hyp) ID counter
    int branchID = 0;
    // %track (confirmed) ID counter
    int trackID = 0;
    std::vector<Track_GNN> tracks;
    // tracks
    // %track array controller
    ArrayController tac;
    // %initial track score
    double initScore;
    // %track score confirmation/deletion thresholds
    double T1;
    double T2;

    // методы
    //  инициализация трекера
    Tracker_GNN_Lite() {
        // заполняем массив track_array пустыми траекториями
        for (int i = 0; i < nTrackMax; i++) {
            Track_GNN j;
            this->tracks.push_back(j);
        }
        // инициализируем tac
        this->tac = ArrayController(nTrackMax);
        // d2, detS, S
        MatrixXd d_2(nMeasMax, nTrackMax);
        MatrixXd det_S(nMeasMax, nTrackMax);
        this->d2 = d_2;
        this->detS = det_S;
        this->S.resize(10);
        for (int i = 0; i < 10; i++) {
            S[i].resize(10);
        }
        // считаем начальный вес
        this->initScore = log(this->Pd * this->betaNT / this->betaFA);
        // пока не сделано !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        //const auto [y, R] = GetMeasVector(meases[i]);
        const auto [buf1, buf2] = SPRT_thresholds(falseTrackConfirmProb, trueTrackDeletionProb, initScore);
        this->T1 = buf1;
        this->T2 = buf2;
    };
    // главный метод - обновление
    void Update(double t, std::vector<std::vector<double> > meases) {
        // кол-во траекторий и измерений
        int nTrk = this->tac.used;
        int nMeas = meases.size();
        std::cout << "________new iteration________" << std::endl;
        std::vector<int> list_of_track = this->tac.list;
        // для каждой существующей траектории делаем предсказание
        for (int i: list_of_track) {
            this->tracks[i].Predict(t);
        }
        // далее стробирование и присвоение отметок траекториям
        // стробирование
        std::vector<int> cInd; // индексы подтвержденных траекторий
        std::vector<int> iInd; // инициализированных траекторий
        std::vector<int> tInd; // пробные траектории
        for (int i: list_of_track) {
            // если подтвержденный
            if (this->tracks[i].isConfirmed) {
                cInd.push_back(i);
            }
            // если инициатор
            else if (this->tracks[i].score == this->initScore) {
                iInd.push_back(i);
                //std::cout << "Добавили " << i << " в iInd" << std::endl;
            }
            // пробные траектории
            else {
                tInd.push_back(i);
            }
        }
        // assigned meas idx
        std::vector<bool> assignedMeasIdx(nMeas);
        // assigned track idx
        std::vector<bool> assignedTrackIdx(nTrk);
        // all assignments (M2TA)
        std::vector<int> assignments(nMeas, -1);
        // сопоставляем каждую отметку с каждым спрогнозированным вектором
        if ((nMeas > 0) and (nTrk > 0)) {
            // размерность вектора измерений
            int M = 2;
            // для каждого измерения
            for (int i = 0; i < nMeas; i++) {
                // находим матрицу R и вектор y

                const auto [y, R] = GetMeasVector(meases[i]);
                // для каждой существующей траектории
                for (int j = 0; j < nTrk; j++) {
                    // индекс рассматриваемого трека
                    int iTr = this->tac.list[j];
                    // вектор и ков матрица траектории
                    MatrixXd x = this->tracks[iTr].vec();
                    MatrixXd P = this->tracks[iTr].cov();
                    // вычисление предугаданного измерения и его матрицы H
                    auto [z, H] = GetPredictedMeas(x);
                    MatrixXd dz = y - z;
                    // обновляем S
                    this->S[i][j] = H * P * H.transpose() + R;

                    // ищем расстояние
                    this->d2(i, j) = MahalDist(dz, S[i][j]);
                    // // находим определитель S
                    this->detS(i, j) = S[i][j].determinant();
                }
            }

            // D = this.d2(1:nMeas, 1:nTrk) + log(this.detS(1:nMeas, 1:nTrk)); %log(|S|) is penalty for large covariance
            // D = D - min(D); %cost mat must be non-zero
            // D(this.d2(1:nMeas, 1:nTrk) > this.gatingThresholds(M)) = inf;  %measurement validation

            MatrixXd D(nMeas, nTrk);
            for (int i = 0; i < nMeas; i++) {
                for (int j = 0; j < nTrk; j++) {
                    D(i, j) = this->d2(i, j) + log(this->detS(i, j));
                }
            }

            // вычитание минимальной цены из соответствующих столбцов
            // присвоение бесконечности
            for (int i = 0; i < D.cols(); i++) {
                double min = 1000000000;
                for (int j = 0; j < D.rows(); j++) {
                    if (D(j, i) < min) {
                        min = D(j, i);
                    }
                }
                for (int j = 0; j < D.rows(); j++) {
                    D(j, i) = D(j, i) - min;
                    if (D(j, i) > GatingThresholds(this->Pg, 2)) {
                        D(j, i) = std::numeric_limits<double>::infinity();
                    }
                }
            }

            // сначала присваиваем отметки в подтвержденные треки
            if (cInd.size() > 0) {
                std::pair<MatrixXd, double> assig = assignmentoptimal(getColsMatrix(D, cInd));
                MatrixXd m2ta = std::get<0>(assig);
                for (int i = 0; i < m2ta.rows(); i++) {
                    if (m2ta(i, 0) > 0) {
                        assignments[i] = cInd[m2ta(i, 0) - 1];
                        assignedMeasIdx[i] = true;
                        assignedTrackIdx[cInd[m2ta(i, 0) - 1]] = true;
                        for (int j; j < D.cols(); j++) {
                            D(i, j) = std::numeric_limits<double>::infinity();
                        }
                    }
                }
            }

            // затем в находящиеся на подтверждении
            if (tInd.size() > 0) {
                std::pair<MatrixXd, double> assig = assignmentoptimal(getColsMatrix(D, tInd));
                MatrixXd m2ta = std::get<0>(assig);
                for (int i = 0; i < m2ta.rows(); i++) {
                    if (m2ta(i, 0) > 0) {
                        assignments[i] = tInd[m2ta(i, 0) - 1];
                        assignedMeasIdx[i] = true;
                        assignedTrackIdx[tInd[m2ta(i, 0) - 1]] = true;
                        for (int j = 0; j < D.cols(); j++) {
                            D(i, j) = std::numeric_limits<double>::infinity();
                        }
                    }
                }
            }

            // и в последнюю очередь сопоставляем их в траектории с 1 отметкой (инициаторами)
            if (iInd.size() > 0) {
                std::pair<MatrixXd, double> assig = assignmentoptimal(getColsMatrix(D, iInd));
                MatrixXd m2ta = std::get<0>(assig);
                for (int i = 0; i < m2ta.rows(); i++) {
                    if (m2ta(i, 0) > 0) {
                        assignments[i] = iInd[m2ta(i, 0) - 1];
                        assignedMeasIdx[i] = true;
                        assignedTrackIdx[iInd[m2ta(i, 0) - 1]] = true;
                        for (int j = 0; j < D.cols(); j++) {
                            D(i, j) = std::numeric_limits<double>::infinity();
                        }
                    }
                }
            }


            // обновляем свойства траекторий, в которые не попали отметки исходя из их статуса
            // не присвоенные метки задают инициаторов
            // удаляем траектории с низким score
            // удаляем траектории с некоторым числом зондирований без присвоения
            // удаляем траектории в которые давно не попадало отметок
        }

        std::vector<int> ind;
        for (int i = 0; i < assignments.size(); i++) {
            if ((assignments[i]) >= 0) {
                ind.push_back(i);
            }
        }

        int M = 2;
        for (int im: ind) {
            int it = assignments[im];
            std::vector<int> tmp = tac.list;
            int iTr = this->tac.list[it];
            double dLLR = ScoreIncrement(true, this->Pd, this->betaFA, M, this->detS(im, it), this->d2(im, it));
            this->tracks[iTr].score = this->tracks[iTr].score + dLLR;
            this->tracks[iTr].isConfirmed = (this->tracks[iTr].isConfirmed) || (this->tracks[iTr].score > this->T2);

            if ((this->tracks[iTr].isConfirmed) && !(this->tracks[iTr].trackID > 0)) {
                this->trackID = this->trackID + 1;
                this->tracks[iTr].trackID = this->trackID;
            }
            //---------------------------------------------------------------------------------------------
            // не понятная часть
            this->tracks[iTr].measIDHist.push_back(meases[im][4]);
            //this->tracks[iTr].measIDHist = Add(this.tracks[iTr].measIDHist, meases[im].id);
            this->tracks[iTr].nLost = 0;
            this->tracks[iTr].Update(t, meases[im]);
            //---------------------------------------------------------------------------------------------
        }

        //           %NOT ASSIGNED TRACKS --> UPDATE TRACK LOGIC
        ind.clear();
        // индексы траекторий которым не присвоилась отметка и они не на инициализации
        for (int i: list_of_track) {
            if (!assignedTrackIdx[i]) {
                bool flag = true;

                for (int j: iInd) {
                    if (i == j) {
                        flag = false;
                        break;
                    }
                }

                if (flag) ind.push_back(i);
            }
        }


        for (int it: ind) {
            // не заходим в данный блок кода
            int iTr = this->tac.list[it];
            int M = 2;
            double dLLR = ScoreIncrement(false, this->Pd, this->betaFA, M, 0, 0);
            this->tracks[iTr].score = this->tracks[iTr].score + dLLR;
            this->tracks[iTr].measIDHist.push_back(0);
            this->tracks[iTr].nLost = this->tracks[iTr].nLost + 1;
            this->tracks[iTr].FillHistory();
        }

        // инициаторы в которые не попали отметки удаляем
        ind.clear();
        // индексы траекторий которым не присвоилась отметка и они инициаторы
        for (int i: iInd) {
            if (assignedTrackIdx[i] == 0) {
                ind.push_back(i);
            }
        }
        // удаляем
        std::vector<int> to_del;
        for (int i: ind) {
            to_del.push_back(i);
        }
        this->tac.Deallocate(to_del);
        // не присвоенные отметки создают траектории
        ind.clear();
        // индексы не присвоенных отметок
        for (int i = 0; i < nMeas; i++) {
            if (!assignedMeasIdx[i]) {
                ind.push_back(i);
            }
        }
        std::vector<int> iTr_list = this->tac.Allocate(ind.size());
        int iTr_list_itr = 0;
        for (int im: ind) {
            this->branchID = this->branchID + 1;
            int iTr = iTr_list[iTr_list_itr];
            this->tracks[iTr].branchID = this->branchID;
            this->tracks[iTr].score = this->initScore;
            this->tracks[iTr].Reset();
            this->tracks[iTr].measIDHist.push_back(meases[im][4]);
            this->tracks[iTr].nLost = 0;
            this->tracks[iTr].Init(t, meases[im]);
            iTr_list_itr = iTr_list_itr += 1;
        }
        std::vector<int> ind_to_dealocate;
        for (int i: this->tac.list) {
            // удаление треков с маленьким счетом
            if (this->tracks[i].score < this->T1 ||
                // с большим количеством пропущенных присвоений
                this->tracks[i].nLost > this->nLostMax ||
                // удаление по прошедшему времени от последнего обновления
                t - this->tracks[i].tUpdate >= this->tLostMax) {
                ind_to_dealocate.push_back(i);
            }
            this->tac.Deallocate(ind_to_dealocate);
            ind_to_dealocate.clear();
        }
    }
};


int main(int argc, char const *argv[]) {
    Tracker_GNN_Lite Tracker;

    double t = 0.1;
    std::vector<std::vector<double> > meases;
    meases = get_vec("../Marks.csv");

    for (int i = 0; i < 10; i++) {
        std::vector<std::vector<double> > meas;
        meas.push_back(meases[i]);
        meas.push_back(meases[i + 1]);
        Tracker.Update(t, meas);
        t += 0.1;
        i++;
        for (int j = 0; j < Tracker.tac.used; j++) {
            std::cout << Tracker.tracks[j].score << std::endl;
        }
    }
    std::cout << "all ok \n";
}
