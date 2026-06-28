#ifndef CSOS_SENSORS_SENSOR_H_
#define CSOS_SENSORS_SENSOR_H_

#include <Arduino.h>
#include <ArduinoJson.h>

#include <types.h>

#include <I2CIP.h>
#include <state.h>

namespace ControlSystemsOS {
  class Datum {
    private:
      const char* label;
      csos_data_t value;
      csos_pdata_t ptr;
      csos_types_t type;

    public:
      ~Datum() { delete next; }
      // Datum(const csos_types_t& type, const csos_pdata_t& value) : type(type), next(nullptr) {
      //   switch(type) {
      //     case CSOS_BOOL:
      //       this->value.b = *value.b;
      //       break;
      //     case CSOS_UINT8:
      //       this->value.u8 = *value.u8;
      //       break;
      //     case CSOS_INT8:
      //       this->value.i8 = *value.i8;
      //       break;
      //     case CSOS_UINT16:
      //       this->value.u16 = *value.u16;
      //       break;
      //     case CSOS_INT16:
      //       this->value.i16 = *value.i16;
      //       break;
      //     case CSOS_UINT32:
      //       this->value.u32 = *value.u32;
      //       break;
      //     case CSOS_INT32:
      //       this->value.i32 = *value.i32;
      //       break;
      //     case CSOS_FLOAT:
      //       this->value.f = *value.f;
      //       break;
      //     case CSOS_DOUBLE:
      //       this->value.d = *value.d;
      //       break;
      //     case CSOS_SCHAR:
      //       this->value.c = *value.c;
      //       break;
      //     default:
      //       this->value.null = nullptr;
      //       break;
      //   }
      // }

      Datum* const next;
      
      Datum(const csos_types_t& type, csos_data_t value, const char* label, Datum* next = nullptr) : type(type), value(value), label(label), next(next) {}

      template <typename T> bool applyState(State<T>* state);

      const char* getLabel(void) { return this->label; }

      char* toString(void);

      void addToJSON(JsonObject& json);

      void print(Print& p);
  };

  class Sensor {
    public:
      Sensor(InputGetter* getter) { getter->setSensor(this); }
      virtual Datum* datumFactory(void) = 0;
  };

};

#endif