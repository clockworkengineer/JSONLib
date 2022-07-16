#pragma once
// =======
// C++ STL
// =======
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
// =========
// NAMESPACE
// =========
namespace JSONLib {
// ===================================================
// Forward declarations for interfaces/classes/structs
// ===================================================
class JSON_Impl;
class ISource;
class IDestination;
class IConverter;
class ITranslator;
struct JNode;
// ================
// CLASS DEFINITION
// ================
class JSON {
public:
  // ==========================
  // PUBLIC TYPES AND CONSTANTS
  // ==========================
  // ======================
  // CONSTRUCTOR/DESTRUCTOR
  // ======================
  explicit JSON(ITranslator *translator = nullptr,
                IConverter *converter = nullptr);
  explicit JSON(const std::string &jsonString);
  JSON(const JSON &other) = delete;
  JSON &operator=(const JSON &other) = delete;
  JSON(JSON &&other) = delete;
  JSON &operator=(JSON &&other) = delete;
  ~JSON();
  // ==============
  // PUBLIC METHODS
  // ==============
  [[nodiscard]] std::string version();
  void parse(ISource &source);
  void parse(ISource &&source);
  void stringify(IDestination &destination);
  void stringify(IDestination &&destination);
  void strip(ISource &source, IDestination &destination);
  void strip(ISource &source, IDestination &&destination);
  void strip(ISource &&source, IDestination &destination);
  void strip(ISource &&source, IDestination &&destination);
  // [[nodiscard]] JNode &root() { return (*m_jNodeRoot); }
  [[nodiscard]] const JNode &root() const { return (*m_jNodeRoot); }
  JNode &operator[](const std::string &key);
  const JNode &operator[](const std::string &key) const;
  JNode &operator[](std::size_t index);
  const JNode &operator[](std::size_t index) const;
  // ================
  // PUBLIC VARIABLES
  // ================
private:
  // ===========================
  // PRIVATE TYPES AND CONSTANTS
  // ===========================
  // ===============
  // PRIVATE METHODS
  // ===============
  // =================
  // PRIVATE VARIABLES
  // =================
  // Root of JSON tree
  std::unique_ptr<JNode> m_jNodeRoot;
  // JSON implementation
  const std::unique_ptr<JSON_Impl> m_jsonImplementation;
};
} // namespace JSONLib