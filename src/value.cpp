/*
**  - Cedric GESTES <gestes@aldebaran-robotics.com>
**
** Copyright (C) 2013 Aldebaran Robotics
*/

#include <qi/log.hpp>
#include <qic/value.h>
#include "value_p.h"
#include "object_p.h"

qiLogCategory("qi.c");

//# GENERIC POD IMPL
template<typename T>
int       qi_value_set_pod(qi_value_t  *msg, T val) {
  qi::AnyValue &gv = qi_value_cpp(msg);
  try {
    gv.set<T>(val);
    return 1;
  } catch (std::runtime_error &) {}
  return 0;
}

template<typename T>
int qi_value_get_pod(qi_value_t *msg, T *result) {
  qi::AnyValue &gv = qi_value_cpp(msg);
  try {
    *result = gv.to<T>();
    return 1;
  } catch (std::runtime_error &) {
  }
  return 0;
}

template<typename T>
T qi_value_get_pod_default(qi_value_t *msg, T defvalue) {
  qi::AnyValue &gv = qi_value_cpp(msg);
  try {
    return gv.to<T>();
  } catch (std::runtime_error &) {
  }
  return defvalue;
}

#ifdef __cplusplus
extern "C"
{
#endif

qi_value_t* qi_value_create(const char *signature)
{
  qi::AnyValue* v;

  if (!signature || !strcmp(signature, ""))
    v = new qi::AnyValue();
  else {
    //TODO: check signature correctness
    qi::TypeInterface *type = qi::TypeInterface::fromSignature(signature);
    v = new qi::AnyValue(type);
  }
  return (qi_value_t*)v;
}

qi_value_t *qi_value_create_from_type(qi_type_t *type)
{
  auto t = reinterpret_cast<qi::TypeInterface*>(type);
  return reinterpret_cast<qi_value_t*>(new qi::AnyValue(t));
}

void qi_value_destroy(qi_value_t* val)
{
  qi::AnyValue *v = &qi_value_cpp(val);
  delete v;
}

int         qi_value_reset(qi_value_t* value, const char *signature)
{
  qi::AnyValue *v = &qi_value_cpp(value);
  if (!signature || !strcmp(signature, ""))
    v->reset();
  else {
    //TODO: check signature correctness
    qi::TypeInterface *type = qi::TypeInterface::fromSignature(signature);
    v->reset(type);
  }
  return 1;
}

void        qi_value_swap(qi_value_t* dest, qi_value_t* src)
{
  qi::AnyValue *s = &qi_value_cpp(src);
  qi::AnyValue *d = &qi_value_cpp(dest);
  s->swap(*d);
}

qi_value_t* qi_value_copy(qi_value_t* src)
{
  qi::AnyValue *s = &qi_value_cpp(src);
  qi::AnyValue *d = new qi::AnyValue;
  *d = *s;
  return (qi_value_t*)d;
}


qi_value_kind_t qi_value_get_kind(qi_value_t* value) {
  if (!value)
    return QI_VALUE_KIND_UNKNOWN;
  qi::AnyValue &val = qi_value_cpp(value);
  return (qi_value_kind_t)(val.kind());
}

const char*     qi_value_get_signature(qi_value_t* value, int resolveDynamics)
{
  qi::AnyValue &val = qi_value_cpp(value);
  return qi::os::strdup(val.signature(!!resolveDynamics).toString().c_str());
}

#define QI_CONCAT(a, b) a##b
#define QI_CONCAT3(a, b, c) a##b##c
#define IMPL_POD_SET_GET(name, type)                                           \
  QIC_API int QI_CONCAT(qi_value_set_, name)(qi_value_t * value, type val)     \
  {                                                                            \
    return qi_value_set_pod<type>(value, val);                                 \
  }                                                                            \
  QIC_API int QI_CONCAT(qi_value_get_, name)(qi_value_t * value, type * val)   \
  {                                                                            \
    return qi_value_get_pod<type>(value, val);                                 \
  }                                                                            \
  QIC_API type QI_CONCAT3(qi_value_get_, name, _default)(                      \
      qi_value_t * value, type defVal)                                         \
  {                                                                            \
    return qi_value_get_pod_default<type>(value, defVal);                      \
  }

IMPL_POD_SET_GET(int8, signed char)
IMPL_POD_SET_GET(int16, short)
IMPL_POD_SET_GET(int32, int)
IMPL_POD_SET_GET(int64, long long)

IMPL_POD_SET_GET(uint8, unsigned char)
IMPL_POD_SET_GET(uint16, unsigned short)
IMPL_POD_SET_GET(uint32, unsigned int)
IMPL_POD_SET_GET(uint64, unsigned long long)

IMPL_POD_SET_GET(float, float)
IMPL_POD_SET_GET(double, double)

#undef IMPL_POD_SET_GET
#undef QI_CONCAT
#undef QI_CONCAT3

//# STRING
int        qi_value_set_string(qi_value_t *container, const char *s)
{
  qi::AnyValue &gv = qi_value_cpp(container);
  try {
    gv.setString(s);
    return 1;
  } catch (std::runtime_error &) {
  }
  return 0;
}

const char* qi_value_get_string(qi_value_t *msg)
{
  if (!msg)
    return 0;
  qi::AnyValue &gv = qi_value_cpp(msg);
  try {
    return qi::os::strdup(gv.toString().c_str());
  } catch (std::runtime_error &) {
  }
  return 0;
}

//# TUPLE
int        qi_value_tuple_set(qi_value_t *msg, unsigned int idx, qi_value_t *value)
{
  qi::AnyValue &container = qi_value_cpp(msg);
  qi::AnyValue &val = qi_value_cpp(value);

  try {
    if (container.kind() != qi::TypeKind_Tuple || idx >= container.size()) {
      return 0;
    }
    container[idx].update(val);
    return 1;
  } catch (std::runtime_error &) {}
  return 0;
}

qi_value_t*  qi_value_tuple_get(qi_value_t *msg, unsigned int idx) {
  qi::AnyValue &container = qi_value_cpp(msg);
  if (container.kind() != qi::TypeKind_Tuple || idx >= container.size()) {
    return 0;
  }
  qi_value_t* ret = qi_value_create("");
  qi::AnyValue &gv = qi_value_cpp(ret);
  gv = container[idx];
  return ret;
}

int          qi_value_tuple_size(qi_value_t *msg)
{
  qi::AnyValue &container = qi_value_cpp(msg);
  if (container.kind() != qi::TypeKind_Tuple) {
    return -1;
  }
  return container.size();
}

//# LIST
int          qi_value_list_set(qi_value_t *msg, unsigned int idx, qi_value_t *value)
{
  qi::AnyValue &container = qi_value_cpp(msg);
  qi::AnyValue &val = qi_value_cpp(value);

  if ((container.kind() != qi::TypeKind_VarArgs && container.kind() != qi::TypeKind_List) || idx >= container.size()) {
    return 0;
  }
  try {
    container[idx].set(val);
    return 1;
  } catch (std::runtime_error &e) {
    qiLogError() << "Can't set list item at index " << idx << " :" << e.what();
  }
  return 0;
}

qi_value_t*  qi_value_list_get(qi_value_t *msg, unsigned int idx)
{
  qi::AnyValue &container = qi_value_cpp(msg);
  if ((container.kind() != qi::TypeKind_VarArgs && container.kind() != qi::TypeKind_List) || idx >= container.size()) {
    return 0;
  }
  qi_value_t* ret = qi_value_create("");
  qi::AnyValue &gv = qi_value_cpp(ret);
  gv = container[idx];
  return ret;
}

int  qi_value_list_push_back(qi_value_t *msg, qi_value_t*val)
{
  qi::AnyValue &container = qi_value_cpp(msg);
  qi::AnyValue &gval = qi_value_cpp(val);
  if (container.kind() != qi::TypeKind_List && container.kind() != qi::TypeKind_VarArgs) {
    return 0;
  }
  container.append(gval);
  return 1;
}

int          qi_value_list_size(qi_value_t *msg)
{
  qi::AnyValue &container = qi_value_cpp(msg);
  if (container.kind() != qi::TypeKind_List && container.kind() != qi::TypeKind_VarArgs) {
    return -1;
  }
  return container.size();
}

//# OBJECT
qi_object_t* qi_value_get_object(qi_value_t* val)
{
  qi::AnyValue &gv = qi_value_cpp(val);
  try {
    qi::AnyObject obj = gv.as<qi::AnyObject>();
    return qi_object_create_from(obj);
  } catch(std::runtime_error) {}
  return 0;
}

int          qi_value_set_object(qi_value_t* value, qi_object_t *o)
{
  qi::AnyValue &gv = qi_value_cpp(value);
  qi::AnyObject &obj = qi_object_cpp(o);
  try {
    gv.set<qi::AnyObject>(obj);
    return 1;
  } catch (std::runtime_error&) {}
  return 0;
}

//# DYNAMIC
int          qi_value_dynamic_set(qi_value_t *container, qi_value_t* value)
{
  qi::AnyValue &gvcont = qi_value_cpp(container);
  qi::AnyValue &gvval = qi_value_cpp(value);
  try {
    gvcont.setDynamic(gvval.asReference());
    return 1;
  } catch (std::runtime_error&) {}
  return 0;
}

qi_value_t*  qi_value_dynamic_get(qi_value_t *container)
{
  qi::AnyValue &gv = qi_value_cpp(container);
  try {
    qi::AnyReference gvp = gv.content();
    qi_value_t *ret = qi_value_create("");
    qi::AnyValue &val = qi_value_cpp(ret);
    val = gvp.clone();
    return ret;
  } catch (std::runtime_error&) {}
  return 0;
}

//# MAP
unsigned int qi_value_map_size(qi_value_t *msg)
{
  qi::AnyValue &container = qi_value_cpp(msg);
  if (container.kind() != qi::TypeKind_Map) {
    return -1;
  }
  return container.size();
}

int         qi_value_map_set(qi_value_t *msg, qi_value_t *key, qi_value_t *value)
{
  qi::AnyValue &container = qi_value_cpp(msg);
  qi::AnyValue &k = qi_value_cpp(key);
  qi::AnyValue &val = qi_value_cpp(value);

  if (container.kind() != qi::TypeKind_Map) {
    return 0;
  }
  try {
    container[k].update(val);
    return 1;
  } catch (std::runtime_error &) {}
  return 0;
}

qi_value_t*  qi_value_map_get(qi_value_t *msg, qi_value_t *key)
{
  qi::AnyValue &container = qi_value_cpp(msg);
  qi::AnyValue &k = qi_value_cpp(key);

  qi::AnyReference r;
  if (container.kind() != qi::TypeKind_Map) {
    return 0;
  }
  try {
    r = container[k.asReference()];
    qi_value_t* ret = qi_value_create("");
    qi::AnyValue &gv = qi_value_cpp(ret);
    gv = r;
    return ret;
  } catch (std::runtime_error &) {}
  return 0;
}

qi_value_t* qi_value_map_keys(qi_value_t *msg)
{
  qi::AnyValue &container = qi_value_cpp(msg);

  std::map<qi::AnyReference, qi::AnyReference>::iterator it;
  std::map<qi::AnyReference, qi::AnyReference> m = container.asMapValuePtr();

  qi::TypeInterface* listType = qi::makeListType(((qi::MapTypeInterface*)container.type())->keyType());
  qi::AnyValue* ar = new qi::AnyValue(listType);
  //construct a list<k>
  for (it = m.begin(); it != m.end(); ++it) {
    ar->append(it->first);
  }
  return (qi_value_t*)(ar);
}

//# RAW
int          qi_value_raw_set(qi_value_t* value, const char* data, int size){
  return 0;
}

int          qi_value_raw_get(qi_value_t* value, const char**data, int *size) {
  return 0;
}

qi_type_t*   qi_value_get_type(qi_value_t* value) {
  qi::AnyValue &val = qi_value_cpp(value);
  return (qi_type_t*)val.type();
}


#ifdef __cplusplus
}
#endif
