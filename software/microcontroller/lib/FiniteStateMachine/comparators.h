#ifndef FSM_COMPARATORS_H_
#define FSM_COMPARATORS_H_

#include <number.h>

#define NOP_COMP_BOOL true
#define NOP_COMP_NUMBER false

typedef enum {
  CMP_NOP = 0,
  CMP_EQU,
  CMP_NEQ,
  CMP_LES,
  CMP_LEQ,
  CMP_GTR,
  CMP_GEQ,
} comparators_t;

// Helper Functions
const char* parseComparator(const comparators_t& cmp, bool result = true);

typedef enum {
  CB_NONE = 0,   // void (* const)(void) - NO ARGS
  CB_COMP,       // void (* const)(bool) - ADD: CONDITION RESULT
  CB_COMPVAL,    // void (* const)(bool, T) - ADD: PASSED VALUE
  CB_COMPVALREF, // void (* const)(bool, T, T) - ADD: REFERENCE VALUE
} callback_type_t;

// template <typename A> struct conditional_callbacks_t {
//   typedef ;
//   typedef ;
//   typedef );
//   typedef void (* const cb_ref_t)(bool comp, A val, A ref);
//   typedef A (* const cb_getref_t)(void);
// };

// TODO: Compound conditionals?

// CONSTRUCTION: 



// USE: CONDITION - COMPARISON OF VALUE <TYPE T> TO REFERENCE
// IF TRUE: EXECUTION

/**
 * @param T type of the two values to be compared
*/
template <typename T> class ConditionalCallback {
  public:
    typedef const T& (* cb_getref_t)(const T&);
    typedef void (* const cb_none_t)(void);
    typedef void (* const cb_comp_t)(bool);
    typedef void (* const cb_compval_t)(bool, const T&);
    typedef void (* const cb_compvalref_t)(bool, const T&, const T&);
    
    /**
     * @param cmp COMPARATOR
     * @param ref REFERENCE VALUE <TYPE T>
     * @param cb CALLBACK FUNCTION POINTER (CAST TO CONST VOID*)
     * @param cbtype CALLBACK TYPE - dictates argument set, cb pointer recast
     * REFERENCE UPDATOR (OPTIONAL)
     **/
    ConditionalCallback(comparators_t cmp, const T& ref, const void* cb, callback_type_t cbtype, cb_getref_t getReference = nullptr);
    ConditionalCallback(comparators_t cmp, const T& ref, cb_none_t cb, const T&(* const getReference)(const T&) = nullptr);
    ConditionalCallback(comparators_t cmp, const T& ref, cb_comp_t cb, cb_getref_t getReference = nullptr);
    ConditionalCallback(comparators_t cmp, const T& ref, cb_compval_t cb, cb_getref_t getReference = nullptr);
    ConditionalCallback(comparators_t cmp, const T& ref, cb_compvalref_t cb, cb_getref_t getReference = nullptr);

    ConditionalCallback(comparators_t cmp, const T& ref, const void* cb, callback_type_t cbtype, bool invert, cb_getref_t getReference = nullptr);
    ConditionalCallback(comparators_t cmp, const T& ref, cb_none_t cb, bool invert, cb_getref_t getReference = nullptr);
    ConditionalCallback(comparators_t cmp, const T& ref, cb_comp_t cb, bool invert, cb_getref_t getReference = nullptr);
    ConditionalCallback(comparators_t cmp, const T& ref, cb_compval_t cb, bool invert, cb_getref_t getReference = nullptr);
    ConditionalCallback(comparators_t cmp, const T& ref, cb_compvalref_t  cb, bool invert, cb_getref_t getReference = nullptr);

    ConditionalCallback<T>& operator=(ConditionalCallback<T> const& other);

    void operator()(const T& val);
    void operator()(const T& val, const T& ref);

    static bool compare(comparators_t cmp, const T& val, const T& ref);

    void disable();
    void enable();
    void enable(const T& val);

  protected:
    // NO CALLBACK - ASSUMES childCallback is implemented
    ConditionalCallback(comparators_t cmp, const T& ref, cb_getref_t getReference = nullptr);
    ConditionalCallback(comparators_t cmp, const T& ref, bool invert, cb_getref_t getReference = nullptr);

    // FALLBACKS FOR FUNCPOINTERS
    virtual void childCallback(bool comp, const T& val, const T& ref); // NOP
    virtual const T& childReference(const T& val); // NOP

    // PROPERTIES

    void setComparator(comparators_t cmp);

    comparators_t comparator;

    virtual void callOperator(const T& val, const T& ref);

    T reference;

    const bool invert = false;

    bool disabled = false;

    const void* callback;

    const callback_type_t callbacktype;

    const cb_getref_t getReference = nullptr;

    void executeCallback(const T& val, const T& ref);
};

class BangBangConditional : public ConditionalCallback<Number> {
  private:
    bool state = false;

    // Has the callback been triggered yet?
    bool triggered = false;

    const Number low;
    const Number high;

    const void* rising;
    const void* falling;

    const callback_type_t latchcbtype;

  protected:
    void childCallback(bool comp, const Number& val, const Number& ref) override;
    const Number& childReference(const Number& val) override;
    void callOperator(const Number& val, const Number& ref) override;
  public:
    /**
     * When val < `lo`, callback(true)
     * When val > `hi`, callback(false)
     * `invert` inverts argument value
     * Args are parsed with min/max so that hi > lo
    */
    BangBangConditional(const Number& lo, const Number& hi, cb_none_t rising, cb_none_t falling);
    BangBangConditional(const Number& lo, const Number& hi, const void* rising, const void* falling, callback_type_t cbtype);
    BangBangConditional(const Number& lo, const Number& hi, cb_none_t cb);
    BangBangConditional(const Number& lo, const Number& hi, const void* cb, callback_type_t cbtype);
};

class LatchingConditional : public ConditionalCallback<bool> {
  private:
    bool state = true;

    // Has the callback been triggered yet?
    bool triggered = false;

    const void* rising;
    const void* falling;

    const callback_type_t latchcbtype;

  protected:
    void childCallback(bool comp, const bool& val, const bool& ref) override;
    const bool& childReference(const bool& val) override;
    void callOperator(const bool& val, const bool& ref) override;
  public:
    LatchingConditional(cb_none_t rising, cb_none_t falling, bool invert = false);
    LatchingConditional(const void* rising, const void* falling, callback_type_t cbtype, bool invert = false);
    explicit LatchingConditional(cb_none_t cb, bool invert = false);
    LatchingConditional(const void* cb, callback_type_t cbtype, bool invert = false);
};

#include <comparators.tpp>

#endif