// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2005-2011, Jari Sundell <jaris@ifi.uio.no>

#include <cstdio>
#include <torrent/download/resource_manager.h>
#include <torrent/rate.h>
#include <torrent/throttle.h>
#include <torrent/utils/address_info.h>

#include "core/manager.h"
#include "rpc/parse.h"
#include "rpc/parse_commands.h"
#include "ui/root.h"

#include "command_helpers.h"
#include "control.h"
#include "globals.h"

std::pair<uint32_t, uint32_t>
parse_address_range(const torrent::Object::list_type&          args,
                    torrent::Object::list_type::const_iterator itr) {
  unsigned int                  prefixWidth, ret;
  char                          dummy;
  char                          host[1024];
  torrent::utils::address_info* ai;

  ret = std::sscanf(
    itr->as_string().c_str(), "%1023[^/]/%u%c", host, &prefixWidth, &dummy);
  if (ret < 1 || torrent::utils::address_info::get_address_info(
                   host, PF_INET, SOCK_STREAM, &ai) != 0)
    throw torrent::input_error("Could not resolve host.");

  uint32_t                       begin, end;
  torrent::utils::socket_address sa;
  sa.copy(*ai->address(), ai->length());
  begin = end = sa.sa_inet()->address_h();
  torrent::utils::address_info::free_address_info(ai);

  if (ret == 2) {
    if (++itr != args.end())
      throw torrent::input_error("Cannot specify both network and range end.");

    uint32_t netmask = std::numeric_limits<uint32_t>::max()
                       << (32 - prefixWidth);
    if (prefixWidth >= 32 || sa.sa_inet()->address_h() & ~netmask)
      throw torrent::input_error("Invalid address/prefix.");

    end = sa.sa_inet()->address_h() | ~netmask;

  } else if (++itr != args.end()) {
    if (torrent::utils::address_info::get_address_info(
          itr->as_string().c_str(), PF_INET, SOCK_STREAM, &ai) != 0)
      throw torrent::input_error("Could not resolve host.");

    sa.copy(*ai->address(), ai->length());
    torrent::utils::address_info::free_address_info(ai);
    end = sa.sa_inet()->address_h();
  }

  // convert to [begin, end) making sure the end doesn't overflow
  // (this precludes 255.255.255.255 from ever matching, but that's not a real
  // IP anyway)
  return std::make_pair((uint32_t)begin, (uint32_t)std::max(end, end + 1));
}

torrent::Object
apply_throttle(const torrent::Object::list_type& args, bool up) {
  torrent::Object::list_const_iterator argItr = args.begin();

  if (argItr == args.end())
    throw torrent::input_error("Missing throttle name.");

  const std::string& name = argItr->as_string();
  if (name.empty() || name == "NULL")
    throw torrent::input_error("Invalid throttle name '" + name + "'.");

  if (++argItr == args.end() || argItr->as_string().empty())
    throw torrent::input_error("Missing throttle rate for '" + name + "'.");

  int64_t rate;
  rpc::parse_whole_value_nothrow(argItr->as_string().c_str(), &rate);

  if (rate < 0)
    throw torrent::input_error("Throttle rate must be non-negative.");

  core::ThrottleMap::iterator itr = control->core()->throttles().find(name);
  if (itr == control->core()->throttles().end())
    itr =
      control->core()
        ->throttles()
        .insert(std::make_pair(name, torrent::ThrottlePair(nullptr, nullptr)))
        .first;

  torrent::Throttle*& throttle = up ? itr->second.first : itr->second.second;
  if (rate != 0 && throttle == nullptr)
    throttle =
      (up ? torrent::up_throttle_global() : torrent::down_throttle_global())
        ->create_slave();

  if (throttle != nullptr)
    throttle->set_max_rate(rate * 1024);

  return torrent::Object();
}

static constexpr int throttle_info_up   = (1 << 0);
static constexpr int throttle_info_down = (1 << 1);
static constexpr int throttle_info_max  = (1 << 2);
static constexpr int throttle_info_rate = (1 << 3);

torrent::Object
retrieve_throttle_info(const torrent::Object::string_type& name, int flags) {
  core::ThrottleMap::iterator itr = control->core()->throttles().find(name);
  torrent::ThrottlePair throttles = itr == control->core()->throttles().end()
                                      ? torrent::ThrottlePair(nullptr, nullptr)
                                      : itr->second;
  torrent::Throttle*    throttle =
    flags & throttle_info_down ? throttles.second : throttles.first;
  torrent::Throttle* global = flags & throttle_info_down
                                ? torrent::down_throttle_global()
                                : torrent::up_throttle_global();

  if (throttle == nullptr && name.empty())
    throttle = global;

  if (throttle == nullptr)
    return flags & throttle_info_rate ? (int64_t)0 : (int64_t)-1;
  else if (!throttle->is_throttled() || !global->is_throttled())
    return (int64_t)0;
  else if (flags & throttle_info_rate)
    return (int64_t)throttle->rate()->rate();
  else
    return (int64_t)throttle->max_rate();
}

torrent::Object
apply_address_throttle(const torrent::Object::list_type& args) {
  if (args.size() < 2 || args.size() > 3)
    throw torrent::input_error("Incorrect number of arguments.");

  std::pair<uint32_t, uint32_t> range =
    parse_address_range(args, ++args.begin());
  core::ThrottleMap::iterator throttleItr =
    control->core()->throttles().find(args.begin()->as_string().c_str());
  if (throttleItr == control->core()->throttles().end())
    throw torrent::input_error("Throttle not found.");

  control->core()->set_address_throttle(
    range.first, range.second, throttleItr->second);
  return torrent::Object();
}

torrent::Object
throttle_update(const char* variable, int64_t value) {
  rpc::commands.call_command(variable, value);

  control->ui()->adjust_up_throttle(0);
  control->ui()->adjust_down_throttle(0);
  return torrent::Object();
}

void
initialize_command_throttle() {
  CMD2_ANY("throttle.unchoked_uploads", [](const auto&, const auto&) {
    return torrent::resource_manager()->currently_upload_unchoked();
  }, true);
  CMD2_ANY("throttle.max_unchoked_uploads", [](const auto&, const auto&) {
    return torrent::resource_manager()->max_upload_unchoked();
  }, true);
  CMD2_ANY("throttle.unchoked_downloads", [](const auto&, const auto&) {
    return torrent::resource_manager()->currently_download_unchoked();
  }, true);
  CMD2_ANY("throttle.max_unchoked_downloads", [](const auto&, const auto&) {
    return torrent::resource_manager()->max_download_unchoked();
  }, true);

  CMD2_VAR_VALUE("throttle.min_peers.normal", 100, false);
  CMD2_VAR_VALUE("throttle.max_peers.normal", 200, false);
  CMD2_VAR_VALUE("throttle.min_peers.seed", -1, false);
  CMD2_VAR_VALUE("throttle.max_peers.seed", -1, false);

  CMD2_VAR_VALUE("throttle.min_uploads", 0, false);
  CMD2_VAR_VALUE("throttle.max_uploads", 50, false);
  CMD2_VAR_VALUE("throttle.min_downloads", 0, false);
  CMD2_VAR_VALUE("throttle.max_downloads", 50, false);

  CMD2_VAR_VALUE("throttle.max_uploads.div._val", 1, false);
  CMD2_VAR_VALUE("throttle.max_uploads.global._val", 0, false);
  CMD2_VAR_VALUE("throttle.max_downloads.div._val", 1, false);
  CMD2_VAR_VALUE("throttle.max_downloads.global._val", 0, false);

  CMD2_REDIRECT_GENERIC("throttle.max_uploads.div",
                        "throttle.max_uploads.div._val");
  CMD2_REDIRECT_GENERIC("throttle.max_uploads.global",
                        "throttle.max_uploads.global._val");
  CMD2_REDIRECT_GENERIC("throttle.max_downloads.div",
                        "throttle.max_downloads.div._val");
  CMD2_REDIRECT_GENERIC("throttle.max_downloads.global",
                        "throttle.max_downloads.global._val");

  CMD2_ANY_VALUE(
    "throttle.max_uploads.div.set", [](const auto&, const auto& value) {
      return throttle_update("throttle.max_uploads.div._val.set", value);
    }, false);
  CMD2_ANY_VALUE(
    "throttle.max_uploads.global.set", [](const auto&, const auto& value) {
      return throttle_update("throttle.max_uploads.global._val.set", value);
    }, false);
  CMD2_ANY_VALUE(
    "throttle.max_downloads.div.set", [](const auto&, const auto& value) {
      return throttle_update("throttle.max_downloads.div._val.set", value);
    }, false);
  CMD2_ANY_VALUE(
    "throttle.max_downloads.global.set", [](const auto&, const auto& value) {
      return throttle_update("throttle.max_downloads.global._val.set", value);
    }, false);

  // TODO: Move the logic into some libtorrent function.
  CMD2_ANY("throttle.global_up.rate",
           [](const auto&, const auto&) { return torrent::up_rate()->rate(); }, true);
  CMD2_ANY("throttle.global_up.total", [](const auto&, const auto&) {
    return torrent::up_rate()->total();
  }, true);
  CMD2_ANY("throttle.global_up.max_rate", [](const auto&, const auto&) {
    return torrent::up_throttle_global()->max_rate();
  }, true);
  CMD2_ANY_VALUE_V("throttle.global_up.max_rate.set",
                   [](const auto&, const auto& throttle) {
                     return control->ui()->set_up_throttle_i64(throttle);
                   }, false);
  CMD2_ANY_VALUE_KB("throttle.global_up.max_rate.set_kb",
                    [](const auto&, const auto& throttle) {
                      return control->ui()->set_up_throttle_i64(throttle);
                    }, false);
  CMD2_ANY("throttle.global_down.rate", [](const auto&, const auto&) {
    return torrent::down_rate()->rate();
  }, true);
  CMD2_ANY("throttle.global_down.total", [](const auto&, const auto&) {
    return torrent::down_rate()->total();
  }, true);
  CMD2_ANY("throttle.global_down.max_rate",
           [down_throttle_global =
              torrent::down_throttle_global()](const auto&, const auto&) {
             return down_throttle_global->max_rate();
           }, true);
  CMD2_ANY_VALUE_V("throttle.global_down.max_rate.set",
                   [](const auto&, const auto& throttle) {
                     return control->ui()->set_down_throttle_i64(throttle);
                   }, false);
  CMD2_ANY_VALUE_KB("throttle.global_down.max_rate.set_kb",
                    [](const auto&, const auto& throttle) {
                      return control->ui()->set_down_throttle_i64(throttle);
                    }, false);

  // Temporary names, need to change this to accept real rates rather
  // than kB.
  CMD2_ANY_LIST("throttle.up", [](const auto&, const auto& args) {
    return apply_throttle(args, true);
  }, true);
  CMD2_ANY_LIST("throttle.down", [](const auto&, const auto& args) {
    return apply_throttle(args, false);
  }, true);
  CMD2_ANY_LIST("throttle.ip", [](const auto&, const auto& args) {
    return apply_address_throttle(args);
  }, true);

  CMD2_ANY_STRING("throttle.up.max", [](const auto&, const auto& name) {
    return retrieve_throttle_info(name, throttle_info_up | throttle_info_max);
  }, true);
  CMD2_ANY_STRING("throttle.up.rate", [](const auto&, const auto& name) {
    return retrieve_throttle_info(name, throttle_info_up | throttle_info_rate);
  }, true);
  CMD2_ANY_STRING("throttle.down.max", [](const auto&, const auto& name) {
    return retrieve_throttle_info(name, throttle_info_down | throttle_info_max);
  }, true);
  CMD2_ANY_STRING("throttle.down.rate", [](const auto&, const auto& name) {
    return retrieve_throttle_info(name,
                                  throttle_info_down | throttle_info_rate);
  }, true);
}
