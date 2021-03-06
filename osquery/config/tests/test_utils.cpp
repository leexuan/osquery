/**
 *  Copyright (c) 2014-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under both the Apache 2.0 license (found in the
 *  LICENSE file in the root directory of this source tree) and the GPLv2 (found
 *  in the COPYING file in the root directory of this source tree).
 *  You may select, at your option, one of the above-listed licenses.
 */

#include <osquery/config/tests/test_utils.h>

#include <osquery/filesystem/filesystem.h>

#include <gtest/gtest.h>

#include <boost/io/detail/quoted_manip.hpp>

#include <cstdlib>

namespace {

namespace fs = boost::filesystem;

fs::path getConfDirPathImpl() {
  char const* kEnvVarName = "TEST_CONF_FILES_DIR";
  auto const value = std::getenv(kEnvVarName);
  EXPECT_NE(value, nullptr)
      << "Env var " << boost::io::quoted(kEnvVarName) << " was not found, "
      << " looks like cxx_test argument 'env' is not set up.";
  return fs::path(value);
}

}

namespace osquery {

fs::path const& getTestConfigDirectory() {
    static auto const path = getConfDirPathImpl();
    return path;
}

std::map<std::string, std::string> getTestConfigMap(const std::string& file) {
  std::string content;
  auto const filepath = getTestConfigDirectory() / file;
  auto status = readFile(filepath, content);
  EXPECT_TRUE(status.ok())
      << "Could not read file: " << boost::io::quoted(filepath.string())
      << ", because: " << status.what();
  std::map<std::string, std::string> config;
  config["awesome"] = content;
  return config;
}

JSON getExamplePacksConfig() {
  std::string content;
  auto const filepath = getTestConfigDirectory() / "test_inline_pack.conf";
  auto status = readFile(filepath, content);
  EXPECT_TRUE(status.ok())
      << "Could not read file: " << boost::io::quoted(filepath.string())
      << ", because: " << status.what();
  JSON doc = JSON::newObject();
  doc.fromString(content);
  return doc;
}

/// no discovery queries, no platform restriction
JSON getUnrestrictedPack() {
  auto doc = getExamplePacksConfig();
  return JSON::newFromValue(doc.doc()["packs"]["unrestricted_pack"]);
}

// several restrictions (version, platform, shard)
JSON getRestrictedPack() {
  auto doc = getExamplePacksConfig();
  return JSON::newFromValue(doc.doc()["packs"]["restricted_pack"]);
}

/// 1 discovery query, darwin platform restriction
JSON getPackWithDiscovery() {
  auto doc = getExamplePacksConfig();
  return JSON::newFromValue(doc.doc()["packs"]["discovery_pack"]);
}

/// 1 discovery query which will always pass
JSON getPackWithValidDiscovery() {
  auto doc = getExamplePacksConfig();
  return JSON::newFromValue(doc.doc()["packs"]["valid_discovery_pack"]);
}

/// no discovery queries, no platform restriction, fake version string
JSON getPackWithFakeVersion() {
  auto doc = getExamplePacksConfig();
  return JSON::newFromValue(doc.doc()["packs"]["fake_version_pack"]);
}

} // namespace osquery
