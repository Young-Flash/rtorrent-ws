// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2005-2011, Jari Sundell <jaris@ifi.uio.no>

#include <algorithm>
#include <torrent/exceptions.h>
#include <torrent/object.h>
#include <torrent/utils/functional.h>

#include "control.h"
#include "globals.h"
#include "rpc/parse_commands.h"

#include "core/download.h"
#include "core/download_list.h"
#include "core/manager.h"
#include "core/view.h"
#include "core/view_manager.h"

namespace core {

void
ViewManager::clear() {
  std::for_each(begin(), end(), torrent::utils::call_delete<View>());

  base_type::clear();
}

ViewManager::iterator
ViewManager::insert(const std::string& name) {
  if (name.empty())
    throw torrent::input_error("View with empty name not supported.");

  if (find(name) != end())
    throw torrent::input_error("View with same name already inserted.");

  View* view = new View();
  view->initialize(name);

  return base_type::insert(end(), view);
}

ViewManager::iterator
ViewManager::find(const std::string& name) {
  return std::find_if(
    begin(), end(), torrent::utils::equal(name, std::mem_fn(&View::name)));
}

ViewManager::iterator
ViewManager::find_throw(const std::string& name) {
  iterator itr = std::find_if(
    begin(), end(), torrent::utils::equal(name, std::mem_fn(&View::name)));

  if (itr == end())
    throw torrent::input_error("Could not find view: " + name);

  return itr;
}

void
ViewManager::sort(const std::string& name, uint32_t timeout) {
  iterator viewItr = find_throw(name);

  if ((*viewItr)->last_changed() +
        torrent::utils::timer::from_seconds(timeout) >
      cachedTime)
    return;

  // Should we rename sort, or add a seperate function?
  (*viewItr)->filter();
  (*viewItr)->sort();
}

void
ViewManager::set_filter(const std::string& name, const torrent::Object& cmd) {
  iterator viewItr = find_throw(name);

  (*viewItr)->set_filter(cmd);
  (*viewItr)->filter();
}

void
ViewManager::set_filter_temp(const std::string&     name,
                             const torrent::Object& cmd) {
  iterator viewItr = find_throw(name);

  (*viewItr)->set_filter_temp(cmd);
  (*viewItr)->filter();
}

void
ViewManager::set_filter_on(const std::string& name, const filter_args& args) {
  iterator viewItr = find_throw(name);

  (*viewItr)->clear_filter_on();

  // TODO: Ensure the filter keys are rlookup.

  for (filter_args::const_iterator itr = args.begin(); itr != args.end(); ++itr)
    (*viewItr)->set_filter_on_event(*itr);
}

}
