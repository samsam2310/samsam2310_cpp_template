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

// ----------------------------------------------

/** Fast getchar();
  * char c = readchar();
*/
inline int readchar() {
    const int N = 1048576;
    static char buf[N];
    static char *p = buf, *end = buf;
    if(p == end) {
        if((end = buf + fread(buf, 1, N, stdin)) == buf) return EOF;
        p = buf;
    }
    return *p++;
}
/** Fast input int or long long
  * Need readchar();
  * int a,b,c; gin(a,b,c);
*/
template<typename T>
inline bool gin(T &x){
    char c=0;bool flag=0;
    while(c=readchar(),c<'0'&&c!='-'||c>'9')if(c==-1)return false;
    c=='-'?(flag=1,x=0):(x=c-'0');
    while(c=readchar(),c>='0'&&c<='9')x=x*10+c-'0';
    if(flag)x=-x;
    return true;
}
template<typename T, typename ...Args>
inline bool gin(T &x, Args &...args){
    return gin(x)&&gin(args...);
}

/** Unsigned version, NO EOF.
 * Need readchar();
 */
template<typename T>
inline void ugin(T &x){
    char c=0;
    while(c=readchar(),c<'0'||c>'9');
    x=c-'0';
    while(c=readchar(),c>='0'&&c<='9')x=x*10+c-'0';
}
template<typename T, typename ...Args>
inline void ugin(T &x, Args &...args){
    ugin(x);ugin(args...);
}

// ----------------------------------------------

/** Compile Fibonacci
  * cout<<(Fib<i>::x)<<endl; i is a const int;
 */
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

// ----------------------------------------------

/** Magic method for geting 1/sqrt(x);
 */
float InvSqrt(float x)
{
    float xhalf = 0.5f*x;
    int i = *(int*)&x; // get bits for floating VALUE 
    i = 0x5f375a86- (i>>1); // gives initial guess y0
    x = *(float*)&i; // convert bits BACK to float
    x = x*(1.5f-xhalf*x*x); // Newton step, repeating increases accuracy
    return x;
}

// ----------------------------------------------

main(){
    cout<<(Fib<900>::x)<<endl;
    gp_hash_table<int,int> map;
    int x,y;
    while(gin(x,y)){
        cout<<x<<' '<<y<<endl;
    }
    auto fun = [](){};
}