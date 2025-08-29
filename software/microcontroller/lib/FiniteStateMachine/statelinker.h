#ifndef FSM_STATELINKER_H_
#define FSM_STATELINKER_H_

// Class Forwarrd-Declarations (for <state.h>)
template <typename T> class FlagSetCondition;
class LatchingFlagSet;
class BangBangFlagSet;

#include <state.h>
#include <comparators.h>
#include <number.h>

template <typename T> class FlagSetCondition : public ConditionalCallback<T> {
  protected:
    Flag* flag;

    void childCallback(bool comp, const T& val, const T& ref) override;
    
  public:
    FlagSetCondition(comparators_t cmp, const T& ref, Flag* flag, typename ConditionalCallback<T>::cb_getref_t getReference = nullptr);
    FlagSetCondition(comparators_t cmp, const T& ref, Flag* flag, bool invert, typename ConditionalCallback<T>::cb_getref_t getReference = nullptr);
};

class BangBangFlagSet : public FlagSetCondition<Number> {
  private:
    bool state = false;

    // Has the callback been triggered yet?
    bool triggered = false;

    const Number low;
    const Number high;
  protected:
    void callOperator(const Number& val, const Number& ref) override;
    void childCallback(bool comp, const Number& val, const Number& ref) override;
    const Number& childReference(const Number& val) override;
  public:
    BangBangFlagSet(const Number& lo, const Number& hi, Flag* flag);
};

// Mostly useless
class LatchingFlagSet : public FlagSetCondition<bool> {
  private:
    bool state = true;

    // Has the callback been triggered yet?
    bool triggered = false;
  protected:
    void callOperator(const bool& val, const bool& ref) override;
    void childCallback(bool comp, const bool& val, const bool& ref) override;
    const bool& childReference(const bool& val) override;
  public:
    explicit LatchingFlagSet(Flag* flag, bool invert = false);
};

// template <typename T> class OneShotFlagReset : public FlagSetCondition<T> {
//   private:
//     bool triggered = false;

//     void callOperator(T val, T ref);
//   public:
//     OneShotFlagSet(comparators_t cmp, T ref, Flag* flag);

//     void operator()(T val);
//     void operator()(T val, T ref);
// };

#include <statelinker.tpp>

#endif