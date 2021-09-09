#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <math.h>
#include <morph/Config.h>
#include <morph/HdfData.h>
#include <morph/Random.h>

using namespace std;

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

struct evaluation {
public:
    bool solution;
    double distance;
    int cycleLength;
    int stepsToCycle;
    evaluation(bool solution, double distance, int cycleLength, int stepsToCycle){
        this->solution = solution;
        this->distance = distance;
        this->cycleLength = cycleLength;
        this->stepsToCycle = stepsToCycle;
    }

};

class Alphabet {

public:
    unsigned int bitsPerSymbol;
    int Nassigned;
    vector<string> strings;
    vector<vector<bool> > binaries;

    Alphabet(int bitsPerSymbol){
        Nassigned = 0;
        this->bitsPerSymbol = bitsPerSymbol;
        strings.resize(pow(2,bitsPerSymbol),"?");
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


string printState(vector<int> SS, Alphabet A, int nLetters){
    stringstream ss;
    int k=1;
    for(int i=0;i<nLetters;i++){
        vector<bool> letter;
        for(int j=0;j<A.bitsPerSymbol;j++){
            letter.push_back(SS[k]==1);
            k++;
        }
        ss<<A.getString(letter);
    }
    if(SS[0]==1){
        ss<<"+";
    } else {
        ss<<"-";
    }
    return ss.str();
}

string printState(vector<int> SS, bool repeatedState){
    stringstream ss;
    for(int i=0;i<SS.size();i++){
        switch(SS[i]){
            case(0):{  ss<<"0"; } break;
            case(1):{  ss<<"+"; } break;
            case(-1):{ ss<<"-"; } break;
            default:{  ss<<"?"; } break;
        }
    }
    return ss.str();
}



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

        for(int i=0;i<N;i++){
            if(S[i]!=Scp[i]){
                return false;
            }
        }
        return true;
    }

    double getDistanceOfStateFrom(vector<int> T){
        double d = 0;
        for(int i=0;i<N;i++){
            d -= S[i]*T[i];
        }
        return d;
    }

    evaluation evaluate(vector<int> I, vector<int> T, int steps){

        double distance = 1e9;
        int cycleLength = 0;

        S = I;
        vector<vector<int> > buffer(1,S);

        // step dynamics until cycle
        int t;
        for(t=0;t<steps;t++){

            step();

            // check buffer for repeated states
            int k = buffer.size()-1;
            for(int j=1;j<=buffer.size();j++){
                bool rep = true;
                for(int i=0;i<N;i++){
                    if(S[i]!=buffer[k][i]){
                        rep = false;
                    }
                }
                if(rep){
                    cycleLength = j;
                    break;
                }
                k--;
            }

            // increment buffer
            buffer.push_back(S);

            // calculate the distance per state in limit cycle
            if(cycleLength>0){

                // step dynamics around cycle
                double cycleSumDistance = 0.;
                for(int w=0;w<cycleLength;w++){
                    step();
                    cycleSumDistance += getDistanceOfStateFrom(T);
                }
                distance = cycleSumDistance;
                // distance *= (steps-t); // Reduce tail length too?
                break;
            }
        }

        return evaluation(((cycleLength==1) && getDistanceOfStateFrom(T)==-N),distance,cycleLength,t);
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
        logfile.open(ss.str().c_str(),ios::out|ios::app);//|std::ofstream::trunc);
    }
    morph::RandUniform<double, std::mt19937_64> randDouble(stoi(argv[3]));

    double P = conf.getDouble("p",0.05);
    unsigned int trials = conf.getUInt("trials", 1000000);


    // Read in contexts info

    stringstream s;
    vector<string> AA, BB;

    const Json::Value assocs = conf.getArray ("associations");

    for (unsigned int i = 0; i < assocs.size(); ++i) {
        Json::Value associ = assocs[i];

        Json::Value associA = associ["A"];
        std::string A = associA.asString();
        AA.push_back(A);
        s<<A;

        Json::Value associB = associ["B"];
        std::string B = associB.asString();
        BB.push_back(B);
        s<<B;
    }

    cout<<AA.size()<<endl;

    vector<char> su = getUniqueInString(s.str());

    unsigned int symbolsPerWord = AA[0].size();
    for(int i=1;i<AA.size();i++){
        if((symbolsPerWord != AA[i].size())|| (symbolsPerWord != BB[i].size())){
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

    int numContexts = AA.size();

    vector<vector<int> > I(numContexts,vector<int>(N,-1)); // initial
    vector<vector<int> > T(numContexts,vector<int>(N,-1)); // target

    for(int i=0;i<AA.size();i++){
        vector<bool> word (1,false);
        for(int j=0;j<AA[i].size();j++){
            stringstream q; q<<AA[i][j];
            vector<bool> binaryRep = A.getBinary(q.str());
            for(int k=0;k<A.bitsPerSymbol;k++){ word.push_back(binaryRep[k]); }
            I[i] = bools2poles(word);
            I[i][0] = +1;
        }
    }

    for(int i=0;i<BB.size();i++){
        vector<bool> word (1,false);
        for(int j=0;j<BB[i].size();j++){
            stringstream q; q<<BB[i][j];
            vector<bool> binaryRep = A.getBinary(q.str());
            for(int k=0;k<A.bitsPerSymbol;k++){ word.push_back(binaryRep[k]); }
            T[i] = bools2poles(word);
            T[i][0] = -1;
        }
    }


    Hopfield H(N);

    vector<int> trialInc;
    vector<double> errorInc;
    vector<double> stepsInc;
    vector<double> cyLenInc;
    vector<double> cyDisInc;

    double errorMin = 1e9;
    vector<bool> solved(numContexts,false);

    for(int t=0;t<trials;t++){

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


        // evaluate network from each context and measure
        double error = 0.0;
        double stepsToCycle = 0.0;
        double cycleLength = 0.0;
        double cycleDistance = 0.0;
        bool solution = true;
        for(int c=0;c<numContexts;c++){
            evaluation E = H.evaluate(I[c],T[c],steps);
            error += E.distance / (double) E.cycleLength;
            stepsToCycle += E.stepsToCycle;
            cycleLength += E.cycleLength;
            cycleDistance += E.distance;

            if(!E.solution){
                solution = false;
            }
        }
        error /= (double)numContexts;
        stepsToCycle /= (double)numContexts;
        cycleLength /= (double)numContexts;
        cycleDistance /= (double)numContexts;


        if(solution){
            trialInc.push_back(t);
            errorInc.push_back(error);
            stepsInc.push_back(stepsToCycle);
            cyLenInc.push_back(cycleLength);
            cyDisInc.push_back(cycleDistance);
            break;
        }

        if(error<=errorMin){ // BETTER OR SAME
            if(error<errorMin){
                trialInc.push_back(t);
                errorInc.push_back(error);
                stepsInc.push_back(stepsToCycle);
                cyLenInc.push_back(cycleLength);
                cyDisInc.push_back(cycleDistance);
            }
            errorMin = error;
        } else {
            H.W = Wcp;
        }

    }


    // PRINT OUTPUTS TO FILE
    logfile<<"SEED: "<<stoi(argv[3])<<endl<<endl;


    // PRINT ASSOCIATIONS TO FILE
    for(int c=0;c<numContexts;c++){
        evaluation E = H.evaluate(I[c],T[c],steps);
        logfile<<"ASSOC. "<<c<<" ("<<printState(I[c],A,symbolsPerWord)<<" --> "<<printState(T[c],A,symbolsPerWord)<<"):"<<endl;
        H.S = I[c];
        logfile<<" "<<printState(H.S,A,symbolsPerWord)<<endl;
        for(int t=0;t<E.stepsToCycle;t++){
            H.step();
            logfile<<" "<<printState(H.S,A,symbolsPerWord)<<endl;
        }
        logfile<<"      cycle:"<<endl;
        for(int t=0;t<E.cycleLength;t++){
            H.step();
            logfile<<"        "<<printState(H.S,A,symbolsPerWord)<<endl;
        }
        logfile<<endl;
    }

    // PRINT RECALL TO FILE
    vector<double> finalRecallErr(1,0);
    stringstream rr;
    logfile<<"TRAIN:"<<endl;
    H.S = I[0];
    logfile<<" "<<printState(H.S,A,symbolsPerWord)<<endl;
    rr<<printState(H.S,A,symbolsPerWord)<<" ";
    for(int i=0;i<I.size();i++){
        for(int t=0;t<steps;t++){
            bool repeated = H.step();
            logfile<<" "<<printState(H.S,A,symbolsPerWord);
            if(repeated){
                logfile<<"*"<<endl;
                rr<<printState(H.S,A,symbolsPerWord)<<" ";
                finalRecallErr[0] = H.getDistanceOfStateFrom(T[T.size()-1]);
                H.S[0] = +1;
                break;
            }
            logfile<<endl;
        }
    }
    logfile<<endl<<"RECALL: "<<endl<<" "<<'"'<<rr.str()<<'"'<<endl;


    // STORE DATA
    std::stringstream fname;
    fname << logpath << "/data.h5";
    morph::HdfData data(fname.str());
    std::stringstream ss;

    // SAVE TRAINING METRICS (mean over contexts)
    ss.str(""); ss.clear(); ss<<"/trialInc";
    data.add_contained_vals (ss.str().c_str(), trialInc);
    ss.str(""); ss.clear(); ss<<"/errorInc";
    data.add_contained_vals (ss.str().c_str(), errorInc);
    ss.str(""); ss.clear(); ss<<"/stepsInc";
    data.add_contained_vals (ss.str().c_str(), stepsInc);
    ss.str(""); ss.clear(); ss<<"/cyLenInc";
    data.add_contained_vals (ss.str().c_str(), cyLenInc);
    ss.str(""); ss.clear(); ss<<"/cyDisInc";
    data.add_contained_vals (ss.str().c_str(), cyDisInc);


    // SAVE TESTING METRICS (for each context)
    vector<double> errorFin;
    vector<double> stepsFin;
    vector<double> cyLenFin;
    vector<double> cyDisFin;
    for(int c=0;c<numContexts;c++){
        evaluation E = H.evaluate(I[c],T[c],steps);
        errorFin.push_back(E.distance / (double) E.cycleLength);
        stepsFin.push_back(E.stepsToCycle);
        cyLenFin.push_back(E.cycleLength);
        cyDisFin.push_back(E.distance);
    }
    ss.str(""); ss.clear(); ss<<"/errorFin";
    data.add_contained_vals (ss.str().c_str(), errorFin);
    ss.str(""); ss.clear(); ss<<"/stepsFin";
    data.add_contained_vals (ss.str().c_str(), stepsFin);
    ss.str(""); ss.clear(); ss<<"/cyLenFin";
    data.add_contained_vals (ss.str().c_str(), cyLenFin);
    ss.str(""); ss.clear(); ss<<"/cyDisFin";
    data.add_contained_vals (ss.str().c_str(), cyDisFin);

    // SAVE FINAL WEIGHTS
    ss.str(""); ss.clear(); ss<<"/weights";
    data.add_contained_vals (ss.str().c_str(), H.W);

    ss.str(""); ss.clear(); ss<<"/finalDistance";
    data.add_contained_vals (ss.str().c_str(), finalRecallErr);

    ss.str(""); ss.clear(); ss<<"/sentence: "<<rr.str();
    data.add_contained_vals (ss.str().c_str(), vector<int>(1,0));

    ss.str(""); ss.clear(); ss<<"/N";
    data.add_contained_vals (ss.str().c_str(), vector<int>(1,H.N));


    logfile.close();

    return 0;
};
