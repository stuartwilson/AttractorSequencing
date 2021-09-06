#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <math.h>
#include <morph/Config.h>
#include <morph/HdfData.h>
#include <morph/Random.h>

using namespace std;


//double randDouble(void){
//    return ((double) rand())/(double)RAND_MAX;
//}

vector<string> getSplitSentence(std::string sequence){
    vector<string> seq;
    std::string s = sequence;
    std::string delimiter = " ";
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        seq.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    seq.push_back(s);
    return seq;
}

string removeSpaces(std::string sequence){
    stringstream seq;
    std::string s = sequence;
    std::string delimiter = " ";
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        seq<<token;
        s.erase(0, pos + delimiter.length());
    }
    seq<<s;
    return seq.str();
}

std::vector<char> getUniqueInString(string st, bool removespaces=true){
    string s = st;
    if(removespaces){
        s = removeSpaces(st);
    }

    std::vector<char> unique;
    for(int i=0;i<s.size();i++){
        bool uni = true;
        for(int k=0;k<unique.size();k++){
            if(s[i]==unique[k]){ uni = false; break; }
        } if(uni){
            unique.push_back(s[i]);
        }
    }
    return unique;
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

/*
vector<int> randPermute(int x){
    vector<int> X(x);
    for(int i=0;i<x;i++){
        X[i] = i;
    }
    vector<int> Y(x);
    for(int i=0;i<x;i++){
        int index = floor(randDouble.get()*X.size());
        Y[i] = X[index];
        X.erase (X.begin()+index);
    }
    return Y;
}
*/

class Alphabet {

public:
    unsigned int bitsPerSymbol;
    int Nassigned;
    vector<string> strings;
    vector<vector<bool> > binaries;

    Alphabet(int bitsPerSymbol){
        Nassigned = 0;
        this->bitsPerSymbol = bitsPerSymbol;
        strings.resize(pow(2,bitsPerSymbol),"#");
        for(int i=0;i<pow(2,bitsPerSymbol);i++){
            binaries.push_back(int2bin(i,bitsPerSymbol));
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
                s += W[k]*Scp[j]; // USING S RATHER THAN Scp HERE MAKES UPDATING (FIXED ORDER) ASYNCHRONOUS
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

    string printState(bool repeatedState){

        stringstream ss;
        for(int i=0;i<N;i++){
            switch(S[i]){
                case(0):{  ss<<"0"; } break;
                case(1):{  ss<<"+"; } break;
                case(-1):{ ss<<"-"; } break;
                default:{  ss<<"?"; } break;
            }
        }
        if(repeatedState){
            ss<<"*  ";
        } else {
            ss<<"   ";
        }
        ss<<endl;
        return ss.str();
    }


    string printState(bool repeatedState, Alphabet A, int nLetters){

        stringstream ss;
        if(S[0]==1){
            ss<<"# ";
        } else {
            ss<<"  ";
        }
        int k=1;
        for(int i=0;i<nLetters;i++){ // nletters in word
            vector<bool> letter;
            for(int j=0;j<A.bitsPerSymbol;j++){
                letter.push_back(S[k]==1);
                k++;
            }
            ss<<A.getString(letter);
        }

        if(repeatedState){
            ss<<" *  ";
        } else {
            ss<<"    ";
        }
        ss<<endl;
        return ss.str();
    }

};

int main(int argc, char** argv){

    morph::Config conf(argv[1]);
    std::string logpath = argv[2];
    morph::Tools::createDir (logpath);
    ofstream logfile;
    {
        std::stringstream ss;
        ss<<logpath<<"/log.txt";
        logfile.open(ss.str().c_str(),ios::out|ios::app);
    }
    morph::RandUniform<double, std::mt19937_64> randDouble(stoi(argv[3]));

    double P = conf.getDouble("p",0.05);
    unsigned int G = conf.getUInt("generations", 1000000);
    string s = conf.getString("sequence", "pip ate her egg");
    vector<string> seq = getSplitSentence(s);
    vector<char> su = getUniqueInString(s);

    unsigned int symbolsPerWord = seq[0].size();
    for(int i=1;i<seq.size();i++){
        if(symbolsPerWord != seq[i].size()){
            cout<<"Words can't be of different length";
            return 0;
        }
    }

    int minBits = 0;
    {
        int k=2;
        for(int i=1;i<100;i++){
            if(su.size()<=k){
                minBits = i;
                break;
            }
            k*=2;
        }
    }
    unsigned int bitsPerSymbol = conf.getUInt("bitsPerSymbol", minBits);
    if(bitsPerSymbol<minBits){
        cout<<"More unique symbols than allowable combinations. Increase bitsPerSymbol in config"<<endl;
        return 0;
    }

    Alphabet A(bitsPerSymbol);
    for(int i=0;i<su.size();i++){
        stringstream ss; ss<<su[i];
        A.addSymbol(ss.str());
    }

    int N = symbolsPerWord*bitsPerSymbol+1;

    unsigned int steps = conf.getUInt("steps",N*N*2);

    int numContexts = seq.size()-1;

    vector<vector<int> > I(numContexts,vector<int>(N,-1)); // initial
    vector<vector<int> > T(numContexts,vector<int>(N,-1)); // target

    for(int i=0;i<seq.size();i++){
        vector<bool> word (1,false);
        for(int j=0;j<seq[i].size();j++){
            stringstream q; q<<seq[i][j];
            vector<bool> binaryRep = A.getBinary(q.str());
            for(int k=0;k<A.bitsPerSymbol;k++){ word.push_back(binaryRep[k]); }
        }
        if(i<seq.size()-1){
            I[i] = bools2poles(word);
            I[i][0] = +1;
        }
        if(i>0){
            T[i-1] = bools2poles(word);
            T[i-1][0] = -1;
        }
    }


    Hopfield H(N);

    vector<int> genBetter;
    vector<double> magBetter;

    int Dmin = 1e9;
    vector<bool> solved(numContexts,false);
    bool failed = true;
    for(int g=0;g<G;g++){

        // PERTURB GENOME
        vector<int> Wcp = H.W;
        for(int i=0;i<H.Nsq;i++){
            if(randDouble.get()<P){
                if(randDouble.get()<0.5){
                    H.W[i]++;
                } else {
                    H.W[i]--;
                }
            }
        }

        double D = 0;

        for (int c=0;c<numContexts;c++){

            // INTIALIZE CONTEXT
            solved[c] = false;
            for(int i=0;i<H.N;i++){
                H.S[i] = I[c][i];
            }

            // EVALUATE NETWORK
            for(int t=0;t<steps;t++){
                bool repeatedState = H.step();
                double d = H.getDistanceOfStateFrom(T[c]);
                if(repeatedState && d==0){
                    solved[c] = true;
                }
                if(repeatedState){
                    D += d*(steps-t);
                    break;
                } else {
                    D += d;
                }
            }
        }

        // CORRECT POINT ATTRACTORS FOR ALL CONTEXTS?
        bool solution = true;
        for(int c=0;c<numContexts;c++){
            if(~solved[c]){
                solution = false;
            }
        }
        if(solution){
            genBetter.push_back(g);
            magBetter.push_back(D);
            failed = false;
            break;
        }

        if(D<=Dmin){ // BETTER OR SAME
            if(D<Dmin){
                genBetter.push_back(g);
                magBetter.push_back(D);
            }
            Dmin = D;
        } else {
            H.W = Wcp;
        }

    }

    logfile<<"Associations:"<<endl<<endl;
    for(int c=0;c<numContexts; c++){
        for(int i=0;i<H.N;i++){
            H.S[i] = I[c][i];
        }
        logfile<<H.printState(false,A,symbolsPerWord);
        for(int t=0;t<steps;t++){
            bool repeatedState = H.step();
            logfile<<H.printState(repeatedState,A,symbolsPerWord);
            if(repeatedState){
                break;
            }
        }
        logfile<<endl;
    }
    logfile<<"Recall:"<<endl<<endl;
    H.S = I[0];
    logfile<<H.printState(false,A,symbolsPerWord);
    for(int i=0;i<I.size();i++){
        for(int t=0;t<steps;t++){
            bool repeatedState = H.step();
            logfile<<H.printState(repeatedState,A,symbolsPerWord);
            H.S[0] = repeatedState*2-1;
            if(repeatedState){
                //H.S[0]=-H.S[0];
                break;
            }
        }
    }

    // SAVE VALUES
    std::stringstream fname;
    fname << logpath << "/data.h5";
    morph::HdfData data(fname.str());
    std::stringstream ss;
    ss.str("");
    ss.clear();
    ss<<"/generations";
    data.add_contained_vals (ss.str().c_str(), genBetter);
    ss.str("");
    ss.clear();
    ss<<"/magnitude";
    data.add_contained_vals (ss.str().c_str(), magBetter);
    ss.str("");
    ss.clear();
    ss<<"/weights";
    data.add_contained_vals (ss.str().c_str(), H.W);

    logfile.close();

    return 0;
};
