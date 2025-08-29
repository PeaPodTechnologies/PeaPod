#ifndef FSM_TIMER_H_
#define FSM_TIMER_H_

#include <state.h>
#include <comparators.h>

#define MAX_INTERVALS 32
#define INTERVAL_RESET_WINDOW 1.05
#define TWENTYFOURHRS_MILLIS ((fsm_timestamp_t)86400000)

class IntervalCallback : private ConditionalCallback<fsm_timestamp_t> {
  private:
    // Time since last reset (ms)
    fsm_timestamp_t now = 0;
    const fsm_timestamp_t phase = 0;
    const fsm_timestamp_t delta;

    fsm_timestamp_t refholder;
  protected:
    const fsm_timestamp_t& childReference(const fsm_timestamp_t& val) override;
  public:
    IntervalCallback(const fsm_timestamp_t& delta, const void* cb, callback_type_t cbtype, bool invert = false);

    IntervalCallback(const fsm_timestamp_t& delta, const fsm_timestamp_t& phase, const void* cb, callback_type_t cbtype, bool invert = false);

    fsm_timestamp_t get(void);
    void set(const fsm_timestamp_t& val);
};

class FlagInterval : public IntervalCallback {
  private:
    Flag* flag;
  protected:
    void childCallback(bool comp, const fsm_timestamp_t& val, const fsm_timestamp_t& ref) override;
  public:
    FlagInterval(const fsm_timestamp_t& delta, Flag* flag, bool invert = false);
    FlagInterval(const fsm_timestamp_t& delta, const fsm_timestamp_t& phase,  Flag* flag, bool invert = false);
};

class ChronoEvent : public ConditionalCallback<fsm_timestamp_t> {
  private:
    // Has the callback been triggered yet?
    bool triggered = false;
  protected:
    void callOperator(const fsm_timestamp_t& val, const fsm_timestamp_t& ref) override;
  public:
    ChronoEvent(const fsm_timestamp_t& timestamp, typename ConditionalCallback<fsm_timestamp_t>::cb_compval_t cb, bool invert = false);
};

class FlagEvent : public FlagSetCondition<fsm_timestamp_t> {
  private:
    // Has the callback been triggered yet?
    bool triggered = false;
  protected:
    void callOperator(const fsm_timestamp_t& val, const fsm_timestamp_t& ref) override;
    void childCallback(bool comp, const fsm_timestamp_t& val, const fsm_timestamp_t& ref) override;
  public:
    FlagEvent(const fsm_timestamp_t& timestamp, Flag* flag, bool invert = false);
};

class Chronograph : private State<fsm_timestamp_t> {
  private:
    const fsm_timestamp_t start;
    const fsm_timestamp_t last;

    IntervalCallback* intervals[MAX_INTERVALS] = { nullptr };
  public:
    Chronograph(const fsm_timestamp_t& start = 1000);

    ConditionalCallback<fsm_timestamp_t>* addEvent(const fsm_timestamp_t& timestamp, typename ConditionalCallback<fsm_timestamp_t>::cb_compval_t cb, bool invert = false);
    ConditionalCallback<fsm_timestamp_t>* addEventFlag(const fsm_timestamp_t& timestamp, Flag* flag, bool invert = false);
    IntervalCallback* addInterval(const fsm_timestamp_t& delta, typename ConditionalCallback<fsm_timestamp_t>::cb_compval_t cb, bool invert = false);
    IntervalCallback* addInterval(const fsm_timestamp_t& delta, const fsm_timestamp_t& phase, typename ConditionalCallback<fsm_timestamp_t>::cb_compval_t cb, bool invert = false);
    IntervalCallback* addIntervalFlag(const fsm_timestamp_t& delta, Flag* flag, bool invert = false);
    IntervalCallback* addIntervalFlag(const fsm_timestamp_t& delta, const fsm_timestamp_t& phase, Flag* flag, bool invert = false);
    
    IntervalCallback* addTwentyFourTimeout(const fsm_timestamp_t& delay, typename ConditionalCallback<fsm_timestamp_t>::cb_compval_t cb, bool invert = false);

    // EXPOSE SOME PUBLIC BASE CLASS MEMBERS

    void set(const fsm_timestamp_t& val);
    fsm_timestamp_t get(void);

    // ConditionalCallback<fsm_timestamp_t>* addLoggerCallback(void (*cb)(bool, fsm_timestamp_t));

    #ifdef FSM_DEBUG_SERIAL
      static void debugTimestamp(const fsm_timestamp_t& timestamp);
    #endif
};

extern Chronograph Chronos;

#endif