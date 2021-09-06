#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <math.h>

using namespace std;

double randDouble(void){
    return ((double) rand())/(double)RAND_MAX;
}

vector<int> bools2poles(vector<bool> x){
    vector<int> y;
    for(int i=0;i<x.size();i++){
        if(x[i]){
            y.push_back(1);
        } else {
            y.push_back(-1);
        }
    }
    return y;
}

// CONVERT AN INTEGER TO A BINARY (VECTOR OF BOOLS)
vector<bool> int2bin(long int i, long int n){
    vector<bool> x(n,false);
    long int a = pow(2,n-1);
    for(long int k=0;k<n;k++){
        x[k] = (i>=a);
        if(x[k]) i-=a;
        a/=2;
    }
    return x;
}

int bin2int(vector<bool> b){
    int k = 0;
    int s = 1;
    for(int i=b.size();i--;){
        k += s*b[i];
        s *= 2;
    }
    return k;
}


vector<int> randPermute(int x){
    vector<int> X(x);
    for(int i=0;i<x;i++){
        X[i] = i;
    }
    vector<int> Y(x);
    for(int i=0;i<x;i++){
        int index = floor(randDouble()*X.size());
        Y[i] = X[index];
        X.erase (X.begin()+index);
    }
    return Y;
}



class Alphabet {

    public:
        int Nbits;
        int Nassigned;
        vector<string> strings;
        vector<vector<bool> > binaries;

    Alphabet(int Nbits){
        Nassigned = 0;
        this->Nbits = Nbits;
        strings.resize(pow(2,Nbits),"#");
        for(int i=0;i<pow(2,Nbits);i++){
            binaries.push_back(int2bin(i,Nbits));
        }
    }

    void addSymbol(string symb){
        strings[Nassigned]=symb;
        Nassigned++;
    }

    vector<bool> getBinary(string symb){
        for(int i=0;i<strings.size();i++){
            if(strings[i]==symb){
                return binaries[i];
            }
        }
        cout<<"Could not find symbol "<<symb<<"in alphabet!"<<endl;
        return vector<bool>(0);
    }

    string getString(vector<bool> bin){
        return strings[bin2int(bin)];
    }

/*
    void initAlphabet(void){
        vector<string> a = {"a","b","c","d","e","f","g","h","i","j","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z"};
        for(int i=0;i<a.size();i++){
            addSymbol(a[i]);
        }
    }
*/

    void initAlphabet(void){
        vector<string> a = {"p","i","a","t","e","h","r","g"};
        for(int i=0;i<a.size();i++){
            addSymbol(a[i]);
        }
    }


};


class Hopfield {

public:
    int N, Nsq;
    vector<int> W, S, Scp;

    Hopfield(int N){
        this->N = N;
        Nsq = N*N;
        W.resize(Nsq,0);
        S.resize(N,0);
        Scp.resize(N,0);
    }

    double getDistanceOfStateFrom(vector<int> T){
        double d = 0;
        for(int i=0;i<N;i++){
            d -= S[i]*T[i];
        }
        return d;
    }


    bool step(void){

        Scp = S;
        int k=0;
        for(int i=0;i<N;i++){
            int s = 0;
            for(int j=0;j<N;j++){
                s += W[k]*Scp[j]; // USING S RATHER THAN Scp HERE MAKES IT ASYNCHRONOUS (TECHNICALLY)
                k++;
            }

            if(s>0){
                S[i] = 1;
            } else {
                S[i] = -1;
            }

        }


        //CHECK POINT ATTRACTOR
        bool repeatedState = true;
        for(int i=0;i<N;i++){
            if(S[i]!=Scp[i]){
                repeatedState = false;
                break;
            }
        }
        return repeatedState;
    }

    void printWeights(void){
        cout<<"W=["<<endl;
        int k=0;
        for(int i=0;i<N;i++){
            cout<<"[";
            for(int j=0;j<N;j++){
                cout<<W[k]<<",";
                k++;
            }
            cout<<"],";
            cout<<endl;
        }
        cout<<"]"<<endl;
    }

    void printState(bool repeatedState){

        for(int i=0;i<N;i++){
            switch(S[i]){
                case(0):{  cout<<"0"; } break;
                case(1):{  cout<<"+"; } break;
                case(-1):{ cout<<"-"; } break;
                default:{  cout<<"?"; } break;
            }
        }
        if(repeatedState){
            cout<<"*  ";
        } else {
            cout<<"   ";
        }
    }


    void printState(bool repeatedState, Alphabet A, int nLetters){

        if(S[0]==1){
            cout<<"# ";
        } else {
            cout<<"  ";
        }
        int k=1;
        for(int i=0;i<nLetters;i++){ // nletters in word
            vector<bool> letter;
            for(int j=0;j<A.Nbits;j++){ // nbits in letter
                letter.push_back(S[k]==1);
                k++;
            }
            cout<<A.getString(letter);
        }

        if(repeatedState){
            cout<<" *  ";
        } else {
            cout<<"    ";
        }
        cout<<endl;

    }


};

int main(int argc, char** argv){

    std::stringstream ss;
    ofstream logfile;
    ss<<argv[1];
    logfile.open(ss.str().c_str(),ios::out|ios::app);

    Alphabet A(4);
    int nLetters = 3;
    int N = nLetters*A.Nbits+1;//16;//stoi(argv[2]); // 3x5-bit letters and 1 transition bit
    int G = 1000000;//1000000;
    double pMutate = stof(argv[3]);
    int samples = stoi(argv[4]);

    int Ncx = 3;

    srand(stoi(argv[5]));
    bool printout = samples<2;

    int Nsq = N*N;
    int tMax = Nsq*2;

    vector<vector<int> > I(Ncx,vector<int>(N,-1)); // initial
    vector<vector<int> > T(Ncx,vector<int>(N,-1)); // target



    A.initAlphabet();
    vector<bool> w1(1,false); // first bit is switching flag
    vector<bool> w2(1,false);
    vector<bool> w3(1,false);
    vector<bool> w4(1,false);
    {
        int nbits = A.Nbits;
        vector<bool> p = A.getBinary("p");
        vector<bool> i = A.getBinary("i");
        vector<bool> a = A.getBinary("a");
        vector<bool> t = A.getBinary("t");
        vector<bool> e = A.getBinary("e");
        vector<bool> h = A.getBinary("h");
        vector<bool> r = A.getBinary("r");
        vector<bool> g = A.getBinary("g");

        for(int k=0;k<nbits;k++){ w1.push_back(p[k]); }  //P
        for(int k=0;k<nbits;k++){ w1.push_back(i[k]); }  //PI
        for(int k=0;k<nbits;k++){ w1.push_back(p[k]); }  //PIP

        for(int k=0;k<nbits;k++){ w2.push_back(a[k]); }  //A
        for(int k=0;k<nbits;k++){ w2.push_back(t[k]); }  //AT
        for(int k=0;k<nbits;k++){ w2.push_back(e[k]); }  //ATE

        for(int k=0;k<nbits;k++){ w3.push_back(h[k]); }  //H
        for(int k=0;k<nbits;k++){ w3.push_back(e[k]); }  //HE
        for(int k=0;k<nbits;k++){ w3.push_back(r[k]); }  //HER

        for(int k=0;k<nbits;k++){ w4.push_back(e[k]); }  //E
        for(int k=0;k<nbits;k++){ w4.push_back(g[k]); }  //EG
        for(int k=0;k<nbits;k++){ w4.push_back(g[k]); }  //EGG

    }
    vector<bool> w1p = w1; w1p[0]=true; // first bit is the switching flag
    vector<bool> w2p = w2; w2p[0]=true;
    vector<bool> w3p = w3; w3p[0]=true;
    vector<bool> w4p = w4; w4p[0]=true;

    I[0] = bools2poles(w1p); // 1_PIP
    T[0] = bools2poles(w2);  // 0_ATE
    I[1] = bools2poles(w2p); // 1_ATE
    T[1] = bools2poles(w3);  // 0_HER
    I[2] = bools2poles(w3p); // 1_HER
    T[2] = bools2poles(w4);  // 0_EGG

    int nojoy = 0;

    for(int q=0;q<samples;q++){

        cout<<"."<<flush;

        Hopfield H(N);

        vector<int> genBetter;
        vector<double> magBetter;

        int Dmin = 1e9;
        vector<bool> solved(Ncx,false);
        bool failed = true;
        for(int g=0;g<G;g++){

            // MUTATE GENOME
            vector<int> Wcp = H.W;
            for(int i=0;i<H.Nsq;i++){
                if(randDouble()<pMutate){
                    if(randDouble()<0.5){
                        H.W[i]++;
                    } else {
                        H.W[i]--;
                    }
                }
            }

            double D = 0;

            for (int c=0;c<Ncx;c++){

                // INTIALIZE CONTEXT
                solved[c] = false;
                for(int i=0;i<H.N;i++){
                    H.S[i] = I[c][i];
                }

                // EVALUATE NETWORK
                for(int t=0;t<tMax;t++){
                    bool repeatedState = H.step();
                    double d = H.getDistanceOfStateFrom(T[c]);
                    if(repeatedState && d==0){
                        solved[c] = true;
                    }
                    if(repeatedState){
                        D += d*(tMax-t);
                        break;
                    } else {
                        D += d;
                    }
                }
            }

            // CORRECT POINT ATTRACTORS FOR ALL CONTEXTS?
            bool solution = true;
            for(int c=0;c<Ncx;c++){
                if(~solved[c]){
                    solution = false;
                }
            }
            if(solution){
                genBetter.push_back(g);
                magBetter.push_back(D);
                logfile<<g<<","<<flush;
                failed = false;
                break;
            }

            if(D<=Dmin){ // BETTER OR SAME
                if(D<Dmin && printout){
                    genBetter.push_back(g);
                    magBetter.push_back(D);
                }
                Dmin = D;
            } else {
                H.W = Wcp;
            }

        }
        if(failed){
            nojoy++ ;
        }

        if(printout){

            cout<<endl;
            H.printWeights();

            cout<<"gen:"<<endl;
            for(int k=0;k<genBetter.size();k++){
                cout<<genBetter[k]<<",";
            } cout<<endl;

            cout<<"mag:"<<endl;
            for(int k=0;k<magBetter.size();k++){
                cout<<magBetter[k]<<",";
            } cout<<endl;

            for(int c=0;c<Ncx; c++){
                for(int i=0;i<H.N;i++){
                    H.S[i] = I[c][i];
                }
                H.printState(false,A,nLetters);
                cout<<endl;
                for(int t=0;t<tMax;t++){
                    bool repeatedState = H.step();
                    H.printState(repeatedState,A,nLetters);
                    cout<<endl;
                    if(repeatedState){
                        break;
                    }
                }
                cout<<endl;
            }
            cout<<"###### RECALL #########"<<endl<<endl;

            H.S = I[0];
            H.printState(false,A,nLetters);
            cout<<endl;
            for(int i=0;i<3;i++){
                for(int t=0;t<tMax;t++){
                    bool repeatedState = H.step();
                    H.printState(repeatedState,A,nLetters);
                    H.S[0] = repeatedState*2-1;
                    cout<<endl;
                    if(repeatedState){
                        //H.S[0]=-H.S[0];
                         break;
                    }
                }
            }

        }


    }
    logfile<<endl;
    logfile<<"No joy count: " << nojoy;
    logfile.close();

    return 0;
};
