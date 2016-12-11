#include <bits/stdc++.h>
using namespace std;

// ---- Start ----
template<typename T, typename Cmp=less<T> >
void quick_sort(T *beg, T *end, const Cmp &cmp = Cmp()){
    if(beg == end || beg+1==end)return;
    T mid = *beg;
    T *l = beg+1, *r = end-1;
    while(l != r+1){
        if(cmp(mid, *r))r--;
        else if(cmp(*l, mid))l++;
        else swap(*l,*r);
    }
    swap(*beg, *r);
    quick_sort(beg,r,cmp);
    quick_sort(r+1,end,cmp);
}
// ---- End ----


// ---- Demo ----
const int N = 10;
int A[N];
void print(){
    cout<<"A: ";
    for(int i=0;i<N;i++){
        cout<<A[i]<<' ';
    }
    cout<<endl;
}

int main(){
    for(int i=0;i<N;i++){
        A[i] = i;
    }
    random_shuffle(A,A+N);
    print();
    quick_sort(A,A+N);
    print();
    while(1);
}