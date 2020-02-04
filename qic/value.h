/*
**
** Author(s):
**  - Cedric GESTES <gestes@aldebaran-robotics.com>
**  - Laurent LEC <llec@aldebaran-robotics.com>
**
** Copyright (C) 2010, 2011, 2013 Aldebaran Robotics
*/

#ifndef _QIC_VALUE_H_
#define _QIC_VALUE_H_

#include <qic/api.h>

#ifdef __cplusplus
extern "C"
{
#endif



  //this follow qi::TypeKind
  typedef enum {
  QI_VALUE_KIND_INVALID  = -1,
  QI_VALUE_KIND_UNKNOWN  = 0,
  QI_VALUE_KIND_VOID     = 1,
  QI_VALUE_KIND_INT      = 2,
  QI_VALUE_KIND_FLOAT    = 3,
  QI_VALUE_KIND_STRING   = 4,
  QI_VALUE_KIND_LIST     = 5,
  QI_VALUE_KIND_MAP      = 6,
  QI_VALUE_KIND_OBJECT   = 7,
  QI_VALUE_KIND_POINTER  = 8,
  QI_VALUE_KIND_TUPLE    = 9,
  QI_VALUE_KIND_DYNAMIC  = 10,
  QI_VALUE_KIND_RAW      = 11,
  QI_VALUE_KIND_ITERATOR = 13,
  QI_VALUE_KIND_FUNCTION = 14,
  QI_VALUE_KIND_SIGNAL = 15,
  QI_VALUE_KIND_PROPERTY = 16,
  QI_VALUE_KIND_VARARGS = 17,
  QI_VALUE_KIND_OPTIONAL = 18,
  } qi_value_kind_t;


  //# CTOR/DTOR/COPY
  QIC_API qi_value_t* qi_value_create(const char *sig);
  QIC_API void        qi_value_destroy(qi_value_t *v);


  QIC_API int         qi_value_reset(qi_value_t* value, const char *sig);
  QIC_API void        qi_value_swap(qi_value_t* dest, qi_value_t* src);
  QIC_API qi_value_t* qi_value_copy(qi_value_t* src);

  //# TYPE
  QIC_API qi_value_kind_t qi_value_get_kind(qi_value_t* value);
  QIC_API const char*     qi_value_get_signature(qi_value_t* value, int resolveDynamics);
  QIC_API qi_type_t*      qi_value_get_type(qi_value_t* value);

#define QI_CONCAT(a, b) a##b
#define QI_CONCAT3(a, b, c) a##b##c
#define DEF_POD_SET_GET(name, type)                                            \
  QIC_API int QI_CONCAT(qi_value_set_, name)(qi_value_t * value, type val);    \
  QIC_API int QI_CONCAT(qi_value_get_, name)(qi_value_t * value, type * val);  \
  QIC_API type QI_CONCAT3(qi_value_get_, name, _default)(                      \
      qi_value_t * value, type defVal);

  DEF_POD_SET_GET(int8, signed char)
  DEF_POD_SET_GET(int16, short)
  DEF_POD_SET_GET(int32, int)
  DEF_POD_SET_GET(int64, long long)

  DEF_POD_SET_GET(uint8, unsigned char)
  DEF_POD_SET_GET(uint16, unsigned short)
  DEF_POD_SET_GET(uint32, unsigned int)
  DEF_POD_SET_GET(uint64, unsigned long long)

  DEF_POD_SET_GET(float, float)
  DEF_POD_SET_GET(double, double)

#undef DEF_POD_SET_GET
#undef QI_CONCAT
#undef QI_CONCAT3

  //# STRING
  QIC_API int         qi_value_set_string(qi_value_t *value, const char *s);
  //return a copy that should be freed
  QIC_API const char* qi_value_get_string(qi_value_t *value);


  //# LIST
  QIC_API int          qi_value_list_set(qi_value_t *container, unsigned int idx, qi_value_t *element);
  QIC_API qi_value_t*  qi_value_list_get(qi_value_t *container, unsigned int idx);
  QIC_API int          qi_value_list_push_back(qi_value_t* msg, qi_value_t* val);
  QIC_API int          qi_value_list_size(qi_value_t *container);

  //# MAP
  QIC_API unsigned int qi_value_map_size(qi_value_t *msg);
  QIC_API int          qi_value_map_set(qi_value_t *msg, qi_value_t *key, qi_value_t *value);
  QIC_API qi_value_t*  qi_value_map_get(qi_value_t *msg, qi_value_t *key);
  QIC_API qi_value_t*  qi_value_map_keys(qi_value_t *msg);

  //# OBJECT
  QIC_API qi_object_t* qi_value_get_object(qi_value_t* value);
  QIC_API int          qi_value_set_object(qi_value_t* value, qi_object_t *obj);

  //# TUPLE
  QIC_API int          qi_value_tuple_set(qi_value_t *container, unsigned int idx, qi_value_t *element);
  QIC_API qi_value_t*  qi_value_tuple_get(qi_value_t *container, unsigned int idx);
  QIC_API int          qi_value_tuple_size(qi_value_t *container);

  //# DYNAMIC
  QIC_API int          qi_value_dynamic_set(qi_value_t *container, qi_value_t* value);
  QIC_API qi_value_t*  qi_value_dynamic_get(qi_value_t *container);

  //# RAW
  QIC_API int          qi_value_raw_set(qi_value_t* value, const char* data, int size);
  QIC_API int          qi_value_raw_get(qi_value_t* value, const char**data, int *size);


#ifdef __cplusplus
}
#endif

#endif  // _QIMESSAGING_MESSAGE_H_
