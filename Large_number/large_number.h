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

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>


namespace chino{

template<typename T>
struct _bitset
{
    std::vector<T> _M_v;

    _bitset()
    {
        _M_v.push_back(0);
    }
};

struct _Base_integer
{
    std::vector<bool> _M_v;

    _Base_integer()
    {
        _M_v.push_back(0);
    }

    _Base_integer(unsigned int __val){
        _M_from_ulong(__val);
    }

    _Base_integer(std::vector<bool>&&__val)
    {
        _M_v = std::move(__val);
    }


    static void _S_reset_vector(std::vector<bool>&__v)
    {
        while(__v.size() > 1 && !__v.back())__v.pop_back();
    }
    

    void _M_from_ulong(unsigned int __val)
    {
        _M_v.clear();
        if(__val == 0){
            _M_v.push_back(0);
            return;
        }
        while(__val){
            _M_v.push_back(__val&1);
            __val >>= 1;
        }
    }

    unsigned int _M_to_ulong() const throw(std::overflow_error)
    {
        if(_M_size() > 32){
            throw std::overflow_error("_Base_integer::_M_to_ulong");
        }
        unsigned int __res;
        _M_copy_to_bits(__res);
        return __res;
    }

    template<typename T>
    void _M_copy_to_bits(T&__res) const
    {
        const size_t __nbit = std::min(8 * sizeof(__res), _M_size());
        __res = 0;
        for(size_t __i = 0; __i < __nbit; __i++){
            __res |= (T)((bool)_M_v[__i]) << __i;
        }
    }

    void _M_copy_from_string(std::string __s) throw(std::range_error)
    {
        if(__s.empty()) throw(std::range_error("_Base_integer::_M_copy_from_string"));
        for(char __c: __s){
            if(__c < '0' || __c > '9') throw(std::range_error("_Base_integer::_M_copy_from_string"));
        }
        auto __p = __s.begin();
        _M_v.resize(0);
        while(__p != __s.end()){
            _M_v.push_back( (__s.back()-'0')&1 );
            bool __crr = false, __pre;
            for(auto __i = __p; __i != __s.end(); __i++){
                __pre = __crr;
                __crr = (*__i-'0')&1;
                *__i = (*__i-'0'>>1) + (__pre?'5':'0');
            }
            while(__p != __s.end() && *__p == '0')__p++;
        }
    }

    void _M_copy_to_string(std::string &__s) const
    {
        if(_M_is_zero()){
            __s = "0";
            return;
        }
        const _Base_integer __ten(10);
        _Base_integer __q, __r;
        __q = *this;
        __s.clear();
        while(!__q._M_is_zero()){
            __q._M_do_div(__ten, __r);
            __s.push_back('0' + __r._M_to_ulong());
        }
        std::reverse(__s.begin(), __s.end());
    }

    void _M_do_add(const _Base_integer&__y)
    {
        const size_t __nbit = std::min(_M_size(), __y._M_size());
        std::vector<bool> __res;
        __res.resize( std::max(_M_size(), __y._M_size()) );
        bool __crr = false;
        for(size_t __i = 0; __i < __nbit; __i++){
            __res[__i] = __crr ^ _M_v[__i] ^ __y._M_v[__i];
            __crr = _M_v[__i] && __y._M_v[__i] || __crr && (_M_v[__i] || __y._M_v[__i]);
        }
        if(_M_size() < __y._M_size()){
            for(size_t __i = _M_size(); __i < __y._M_size(); __i++){
                __res[__i] = __crr ^ __y._M_v[__i];
                __crr &= __y._M_v[__i];
            }
        }else if(_M_size() > __y._M_size()){
            for(size_t __i = __y._M_size(); __i < _M_size(); __i++){
                __res[__i] = __crr ^ _M_v[__i];
                __crr &= _M_v[__i];
            }
        }
        if(__crr)__res.push_back(true);
        _M_v = std::move(__res);
    }

    void _M_do_sub(const _Base_integer&__y) // if a < b then return b-a;
    {
        const size_t __nbit = std::min(_M_size(), __y._M_size());
        std::vector<bool> __res;
        __res.resize( std::max(_M_size(), __y._M_size()) );
        bool __crr = false;
        for(size_t __i = 0; __i < __nbit; __i++){
            __res[__i] = __crr ^ _M_v[__i] ^ __y._M_v[__i];
            __crr = __crr && __y._M_v[__i] || !_M_v[__i] && (__crr || __y._M_v[__i]);
        }
        if(_M_size() < __y._M_size()){
            for(size_t __i = _M_size(); __i < __y._M_size(); __i++){
                __res[__i] = __crr ^ __y._M_v[__i];
                __crr |= __y._M_v[__i];
            }
        }else if(_M_size() > __y._M_size()){
            for(size_t __i = __y._M_size(); __i < _M_size(); __i++){
                __res[__i] = __crr ^ _M_v[__i];
                __crr &= !_M_v[__i];
            }
        }
        // if small minus big
        if(__crr){
            __crr = false;
            for(size_t __i = 0; __i < __res.size(); __i++){
                bool __tmp = __res[__i];
                __res[__i] = __crr ^ __tmp;
                __crr |= __tmp;
            }
        }
        _S_reset_vector(__res);
        _M_v = std::move(__res);
    }

    void _M_do_mul(const _Base_integer&__y)
    {
        std::vector<bool> __res;
        __res.resize(_M_size() + __y._M_size() -1);
        bool __crr = false, __pre;
        for(size_t __i = 0; __i < _M_size(); __i++){
            if(!_M_v[__i])continue;
            for(size_t __j = 0; __j < __y._M_size(); __j++){
                __pre = __crr;
                __crr = __res[__i+__j] && __y._M_v[__j] || __crr && (__res[__i+__j] || __y._M_v[__j]);
                __res[__i+__j] = __pre ^ __res[__i+__j] ^ __y._M_v[__j];
            }
            if(__crr && __i < _M_size()-1){
                __res[__i+__y._M_size()] = true;
                __crr = false;
            }
        }
        if(__crr)__res.push_back(true);
        _S_reset_vector(__res);
        _M_v = std::move(__res);
    }

    // Divid by zero is undefined behavior;
    // if this == &__z then this = remainder;
    void _M_do_div(const _Base_integer&__y, _Base_integer&__z) // __z -> remainder
    {
        if(_M_size() < __y._M_size()){
            if(this != &__z){
                __z = *this;
                _M_v.clear();
                _M_v.push_back(0);
            }
            return;
        }
        const size_t __nbit = _M_size() - __y._M_size() + 1;
        std::vector<bool> __res, __rem;
        __res.resize(__nbit);
        __rem = _M_v;
        bool __crr = false, __pre;
        // __i is unsigned, use __i+1 > 0; instead of __i >=0; 
        for(size_t __i = __nbit-1; __i+1 > 0; __i--){
            if(!__crr){
                size_t __p = __y._M_size()-1;
                while(__p+1 > 0 && __rem[__p+__i] == __y._M_v[__p])__p--;
                if(__p+1 > 0 && __y._M_v[__p]){
                    if(__i){
                        __crr = __rem[__i+__y._M_size()-1];
                        __rem[__i+__y._M_size()-1] = false;
                    }
                    continue;
                }
            }
            __res[__i] = true;
            __crr = false;
            for(size_t __j = 0; __j < __y._M_size(); __j++){
                __pre = __crr;
                __crr = __crr && __y._M_v[__j] || !__rem[__i+__j] && (__crr || __y._M_v[__j]);
                __rem[__i+__j] = __pre ^ __rem[__i+__j] ^ __y._M_v[__j];
            }
            if(__i){
                __crr = __rem[__i+__y._M_size()-1];
                __rem[__i+__y._M_size()-1] = false;
            }
        }
        if(this != &__z){
            _S_reset_vector(__res);
            _M_v = std::move(__res);
        }
        _S_reset_vector(__rem);
        __z._M_v = std::move(__rem);
    }

    // 0 = equal, 1 = bigger, -1 = smaller
    char _M_compare_abs(const _Base_integer&__y) const
    {
        if(_M_size() != __y._M_size()) return _M_size() < __y._M_size()? -1: 1;
        for(size_t __i = _M_size() -1; __i+1 > 0; __i--){
            if(_M_v[__i] != __y._M_v[__i]) return __y._M_v[__i]? -1: 1;
        }
        return 0;
    }

    void _M_do_left_shift(size_t __shift)
    {
        const size_t __pos = _M_size() + __shift;
        _M_v.resize(__pos);
        for(size_t __i = __pos-1; __i+1 > __shift; __i--){
            _M_v[__i] = _M_v[__i - __shift];
        }
        std::fill(_M_v.begin(), _M_v.begin()+__shift, false);
    }

    void _M_do_right_shift(size_t __shift)
    {
        const size_t __pos = _M_size() - std::min(__shift, _M_size());
        for(size_t __i = 0; __i < __pos; __i++){
            _M_v[__i] = _M_v[__i + __shift];
        }
        _M_v.resize(__pos);
        if(!_M_size())_M_v.push_back(false);
    }

    inline size_t _M_size() const
    {
        return _M_v.size();
    }

    inline bool _M_is_zero() const
    {
        return _M_size() == 1 && !_M_v[0];
    }

    friend std::ostream& operator<<(std::ostream& ost, _Base_integer& __x) // debug
    {
        for(int i=__x._M_v.size()-1;i>=0;i--){
            ost<<(__x._M_v[i]?'1':'0');
        }
        return ost;
    }
};

class integer: private _Base_integer{
private:
    bool _neg;
    typedef _Base_integer _Base;

    void _M_copy_from_string(std::string&&__s) throw(std::range_error)
    {
        if(__s.front() == '-'){
            _neg = true;
            __s = __s.substr(1);
        }else{
            _neg = false;
        }
        _Base::_M_copy_from_string(__s);
    }

    char _M_compare(const integer&__y) const
    {
        if(_M_is_zero() && __y._M_is_zero())return 0;
        if(_neg == __y._neg){
            return _M_compare_abs(__y) * (_neg? -1: 1);
        }else{
            return _neg?-1:1;
        }
    }
public:
    integer():_neg(false), _Base(){ }
    integer(const integer&__oth) = default;
    integer(integer&&__oth):_neg(std::move(__oth._neg)), _Base(std::move(__oth._M_v)){ }
    integer(int __val):_neg(__val < 0), _Base(std::abs(__val)){ }

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

// namespace end;
};