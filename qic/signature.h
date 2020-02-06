#ifndef QIC_SIGNATURE_H
#define QIC_SIGNATURE_H

#include <qic/api.h>

#ifdef __cplusplus
extern "C"
{
#endif

QIC_API qi_signature_t*   qi_signature_create();
QIC_API qi_signature_t*   qi_signature_from_string(const char* sig);
QIC_API void              qi_signature_destroy(qi_signature_t* signature);
QIC_API qi_signature_t*   qi_signature_clone(qi_signature_t* signature);

// return the signature inner string reference
QIC_API const char*       qi_signature_to_string(qi_signature_t* signature);

#ifdef __cplusplus
}
#endif

#endif // QIC_SIGNATURE_H
