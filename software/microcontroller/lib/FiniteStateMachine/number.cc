#include <number.h>

#include <math.h>

#include <debug.h>

int Number::compare(const Number& other) const {
  if(other.isNaN() && this->isNaN()) return 0;
  if(other.isNaN()) return 1;
  if(this->isNaN()) return -1;

  double diff = this->value - other.value;
  return (diff < NUMBER_FLOP_EPSILON) ? (diff > -NUMBER_FLOP_EPSILON ? 0 : -1) : 1;
}

// float Number::getValue() { return this->value; }
// float Number::isFloatingPoint() { return this->isFloating; }
// float Number::isSignedInteger() { return this->isSigned; }

void Number::set(const double& val, bool isFloating, bool isSigned) { 
  this->value = val; 
  this->isFloating = isFloating; 
  this->isSigned = isSigned;
}

void Number::setValue(const long& val) { this->set((double)val, false); }
void Number::setValue(const unsigned long& val) { this->set((double)val, false, false); }

// Master Constructor
Number::Number(const double& val, bool isFloating, bool isSigned) : value(val), isFloating(isFloating), isSigned(isFloating || isSigned) { }

// NaN Constructor
Number::Number() : value((double)NAN), isFloating(true), isSigned(true), notanumber(true) {
  // delay(10);
  // Serial.print("[NaN Constructor]\n");
  // delay(10);
}

Number::Number(const unsigned long& val) : Number((double)val, false, false) { }
Number::Number(const long& val) : Number((double)val, false, val < 0) { }
Number::Number(const int& val) : Number((double)val, false, val < 0) { }
Number::Number(const Number& other) : Number(other.value, other.isFloating, other.isSigned) {
  if(other.isNaN()) {
    // delay(10);
    // Serial.print("[NaN Copy Constructor]\n");
    // delay(10);
    this->notanumber = true;
  }
}

Number::~Number() { }

bool Number::isNaN(void) const { 
  // if(this->notanumber) {
  //   delay(10);
  //   Serial.print("[IS NAN]\n");
  //   delay(10);
  // }
  return this->notanumber;
}

double Number::operator()(void) const { return this->operator double(); }

Number::operator double() const { return this->isNaN() ? (double)NAN : this->value; }

void Number::operator=(const double& rhs) { this->set(rhs); }
void Number::operator=(const float& rhs) { this->set(rhs); }
void Number::operator=(const long& rhs) { this->set((double)rhs, false, rhs < 0); }
void Number::operator=(const int& rhs) { this->set((double)rhs, false, rhs < 0); }
void Number::operator=(const unsigned long& rhs) { this->set((double)rhs, false, false); }
void Number::operator=(const unsigned int& rhs) { this->set((double)rhs, false, false); }

Number& Number::operator=(const Number& other) { 
  if(other.isNaN()) { 
    this->notanumber = true; 
    this->set((double)NAN, true); 
  } else { 
    this->set(other.value, other.isFloating, other.isSigned); 
  } 
  return (*this); 
}

bool Number::operator==(const Number& rhs) const { return (compare(rhs) == 0) && !(this->isFloating ^ rhs.isFloating); }

bool Number::operator!=(const Number& rhs) const { return (compare(rhs) != 0); }

bool Number::operator<(const Number& rhs) const { return (compare(rhs) < 0); }
bool Number::operator<=(const Number& rhs) const { return (compare(rhs) <= 0); }
bool Number::operator>(const Number& rhs) const { return (compare(rhs) > 0); }
bool Number::operator>=(const Number& rhs) const { return (compare(rhs) >= 0); }

// Number::operator double() const { return this->value; }
// Number::operator long() const { return (this->isSigned ? (long)this->value : (unsigned long)this->value); }
// Number::operator int() const { return (this->isSigned ? (int)this->value : (unsigned int)this->value); }

// NUMBER FLAG RULES

// Addition/Subtraction:
// Is Floating: IFF this is floating OR rhs is floating
// Is Signed: IFF the result is floating OR signed

Number Number::operator+(const Number& rhs) const { 
  if(rhs.isNaN() || this->isNaN()) return Number();

  double result = rhs.value + this->value;
  return Number(result, rhs.isFloating || this->isFloating, result < 0.0); 
}

Number Number::operator-(const Number& rhs) const { 
  if(rhs.isNaN() || this->isNaN()) return Number();

  double result = this->value - rhs.value; 
  return Number(result, rhs.isFloating || this->isFloating, result < 0.0); 
}

// Multiplication/Division:
// Is Floating: IFF this is floating OR rhs is floating
// Is Signed: IFF this is floating OR rhs is floating OR this is signed OR rhs is signed

Number Number::operator*(const Number& rhs) const {
  if(rhs.isNaN() || this->isNaN()) return Number();

  double result = rhs.value * this->value;
  return Number(result, rhs.isFloating || this->isFloating, rhs.isSigned || this->isSigned); 
}

Number Number::operator/(const Number& rhs) const {
  if(rhs.isNaN() || this->isNaN()) return Number();

  return (rhs.value < NUMBER_FLOP_EPSILON && rhs.value > -NUMBER_FLOP_EPSILON) ? Number() : Number(this->value / rhs.value, true, rhs.isSigned || this->isSigned); 
}

const Number& Number::minimum(const Number& a, const Number& b) { return a > b ? b : a; }

const Number& Number::maximum(const Number& a, const Number& b) { return a < b ? b : a; }