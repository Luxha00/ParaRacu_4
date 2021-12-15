#include <iostream>
#include <vector>
#include <random>
#include <thread>
#include <mutex>
#include <omp.h>

using namespace std;
using namespace std::chrono;

int PSL = 9999;
double MF = 0;
int L = 2501;
int nfesLmt = 1000000;
int nfes = 0;
int seed = 0;
int threads = 0;
vector <int> sequence;
vector <vector<int>> sosedi;
vector <vector<int>> kandidati;
vector <int> bestPSLSequence;
vector <int> bestMFSequence;
mutex myMutex;
vector <int> borderFrom;
vector <int> borderTo;

void sequenceFill(){
    sequence.push_back(1);
    sequence.push_back(1);
    sequence.push_back(1);
    sequence.push_back(-1);
    sequence.push_back(-1);
    sequence.push_back(-1);
    sequence.push_back(1);
    sequence.push_back(-1);
    sequence.push_back(-1);
    sequence.push_back(1);
    sequence.push_back(-1);
    sequence.push_back(-1);
}

void getSosede(){
    kandidati.push_back(sosedi[0]);
    for(int i = 1; i <= L; i++){
        if (sequence[i-1] == -1){
            sosedi[i][i-1] = 1;
        }
        else{
            sosedi[i][i-1] = -1;
        }
    }
}

void randomize(){
    random_device rd;
    mt19937 mt(seed+nfes);
    uniform_int_distribution<int> dist(0,1);
    for ( int i = 0; i < L; i++){
        sequence.push_back(dist(mt));
    }
    for ( int i = 0; i < L; i++){
        if(sequence[i] == 0){
            sequence[i] = -1;
        }
    }
}

void getPSL(int ckHigh){
    if (ckHigh < PSL){
        myMutex.lock();
        PSL = ckHigh;
        bestPSLSequence = sequence;
        myMutex.unlock();
    }
}

void getMF(vector <int> sequence, double ckVsota){
    double L2 = L * L;
    double multi = 2;
    double mf = L2/(multi*ckVsota);
    if (mf > MF){
        MF = mf;
        bestMFSequence = sequence;
    }
}

void Ck(vector <int> sequence){
    vector <int> Cks;
    int ck = 0;
    double ckVsota = 0;
    int ckHigh = 0;
    for (int k = 1; k < L; k++){
        for(int i = 0; i < L-k; i++){
            ck += sequence[i] * sequence[i+k];
        }
        int ck2 = ck * ck;
        if (ck <0){
            ck = -ck;
        }
        if (ck > ckHigh){
            ckHigh = ck;
        }
        ckVsota += ck2;
        ck = 0;
    }
    getPSL(ckHigh);
    getMF(sequence, ckVsota);
}

void threadRun(int id){
    int from = borderFrom[id];
    int to = borderTo[id];
    while (from < to){
        Ck(sosedi[from]);
        from++;
    }
}

void exeThreads(int threads){
    #pragma omp parallel num_threads (threads)
    {
        int id = omp_get_thread_num();
        threadRun(id);
    }
}

void getRandomSequence(int L){
    int from = 0;
    int to = L/threads;
    int multi = L/threads;
    for (int i = 0; i < threads; i++){
        borderFrom.push_back(from);
        borderTo.push_back(to);
        from = to;
        to += multi;
    }

    while (nfes < nfesLmt){
        randomize();
        for ( int i = 0; i <= L; i++){
            if(nfes < nfesLmt){
                sosedi.push_back(sequence);
                nfes++;
            }
        }
        getSosede();
        exeThreads(threads);
        sequence.clear();
        sosedi.clear();
    }
}

int main(int argc, char *argv[]) {
    //for (int i = 0; i < argc; ++i)
    //    cout << argv[i] << "\n";
    //L = reinterpret_cast<int>(argv[0]);
    //seed = reinterpret_cast<int>(argv[1]);
    //nfesLmt = reinterpret_cast<int>(argv[2]);
    cout << "enter seed: " << endl;
    cin >> seed;
    cout << "enter threads: " << endl;
    cin >> threads;
    auto start = high_resolution_clock::now();
    getRandomSequence(L);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(stop - start);
    cout << "L: " << L << endl;
    cout << "NfesLmt: " << nfesLmt << endl;
    cout << "seed: " << seed << endl;
    cout << "Runtime in seconds:" << duration.count() << endl;
    double speed = nfesLmt / duration.count();
    cout << "Speed: " << speed << endl;
    cout << "\n" << "best PSL: " << PSL << endl;
    cout << "sequence: " << endl;
    for (int i = 0; i < L; ++i) {
        cout << bestPSLSequence[i] << ", ";
    }
    cout << "\n" <<"best MF: " << MF << endl;
    cout << "sequence: " << endl;
    for (int i = 0; i < L; ++i) {
        cout << bestMFSequence[i] << ", ";
    }
    return 0;
}