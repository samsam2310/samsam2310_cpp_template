#include <bits/stdc++.h>
using namespace std;

// ---- Start ----
template<typename T, typename Cmp=less<T> >
void quick_sort(T *beg, T *end, const Cmp &cmp = Cmp()){
    if(beg == end || beg+1==end)return;
    T *l = beg;
    for(T*p=beg+1; p!=end; p++){
        if(cmp(*p,*beg))swap(*(++l),*p);
    }
    swap(*beg, *l);
    quick_sort(beg,l,cmp);
    quick_sort(l+1,end,cmp);
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
    srand(time(NULL));
    for(int i=0;i<N;i++){
        A[i] = rand()%N;
    }
    random_shuffle(A,A+N);
    print();
    quick_sort(A,A+N);
    print();
    while(1);
}