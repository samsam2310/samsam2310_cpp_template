/** Some C++11 features
*/

#pragma GCC diagnostic error "-std=c++11"
#pragma GCC optimize ("O3")
#include <bits/stdc++.h>
#include <ext/pb_ds/assoc_container.hpp>
#define F(n) FI(i,n)
#define FI(i,n) FL(i,0,n)
#define FL(i,l,n) for(int i=l;i<n;i++)
using namespace std;
using namespace __gnu_pbds;

template<typename T>
bool gin(T &x){
    char c=0;bool flag=0;
    while(c=getchar(),c<'0'&&c!='-'||c>'9')if(c==-1)return false;
    c=='-'?(flag=1,x=0):(x=c-'0');
    while(c=getchar(),c>='0'&&c<='9')x=x*10+c-'0';
    if(flag)x=-x;
    return true;
}
template<typename T, typename ...Args>
bool gin(T &x, Args &...args){
    return gin(x)&&gin(args...);
}

template<long long X>
struct Fib{
    const static long long x = (Fib<X-2>::x+Fib<X-1>::x)%1000000007;
};
template<>
struct Fib<1>{
    const static long long x=1;
};
template<>
struct Fib<0>{
    const static long long x=0;
};

main(){
    cout<<(Fib<900>::x)<<endl;
    gp_hash_table<int,int> map;
    int x,y;
    while(gin(x,y)){
        cout<<x<<' '<<y<<endl;
    }
    auto fun = [](){};
}