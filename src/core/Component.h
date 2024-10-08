#pragma once

#define ETL_MESSAGE_TIMER_USE_INTERRUPT_LOCK
#define ETL_MESSAGE_TIMER_ENABLE_INTERRUPTS \
  {}
#define ETL_MESSAGE_TIMER_DISABLE_INTERRUPTS \
  {}

#include "debug.h"
#include "etl/list.h"
#include "etl/message.h"
#include "etl/message_bus.h"
#include "etl/message_router.h"
#include "etl/message_timer.h"
#include "etl/timer.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <functional>

void ____loop() {
  loop();
}

namespace Core {

enum {
  MESSAGE_LOOP,
  MESSAGE_GET_STATUS,
  MESSAGE_SET_CONFIG,
  MESSAGE_TIMER,
  MESSAGE_COMMAND,
  MESSAGE_PUBLISH,
  MESSAGE_GET_INFO,
  MESSAGE_ART_COMMAND,
};

enum {
  COMPONENT_CLASS_GENERIC,
  COMPONENT_CLASS_SENSOR,
  COMPONENT_CLASS_ACTUATOR,
  COMPONENT_CLASS_COMMUNICATION,
  COMPONENT_CLASS_POWER_MANAGER,
  COMPONENT_CLASS_NETWORK,
};

typedef enum {
  TIMER_TYPE_250_MS,
  TIMER_TYPE_1000_MS,
  TIMER_TYPE_COUNT
} timer_type_t;

typedef enum {
  COMMAND_TYPE_TUNE,
  COMMAND_TYPE_ACTUATE,
  COMMAND_TYPE_CONFIG,
  COMMAND_TYPE_RESTART,
  COMMAND_TYPE_ARTESIAN,
  COMMAND_TYPE_GET_DATA,
  COMMAND_TYPE_ON,
  COMMAND_TYPE_OFF,
  COMMAND_SET_CONTROL_PARAMS,
  COMMAND_TYPE_UNKNOWN,
} command_type_t;

struct MessageLoop : public etl::message<MESSAGE_LOOP> {};
static MessageLoop msg_loop = {};

struct MessageGetStatus : public etl::message<MESSAGE_GET_STATUS> {
  MessageGetStatus(JsonObject &doc_) : doc(doc_){};
  JsonObject &doc;
};

struct MessageSetConfig : public etl::message<MESSAGE_SET_CONFIG> {
  MessageSetConfig(const JsonObject &doc_) : doc(doc_){};
  const JsonObject &doc;
};

struct MessageTimer : public etl::message<MESSAGE_TIMER> {
  MessageTimer(const timer_type_t type) : type(type){};
  const timer_type_t type;
};

struct MessageCommand : public etl::message<MESSAGE_COMMAND> {
  MessageCommand(const command_type_t type, const JsonObject &doc_) : type(type), doc(doc_){};
  const command_type_t type;
  const JsonObject &doc;
};

struct MessagePublish : public etl::message<MESSAGE_PUBLISH> {
  MessagePublish(const char *topic_, const JsonDocument &doc_) : topic(topic_), doc(doc_){};
  const char *topic;
  const JsonDocument &doc;
};

struct MessageGetInfo : public etl::message<MESSAGE_GET_INFO> {
  MessageGetInfo(JsonObject &doc_) : doc(doc_){};
  JsonObject &doc;
};

struct MessageArtCommand : public etl::message< MESSAGE_ART_COMMAND> {
  MessageArtCommand(const command_type_t type, const JsonObject &doc_, const JsonObject &reply_) : type(type), doc(doc_), reply(reply_) {};
  const command_type_t type;
  const JsonObject &doc;
  const JsonObject &reply;
};


typedef etl::message_bus<16> MessageBus_t;
typedef etl::message_timer<TIMER_TYPE_COUNT> MessageTimer_t;

static MessageBus_t bus;
static MessageTimer_t timer;

class Component : public etl::message_router<Component, MessageLoop, MessageGetStatus, MessageSetConfig, MessageTimer, MessageCommand, MessagePublish, MessageGetInfo, MessageArtCommand> {
private:
  MessageBus_t *_bus;

protected:
  command_type_t parseCommand(String &cmd) {
    if (cmd == "tune") {
      return COMMAND_TYPE_TUNE;
    }
    if (cmd == "actuate") {
      return COMMAND_TYPE_ACTUATE;
    }
    if (cmd == "config") {
      return COMMAND_TYPE_CONFIG;
    }
    if (cmd == "restart") {
      return COMMAND_TYPE_RESTART;
    }
    if (cmd == "getData") {
      return COMMAND_TYPE_GET_DATA;
    }
    if (cmd == "on") {
      return COMMAND_TYPE_ON;
    }
    if (cmd == "off") {
      return COMMAND_TYPE_OFF;
    }
    if (cmd == "setControlParams") {
      return COMMAND_SET_CONTROL_PARAMS;
    }
    return COMMAND_TYPE_UNKNOWN;
  }

public:
  Component(etl::message_router_id_t id) : message_router(id) {
    _bus = &bus;
    _bus->subscribe(*this);
  };

  void on_receive_unknown(const etl::imessage &msg){};
  void on_receive_unknown(etl::imessage_router &sender, const etl::imessage &msg){};

  void on_receive(etl::imessage_router &sender, const MessageLoop &msg) { loop(); }
  void on_receive(const MessageLoop &msg) { loop(); };

  void on_receive(etl::imessage_router &sender, const MessageGetStatus &msg) { getStatus(msg.doc); }
  void on_receive(const MessageGetStatus &msg) { getStatus(msg.doc); };

  void on_receive(etl::imessage_router &sender, const MessageSetConfig &msg) { setConfig(msg.doc); }
  void on_receive(const MessageSetConfig &msg) { setConfig(msg.doc); };

  void on_receive(etl::imessage_router &sender, const MessageTimer &msg) { on_receive(msg); }
  void on_receive(const MessageTimer &msg) {
    switch (msg.type) {
    case TIMER_TYPE_250_MS:
      timer250();
      break;
    case TIMER_TYPE_1000_MS:
      timer1000();
      break;
    default:
      break;
    }
  };
  void on_receive(etl::imessage_router &sender, const MessageCommand &msg) { on_receive(msg); }
  void on_receive(const MessageCommand &msg) { onCommand(msg.type, msg.doc); };

  void on_receive(etl::imessage_router &sender, const MessagePublish &msg) { on_receive(msg); }
  void on_receive(const MessagePublish &msg) { onPublish(msg.topic, msg.doc); };

  void on_receive(etl::imessage_router &sender, const MessageGetInfo &msg) { getInfo(msg.doc); }
  void on_receive(const MessageGetInfo &msg) { getInfo(msg.doc); };

  void on_receive(etl::imessage_router &sender, const MessageArtCommand &msg) { on_receive(msg); }
  void on_receive(const MessageArtCommand &msg) { onArtCommand(msg.type, msg.doc, msg.reply); };

  virtual void getStatus(JsonObject &doc) const {};
  virtual void setConfig(const JsonObject &doc){};
  virtual void onCommand(command_type_t type, const JsonObject &doc){};
  virtual void onPublish(const char *topic, const JsonDocument &doc){};
  virtual void getInfo(JsonObject &doc) const {};

  virtual void onArtCommand(command_type_t type, const JsonObject &doc, const JsonObject &reply){};

  virtual void init(){};
  virtual void loop(){};
  virtual void timer250(){};
  virtual void timer1000(){};
  void yeld() const {
    ____loop();
    //_bus->receive(msg_loop);
  };
  void busyWait(std::function<bool(void)> busy) {
    while (busy()) {
      yeld();
    }
  }
  void getGlobalStatus(JsonObject &doc) const {
    MessageGetStatus msg = {doc};
    _bus->receive(msg);
  };
  virtual void broadcastCommand(command_type_t type, const JsonObject &doc) {
    MessageCommand msg = {type, doc};
    _bus->receive(msg);
  };
  void setGlobalConfig(const JsonObject &doc) const {
    MessageSetConfig msg = {doc};
    _bus->receive(msg);
  };
  void publish(const String &topic, const JsonDocument &doc) const {
    MessagePublish msg = {topic.c_str(), doc};
    _bus->receive(msg);
  };
  void getGlobalInfo(JsonObject &doc) const {
    MessageGetInfo msg = {doc};
    _bus->receive(msg);
  };
  virtual void broadcastArtCommand(command_type_t type, const JsonObject &doc, const JsonObject &reply) {
    MessageArtCommand msg = {type, doc, reply};
    _bus->receive(msg);
  };
};

class ComponentManager {
private:
  MessageBus_t *_bus;
  MessageTimer_t *_timer;
  etl::timer::id::type _tm250id;
  etl::timer::id::type _tm1000id;

public:
  ComponentManager() {
    _bus = &bus;
    _timer = &timer;
  }

  void init() {
    static MessageTimer tm250 = {TIMER_TYPE_250_MS};
    static MessageTimer tm1000 = {TIMER_TYPE_1000_MS};
    _tm250id = _timer->register_timer(tm250, bus, 250, true);
    _tm1000id = _timer->register_timer(tm1000, bus, 1000, true);
    _timer->enable(true);
    _timer->start(_tm250id);
    _timer->start(_tm1000id);
  };

  void loop() const {
    static uint32_t lastMillis = millis();
    uint32_t currentMillis = millis();
    _timer->tick(constrain(currentMillis - lastMillis, 0, 250));
    lastMillis = currentMillis;
    _bus->receive(msg_loop);
  };

  void getStatus(JsonObject &doc) const {
    static MessageGetStatus msg = {doc};
    _bus->receive(msg);
  };
};

} // namespace Core