#ifndef I2CIP_HASHTABLE_H_
#error __FILE__ should only be included AFTER <hashtable.h>
#endif

#ifdef I2CIP_HASHTABLE_H_

#ifndef I2CIP_HASHTABLE_T_
#define I2CIP_HASHTABLE_T_

inline static uint8_t hash(const char* s) {
  unsigned index;
  for (index = 0; *s != '\0'; s++) {
    index = *s + HASHTABLE_OFFSET * index;
  }
  return index % HASHTABLE_SLOTS;
}

// HASH TABLE ENTRY

template <typename T> HashTableEntry<T>::HashTableEntry(const char* key, T value, HashTableEntry<T>* last) : key(key), value(value), next(last) { }

template <typename T> HashTableEntry<T>::~HashTableEntry() {
  // Free our key then trigger the next entry
  delete this->key;
  delete this->next;
}

// HASH TABLE

template <typename T> HashTable<T>::HashTable() { 
  for(int i = 0; i < HASHTABLE_SLOTS; i++) {
    hashtable[i] = nullptr;
  }
}

template <typename T> HashTable<T>::~HashTable() {
  // Free all allocated entries (and their keys) recursively; slots are static
  for (uint8_t i = 0; i < HASHTABLE_SLOTS; i++) {
    delete(this->hashtable[i]);
  }
}

template <typename T> HashTableEntry<T>* HashTable<T>::operator[](const char* key) {
  return get(key);
}

// Public methods

template <typename T> HashTableEntry<T>* HashTable<T>::set(const char* key, T value, bool overwrite) {
  HashTableEntry<T>* head = get(key);

  // Match found?
  if (head != nullptr) {
    if (overwrite) head->value = value;
    return head;
  }
  // No match, allocate new; point "next" to the top entry
  uint8_t index = hash(key);
  head = new HashTableEntry<T>(key, value, hashtable[index]);

  // If allocation was successful:
  if (head != nullptr && head->key != nullptr) {
    // Point the top of the hashtable to the new entry
    hashtable[index] = head;
  }
  #ifdef I2CIP_DEBUG_SERIAL 
    else { I2CIP_DEBUG_SERIAL.print(F("HashTable(x) ENOMEM\n")); }
  #endif

  return head;
}

template <typename T> HashTableEntry<T>* HashTable<T>::get(const char* key) {
  HashTableEntry<T>* np;
  for (np = hashtable[hash(key)]; np != nullptr; np = np->next) {
    if (strcmp(key, np->key) == 0) {
      return np; /* found */
    }
  }
  #ifdef I2CIP_DEBUG_SERIAL 
    I2CIP_DEBUG_SERIAL.print(F("HashTable[''] ENOENT\n"));
  #endif
  return nullptr; /* not found */
}

template <typename T> bool HashTable<T>::remove(const char* key) {
  // Find ptr.next.key == key
  uint8_t index = hash(key);
  HashTableEntry<T>* ptr = hashtable[index];

  // Check top-level
  if (strcmp(key, ptr->key) == 0) {
    // Found; point the slot to next
    hashtable[index] = ptr->next;
    ptr->next = nullptr;
    delete(ptr);
    return true;
  }

  for (; ptr->next != nullptr; ptr = ptr->next) {
    if (strcmp(key, ptr->next->key) == 0) {
      // Found; point the preceding entry to the subsequent one
      HashTableEntry<T>* entry = ptr->next;
      ptr->next = ptr->next->next;
      entry->next = nullptr;
      delete(entry);
      return true;
    }
  }
  // Not found
  return false;
}

#endif
#endif
