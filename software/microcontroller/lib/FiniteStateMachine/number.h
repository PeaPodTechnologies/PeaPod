#ifndef FSM_NUMBER_H_
#define FSM_NUMBER_H_

#define NUMBER_FLOP_EPSILON 0.00001

#include <math.h>

typedef unsigned long fsm_timestamp_t;

class Number {
  private:
    double value;
    bool isFloating;
    bool isSigned;
    bool notanumber = false;

  protected:
    int compare(const Number& other) const;

    // float getValue(void);
    // float isFloatingPoint(void);
    // float isSignedInteger(void);

    void set(const double& val, bool isFloating = true, bool isSigned = true);
    void setValue(const long& val);
    void setValue(const unsigned long& val);

  public:
    Number(); // NaN Constructor
    Number(const double& val, bool isFloating = true, bool isSigned = true);
    Number(const int& val);
    Number(const long& val);
    Number(const unsigned long& val);
    Number(const Number& other);

    ~Number();

    bool isNaN(void) const;

    double operator()(void) const;

    void operator=(const double& rhs);
    void operator=(const float& rhs);
    void operator=(const long& rhs);
    void operator=(const int& rhs);
    void operator=(const unsigned long& rhs);
    void operator=(const unsigned int& rhs);

    Number& operator=(const Number& other);

    bool operator==(const Number& rhs) const;
    bool operator!=(const Number& rhs) const;

    bool operator<(const Number& rhs) const;
    bool operator<=(const Number& rhs) const;
    bool operator>(const Number& rhs) const;
    bool operator>=(const Number& rhs) const;

    operator double() const;
    explicit operator fsm_timestamp_t()  const;
    // operator long() const;
    // operator int() const;

    Number operator+(const Number& rhs) const;
    Number operator-(const Number& rhs) const;
    Number operator*(const Number& rhs) const;
    Number operator/(const Number& rhs) const;

    static const Number& minimum(const Number& a, const Number& b);

    static const Number& maximum(const Number& a, const Number& b);
};

#endif