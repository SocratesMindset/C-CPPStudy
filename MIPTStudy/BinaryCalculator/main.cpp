#include <iostream>
#include <string>
using namespace std;

unsigned long long binToUll(string s){
    if(s.rfind("0b",0)==0 || s.rfind("0B",0)==0) s = s.substr(2);
    unsigned long long v = 0;
    for(char c: s){
        if(c!='0' && c!='1') return 0;
        v = (v<<1) | (c=='1');
    }
    return v;
}

string ullToBin(unsigned long long v){
    if(v==0) return "0";
    string r;
    while(v){ r.push_back((v&1ULL)?'1':'0'); v>>=1; }
    for(size_t i=0,j=r.size()-1; i<j; ++i,--j) swap(r[i], r[j]);
    return r;
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cout << "enter: <a> <op> <b>  or  ~ <a>  (exit to quit)\n";

    string t1, op, t2;
    while (true){
        cout << "> ";
        if(!(cin >> t1)) break;
        if(t1=="exit") break;

        if(t1=="~"){
            if(!(cin >> t2)) break;
            unsigned long long a = binToUll(t2);
            cout << ullToBin(~a) << "\n";
            continue;
        }

        if(!(cin >> op >> t2)) break;

        unsigned long long a = binToUll(t1);
        unsigned long long b = binToUll(t2);
        unsigned long long r = 0;

        if(op=="+") r = a + b;
        else if(op=="-") r = a - b;
        else if(op=="*") r = a * b;
        else if(op=="/") { if(b==0){ cout<<"err\n"; continue;} r = a / b; }
        else if(op=="%") { if(b==0){ cout<<"err\n"; continue;} r = a % b; }
        else if(op=="&") r = a & b;
        else if(op=="|") r = a | b;
        else if(op=="^") r = a ^ b;
        else if(op=="<<") r = a << b;
        else if(op==">>") r = a >> b;
        else { cout<<"err\n"; continue; }

        cout << ullToBin(r) << "\n";
    }
    return 0;
}

