#include <bits/stdc++.h>
#include "large_number.hpp"
#define F(n) Fi(i,n)
#define Fi(i,n) Fl(i,0,n)
#define Fl(i,l,n) for(int i=0;i<n;i++)
using namespace std;
main(){
    ios_base::sync_with_stdio(false);
    cin.tie(0);
    chino::Integer a,b;
    cin>>a>>b;
    // cout<<a.m_digit[0]<<endl;
    cout<<"JIZZ "<<a<<' '<<b<<endl;
    cout<<"Add "<<a+b<<endl;
    cout<<"Sub "<<a-b<<endl;
    cout<<"Mul "<<a*b<<endl;
    cout<<"Div "<<a/b<<endl;
    cout<<"Mod "<<a%b<<endl;
    while(1);
    // a = 1;
    // a<<=100000;
    // cout<<(a)<<endl;
    // while(cin>>a>>b){
        // cout<<__gcd(a,b)<<endl;
    // }
}