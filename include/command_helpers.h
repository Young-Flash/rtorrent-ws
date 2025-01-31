// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2005-2011, Jari Sundell <jaris@ifi.uio.no>

#ifndef RTORRENT_UTILS_COMMAND_HELPERS_H
#define RTORRENT_UTILS_COMMAND_HELPERS_H

#include "rpc/command.h"
#include "rpc/object_storage.h"
#include "rpc/parse_commands.h"

#include "globals.h"

void
initialize_commands();

void
cleanup_commands();

//
// New std::function based command_base helper functions:
//

#define CMD2_A_FUNCTION(key, function, slot, parm, doc, is_readonly)           \
  do {                                                                         \
    if (is_readonly)                                                           \
      rpc::readonly_command.insert(key);                                       \
    rpc::commands.insert_slot<rpc::command_base_is_type<rpc::function>::type>( \
      key,                                                                     \
      slot,                                                                    \
      &rpc::function,                                                          \
      rpc::CommandMap::flag_dont_delete | rpc::CommandMap::flag_public,        \
      NULL,                                                                    \
      NULL);                                                                   \
  } while (0)

#define CMD2_A_FUNCTION_PRIVATE(key, function, slot, parm, doc, is_readonly)   \
  do {                                                                         \
    rpc::commands.insert_slot<rpc::command_base_is_type<rpc::function>::type>( \
      key,                                                                     \
      slot,                                                                    \
      &rpc::function,                                                          \
      rpc::CommandMap::flag_dont_delete,                                       \
      NULL,                                                                    \
      NULL);                                                                   \
  } while (0)

#define CMD2_ANY(key, slot, is_readonly)                                       \
  CMD2_A_FUNCTION(                                                             \
    key, command_base_call<rpc::target_type>, slot, "i:", "", is_readonly)

#define CMD2_ANY_P(key, slot, is_readonly)                                     \
  CMD2_A_FUNCTION_PRIVATE(                                                     \
    key, command_base_call<rpc::target_type>, slot, "i:", "", is_readonly)

#define CMD2_ANY_VOID(key, slot, is_readonly)                                  \
  CMD2_A_FUNCTION(key,                                                         \
                  command_base_call<rpc::target_type>,                         \
                  object_convert_void(slot),                                   \
                  "i:",                                                        \
                  "",                                                          \
                  is_readonly)

#define CMD2_ANY_V(key, slot, is_readonly)                                     \
  CMD2_A_FUNCTION(key,                                                         \
                  command_base_call_list<rpc::target_type>,                    \
                  object_convert_void(slot),                                   \
                  "i:",                                                        \
                  "",                                                          \
                  is_readonly)

#define CMD2_ANY_L(key, slot, is_readonly)                                     \
  CMD2_A_FUNCTION(key,                                                         \
                  command_base_call_list<rpc::target_type>,                    \
                  slot,                                                        \
                  "A:",                                                        \
                  "",                                                          \
                  is_readonly)

#define CMD2_ANY_VALUE(key, slot, is_readonly)                                 \
  CMD2_A_FUNCTION(key,                                                         \
                  command_base_call_value<rpc::target_type>,                   \
                  slot,                                                        \
                  "i:i",                                                       \
                  "",                                                          \
                  is_readonly)

#define CMD2_ANY_VALUE_V(key, slot, is_readonly)                               \
  CMD2_A_FUNCTION(key,                                                         \
                  command_base_call_value<rpc::target_type>,                   \
                  object_convert_void(slot),                                   \
                  "i:i",                                                       \
                  "",                                                          \
                  is_readonly)

#define CMD2_ANY_VALUE_KB(key, slot, is_readonly)                              \
  CMD2_A_FUNCTION(key,                                                         \
                  command_base_call_value_kb<rpc::target_type>,                \
                  object_convert_void(slot),                                   \
                  "i:i",                                                       \
                  "",                                                          \
                  is_readonly)

#define CMD2_ANY_STRING(key, slot, is_readonly)                                \
  CMD2_A_FUNCTION(key,                                                         \
                  command_base_call_string<rpc::target_type>,                  \
                  slot,                                                        \
                  "i:s",                                                       \
                  "",                                                          \
                  is_readonly)

#define CMD2_ANY_STRING_V(key, slot, is_readonly)                              \
  CMD2_A_FUNCTION(key,                                                         \
                  command_base_call_string<rpc::target_type>,                  \
                  object_convert_void(slot),                                   \
                  "i:s",                                                       \
                  "",                                                          \
                  is_readonly)

#define CMD2_ANY_LIST(key, slot, is_readonly)                                  \
  CMD2_A_FUNCTION(key,                                                         \
                  command_base_call_list<rpc::target_type>,                    \
                  slot,                                                        \
                  "i:",                                                        \
                  "",                                                          \
                  is_readonly)

#define CMD2_DL(key, slot, is_readonly)                                        \
  CMD2_A_FUNCTION(                                                             \
    key, command_base_call<core::Download*>, slot, "i:", "", is_readonly)

#define CMD2_DL_V(key, slot, is_readonly)                                      \
  CMD2_A_FUNCTION(key,                                                         \
                  command_base_call<core::Download*>,                          \
                  object_convert_void(slot),                                   \
                  "i:",                                                        \
                  "",                                                          \
                  is_readonly)

#define CMD2_DL_VALUE(key, slot, is_readonly)                                  \
  CMD2_A_FUNCTION(key,                                                         \
                  command_base_call_value<core::Download*>,                    \
                  slot,                                                        \
                  "i:",                                                        \
                  "",                                                          \
                  is_readonly)

#define CMD2_DL_VALUE_V(key, slot, is_readonly)                                \
  CMD2_A_FUNCTION(key,                                                         \
                  command_base_call_value<core::Download*>,                    \
                  object_convert_void(slot),                                   \
                  "i:",                                                        \
                  "",                                                          \
                  is_readonly)

#define CMD2_DL_STRING(key, slot, is_readonly)                                 \
  CMD2_A_FUNCTION(key,                                                         \
                  command_base_call_string<core::Download*>,                   \
                  slot,                                                        \
                  "i:",                                                        \
                  "",                                                          \
                  is_readonly)

#define CMD2_DL_STRING_V(key, slot, is_readonly)                               \
  CMD2_A_FUNCTION(key,                                                         \
                  command_base_call_string<core::Download*>,                   \
                  object_convert_void(slot),                                   \
                  "i:",                                                        \
                  "",                                                          \
                  is_readonly)

#define CMD2_DL_LIST(key, slot, is_readonly)                                   \
  CMD2_A_FUNCTION(                                                             \
    key, command_base_call_list<core::Download*>, slot, "i:", "", is_readonly)

#define CMD2_DL_VALUE_P(key, slot, is_readonly)                                \
  CMD2_A_FUNCTION_PRIVATE(key,                                                 \
                          command_base_call_value<core::Download*>,            \
                          slot,                                                \
                          "i:",                                                \
                          "",                                                  \
                          is_readonly)

#define CMD2_DL_STRING_P(key, slot, is_readonly)                               \
  CMD2_A_FUNCTION_PRIVATE(key,                                                 \
                          command_base_call_string<core::Download*>,           \
                          slot,                                                \
                          "i:",                                                \
                          "",                                                  \
                          is_readonly)

#define CMD2_FILE(key, slot, is_readonly)                                      \
  CMD2_A_FUNCTION(                                                             \
    key, command_base_call<torrent::File*>, slot, "i:", "", is_readonly)

#define CMD2_FILE_V(key, slot, is_readonly)                                    \
  CMD2_A_FUNCTION(key,                                                         \
                  command_base_call<torrent::File*>,                           \
                  object_convert_void(slot),                                   \
                  "i:",                                                        \
                  "",                                                          \
                  is_readonly)

#define CMD2_FILE_VALUE_V(key, slot, is_readonly)                              \
  CMD2_A_FUNCTION(key,                                                         \
                  command_base_call_value<torrent::File*>,                     \
                  object_convert_void(slot),                                   \
                  "i:i",                                                       \
                  "",                                                          \
                  is_readonly)

#define CMD2_FILEITR(key, slot, is_readonly)                                   \
  CMD2_A_FUNCTION(key,                                                         \
                  command_base_call<torrent::FileListIterator*>,               \
                  slot,                                                        \
                  "i:",                                                        \
                  "",                                                          \
                  is_readonly)

#define CMD2_PEER(key, slot, is_readonly)                                      \
  CMD2_A_FUNCTION(                                                             \
    key, command_base_call<torrent::Peer*>, slot, "i:", "", is_readonly)

#define CMD2_PEER_V(key, slot, is_readonly)                                    \
  CMD2_A_FUNCTION(key,                                                         \
                  command_base_call<torrent::Peer*>,                           \
                  object_convert_void(slot),                                   \
                  "i:",                                                        \
                  "",                                                          \
                  is_readonly)

#define CMD2_PEER_VALUE_V(key, slot, is_readonly)                              \
  CMD2_A_FUNCTION(key,                                                         \
                  command_base_call_value<torrent::Peer*>,                     \
                  object_convert_void(slot),                                   \
                  "i:i",                                                       \
                  "",                                                          \
                  is_readonly)

#define CMD2_TRACKER(key, slot, is_readonly)                                   \
  CMD2_A_FUNCTION(                                                             \
    key, command_base_call<torrent::Tracker*>, slot, "i:", "", is_readonly)

#define CMD2_TRACKER_V(key, slot, is_readonly)                                 \
  CMD2_A_FUNCTION(key,                                                         \
                  command_base_call<torrent::Tracker*>,                        \
                  object_convert_void(slot),                                   \
                  "i:",                                                        \
                  "",                                                          \
                  is_readonly)

#define CMD2_TRACKER_VALUE_V(key, slot, is_readonly)                           \
  CMD2_A_FUNCTION(key,                                                         \
                  command_base_call_value<torrent::Tracker*>,                  \
                  object_convert_void(slot),                                   \
                  "i:i",                                                       \
                  "",                                                          \
                  is_readonly)

#define CMD2_VAR_BOOL(key, value, is_readonly)                                 \
  control->object_storage()->insert_c_str(                                     \
    key, int64_t(value), rpc::object_storage::flag_bool_type);                 \
                                                                               \
  CMD2_ANY(key,                                                                \
           ([storage = control->object_storage(),                              \
             raw_key = torrent::raw_string::from_c_str(key)](                  \
              const auto&, const auto&) { return storage->get(raw_key); }),    \
           is_readonly);                                                       \
                                                                               \
  CMD2_ANY_VALUE(key ".set",                                                   \
                 ([storage = control->object_storage(),                        \
                   raw_key = torrent::raw_string::from_c_str(key)](            \
                    const auto&, const auto& object) {                         \
                   return storage->set_bool(raw_key, object);                  \
                 }),                                                           \
                 is_readonly)

#define CMD2_VAR_VALUE(key, value, is_readonly)                                \
  control->object_storage()->insert_c_str(                                     \
    key, int64_t(value), rpc::object_storage::flag_value_type);                \
                                                                               \
  CMD2_ANY(key,                                                                \
           ([storage = control->object_storage(),                              \
             raw_key = torrent::raw_string::from_c_str(key)](                  \
              const auto&, const auto&) { return storage->get(raw_key); }),    \
           is_readonly);                                                       \
                                                                               \
  CMD2_ANY_VALUE(key ".set",                                                   \
                 ([storage = control->object_storage(),                        \
                   raw_key = torrent::raw_string::from_c_str(key)](            \
                    const auto&, const auto& object) {                         \
                   return storage->set_value(raw_key, object);                 \
                 }),                                                           \
                 is_readonly)

#define CMD2_VAR_STRING(key, value, is_readonly)                               \
  control->object_storage()->insert_c_str(                                     \
    key, value, rpc::object_storage::flag_string_type);                        \
                                                                               \
  CMD2_ANY(key,                                                                \
           ([storage = control->object_storage(),                              \
             raw_key = torrent::raw_string::from_c_str(key)](                  \
              const auto&, const auto&) { return storage->get(raw_key); }),    \
           is_readonly);                                                       \
  CMD2_ANY_STRING(key ".set",                                                  \
                  ([storage = control->object_storage(),                       \
                    raw_key = torrent::raw_string::from_c_str(key)](           \
                     const auto&, const auto& object) {                        \
                    return storage->set_string(raw_key, object);               \
                  }),                                                          \
                  is_readonly)

#define CMD2_VAR_C_STRING(key, value, is_readonly)                             \
  control->object_storage()->insert_c_str(                                     \
    key, value, rpc::object_storage::flag_string_type);                        \
                                                                               \
  CMD2_ANY(key,                                                                \
           ([storage = control->object_storage(),                              \
             raw_key = torrent::raw_string::from_c_str(key)](                  \
              const auto&, const auto&) { return storage->get(raw_key); }),    \
           is_readonly)

#define CMD2_VAR_LIST(key, is_readonly)                                        \
  control->object_storage()->insert_c_str(                                     \
    key, torrent::Object::create_list(), rpc::object_storage::flag_list_type); \
                                                                               \
  CMD2_ANY(key,                                                                \
           ([storage = control->object_storage(),                              \
             raw_key = torrent::raw_string::from_c_str(key)](                  \
              const auto&, const auto&) { return storage->get(raw_key); }),    \
           is_readonly);                                                       \
                                                                               \
  CMD2_ANY_LIST(key ".set",                                                    \
                ([storage = control->object_storage(),                         \
                  raw_key = torrent::raw_string::from_c_str(key)](             \
                   const auto&, const auto& object) {                          \
                  return storage->set_list(raw_key, object);                   \
                }),                                                            \
                is_readonly);                                                  \
                                                                               \
  CMD2_ANY_VOID(key ".push_back",                                              \
                ([storage = control->object_storage(),                         \
                  raw_key = torrent::raw_string::from_c_str(key)](             \
                   const auto&, const auto& object) {                          \
                  return storage->list_push_back(raw_key, object);             \
                }),                                                            \
                is_readonly)

#define CMD2_FUNC_SINGLE(key, cmds, is_readonly)                               \
  CMD2_ANY(key,                                                                \
           ([raw_cmds = torrent::Object(torrent::raw_string::from_c_str(       \
               cmds))](const auto& target, const auto& args) {                 \
             return rpc::command_function_call_object(raw_cmds, target, args); \
           }),                                                                 \
           is_readonly);

#define CMD2_REDIRECT(from_key, to_key)                                        \
  rpc::commands.create_redirect(from_key,                                      \
                                to_key,                                        \
                                rpc::CommandMap::flag_public |                 \
                                  rpc::CommandMap::flag_dont_delete);
#define CMD2_REDIRECT_GENERIC(from_key, to_key)                                \
  rpc::commands.create_redirect(from_key,                                      \
                                to_key,                                        \
                                rpc::CommandMap::flag_public |                 \
                                  rpc::CommandMap::flag_no_target |            \
                                  rpc::CommandMap::flag_dont_delete);
#define CMD2_REDIRECT_GENERIC_NO_EXPORT(from_key, to_key)                      \
  rpc::commands.create_redirect(from_key,                                      \
                                to_key,                                        \
                                rpc::CommandMap::flag_no_target |              \
                                  rpc::CommandMap::flag_dont_delete);
#define CMD2_REDIRECT_FILE(from_key, to_key)                                   \
  rpc::commands.create_redirect(from_key,                                      \
                                to_key,                                        \
                                rpc::CommandMap::flag_public |                 \
                                  rpc::CommandMap::flag_file_target |          \
                                  rpc::CommandMap::flag_dont_delete);
#define CMD2_REDIRECT_TRACKER(from_key, to_key)                                \
  rpc::commands.create_redirect(from_key,                                      \
                                to_key,                                        \
                                rpc::CommandMap::flag_public |                 \
                                  rpc::CommandMap::flag_tracker_target |       \
                                  rpc::CommandMap::flag_dont_delete);

//
// Conversion of return types:
//

template<typename Functor, typename Result>
struct object_convert_type;

template<typename Functor>
struct object_convert_type<Functor, void> {

  template<typename Signature>
  struct result {
    using type = torrent::Object;
  };

  object_convert_type(Functor s)
    : m_slot(s) {}

  torrent::Object operator()() {
    m_slot();
    return torrent::Object();
  }
  template<typename Arg1>
  torrent::Object operator()(Arg1& arg1) {
    m_slot(arg1);
    return torrent::Object();
  }
  template<typename Arg1, typename Arg2>
  torrent::Object operator()(const Arg1& arg1) {
    m_slot(arg1);
    return torrent::Object();
  }
  template<typename Arg1, typename Arg2>
  torrent::Object operator()(Arg1& arg1, Arg2& arg2) {
    m_slot(arg1, arg2);
    return torrent::Object();
  }
  template<typename Arg1, typename Arg2>
  torrent::Object operator()(const Arg1& arg1, const Arg2& arg2) {
    m_slot(arg1, arg2);
    return torrent::Object();
  }

  Functor m_slot;
};

template<typename T>
object_convert_type<T, void>
object_convert_void(T f) {
  return f;
}

//
// Key creation:
//

template<int postfix_size>
inline const char*
create_new_key(const std::string& key, const char postfix[postfix_size]) {
  char* buffer = new char[key.size() + std::max(postfix_size, 1)];
  std::memcpy(buffer, key.c_str(), key.size() + 1);
  std::memcpy(buffer + key.size(), postfix, postfix_size);
  return buffer;
}

inline const char*
create_new_key(const std::string& key) {
  char* buffer = new char[key.size() + 1];
  std::memcpy(buffer, key.c_str(), key.size() + 1);
  return buffer;
}

#endif
