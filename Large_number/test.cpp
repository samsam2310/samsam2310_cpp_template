#include <bits/stdc++.h>
#include "large_number.h"
#define F(n) Fi(i,n)
#define Fi(i,n) Fl(i,0,n)
#define Fl(i,l,n) for(int i=0;i<n;i++)
using namespace std;
main(){
    ios_base::sync_with_stdio(false);
    cin.tie(0);
    integer a,b;
    a = 1;
    a<<=100000;
    cout<<(a)<<endl;
    while(cin>>a>>b){
        cout<<__gcd(a,b)<<endl;
    }
}