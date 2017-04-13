/** Large Number C++ Template
  * By ZhongSheng-Wu
  * ------------------
  * "Integer" is a unlimited integer type,
  * it will auto resize when the number overflow,
  * it can pass into any STL function that input type is int or long long.
  * Example:
    int main(){
        chino::Integer a,b;
        cin>>a>>b;
        cout<<__gcd(a,b)<<endl;
    }
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
#include <cassert>

namespace chino{

struct BaseInteger;
void dump (BaseInteger *p) ;


struct BaseInteger {
    typedef unsigned long Digit;
    typedef unsigned long long TwoDigit;
    static const size_t CHARBIT = 8,
                 SHIFT = sizeof(Digit)*CHARBIT-1,
                 DECIMAL_BASE = 1e9,
                 DECIMAL_SHIFT = 9,
                 KARATSUBA_CUTOFF = 70;
    static const Digit MASK = ((Digit)1 << SHIFT) - 1U;
    Digit *m_digit;
    ssize_t m_size, m_real_size;

    BaseInteger():m_digit(NULL), m_size(0), m_real_size(0) { }

    BaseInteger(const BaseInteger &oth) {
        m_size = oth.m_size;
        m_real_size = ABS(m_size) + 1; /* Reserved one Digit space for some operation ex: add, sub, shift. */
        m_digit = new Digit[m_real_size];
        memcpy(m_digit, oth.m_digit, sizeof(Digit) * ABS(oth.m_size));
    }

    //! Assert size_n > 0;
    BaseInteger(ssize_t size_n, ssize_t real_size_n = 0) {
        if(!real_size_n)
            real_size_n = size_n + 1; /* Reserved one Digit space for some operation ex: add, sub, shift. */
        m_size = size_n;
        m_real_size = real_size_n;
        m_digit = new Digit[m_real_size]();
    }

    ~BaseInteger() {
        delete[] m_digit;
    }

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

    //! Assert size_n > 0
    void _M_resize(ssize_t size_n) {
        if(m_digit != NULL && size_n <= m_real_size){
            const ssize_t size_org = ABS(m_size);
            if(size_org < size_n)
                memset(m_digit+size_org, 0, sizeof(Digit) * (size_n - size_org));
        }else{
            m_real_size = size_n + 1; /* Reserved one Digit space for some operation ex: add, sub, shift. */
            Digit *p = new Digit[m_real_size]();
            if(m_digit != NULL){
                memcpy(p, m_digit, sizeof(Digit) * ABS(m_size));
                delete[] m_digit;
            }
            m_digit = p;
        }
        m_size = (m_size < 0) ? -(size_n) : size_n;
    }

    void _M_swap(BaseInteger &y) {
        std::swap(m_digit, y.m_digit);
        std::swap(m_size, y.m_size);
        std::swap(m_real_size, y.m_real_size);
    }

    template<typename T>
    void _M_copy_from_bits(T x) {
        const ssize_t size_bit = sizeof(x)*CHARBIT,
                      size_n = (size_bit - 1)/ SHIFT + 1;
        _M_resize(size_n);
        for(ssize_t i = 0; i < size_n; ++i){
            m_digit[i] = x & MASK;
            x >>= SHIFT;
        }
    }

    template<typename T>
    void _M_copy_to_bits(T &x) const {
        const ssize_t size_bit = MIN(sizeof(x)*CHARBIT, ABS(m_size)*SHIFT),
                      size_n = (size_bit - 1)/ SHIFT + 1;
        for(ssize_t i = 0; i < size_n; ++i)
            x |= (T)m_digit[i] << i*SHIFT;
    }

    void _M_copy_from_decimal_string(const char *str, const ssize_t size_s) {
        /*  digits = 1 + floor(log10(a) / log10(BASE))
            log10(BASE) = log10(2) * SHIFT > 0.3010*SHIFT */
        const ssize_t size_n = 1 + size_s*1000 / (301 * SHIFT);
        _M_resize(size_n);
        Digit *pout = m_digit;
        const char *pc = str, *pcend = pc+((size_s-1) % (DECIMAL_SHIFT) + 1);
        ssize_t size = 0;
        do {
            Digit hi = 0;
            while (pc < pcend)
                hi = hi*10 + (*pc++)-'0';
            pcend += DECIMAL_SHIFT;
            for (ssize_t j = 0; j < size; j++) {
                TwoDigit z = (TwoDigit)pout[j] * DECIMAL_BASE + hi;
                hi = (Digit)(z >> SHIFT); /* pout[j] < 2^SHIFT => hi < DECIMAL_BASE */
                pout[j] = (Digit)(z - ((TwoDigit)hi << SHIFT));
            }
            while (hi) {
                pout[size++] = hi & MASK;
                hi >>= SHIFT ;
            }
        } while ( pcend <= str + size_s );
        _M_normalize();
    }

    BaseInteger* _M_copy_to_decimal() const {
        /* digits = 1 + floor(log2(a) / log2(_PyLong_DECIMAL_BASE)) */
        const ssize_t size_a = ABS(m_size), size_n = 1 + size_a * SHIFT / (3 * DECIMAL_SHIFT);
        BaseInteger *res = new BaseInteger(size_n);
        Digit *pin = m_digit,
              *pout = res->m_digit;
        ssize_t size = 0;
        for (ssize_t i = size_a; --i >= 0; ) {
            Digit hi = pin[i];
            for (ssize_t j = 0; j < size; j++) {
                TwoDigit z = (TwoDigit)pout[j] << SHIFT | hi;
                hi = (Digit)(z / DECIMAL_BASE); /* pout[j] < DECIMAL_BASE => hi < 2^SHIFT */
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

    void _M_do_add(const BaseInteger &x,const BaseInteger &y) {
        const BaseInteger *a = &x, *b = &y;
        ssize_t size_a = ABS(a->m_size), size_b = ABS(b->m_size);
        if(ABS(a->m_size) < ABS(b->m_size)) {
            std::swap(a, b);
            std::swap(size_a, size_b);
        }
        _M_resize(size_a+1);
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
            while(--i>=0 && a->m_digit[i] == b->m_digit[i]);
            if(i < 0) {
                _M_resize(1); /* for uninitialized m_digit. */
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
        if (sign < 0)
            m_size = -(m_size);
        _M_normalize();
    }

    //! Assert size_a >= size_b and sizeof(digit_res) >= size_a.
    static Digit _S_real_add(Digit *digit_res, const Digit *digit_a, ssize_t size_a, const Digit *digit_b, ssize_t size_b) {
        assert(size_a >= size_b);
        Digit carry = 0;
        ssize_t i = 0;
        for(; i < size_b; ++i) {
            carry = digit_a[i] + digit_b[i] + carry;
            digit_res[i] = carry & MASK;
            carry >>= SHIFT;
        }
        for (; i < size_a; ++i) {
            carry = digit_a[i] + carry;
            digit_res[i] = carry & MASK;
            carry >>= SHIFT;
        }
        return carry & MASK;
    }

    //! Assert size_a >= size_b and sizeof(digit_res) >= size_a.
    static Digit _S_real_sub(Digit *digit_res, const Digit *digit_a, ssize_t size_a, const Digit *digit_b, ssize_t size_b) {
        assert(size_a >= size_b);
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
        return borrow & MASK;
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
                assert(carry <= MASK);
            }
            if (carry)
                *pres += (Digit)(carry & MASK);
            assert((carry >> SHIFT) == 0);
        }
        res->_M_normalize();
        return res;
    }

    //  Karatsuba
    //! Guarantee that m_size of t1 t2 t3 is positive.
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
        /*  t1 = ha*hb.
            t2 = la*lb. */
        t1 = _S_do_kmul(digit_a + shift, size_a - shift, digit_b + shift, size_b - shift);
        memcpy(res->m_digit + 2*shift, t1->m_digit, t1->m_size*sizeof(Digit));
        t2 = _S_do_kmul(digit_a, shift, digit_b, shift);
        memcpy(res->m_digit, t2->m_digit, t2->m_size*sizeof(Digit));
        /*  res -= t1 << shift + t2 << shift.
            res may be underflow but it's ok because of unsigned arithmetic mod.
            t2 first because cache is fresher. */
        remshift = ABS(res->m_size) - shift;
        _S_real_sub(res->m_digit + shift, res->m_digit + shift, remshift, t2->m_digit, ABS(t2->m_size));
        _S_real_sub(res->m_digit + shift, res->m_digit + shift, remshift, t1->m_digit, ABS(t1->m_size));
        /*  t1 = la+ha,  t2=lb+hb,  t3 = t1 * t2. */
        t1->_M_resize(MAX(shift, size_a - shift) +1);
        t1->m_digit[ABS(t1->m_size)-1] = (
                (size_a - shift >= shift) ?
                    _S_real_add(t1->m_digit, digit_a + shift, size_a - shift, digit_a, shift) :
                    _S_real_add(t1->m_digit, digit_a, shift, digit_a + shift, size_a - shift) );
        t1->_M_normalize();
        t2->_M_resize(size_b - shift +1);
        t2->m_digit[ABS(t2->m_size)-1] = _S_real_add(t2->m_digit, digit_b + shift, size_b - shift, digit_b, shift);
        t2->_M_normalize();
        t3 = _S_do_kmul(t1->m_digit, t1->m_size, t2->m_digit, t2->m_size);
        _S_real_add(res->m_digit + shift, res->m_digit + shift, remshift, t3->m_digit, ABS(t3->m_size));
        delete t1; delete t2; delete t3;
        res->_M_normalize();
        return res;
    }

    static BaseInteger* _S_k_lopsided_mul(const Digit *digit_a, ssize_t size_a, const Digit *digit_b, ssize_t size_b) {
        size_a = _S_noralize_digit(digit_a, size_a);
        size_b = _S_noralize_digit(digit_b, size_b);
        BaseInteger *res, *product;
        ssize_t size_done = 0;
        assert(size_a > KARATSUBA_CUTOFF);
        assert(2 * size_a <= size_b);
        res = new BaseInteger(size_a + size_b);
        while (size_b > 0) {
            const ssize_t size_touse = MIN(size_a, size_b);
            product = _S_do_kmul(digit_a, size_a, digit_b + size_done, size_touse);
            assert(res->m_size - size_done >=  product->m_size);
            _S_real_add(res->m_digit + size_done, res->m_digit + size_done, res->m_size - size_done, product->m_digit, product->m_size);
            delete product;
            size_b -= size_touse;
            size_done += size_touse;
        }
        res->_M_normalize();
        return res;
    }

    ssize_t _S_digit_log2(Digit x) const {
        ssize_t res = 0;
        while(x>>=1)res++;
        return res;
    }

    //! Divid by zero is undefined behavior.
    void _M_do_div(const BaseInteger &y, BaseInteger *&quo, BaseInteger *&rem) const {
        BaseInteger *inv, *inv_nex, *squ, *tmp;
        const ssize_t shift = ABS(y.m_size) - 1,
                      size_ylg2 = _S_digit_log2(y.m_digit[ABS(y.m_size)-1]),
                      bshift = SHIFT - 2*size_ylg2 - 1,
                      size_inv = ABS(m_size) - shift + 2, /* Precision, 1_DIGIT.(1+size-shift)_DIGIT */
                      point = size_inv - 1;
        if(ABS(m_size) < ABS(y.m_size)) {
            quo = new BaseInteger(1);
            rem = new BaseInteger(*this);
            return;
        }
        inv = new BaseInteger(size_inv);
        inv_nex = new BaseInteger(size_inv);
        /* Copy y to inv_nex, and then shift it to meet (1 < y * inv_nex < 2)  */
        ssize_t size_ythrow = MAX(ABS(y.m_size) - point, 0);
        memcpy(inv_nex->m_digit + point - ABS(y.m_size) + size_ythrow, y.m_digit + size_ythrow, sizeof(Digit)*(ABS(y.m_size)-size_ythrow));
        if(bshift > 0)
            inv_nex -> _M_do_left_shift(bshift);
        else if(bshift < 0)
            inv_nex -> _M_do_right_shift(-bshift);
        ssize_t i = size_inv;
        int cnt = 0;
        /*  Newton iteration method to find 1/y.
            Stop when inv == inv_nex. */
        while(i) {
            memcpy(inv->m_digit, inv_nex->m_digit, sizeof(Digit) * size_inv);
            inv_nex->_M_do_left_shift(1);
            squ = _S_do_kmul(inv->m_digit, size_inv, inv->m_digit, size_inv);
            tmp = _S_do_kmul(squ->m_digit, ABS(squ->m_size), y.m_digit, ABS(y.m_size));
            delete squ;
            _S_real_sub(inv_nex->m_digit, inv_nex->m_digit, size_inv, tmp->m_digit + point + shift, ABS(tmp->m_size) - point - shift);
            delete tmp;
            while(i && inv_nex->m_digit[i-1] == inv->m_digit[i-1])
                --i;
        }
        delete inv;
        quo = _S_do_kmul(inv_nex->m_digit, size_inv, m_digit, m_size);
        delete inv_nex;
        ssize_t size_quo_shift = ABS(quo->m_size)-shift-point;
        if(size_quo_shift <= 0)
            quo->m_digit[0] = 0;
        else{
            for(ssize_t j=0; j < size_quo_shift; j++)
                quo->m_digit[j] = quo->m_digit[j+shift+point];
        }
        quo->_M_resize(MAX(size_quo_shift, 1));
        tmp = _S_do_kmul(quo->m_digit, ABS(quo->m_size), y.m_digit, ABS(y.m_size));
        rem = new BaseInteger();
        rem->_M_do_sub(*this, *tmp);
        delete tmp;
        if( rem->m_size < 0 ) { /* rem < 0 */
            std::cerr<<"rem too small\n";
            rem->_M_do_add(*rem, y);
            Digit one = 1;
            _S_real_sub(quo->m_digit, quo->m_digit, ABS(quo->m_size), &one, 1);
        }else if( rem->_M_compare(y) ==  1) { /* rem > divisor */
            std::cerr<<"rem too big\n";
            rem->_M_do_sub(*rem, y);
            Digit one = 1;
            quo->_M_resize(ABS(quo->m_size)+1);
            _S_real_add(quo->m_digit, quo->m_digit, ABS(quo->m_size), &one, 1);
        }
    }

    //! 0 = equal, 1 = bigger, -1 = smaller
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

    //! Assert shift < SHIFT.
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

    //! Assert shift < SHIFT.
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

    void _M_do_and(const BaseInteger &y) {
        const BaseInteger *a = this, *b = &y;
        if(ABS(a->m_size) < ABS(b->m_size))
            std::swap(a, b);
        const ssize_t size_b = ABS(b->m_size);
        _M_resize(size_b);
        for(ssize_t i = 0; i < size_b; ++i)
            m_digit[i] = a->m_digit[i] & b->m_digit[i];
    }

    void _M_do_or(const BaseInteger &y) {
        const BaseInteger *a = this, *b = &y;
        if(ABS(a->m_size) < ABS(b->m_size))
            std::swap(a, b);
        const ssize_t size_a = ABS(a->m_size), size_b = ABS(b->m_size);
        _M_resize(size_a);
        ssize_t i;
        for(i = 0; i < size_b; ++i)
            m_digit[i] = a->m_digit[i] | b->m_digit[i];
        for(; i < size_a; ++i)
            m_digit[i] = a->m_digit[i];
    }

    void _M_do_xor(const BaseInteger &y) {
        const BaseInteger *a = this, *b = &y;
        if(ABS(a->m_size) < ABS(b->m_size))
            std::swap(a, b);
        const ssize_t size_a = ABS(a->m_size), size_b = ABS(b->m_size);
        _M_resize(size_a);
        ssize_t i;
        for(i = 0; i < size_b; ++i)
            m_digit[i] = a->m_digit[i] ^ b->m_digit[i];
        for(; i < size_a; ++i)
            m_digit[i] = a->m_digit[i];
    }

    void _M_do_neg() {
        const ssize_t size_n = ABS(m_size);
        for(ssize_t i = 0; i < size_n; ++i)
            m_digit[i] = ~m_digit[i] & MASK;
    }

    bool _M_is_zero() const {
        return ABS(m_size) == 1 && !m_digit[0];
    }

    void debug(const char *str) {
        std::cerr<<"Debug : "<<str<<" :";
        ssize_t size_a = ABS(m_size);
        for(ssize_t i = 0; i<size_a;++i){
            if(i % 15 == 0)std::cerr<<'\n';
            std::cerr<<m_digit[i]<<' ';
        }
        std::cerr<<'\n';
    }
};

class Integer: private BaseInteger {
private:
    template<typename T>
    void _M_copy_from_sign_bits(T x) {
        if( x < 0 ){
            _M_copy_from_bits(-x);
            m_size = -m_size;
        }else
            _M_copy_from_bits(x);
    }

    template<typename T>
    void _M_copy_to_sign_bits(T &x) {
        _M_copy_to_bits(x);
        if( m_size < 0)
            x = -x;
    }

public:
    Integer():BaseInteger() { }
    Integer(const Integer& oth):BaseInteger(oth) { }
    Integer(Integer &&oth):BaseInteger() {
        _M_swap(oth);
    }

    Integer(int val):BaseInteger() {
        _M_copy_from_sign_bits(val);
    }
    Integer(long long val):BaseInteger() {
        _M_copy_from_sign_bits(val);
    }
    Integer(unsigned int val):BaseInteger() {
        _M_copy_from_bits(val);
    }
    Integer(unsigned long long val):BaseInteger() {
        _M_copy_from_bits(val);
    }
    Integer(const char *str):BaseInteger() {
        _M_copy_from_decimal_string(str, strlen(str));
    }

    Integer& operator=(const Integer &oth) {
        Integer tmp(oth);
        _M_swap(tmp);
        return *this;
    }
    Integer& operator=(Integer &&oth) {
        _M_swap(oth);
        return *this;
    }

    Integer& operator=(int val) {
        _M_copy_from_sign_bits(val);
    }
    Integer& operator=(long long val) {
        _M_copy_from_sign_bits(val);
    }
    Integer& operator=(unsigned int val) {
        _M_copy_from_bits(val);
    }
    Integer& operator=(unsigned long long val) {
        _M_copy_from_bits(val);
    }

    Integer& operator+=(const Integer &y) {
        if(m_size == y.m_size)
            _M_do_add(*this, y);
        else
            _M_do_sub(*this, y);
        return *this;
    }

    Integer& operator-=(const Integer &y) {
        if(m_size != y.m_size)
            _M_do_add(*this, y);
        else
            _M_do_sub(*this, y);
        return *this;
    }

    Integer& operator*=(const Integer &y) {
        Integer *tmp = (Integer*)_S_do_kmul(m_digit, ABS(m_size), y.m_digit, ABS(y.m_size));
        tmp->m_size = ((m_size < 0) != (y.m_size < 0))? -tmp->m_size : tmp->m_size;
        _M_swap(*tmp);
        delete tmp;
        return *this;
    }

    Integer& operator/=(const Integer &y) {
        BaseInteger *quo = NULL, *rem = NULL;
        _M_do_div(y, quo, rem);
        quo->m_size = ((m_size < 0) != (y.m_size < 0))? -quo->m_size : quo->m_size;
        _M_swap(*quo);
        delete quo;
        delete rem;
        return *this;
    }

    //! 5 % -3 == 2, -5 % 3 == -2, -5 % -3 == -2
    Integer operator%=(const Integer &y){
        BaseInteger *quo, *rem;
        _M_do_div(y, quo, rem);
        rem->m_size = (m_size < 0)? -rem->m_size : rem->m_size;
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
        for(ssize_t i = 0; i < size_a - dshift; ++i)
            m_digit[i] = m_digit[i + dshift];
        _M_resize(size_a-dshift);
        _M_do_right_shift(shift % SHIFT);
        return *this;
    }

    Integer& operator&=(const Integer &y) {
        _M_do_and(y);
        return *this;
    }
    Integer& operator|=(const Integer &y) {
        _M_do_or(y);
        return *this;
    }
    Integer& operator^=(const Integer &y) {
        _M_do_xor(y);
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
    explicit operator int(){
        int res;
        _M_copy_to_sign_bits(res);
        return res;
    }
    explicit operator long long(){
        long long res;
        _M_copy_to_sign_bits(res);
        return res;
    }
    explicit operator unsigned int(){
        unsigned int res;
        _M_copy_to_bits(res);
        return res;
    }
    explicit operator unsigned long long(){
        unsigned long long res;
        _M_copy_to_bits(res);
        return res;
    }


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
        // std::cout<<"output: ("<<tmp->m_size<<") ";
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

    friend Integer operator+(const Integer&__x, const Integer&__y) {
        return Integer(__x) += __y;
    }
    friend Integer operator-(const Integer&__x, const Integer&__y) {
        return Integer(__x) -= __y;
    }
    friend Integer operator*(const Integer&__x, const Integer&__y) {
        return Integer(__x) *= __y;
    }
    friend Integer operator/(const Integer&__x, const Integer&__y) {
        return Integer(__x) /= __y;
    }
    friend Integer operator%(const Integer&__x, const Integer&__y) {
        return Integer(__x) %= __y;
    }

    friend Integer operator&(const Integer &x, const Integer &y) {
        return Integer(x) &= y;
    }
    friend Integer operator|(const Integer &x, const Integer &y) {
        return Integer(x) &= y;
    }
    friend Integer operator^(const Integer &x, const Integer &y) {
        return Integer(x) &= y;
    }
    friend Integer operator~(const Integer &x) {
        Integer tmp(x);
        tmp._M_do_neg();
        return tmp;
    }

    friend Integer operator<<(const Integer &x, ssize_t y) {
        return Integer(x) <<= y;
    }
    friend Integer operator>>(const Integer &x, ssize_t y) {
        return Integer(x) >>= y;
    }

};

void dump(BaseInteger*p) {
    std::cerr<<(*(Integer*)p)<<'\n';
}


}; // namespace end;


#undef ABS
#undef MIN
#undef MAX
#undef ISDIGIT

#endif