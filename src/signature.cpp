#include <qic/signature.h>
#include <qi/signature.hpp>

qi_signature_t *qi_signature_create()
{
  return reinterpret_cast<qi_signature_t*>(new qi::Signature());
}

qi_signature_t *qi_signature_from_string(const char *sig)
{
  return reinterpret_cast<qi_signature_t*>(new qi::Signature(sig));
}

void qi_signature_destroy(qi_signature_t *signature)
{
  auto s = reinterpret_cast<qi::Signature*>(signature);
  delete s;
}

qi_signature_t *qi_signature_clone(qi_signature_t *signature)
{
  auto s = reinterpret_cast<qi::Signature*>(signature);
  return reinterpret_cast<qi_signature_t*>(new qi::Signature(*s));
}

const char *qi_signature_to_string(qi_signature_t *signature)
{
  auto s = reinterpret_cast<qi::Signature*>(signature);
  return s->toString().c_str();
}
