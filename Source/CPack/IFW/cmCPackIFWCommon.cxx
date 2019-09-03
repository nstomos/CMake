/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "cmCPackIFWCommon.h"

#include "cmCPackGenerator.h"
#include "cmCPackIFWGenerator.h"
#include "cmCPackLog.h" // IWYU pragma: keep
#include "cmStringAlgorithms.h"
#include "cmSystemTools.h"
#include "cmTimestamp.h"
#include "cmVersionConfig.h"
#include "cmXMLWriter.h"

#include <cstddef>
#include <sstream>
#include <utility>
#include <vector>

cmCPackIFWCommon::cmCPackIFWCommon()
  : Generator(nullptr)
{
}

const char* cmCPackIFWCommon::GetOption(const std::string& op) const
{
  return this->Generator ? this->Generator->cmCPackGenerator::GetOption(op)
                         : nullptr;
}

bool cmCPackIFWCommon::IsOn(const std::string& op) const
{
  return this->Generator ? this->Generator->cmCPackGenerator::IsOn(op) : false;
}

bool cmCPackIFWCommon::IsSetToOff(const std::string& op) const
{
  return this->Generator ? this->Generator->cmCPackGenerator::IsSetToOff(op)
                         : false;
}

bool cmCPackIFWCommon::IsSetToEmpty(const std::string& op) const
{
  return this->Generator ? this->Generator->cmCPackGenerator::IsSetToEmpty(op)
                         : false;
}

bool cmCPackIFWCommon::IsVersionLess(const char* version)
{
  if (!this->Generator) {
    return false;
  }

  return cmSystemTools::VersionCompare(
    cmSystemTools::OP_LESS, this->Generator->FrameworkVersion.data(), version);
}

bool cmCPackIFWCommon::IsVersionGreater(const char* version)
{
  if (!this->Generator) {
    return false;
  }

  return cmSystemTools::VersionCompare(
    cmSystemTools::OP_GREATER, this->Generator->FrameworkVersion.data(),
    version);
}

bool cmCPackIFWCommon::IsVersionEqual(const char* version)
{
  if (!this->Generator) {
    return false;
  }

  return cmSystemTools::VersionCompare(
    cmSystemTools::OP_EQUAL, this->Generator->FrameworkVersion.data(),
    version);
}

void cmCPackIFWCommon::ExpandListArgument(
  const std::string& arg, std::map<std::string, std::string>& argsOut)
{
  std::vector<std::string> args = cmExpandedList(arg, false);
  if (args.empty()) {
    return;
  }

  std::size_t i = 0;
  std::size_t c = args.size();
  if (c % 2) {
    argsOut[""] = args[i];
    ++i;
  }

  --c;
  for (; i < c; i += 2) {
    argsOut[args[i]] = args[i + 1];
  }
}

void cmCPackIFWCommon::ExpandListArgument(
  const std::string& arg, std::multimap<std::string, std::string>& argsOut)
{
  std::vector<std::string> args = cmExpandedList(arg, false);
  if (args.empty()) {
    return;
  }

  std::size_t i = 0;
  std::size_t c = args.size();
  if (c % 2) {
    argsOut.insert(std::pair<std::string, std::string>("", args[i]));
    ++i;
  }

  --c;
  for (; i < c; i += 2) {
    argsOut.insert(std::pair<std::string, std::string>(args[i], args[i + 1]));
  }
}

void cmCPackIFWCommon::WriteGeneratedByToStrim(cmXMLWriter& xout)
{
  if (!this->Generator) {
    return;
  }

  std::ostringstream comment;
  comment << "Generated by CPack " << CMake_VERSION << " IFW generator "
          << "for QtIFW ";
  if (this->IsVersionEqual("1.9.9")) {
    comment << "less 2.0";
  } else {
    comment << this->Generator->FrameworkVersion;
  }
  comment << " tools at " << cmTimestamp().CurrentTime("", true);
  xout.Comment(comment.str().c_str());
}
