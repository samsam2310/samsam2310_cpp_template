#include <bits/stdc++.h>
#include "large_number.hpp"
#define F(n) Fi(i,n)
#define Fi(i,n) Fl(i,0,n)
#define Fl(i,l,n) for(int i=0;i<n;i++)
using namespace std;
main(){
    ios_base::sync_with_stdio(false);
    cin.tie(0);
    chino::Integer a("446464846814846415478967845641453156485674856418564684354684864531684864564864864564156454564685747486786453153215345156489674687841534874684534153486415153438778964351"),
                   b = a,
                   c("12348546445645645");
    // cout<<((a*c)/c == b?"YES":"NO")<<endl;
    // cout<<(chino::Integer(5)/chino::Integer("21313213213212311213"))<<endl;
    cout<<c<<endl;
    cout<<"A B "<<a<<' '<<b<<endl;
    cout<<"Add "<<a+b<<endl;
    cout<<"Sub "<<a-b<<endl;
    cout<<"Div "<<a/b<<endl;
    cout<<"Mod "<<a%b<<endl;
    cout<<"And "<<(a&b)<<endl;
    a = 1;
    a<<=100000;
    cout<<"2^100000: "<<(a)<<endl;
    while(cin>>a>>b){
        cout<<__gcd(a,b)<<endl;
    }
}