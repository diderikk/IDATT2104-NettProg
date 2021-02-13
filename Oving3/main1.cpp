#include <iostream>
#include <thread>
#include <vector>
#include <sstream>

using namespace std;

inline bool isPrime(int val){
    if(val == 1 || val == 0) return false;
    for(int i = 2; i <= val/2; i++){
        if(val%i==0) return false;
    }
    return true;
}

void findPrimes(int d,int start, int end, vector<int> &primes){
    for(int i = start; i<end; i++){
        // printf("%d",i);
        if(isPrime(i)){
            printf("Thread: %d Prime: %d\n",d, i);
            primes.push_back(i);
        }
    }
}

int main(int argc, char *argv[]){
    int n, start, end;
    if(argc < 4){ 
        cout << "Not enough arguments: Amount of threads, start, end" << endl;
        return 0;
    }

    stringstream ss;
    ss << argv[1] << " " << argv[2] << " " << argv[3];
    ss >> n >> start >> end;


    thread * threads = new thread [n];
    vector<int> * primes = new vector<int> [n];

    if(end-start < n) n = end-start; 
    int increment = (end-start)/n;

    for(int i = 0; i<n; i++){
        if(i == n-1) threads[i] = thread(findPrimes, i, start+increment*i, end+1, ref(primes[i]));
        else threads[i] = thread(findPrimes, i, start+increment*i, start+increment*(i+1), ref(primes[i]));
    }

    for(int i = 0; i<n; i++){
        threads[i].join();
    }


    for(int i = 0; i<n; i++){
        for(auto &k : primes[i]) cout << k << endl;
    }
    

    return 0;
}