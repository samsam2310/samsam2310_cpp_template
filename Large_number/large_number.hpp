/** Large Number C++ Template
  * By ZhongSheng-Wu
  * ------------------
  * "integer" is a unlimited integer type,
  * it will auto resize when the number overflow,
  * it can pass into any STL function that input is int or long long.
  * Example:
    int main(){
        integer a,b;
        cin>>a>>b;
        cout<<__gcd(a,b)<<endl;
    }
    TODO:
        boolean operator like | & !;
*/

#ifndef LARGE_NUMBER
#define LARGE_NUMBER

#define ABS(x) ((x + (x >> sizeof(x)*8-1)) ^ (x >> sizeof(x)*8-1))
#define MIN(x,y) ((x)<(y)?(x):(y))
#define MAX(x,y) ((x)>(y)?(x):(y))

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstddef>
#include <cstring>

namespace chino{

template<typename Digit=unsigned long, typename TwoDigit=unsigned long long>
struct BaseInteger
{
    const size_t CHARBIT = 8,
                 SHIFT = sizeof(Digit)*CHARBIT-1;
                 KARATSUBA_CUTOFF = 70;
    const Digit MASK = (1 << SHIFT) - 1;
    Digit *m_digit;
    ssize_t m_size;

    BaseInteger() {
        // m_digit = new Digit[1]();
        // m_size = 1;
    }

    BaseInteger(ssize_t size_n) {
        m_digit = new Digit[size_n]();
        m_size = size_n;
    }

    // Integer(unsigned int __val){
    //     _M_from_ulong(__val);
    // }

    // Integer(std::vector<bool>&&__val)
    // {
    //     _M_v = std::move(__val);
    // }

    // void _M_from_ulong(unsigned int __val) {
    //     _M_v.clear();
    //     if(__val == 0){
    //         _M_v.push_back(0);
    //         return;
    //     }
    //     while(__val){
    //         _M_v.push_back(__val&1);
    //         __val >>= 1;
    //     }
    // }

    void _M_normalize() {
        ssize_t i = ABS(m_size), j = i;
        while(i > 0 && m_digit[i-1] == 0)
            --i;
        if (i != j)
            m_size = (m_size < 0) ? -(i) : i;
    }

    void _M_resize(ssize_t size_n) {
        if(size_n <= sizeof(m_digit)){
            ssize_t i = ABS(m_size);
            for(; i < size_n; ++i) {
                m_digit[i] = 0;
            }
        }else{
            Digit *p = new Digit[size_n];
            memcpy(p, m_digit, ABS(m_size));
            delete[] m_digit;
            m_digit = p;
        }
        m_size = (m_size < 0) ? -(size_n) : size_n;
    }

    // unsigned int _M_to_ulong() const throw(std::overflow_error)
    // {
    //     if(_M_size() > 32){
    //         throw std::overflow_error("_Base_integer::_M_to_ulong");
    //     }
    //     unsigned int __res;
    //     _M_copy_to_bits(__res);
    //     return __res;
    // }

    // template<typename T>
    // void _M_copy_to_bits(T&__res) const
    // {
    //     const size_t __nbit = std::min(8 * sizeof(__res), _M_size());
    //     __res = 0;
    //     for(size_t __i = 0; __i < __nbit; __i++){
    //         __res |= (T)((bool)_M_v[__i]) << __i;
    //     }
    // }

    // void _M_copy_from_string(std::string __s) throw(std::range_error)
    // {
    //     if(__s.empty()) throw(std::range_error("_Base_integer::_M_copy_from_string"));
    //     for(char __c: __s){
    //         if(__c < '0' || __c > '9') throw(std::range_error("_Base_integer::_M_copy_from_string"));
    //     }
    //     auto __p = __s.begin();
    //     _M_v.resize(0);
    //     while(__p != __s.end()){
    //         _M_v.push_back( (__s.back()-'0')&1 );
    //         bool __crr = false, __pre;
    //         for(auto __i = __p; __i != __s.end(); __i++){
    //             __pre = __crr;
    //             __crr = (*__i-'0')&1;
    //             *__i = (*__i-'0'>>1) + (__pre?'5':'0');
    //         }
    //         while(__p != __s.end() && *__p == '0')__p++;
    //     }
    // }

    // void _M_copy_to_string(std::string &__s) const
    // {
    //     if(_M_is_zero()){
    //         __s = "0";
    //         return;
    //     }
    //     const _Base_integer __ten(10);
    //     _Base_integer __q, __r;
    //     __q = *this;
    //     __s.clear();
    //     while(!__q._M_is_zero()){
    //         __q._M_do_div(__ten, __r);
    //         __s.push_back('0' + __r._M_to_ulong());
    //     }
    //     std::reverse(__s.begin(), __s.end());
    // }

    void _M_do_add(const BaseInteger &y) {
        BaseInteger *a = this, *b = &y;
        ssize_t size_a = ABS(a->m_size), size_b = ABS(b->m_size);
        if(ABS(a->m_size) < ABS(b->m_size)) {
            std::swap(a, b);
            std::swap(size_a, size_b);
        }
        _M_resize(size_a+1);
        // Digit carry = 0;
        // ssize_t i = 0;
        // for(; i < size_b; ++i) {
        //     carry += a->m_digit[i] + b->m_digit[i];
        //     m_digit[i] = carry & MASK;
        //     carry >>= SHIFT;
        // }
        // for (; i < size_a; ++i) {
        //     carry += a->m_digit[i];
        //     m_digit[i] = carry & MASK;
        //     carry >>= SHIFT;
        // }
        // m_digit[i] = carry;
        m_digit[i] = _S_real_add(m_digit, a->m_digit, size_a, b->m_digit, size_b);
        _M_normalize();
    }

    // Do abs(a) - abs(b)
    void _M_do_sub(const BaseInteger &y) {
        BaseInteger *a = this, *b = &y;
        ssize_t size_a = ABS(a->m_size), size_b = ABS(b->m_size);
        int sign = 1;
        if(size_a < size_b) {
            std::swap(a, b);
            std::swap(size_a, size_b);
        }else if(size_a == size_b) {
            ssize_t i = size_a+1;
            while(--i>=0 && a->m_digit[i] == b->m_digit[i]) ;
            if(i < 0) {
                m_size = 1;
                m_digit[0] = 0;
                return ;
            }
            if(a->m_digit[i] < b->m_digit[i]) {
                sign = -1;
                std::swap(a, b);
            }
            size_a = size_b = i+1;
        }
        _M_resize(size_a);
        // Digit borrow = 0;
        // ssize_t i = 0;
        // for (; i < size_b; ++i) {
        //     /* The following assumes unsigned arithmetic
        //        works module 2**N for some N>PyLong_SHIFT. */
        //     borrow = a->m_digit[i] - b->m_digit[i] - borrow;
        //     m_digit[i] = borrow & MASK;
        //     borrow >>= SHIFT;
        //     borrow &= 1; /* Keep only one sign bit */
        // }
        // for (; i < size_a; ++i) {
        //     borrow = a->m_digit[i] - borrow;
        //     m_digit[i] = borrow & MASK;
        //     borrow >>= SHIFT;
        //     borrow &= 1; /* Keep only one sign bit */
        // }
        _S_real_sub(m_digit, a->m_digit, size_a, b->m_digit, size_b);
        // assert(borrow == 0);
        if (sign < 0)
            m_size = -(m_size);
        _M_normalize();
    }

    // size_a >= size_b, sizeof(digit_res) >= size_a;
    static Digit _S_real_add(Digit *digit_res, Digit *digit_a, ssize_t size_a, Digit *digit_b, ssize_t size_b) {
        Digit carry = 0;
        ssize_t i = 0;
        for(; i < size_b; ++i) {
            carry += digit_a[i] + digit_b[i] + carry;
            digit_res[i] = carry & MASK;
            carry >>= SHIFT;
        }
        for (; i < size_a; ++i) {
            carry += digit_a[i] + carry;
            digit_res[i] = carry & MASK;
            carry >>= SHIFT;
        }
        return carry;
    }

    // size_a >= size_b, sizeof(digit_res) >= size_a;
    static Digit _S_real_sub(Digit *digit_res, Digit *digit_a, ssize_t size_a, Digit *digit_b, ssize_t size_b) {
        Digit borrow = 0;
        ssize_t i = 0;
        for (; i < size_b; ++i) {
            borrow = digit_a[i] - digit_b[i] - borrow;
            digit_res[i] = borrow & MASK;
            borrow >>= SHIFT;
            borrow &= 1; /* Keep only one sign bit */
        }
        for (; i < size_a; ++i) {
            borrow = digit_a[i] - borrow;
            digit_res[i] = borrow & MASK;
            borrow >>= SHIFT;
            borrow &= 1; /* Keep only one sign bit */
        }
        return borrow;
    }

    static BaseInteger* _S_do_mul(Digit *digit_a, ssize_t size_a, Digit *digit_b, ssize_t size_b) {
        BaseInteger *res = new BaseInteger(size_a + size_b);
        for (ssize_t i = 0; i < size_a; ++i) {
            Twodigits carry = 0;
            Twodigits f = a->m_digit[i];
            Digit *pres = res->m_digit + i;
            Digit *pb = b->m_digit;
            Digit *pbend = b->m_digit + size_b;

            while (pb < pbend) {
                carry += *pres + *pb++ * f;
                *pres++ = (Digit)(carry & MASK);
                carry >>= SHIFT;
                // assert(carry <= MASK);
            }
            if (carry)
                *pz += (digit)(carry & MASK);
            // assert((carry >> SHIFT) == 0);
        }
        res->_M_normalize();
        return res;
    }

    // Karatsuba
    // guarantee that m_size of t1 t2 t3 is positive;
    static BaseInteger* _S_do_kmul(Digit *digit_a, ssize_t size_a, Digit *digit_b, ssize_t size_b) {
        BaseInteger *t1 = NULL, *t2 = NULL, *t3 = NULL, *res;
        ssize_t shift, remshift;
        if(size_a > size_b){
            std::swap(digit_a, digit_b);
            std::swap(size_a, size_b);
        }
        if(size_a <= KARATSUBA_CUTOFF){
            return _S_do_mul(digit_a, size_a, digit_b, size_b);
        }
        if(2*size_a <= size_b)
            return _S_k_lopsided_mul(a, size_a, b, size_b);

        shift = size_b >> 1;
        res = new BaseInteger(size_a + size_b);
        // t1 = ha*hb;
        t1 = _S_do_kmul(digit_a + shift, size_a - shift, digit_b + shift, size_b - shift);
        memcpy(res->m_digit + 2*shift, t1->m_digit, t1->m_size*sizeof(Digit));
        // t2 = la*lb;
        t2 = _S_do_kmul(digit_a, MIN(size_a, shift), digit_b, shift);
        memcpy(res->m_digit, t2->m_digit, t2->m_size*sizeof(Digit));
        // res -= t1 << shift + t2 << shift
        // may underflow but it's ok because of unsigned arithmetic mod;
        // t2 first because cache is fresher
        remshift = ABS(res->m_size) - shift;
        _S_real_sub(res->m_digit + shift, res->m_digit + shift, remshift, t2->m_digit, t2->m_size);
        _S_real_sub(res->m_digit + shift, res->m_digit + shift, remshift, t1->m_digit, t1->m_size);
        // t1 = la+ha, t2=lb+hb, t3 = t1 * t2;
        // size of t1,t2 == size_a + size_b;
        t1->m_size = MAX(shift, size_a - shift) +1;
        t1->m_digit[t1->m_size-1] = _S_real_add(t1->m_digit, digit_a, shift, digit_a + shift, size_a - shift);
        t1->_M_normalize();
        t2->m_size = size_b - shift +1; // shift == sizeb /2;
        t2->m_digit[t2->m_size-1] = _S_real_add(t2->m_digit, digit_b, shift, digit_b + shift, size_b - shift);
        t2->_M_normalize();
        t3 = _S_do_kmul(t1->m_digit, t1->m_size, t2->m_digit, t2->m_size);
        _S_real_add(res->m_digit + shift, res->m_digit + shift, remshift, t3->m_digit, t3->m_size);
        delete t1;
        delete t2;
        delete t3;
        res->_M_normalize();
        return res;
    }

    static BaseInteger* _S_k_lopsided_mul(Digit *digit_a, ssize_t size_a, Digit *digit_b, ssize_t size_b) {
        BaseInteger *res, *product;
        ssize_t size_done = 0;
        // assert(size_a。 > KARATSUBA_CUTOFF);
        // assert(2 * size_a <= size_b);
        res = new BaseInteger(size_a + size_b);
        while (size_b > 0) {
            const ssize_t size_touse = MIN(bsize, asize);
            product = _S_do_kmul(digit_a, size_a, digit_b + size_done, size_touse);
            _S_real_add(res->m_digit + nbdone, res->m_size - nbdone, product->m_digit, product->m_size);
            delete product;
            size_b -= size_touse;
            size_done += size_touse;
        }
        res->_M_normalize();
        return res;
    }

    // Divid by zero is undefined behavior;
    // if this == &__z then this = remainder;
    void _M_do_div(const _Base_integer&__y, _Base_integer&__z) // __z -> remainder
    {
    }

    // 0 = equal, 1 = bigger, -1 = smaller
    // char _M_compare_abs(const _Base_integer&__y) const
    // {
    //     if(_M_size() != __y._M_size()) return _M_size() < __y._M_size()? -1: 1;
    //     for(size_t __i = _M_size() -1; __i+1 > 0; __i--){
    //         if(_M_v[__i] != __y._M_v[__i]) return __y._M_v[__i]? -1: 1;
    //     }
    //     return 0;
    // }

    // void _M_do_left_shift(size_t __shift)
    // {
    //     const size_t __pos = _M_size() + __shift;
    //     _M_v.resize(__pos);
    //     for(size_t __i = __pos-1; __i+1 > __shift; __i--){
    //         _M_v[__i] = _M_v[__i - __shift];
    //     }
    //     std::fill(_M_v.begin(), _M_v.begin()+__shift, false);
    // }

    // void _M_do_right_shift(size_t __shift)
    // {
    //     const size_t __pos = _M_size() - std::min(__shift, _M_size());
    //     for(size_t __i = 0; __i < __pos; __i++){
    //         _M_v[__i] = _M_v[__i + __shift];
    //     }
    //     _M_v.resize(__pos);
    //     if(!_M_size())_M_v.push_back(false);
    // }

    inline bool _M_is_zero() const
    {
        return ABS(m_size) == 1 && !m_digit[0];
    }

    // friend std::ostream& operator<<(std::ostream& ost, _Base_integer& __x) // debug
    // {
    //     for(int i=__x._M_v.size()-1;i>=0;i--){
    //         ost<<(__x._M_v[i]?'1':'0');
    //     }
    //     return ost;
    // }
};

class Integer: private BaseInteger{
private:
    // typedef BaseInteger Base;

    // void _M_copy_from_string(std::string&&__s) throw(std::range_error)
    // {
    //     if(__s.front() == '-'){
    //         _neg = true;
    //         __s = __s.substr(1);
    //     }else{
    //         _neg = false;
    //     }
    //     _Base::_M_copy_from_string(__s);
    // }

    // char _M_compare(const integer&__y) const
    // {
    //     if(_M_is_zero() && __y._M_is_zero())return 0;
    //     if(_neg == __y._neg){
    //         return _M_compare_abs(__y) * (_neg? -1: 1);
    //     }else{
    //         return _neg?-1:1;
    //     }
    // }
public:
    integer():_neg(false), _Base(){ }
    integer(const integer&__oth) = default;
    integer(integer&&__oth):_neg(std::move(__oth._neg)), _Base(std::move(__oth._M_v)){ }
    integer(int __val):_neg(__val < 0), _Base(std::abs(__val)){ }
    integer(std::string __s):_Base()
    {
        _M_copy_from_string(std::move(__s));
    }

    integer& operator=(const integer&__oth) = default;
    integer& operator=(int __val)
    {
        _neg = __val < 0;
        _M_from_ulong(std::abs(__val));
    }
    

    integer& operator+=(const integer&__y)
    {
        if(_neg == __y._neg){
            _M_do_add(__y);
        }else{
            if(_M_compare_abs(__y) < 0)_neg = __y._neg;
            _M_do_sub(__y);
        }
        return *this;
    }

    integer operator-=(const integer&__y){
        if(_neg != __y._neg){
            _M_do_add(__y);
        }else{
            if(_M_compare_abs(__y) < 0)_neg = !_neg;
            _M_do_sub(__y);
        }
        return *this;
    }

    integer operator*=(const integer&__y){
        _neg = _neg != __y._neg;
        _M_do_mul(__y);
        return *this;
    }

    integer operator/=(const integer&__y){
        _neg = _neg != __y._neg;
        integer __tmp;
        _M_do_div(__y, __tmp);
        return *this;
    }

    // 5 % -3 == 2, -5 % 3 == -2
    integer operator%=(const integer&__y){
        _M_do_div(__y, *this);
        return *this;
    }

    integer operator<<=(size_t __pos)
    {
        if(__pos)_M_do_left_shift(__pos);
        return *this;
    }

    integer operator>>=(size_t __pos)
    {
        if(__pos)_M_do_right_shift(__pos);
        return *this;
    }

    bool operator==(const integer&__y) const
    {
        return _M_compare(__y) == 0;
    }
    bool operator!=(const integer&__y) const
    {
        return _M_compare(__y) != 0;
    }
    bool operator<(const integer&__y) const
    {
        return _M_compare(__y) == -1;
    }
    bool operator>(const integer&__y) const
    {
        return _M_compare(__y) == 1;
    }
    bool operator<=(const integer&__y) const
    {
        return _M_compare(__y) != 1;
    }
    bool operator>=(const integer&__y) const
    {
        return _M_compare(__y) != -1;
    }


    explicit operator bool(){
        return !_M_is_zero();
    }
    explicit operator size_t(){
        size_t __res;
        _M_copy_to_bits(__res);
        return __res;
    }
    explicit operator int(){
        int __res;
        _M_copy_to_bits(__res);
        return __res;
    }
    explicit operator long long(){
        long long __res;
        _M_copy_to_bits(__res);
        return __res;
    }
    
    friend std::istream& operator>>(std::istream &ist, integer&__x)
    {
        std::string __s;
        ist>>__s;
        if(__s.empty())return ist;
        try{
            __x._M_copy_from_string(std::move(__s));
        }catch(std::range_error& e){
            ist.setstate(std::istream::failbit);
        }
        return ist;
    }

    friend std::ostream& operator<<(std::ostream &ost, const integer&__x)
    {
        std::string __s;
        __x._M_copy_to_string(__s);
        if(__x._neg && !__x._M_is_zero())ost<<'-';
        ost<<__s;
        return ost;
    }

    // void debug() // debug
    // {
    //     for(int i=_M_v.size()-1;i>=0;i--){
    //         std::cout<<(_M_v[i]?'1':'0');
    //     }
    // }
};

integer operator+(const integer&__x, const integer&__y)
{
    return integer(__x) += __y;
}
integer operator-(const integer&__x, const integer&__y)
{
    return integer(__x) -= __y;
}
integer operator*(const integer&__x, const integer&__y)
{
    return integer(__x) *= __y;
}
integer operator/(const integer&__x, const integer&__y)
{
    return integer(__x) /= __y;
}
integer operator%(const integer&__x, const integer&__y)
{
    return integer(__x) %= __y;
}


}; // namespace end;


#endif