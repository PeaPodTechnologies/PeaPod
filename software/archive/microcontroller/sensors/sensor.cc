#include <sensors/sensor.h>
#include <types.h>

// CSOS_BOOL: bool, Number != 0
template <> bool ControlSystemsOS::Datum::applyState(State<bool>* state) {
  if(state == nullptr) return false;

  double d = 0.0;
  switch(this->type) {
    case CSOS_BOOL:
      state->set(this->value.b);
      return true;
    case CSOS_UINT8:
      d = (double)(this->value.u8);
      break;
    case CSOS_INT8:
      d = (double)(this->value.i8);
      break;
    case CSOS_UINT16:
      d = (double)(this->value.u16);
      break;
    case CSOS_INT16:
      d = (double)(this->value.i16);
      break;
    case CSOS_UINT32:
      d = (double)(this->value.u32);
      break;
    case CSOS_INT32:
      d = (double)(this->value.i32);
      break;
    case CSOS_FLOAT:
      d = (double)(this->value.f);
      break;
    case CSOS_DOUBLE:
      d = this->value.d;
      break;
    default:
      return false;
  }

  state->set(d > -NUMBER_FLOP_EPSILON && d < NUMBER_FLOP_EPSILON);

  return true;
}

// CSOS_(U)INT8 through 32, FLOAT, DOUBLE
template <> bool ControlSystemsOS::Datum::applyState(State<Number>* state) {
  if(state == nullptr) return false;

  double d = NAN;
  switch(this->type) {
    case CSOS_BOOL:
      d = this->value.b ? 1.0f : 0.0f;
      break;
    case CSOS_UINT8:
      d = (double)(this->value.u8);
      break;
    case CSOS_INT8:
      d = (double)(this->value.i8);
      break;
    case CSOS_UINT16:
      d = (double)(this->value.u16);
      break;
    case CSOS_INT16:
      d = (double)(this->value.i16);
      break;
    case CSOS_UINT32:
      d = (double)(this->value.u32);
      break;
    case CSOS_INT32:
      d = (double)(this->value.i32);
      break;
    case CSOS_FLOAT:
      d = (double)(this->value.f);
      break;
    case CSOS_DOUBLE:
      d = this->value.d;
      break;
    default:
      return false;
  }

  state->set(d);

  return true;
}

char* ControlSystemsOS::Datum::toString() {
  char* buffer = (char*)malloc(8 * sizeof(char));
  if(buffer == nullptr) return buffer;
  for(uint8_t i = 0; i < 8; i++) buffer[i] = '\0';
  
  switch(this->type) {
    case CSOS_BOOL:
      snprintf(buffer, 8, "%s", (this->value.b) ? "true" : "false");
      break;
    case CSOS_UINT8:
      snprintf(buffer, 8, "%u", this->value.u8);
      break;
    case CSOS_INT8:
      snprintf(buffer, 8, "%d", this->value.i8);
      break;
    case CSOS_UINT16:
      snprintf(buffer, 8, "%u", this->value.u16);
      break;
    case CSOS_INT16:
      snprintf(buffer, 8, "%d", this->value.i16);
      break;
    case CSOS_UINT32:
      snprintf(buffer, 8, "%lu", this->value.u32);
      break;
    case CSOS_INT32:
      snprintf(buffer, 8, "%ld", this->value.i32);
      break;
    case CSOS_FLOAT:
      snprintf(buffer, 8, "%.2f", (double)this->value.f);
      break;
    case CSOS_DOUBLE:
      snprintf(buffer, 8, "%f", this->value.d);
      break;
    case CSOS_SCHAR:
      snprintf(buffer, 8, "%c", this->value.c);
      break;
    default:
      snprintf(buffer, 8, "null");
      break;
  }
  return buffer;
}

void ControlSystemsOS::Datum::addToJSON(JsonObject& json) {
  switch(this->type) {
    case CSOS_BOOL:
      json[this->label] = this->value.b;
      break;
    case CSOS_UINT8:
      json[this->label] = this->value.u8;
      break;
    case CSOS_INT8:
      json[this->label] = this->value.i8;
      break;
    case CSOS_UINT16:
      json[this->label] = this->value.u16;
      break;
    case CSOS_INT16:
      json[this->label] = this->value.i16;
      break;
    case CSOS_UINT32:
      json[this->label] = this->value.u32;
      break;
    case CSOS_INT32:
      json[this->label] = this->value.i32;
      break;
    case CSOS_FLOAT:
      json[this->label] = this->value.f;
      break;
    case CSOS_DOUBLE:
      json[this->label] = this->value.d;
      break;
    // case CSOS_SCHAR:
    //   json[this->label] = this->value.c;
    //   break;
    default:
      json[this->label] = nullptr;
      break;
  }
}

void ControlSystemsOS::Datum::print(Print& p) {
  switch(this->type) {
    case CSOS_BOOL:
      p.print((this->value.b) ? "true" : "false");
      break;
    case CSOS_UINT8:
      p.print(this->value.u8);
      break;
    case CSOS_INT8:
      p.print(this->value.i8);
      break;
    case CSOS_UINT16:
      p.print(this->value.u16);
      break;
    case CSOS_INT16:
      p.print(this->value.i16);
      break;
    case CSOS_UINT32:
      p.print(this->value.u32);
      break;
    case CSOS_INT32:
      p.print(this->value.i32);
      break;
    case CSOS_FLOAT:
      p.print(this->value.f);
      break;
    case CSOS_DOUBLE:
      p.print(this->value.d);
      break;
    case CSOS_SCHAR:
      p.print(this->value.c);
      break;
    default:
      p.print("null");
      break;
  }
}