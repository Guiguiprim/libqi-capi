/*
**
** Author(s):
**  - Laurent LEC <llec@aldebaran-robotics.com>
**
** Copyright (C) 2012 Aldebaran Robotics
*/

#include <qic/session.h>
#include <qic/object.h>
#include <qic/future.h>
#include "future_p.h"

#include <string.h>
#include <qi/session.hpp>
#include <qi/messaging/serviceinfo.hpp>
#include <qi/messaging/authprovider.hpp>
#include <qi/messaging/clientauthenticator.hpp>
#include <qi/messaging/clientauthenticatorfactory.hpp>

namespace {
class PassAuthenticator : public qi::ClientAuthenticator
{
public:
  PassAuthenticator(std::string const &user, std::string const &pwd)
    : _user(user)
    , _pwd(pwd)
  {
  }

  qi::CapabilityMap initialAuthData() override
  {
    qi::CapabilityMap map;
    map["user"] = qi::AnyValue::from(_user);
    map["token"] = qi::AnyValue::from(_pwd);
    return map;
  }

protected:
  qi::CapabilityMap _processAuth(const qi::CapabilityMap &authData) override
  {
    qi::CapabilityMap result;
    auto const found = authData.find(qi::AuthProvider::State_Key);
    if(found == authData.end())
      return result;

    auto const state = found->second.to<qi::AuthProvider::State>();
    switch(state)
    {
    case qi::AuthProvider::State_Cont:
      result["token"] = qi::AnyValue::from(_pwd);
      break;
    case qi::AuthProvider::State_Done:
      break;
    case qi::AuthProvider::State_Error:
      break;
    }

    return result;
  }

private:
  const std::string _user;
  const std::string _pwd;
};

class PassAuthenticatorFactory : public qi::ClientAuthenticatorFactory
{
public:
  PassAuthenticatorFactory(std::string const &user, std::string const &pwd)
    : _user(user)
    , _pwd(pwd)
  {
  }

  qi::ClientAuthenticatorPtr newAuthenticator() override
  {
    return boost::make_shared<PassAuthenticator>(_user, _pwd);
  }

  static qi::ClientAuthenticatorFactoryPtr make(
      std::string const &user, std::string const &pwd)
  {
    return qi::ClientAuthenticatorFactoryPtr(
        new PassAuthenticatorFactory(user, pwd));
  }

private:
  const std::string _user;
  const std::string _pwd;
};

qi::Session *get_session_cpp(qi_session_t* s) {
  qi::SessionPtr &session = (*reinterpret_cast<qi::SessionPtr*>(s));
  return session.get();
}

} // namespace

#ifdef __cplusplus
extern "C"
{
#endif

qi_session_t *qi_session_create()
{
  qi::SessionPtr *session = new qi::SessionPtr(qi::makeSession());
  return reinterpret_cast<qi_session_t*>(session);
}

void qi_session_destroy(qi_session_t *session)
{
  qi::SessionPtr *s  = reinterpret_cast<qi::SessionPtr*>(session);
  delete s;
}

int qi_session_is_connected(qi_session_t *session)
{
  qi::Session *s = get_session_cpp(session);
  return s->isConnected();
}

qi_future_t* qi_session_connect(qi_session_t *session, const char *address)
{
  qi::Session *s = get_session_cpp(session);
  return qi_future_wrap(s->connect(address));
}

qi_future_t *qi_session_connect_with_authentication(qi_session_t *session,
    const char *address, const char *user, const char *token)
{
  qi::Session *s = get_session_cpp(session);
  s->setClientAuthenticatorFactory(PassAuthenticatorFactory::make(user, token));
  return qi_future_wrap(s->connect(address));
}

char * qi_session_url(qi_session_t* session)
{
  qi::Session *s = get_session_cpp(session);
  return qi::os::strdup(s->url().str().c_str());

}

int qi_session_set_identity(qi_session_t *session, char *key, char *crt)
{
  qi::Session *s = get_session_cpp(session);
  return (int)s->setIdentity(key, crt);
}


qi_future_t* qi_session_close(qi_session_t *session)
{
  qi::Session *s = get_session_cpp(session);
  return qi_future_wrap(s->close());
}

qi_future_t* qi_session_get_service(qi_session_t *session, const char *name)
{
  qi::Session *s = get_session_cpp(session);
  return qi_future_wrap(s->service(name));
}

qi_future_t* qi_session_get_services(qi_session_t *session)
{
  qi::Session *s = get_session_cpp(session);
  return qi_future_wrap(s->services());
}

qi_future_t* qi_session_listen(qi_session_t *session, const char *address)
{
  qi::Session *s = get_session_cpp(session);
  return qi_future_wrap(s->listen(address));
}

qi_future_t* qi_session_listen_standalone(qi_session_t *session, const char *address)
{
  qi::Session *s = get_session_cpp(session);
  return qi_future_wrap(s->listenStandalone(address));
}

qi_value_t*  qi_session_endpoints(qi_session_t *session)
{
  qi::Session *s = get_session_cpp(session);

  std::vector<qi::Url> eps = s->endpoints();
  std::vector<std::string> epss;

  std::vector<qi::Url>::iterator it;
  for (it = eps.begin(); it != eps.end(); ++it) {
    epss.push_back(it->str());
  }
  qi_value_t *val = qi_value_create("");
  qi::AnyValue &gvp = qi_value_cpp(val);
  gvp = qi::AnyValue::from(epss);
  return val;
}

qi_future_t* qi_session_register_service(qi_session_t *session, const char *name, qi_object_t *object)
{
  qi::Session *s = get_session_cpp(session);
  qi::AnyObject  *obj = reinterpret_cast<qi::AnyObject *>(object);
  return qi_future_wrap(s->registerService(name, *obj));
}

qi_future_t* qi_session_unregister_service(qi_session_t *session, unsigned int idx)
{
  qi::Session *s = get_session_cpp(session);
  return qi_future_wrap(s->unregisterService(idx));
}

#ifdef __cplusplus
}
#endif
