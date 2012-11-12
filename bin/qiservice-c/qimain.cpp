/*
** Author(s):
**  - Pierre Roullon <proullon@aldebaran-robotics.com>
**
** Copyright (C) 2012 Aldebaran Robotics
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <qimessaging/c/qi_c.h>

void reply(const char *signature, qi_message_t *message, qi_message_t *answer, void *data)
{
  char* msg = qi_message_read_string(message);
  char* rep = (char *) malloc(strlen(msg) + 4);

  memcpy(rep, msg, strlen(msg) + 1);
  printf("Message recv: %s\n", msg);
  strcat(rep, "bim");

  qi_message_write_string(answer, rep);
}

int		main(int argc, char **argv)
{
  qi_application_t* app = qi_application_create(&argc, argv);
  char*     sd_addr = 0;

  // get the program options
  if (argc != 2)
  {
    printf("Usage : ./qi-service-c master-address\n");
    printf("Assuming master address is tcp://127.0.0.1:5555\n");
    sd_addr = strdup("tcp://127.0.0.1:5555");
  }
  else
    sd_addr = argv[1];

  qi_object_builder_t* ob = qi_object_builder_create();
  qi_object_builder_register_method(ob, "reply::s(s)", &reply, 0);

  qi_session_t* session = qi_session_create();

  qi_session_connect(session, sd_addr);

  qi_session_listen(session, "tcp://0.0.0.0:0");
  qi_object_t *object = qi_object_builder_get_object(ob);
  unsigned int id = qi_session_register_service(session, "serviceTest", object);

  if (!id)
  {
    printf("registration failed...\n");
    exit(1);
  }
  else
    printf("Registered as service #%d\n", id);

  qi_application_run(app);
  qi_session_unregister_service(session, id);
  qi_object_builder_destroy(ob);
  qi_object_destroy(object);
  qi_session_destroy(session);
  qi_application_destroy(app);
  return (0);
}
