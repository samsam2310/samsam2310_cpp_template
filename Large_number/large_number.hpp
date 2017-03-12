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

#define ABS(x) ((x)<0?-(x):(x))
#define MIN(x,y) ((x)<(y)?(x):(y))
#define MAX(x,y) ((x)>(y)?(x):(y))
#define ISDIGIT(x) (x>='0'&&x<='9')

#include <iostream>
#include <string>
#include <iomanip>
#include <cstddef>
#include <cstring>

namespace chino{

struct BaseInteger {
    typedef unsigned long Digit;
    typedef unsigned long long TwoDigit;
    static const size_t CHARBIT = 8,
                 SHIFT = sizeof(Digit)*CHARBIT-1,
                 DECIMAL_BASE = 1e10,
                 DECIMAL_SHIFT = 10,
                 KARATSUBA_CUTOFF = 70;
    static const Digit MASK = ((Digit)1 << SHIFT) - 1U;
    Digit *m_digit;
    ssize_t m_size;

    BaseInteger():m_digit(NULL), m_size(0) { }

    BaseInteger(const BaseInteger &oth) {
        // std::cerr<<"copy "<<oth.m_size<<"\n";
        m_size = oth.m_size;
        ssize_t size_a = ABS(m_size);
        m_digit = new Digit[size_a];
        memcpy(m_digit, oth.m_digit, sizeof(Digit) * size_a);
    }
    
    BaseInteger(ssize_t size_n) {
        m_digit = new Digit[size_n]();
        m_size = size_n;
    }

    ~BaseInteger() {
        delete[] m_digit;
    }

    BaseInteger& operator=(const BaseInteger &oth) {
        BaseInteger tmp(oth);
        _M_swap(tmp);
    }

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
        i = _S_noralize_digit(m_digit, i);
        if (i == 0)
            m_size = 1;
        else if (i != j)
            m_size = (m_size < 0) ? -(i) : i;
    }

    static ssize_t _S_noralize_digit(const Digit *digit_n, ssize_t size_n) {
        while(size_n > 0 && digit_n[size_n-1] == 0)
            --size_n;
        return size_n;
    }

    void _M_resize(ssize_t size_n) {
        if(m_digit != NULL && size_n <= sizeof(m_digit)){
            ssize_t i = ABS(m_size);
            for(; i < size_n; ++i) {
                m_digit[i] = 0;
            }
        }else{
            Digit *p = new Digit[size_n]();
            if(m_digit != NULL){
                memcpy(p, m_digit, ABS(m_size));
                delete[] m_digit;
            }
            m_digit = p;
        }
        m_size = (m_size < 0) ? -(size_n) : size_n;
    }

    void _M_swap(BaseInteger &y) {
        std::swap(m_digit, y.m_digit);
        std::swap(m_size, y.m_size);
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

    BaseInteger* _M_copy_to_decimal() const {
        // digits = 1 + floor(log2(a) / log2(_PyLong_DECIMAL_BASE))
        const ssize_t size_a = ABS(m_size), size_n = 1 + size_a * SHIFT / (3 * DECIMAL_SHIFT);
        BaseInteger *res = new BaseInteger(size_n);
        Digit *pin = m_digit,
              *pout = res->m_digit;
        ssize_t size = 0;
        for (ssize_t i = size_a; --i >= 0; ) {
            Digit hi = pin[i];
            for (ssize_t j = 0; j < size; j++) {
                TwoDigit z = (TwoDigit)pout[j] << SHIFT | hi;
                hi = (Digit)(z / DECIMAL_BASE); // pout[j] < DECIMAL_BASE => hi < 2^SHIFT
                pout[j] = (Digit)(z - (TwoDigit)hi * DECIMAL_BASE);
            }
            while (hi) {
                pout[size++] = hi % DECIMAL_BASE;
                hi /= DECIMAL_BASE;
            }
        }
        res->_M_normalize();
        if( m_size < 0) res->m_size = -res->m_size;
        return res;
    }

    void _M_copy_from_decimal_string(const char *str, const ssize_t size_s) {
        // std::cout<<"copy from dec \n"<<str<<'\n';
        // digits = 1 + floor(log10(a) / log10(BASE))
        // log10(BASE) = log10(2) * SHIFT > 0.3010*SHIFT
        const ssize_t size_n = 1 + size_s*1000 / (301 * SHIFT);
        // std::cout<<"size_n "<<size_n<<'\n';
        _M_resize(size_n);
        Digit *pout = m_digit;
        const char *pc = str, *pcend = pc+((size_s-1) % (DECIMAL_SHIFT) + 1);
        // std::cout<<"GG "<<pc<<' '<<pcend<<'\n';
        ssize_t size = 0;
        do {
            // std::cout<<"while \n";
            Digit hi = 0;
            while (pc < pcend)
                hi = hi*10 + (*pc++)-'0';
            // std::cout<<"Hi "<<hi<<' '<<(hi&MASK)<<'\n';
            pcend += DECIMAL_SHIFT;
            for (ssize_t j = 0; j < size; j++) {
                TwoDigit z = (TwoDigit)pout[j] * DECIMAL_BASE + hi;
                // std::cout<<"z "<<z<<'\n';
                hi = (Digit)(z >> SHIFT); // pout[j] < 2^SHIFT => hi < DECIMAL_BASE
                // std::cout<<"hi "<<hi<<'\n';
                pout[j] = (Digit)(z - ((TwoDigit)hi << SHIFT));
                // std::cout<<"pout "<<(z-(TwoDigit)hi << SHIFT)<<' '<<pout[j]<<'\n';
            }
            while (hi) {
                pout[size++] = hi & MASK;
                hi >>= SHIFT ;
            }
            // std::cout<<"j: ";
            // for (ssize_t j = 0; j < size; j++) std::cout<<pout[j]<<' '; std::cout<<'\n';
        }while ( pcend <= str + size_s );
        _M_normalize();
    }

    void _M_do_add(const BaseInteger &x,const BaseInteger &y) {
        const BaseInteger *a = &x, *b = &y;
        ssize_t size_a = ABS(a->m_size), size_b = ABS(b->m_size);
        if(ABS(a->m_size) < ABS(b->m_size)) {
            std::swap(a, b);
            std::swap(size_a, size_b);
        }
        _M_resize(size_a+1);
        // m_digit[i] = carry;
        m_digit[size_a] = _S_real_add(m_digit, a->m_digit, size_a, b->m_digit, size_b);
        _M_normalize();
    }

    // Do abs(a) - abs(b)
    void _M_do_sub(const BaseInteger &x, const BaseInteger &y) {
        const BaseInteger *a = &x, *b = &y;
        ssize_t size_a = ABS(a->m_size), size_b = ABS(b->m_size);
        int sign = 1;
        if(size_a < size_b) {
            sign = -1;
            std::swap(a, b);
            std::swap(size_a, size_b);
        }else if(size_a == size_b) {
            ssize_t i = size_a;
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
        _S_real_sub(m_digit, a->m_digit, size_a, b->m_digit, size_b);
        // assert(borrow == 0);
        if (sign < 0)
            m_size = -(m_size);
        _M_normalize();
    }

    // size_a >= size_b, sizeof(digit_res) >= size_a;
    static Digit _S_real_add(Digit *digit_res, const Digit *digit_a, ssize_t size_a, const Digit *digit_b, ssize_t size_b) {
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
    static Digit _S_real_sub(Digit *digit_res, const Digit *digit_a, ssize_t size_a, const Digit *digit_b, ssize_t size_b) {
        Digit borrow = 0;
        ssize_t i = 0;
        for (; i < size_b; ++i) {
            /* The following assumes unsigned arithmetic
               works module 2**N for some N>PyLong_SHIFT. */
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

    static BaseInteger* _S_do_mul(const Digit *digit_a, ssize_t size_a, const Digit *digit_b, ssize_t size_b) {
        size_a = _S_noralize_digit(digit_a, size_a);
        size_b = _S_noralize_digit(digit_b, size_b);
        BaseInteger *res = new BaseInteger(size_a + size_b);
        for (ssize_t i = 0; i < size_a; ++i) {
            TwoDigit carry = 0;
            TwoDigit f = digit_a[i];
            Digit *pres = res->m_digit + i;
            const Digit *pb = digit_b;
            const Digit *pbend = digit_b + size_b;

            while (pb < pbend) {
                carry += *pres + *pb++ * f;
                *pres++ = (Digit)(carry & MASK);
                carry >>= SHIFT;
                // assert(carry <= MASK);
            }
            if (carry)
                *pres += (Digit)(carry & MASK);
            // assert((carry >> SHIFT) == 0);
        }
        res->_M_normalize();
        return res;
    }

    // Karatsuba
    // guarantee that m_size of t1 t2 t3 is positive;
    static BaseInteger* _S_do_kmul(const Digit *digit_a, ssize_t size_a, const Digit *digit_b, ssize_t size_b) {
        size_a = _S_noralize_digit(digit_a, size_a);
        size_b = _S_noralize_digit(digit_b, size_b);
        BaseInteger *t1 = NULL, *t2 = NULL, *t3 = NULL, *res;
        ssize_t shift, remshift;
        if(size_a > size_b){
            std::swap(digit_a, digit_b);
            std::swap(size_a, size_b);
        }
        if(size_a == 0)
            return new BaseInteger(1);
        if(size_a <= KARATSUBA_CUTOFF)
            return _S_do_mul(digit_a, size_a, digit_b, size_b);
        if(2*size_a <= size_b)
            return _S_k_lopsided_mul(digit_a, size_a, digit_b, size_b);

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

    static BaseInteger* _S_k_lopsided_mul(const Digit *digit_a, ssize_t size_a, const Digit *digit_b, ssize_t size_b) {
        size_a = _S_noralize_digit(digit_a, size_a);
        size_b = _S_noralize_digit(digit_b, size_b);
        BaseInteger *res, *product;
        ssize_t size_done = 0;
        // assert(size_a。 > KARATSUBA_CUTOFF);
        // assert(2 * size_a <= size_b);
        res = new BaseInteger(size_a + size_b);
        while (size_b > 0) {
            const ssize_t size_touse = MIN(size_a, size_b);
            product = _S_do_kmul(digit_a, size_a, digit_b + size_done, size_touse);
            _S_real_add(res->m_digit + size_done, res->m_digit + size_done, res->m_size - size_done, product->m_digit, product->m_size);
            delete product;
            size_b -= size_touse;
            size_done += size_touse;
        }
        res->_M_normalize();
        return res;
    }

    // Divid by zero is undefined behavior;
    void _M_do_div(const BaseInteger &y, BaseInteger *&quo, BaseInteger *&rem) const {
        BaseInteger *inv, *inv_nex, *squ, *tmp;
        const ssize_t size_inv = 2*ABS(m_size) + 1, // Precision
                      point = size_inv - 1,
                      shift = ABS(y.m_size) - 1;
        inv = new BaseInteger(size_inv);
        inv_nex = new BaseInteger(size_inv);
        memcpy(inv_nex->m_digit + point - ABS(y.m_size), y.m_digit, sizeof(Digit)*ABS(y.m_size));
        std::cerr<<"Init\n";
        ssize_t i = size_inv;
        while(i) {
            // std::cerr<<" i: "<<i<<"\n";
            memcpy(inv->m_digit, inv_nex->m_digit, sizeof(Digit) * size_inv);
            inv_nex->_M_do_left_shift(1);
            // inv_nex->debug("inv_nex");
            squ = _S_do_kmul(inv->m_digit, size_inv, inv->m_digit, size_inv);
            tmp = _S_do_kmul(squ->m_digit, ABS(squ->m_size), y.m_digit, ABS(y.m_size));
            delete squ;
            // tmp->debug("tmp");
            _S_real_sub(inv_nex->m_digit, inv_nex->m_digit, size_inv, tmp->m_digit + point + shift, ABS(tmp->m_size) - point - shift);
            delete tmp;
            while(i && inv_nex->m_digit[i-1] == inv->m_digit[i-1])
                --i;
            // inv->debug("inv");
            // std::cin.ignore(1);
        }
        delete inv;
        std::cerr<<"Find inv\n";
        quo = _S_do_kmul(inv_nex->m_digit, size_inv, m_digit, m_size);
        delete inv_nex;
        ssize_t size_quo_shift = ABS(quo->m_size)-shift-point;
        for(ssize_t j=0; j < size_quo_shift; j++)
            quo->m_digit[j] = quo->m_digit[j+shift+point];
        quo->_M_resize(MAX(size_quo_shift, 1));
        std::cerr<<"check quo\n";
        // Check quo and calculate rem
        tmp = _S_do_kmul(quo->m_digit, ABS(quo->m_size), y.m_digit, ABS(y.m_size));
        std::cerr<<"rem sub\n";
        rem = new BaseInteger();
        rem->_M_do_sub(*this, *tmp);
        std::cerr<<"rem sub done"<<rem->m_size<<"\n";
        delete tmp;
        if( rem->m_size < 0 ) { // rem < 0
            // std::cerr<<"rem too small\n";
            rem->_M_do_add(*rem, y);
            Digit one = 1;
            _S_real_sub(quo->m_digit, quo->m_digit, ABS(quo->m_size), &one, 1);
        }else if( rem->_M_compare(y) ==  1) { // rem > divisor
            std::cerr<<"rem too big\n";
            std::cerr<<quo->m_digit[0]<<'\n';            
            rem->_M_do_sub(*rem, y);
            Digit one = 1;
            quo->_M_resize(ABS(quo->m_size)+1);
            _S_real_add(quo->m_digit, quo->m_digit, ABS(quo->m_size), &one, 1);
        }
        // std::cerr<<"div done\n";
    }

    // 0 = equal, 1 = bigger, -1 = smaller
    char _M_compare(const BaseInteger &y) const {
        if(m_size == y.m_size) {
            ssize_t i = m_size;
            while(--i>=0 && m_digit[i] == y.m_digit[i]) ;
            if(i < 0)
                return 0;
            return (m_digit[i] < y.m_digit[i])^(m_size < 0) ? -1 : 1;
        }
        return m_size < y.m_size ? -1 : 1;
    }

    // shift < SHIFT
    void _M_do_left_shift(ssize_t shift) {
        _M_resize(ABS(m_size) + 1);
        Digit carry = 0, *p = m_digit, *pend = m_digit + ABS(m_size);
        TwoDigit acc;
        for(; p != pend ; ++p) {
            acc = (TwoDigit)*p << shift | carry;
            *p = (Digit)acc & MASK;
            carry = (Digit)(acc >> SHIFT);
        }
        _M_normalize();
    }

    // shift < SHIFT
    void _M_do_right_shift(ssize_t shift) {
        const Digit mask = ((Digit)1 << shift) - 1U;
        Digit carry = 0, *p = m_digit + ABS(m_size) - 1, *pend = m_digit - 1;
        TwoDigit acc;
        for(; p != pend; --p) {
            TwoDigit acc = (TwoDigit)carry << SHIFT | *p;
            carry = (Digit)acc & mask;
            *p = (Digit)(acc >> shift);
        }
        _M_normalize();
    }

    bool _M_is_zero() const {
        return ABS(m_size) == 1 && !m_digit[0];
    }

    void debug(const char *str) {
        std::cerr<<"Debug : "<<str<<" :";
        ssize_t size_a = ABS(m_size);
        for(ssize_t i = 0; i<size_a;++i)
            std::cerr<<m_digit[i]<<' ';
        std::cerr<<'\n';
    }
};

class Integer: public BaseInteger {
private:
    ssize_t _M_size() const {
        return m_size;
    }

public:
    Integer():BaseInteger() { }
    Integer(const Integer& oth):BaseInteger(oth) { }
    Integer(Integer &&oth) {
        m_digit = NULL;
        m_size = 0;
        _M_swap(oth);
    }
    // Integer(int __val):_neg(__val < 0), _Base(std::abs(__val)){ }
    // Integer(std::string __s):_Base()
    // {
    //     _M_copy_from_string(std::move(__s));
    // }

    Integer& operator=(const Integer&__oth) = default;
    Integer& operator=(Integer &&oth) {
        Integer tmp(oth);
        _M_swap(tmp);
    }
    // Integer& operator=(int __val) {
    //     _neg = __val < 0;
    //     _M_from_ulong(std::abs(__val));
    // }
    

    Integer& operator+=(const Integer &y) {
        if(_M_size() == y._M_size())
            _M_do_add(*this, y);
        else
            _M_do_sub(*this, y);
        return *this;
    }

    Integer& operator-=(const Integer &y) {
        if(_M_size() != y._M_size())
            _M_do_add(*this, y);
        else
            _M_do_sub(*this, y);
        return *this;
    }

    Integer& operator*=(const Integer &y) {
        Integer *tmp = (Integer*)_S_do_kmul(m_digit, ABS(m_size), y.m_digit, ABS(y.m_size));
        tmp->m_size = ((_M_size() < 0) != (y._M_size() < 0))? -tmp->m_size : tmp->m_size;
        _M_swap(*tmp);
        delete tmp;
        return *this;
    }

    Integer& operator/=(const Integer &y) {
        BaseInteger *quo = NULL, *rem = NULL;
        std::cerr<<"div\n";
        _M_do_div(y, quo, rem);
        quo->m_size = ((_M_size() < 0) != (y._M_size() < 0))? -quo->m_size : quo->m_size;
        _M_swap(*quo);
        delete quo;
        delete rem;
        std::cerr<<"div op done\n";
        return *this;
    }

    // 5 % -3 == 2, -5 % 3 == -2, -5 % -3 == -2
    Integer operator%=(const Integer &y){
        BaseInteger *quo, *rem;
        _M_do_div(y, quo, rem);
        rem->m_size = (_M_size() < 0)? -rem->m_size : rem->m_size;
        _M_swap(*rem);
        delete quo;
        delete rem;
        return *this;
    }

    Integer& operator<<=(ssize_t shift) {
        const ssize_t dshift = shift / SHIFT;
        _M_resize(ABS(m_size)+dshift+1);
        for(ssize_t i = ABS(m_size) - 1; i >= dshift; --i)
            m_digit[i] = m_digit[i - dshift];
        memset(m_digit, 0, sizeof(Digit) * dshift);
        _M_do_left_shift(shift % SHIFT);
        return *this;
    }

    Integer& operator>>=(ssize_t shift) {
        const ssize_t dshift = shift / SHIFT, size_a = ABS(m_size);
        if( dshift >= size_a ) {
            m_size = 1;
            m_digit[0] = 0;
            return *this;
        }
        for(ssize_t i = 0; i < size_a - dshift; --i)
            m_digit[i] = m_digit[i + dshift];
        _M_resize(size_a-dshift);
        _M_do_right_shift(shift % SHIFT);
        return *this;
    }

    bool operator==(const Integer &y) const {
        return _M_compare(y) == 0;
    }
    bool operator!=(const Integer &y) const {
        return _M_compare(y) != 0;
    }
    bool operator<(const Integer &y) const {
        return _M_compare(y) == -1;
    }
    bool operator>(const Integer &y) const {
        return _M_compare(y) == 1;
    }
    bool operator<=(const Integer &y) const {
        return _M_compare(y) != 1;
    }
    bool operator>=(const Integer &y) const {
        return _M_compare(y) != -1;
    }


    explicit operator bool(){
        return !_M_is_zero();
    }
    // explicit operator size_t(){
    //     size_t __res;
    //     _M_copy_to_bits(__res);
    //     return __res;
    // }
    // explicit operator int(){
    //     int __res;
    //     _M_copy_to_bits(__res);
    //     return __res;
    // }
    // explicit operator long long(){
    //     long long __res;
    //     _M_copy_to_bits(__res);
    //     return __res;
    // }
    
    friend std::istream& operator>>(std::istream &ist, Integer &x) {
        std::string s;
        ist>>s;
        int i = 0;
        if(s[0] == '-') i = 1;
        if(s.size() == i){
            ist.setstate(std::istream::failbit);
            return ist;
        }
        for(; i<s.size(); ++i)
            if(!ISDIGIT(s[i])){
                ist.setstate(std::istream::failbit);
                return ist;
            }
        x._M_copy_from_decimal_string(s.c_str(), s.size());
        return ist;
    }

    friend std::ostream& operator<<(std::ostream &ost, const Integer &x) {
        Integer *tmp = (Integer*)x._M_copy_to_decimal();
        std::cout<<"output: ("<<tmp->m_size<<") ";
        if(tmp->_M_is_zero())ost<<0;
        else{
            if(tmp->m_size < 0)ost<<'-';
            for(ssize_t i = ABS(tmp->m_size) - 1; i >= 0; --i){
                ost<<tmp->m_digit[i];
                ost<<std::setfill('0')<<std::setw(DECIMAL_SHIFT-1);
            }
        }
        ost<<std::setw(0);
        return ost;
    }

    // void debug() // debug
    // {
    //     for(int i=_M_v.size()-1;i>=0;i--){
    //         std::cout<<(_M_v[i]?'1':'0');
    //     }
    // }
};

Integer operator+(const Integer&__x, const Integer&__y) {
    return Integer(__x) += __y;
}
Integer operator-(const Integer&__x, const Integer&__y) {
    return Integer(__x) -= __y;
}
Integer operator*(const Integer&__x, const Integer&__y) {
    return Integer(__x) *= __y;
}
Integer operator/(const Integer&__x, const Integer&__y) {
    Integer tmp = Integer(__x) /= __y;
    std::cerr<<"div op / done\n";
    return tmp;
}
Integer operator%(const Integer&__x, const Integer&__y) {
    return Integer(__x) %= __y;
}


}; // namespace end;


#endif