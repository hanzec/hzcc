//
// Created by hanzech on 12/18/21.
//
#include <cstdint>
#include <functional>
#include <memory>
#include <queue>
#include <unordered_map>
#include <vector>

#ifndef MYCC_UTILS_AUTO_FACTORY_H
#define MYCC_UTILS_AUTO_FACTORY_H

namespace Mycc::Utils {
template <typename BaseProduct, typename ProductIdentifier>
class Factory {
 public:
  /**
   * Delete the default constructor and copy constructor to prevent the class to
   * be copied.
   */
  Factory() = delete;
  ~Factory() = delete;
  Factory(Factory&) = delete;
  Factory(Factory&&) = delete;
  Factory& operator=(Factory&) = delete;
  Factory& operator=(Factory&&) = delete;

  using ProductType = BaseProduct;
  using IdentifierType = ProductIdentifier;
  using PTRC_OBJECT = std::function<std::unique_ptr<BaseProduct>()>;

  /**
   * @brief register a command
   *
   * @param command_name the name of the command
   * @param command_ptr the pointer to the command
   */
  static auto& get_product( const ProductIdentifier& command_name) noexcept {
    auto it = Factory::_constructor_map.find(command_name);
    if (it == Factory::_constructor_map.end()) {
      return nullptr;
    }
    return it->second();
  } 

  /**
   * @brief Helper class for expose the private method to outside functions
   *
   * @param cmd the name of the command
   * @param desc the description of the command
   * @param obj the reference of std::function for creating the command object
   */
  class RegisterHelper {
   protected:
    static bool reg_product(
        PTRC_OBJECT&& product_creator,
        const ProductIdentifier& product_identifier) noexcept {
      return internal_reg(-1, std::forward<PTRC_OBJECT>(product_creator),
                          product_identifier);
    };

    static bool reg_product(
        uint_fast16_t priority, PTRC_OBJECT&& product_creator,
        const ProductIdentifier& product_identifier) noexcept {
      return internal_reg(priority, std::forward<PTRC_OBJECT>(product_creator),
                          product_identifier);
    };
  };
  using HelperType = Factory::RegisterHelper;

 private:
  using ProducerNode = std::pair<int, PTRC_OBJECT>;

  /**
   * @brief
   *
   * @param lhs
   * @param rhs
   * @return true
   * @return false
   */
  constexpr static bool compare_priority(const ProducerNode& lhs,
                                         const ProducerNode& rhs) noexcept {
    return lhs.first > rhs.first;
  }

  using ProducerList =
      std::priority_queue<ProducerNode, std::vector<ProducerNode>,
                          decltype(&compare_priority)>;

  /**
   * @brief static map storage for all the registered producted and its
   * identifier
   */
  inline static std::unordered_map<ProductIdentifier, ProducerList>
      _constructor_map;

  /**
   * @brief private function which regist command dynamically
   *
   * @param command the command in string
   * @param description the short description of the string
   * @param command_obj the reference of std::function for creating the command
   * object
   * @return bool the registration status of current register process
   */
  static bool internal_reg(
      int_fast32_t priority, PTRC_OBJECT&& product_creator,
      const ProductIdentifier& product_identifier) noexcept {
    // If it's first time to register the producer, create a new list
    if (Factory::_constructor_map.find(product_identifier) ==
        Factory::_constructor_map.end()) {
      Factory::_constructor_map.emplace(product_identifier,
                                        ProducerList(&compare_priority));
    }

    // inserting the producer to the list
    Factory::_constructor_map[product_identifier].emplace(
        priority, std::move(product_creator));
    return true;
  }
};

template <typename T>
struct is_derived_from_factory {
 private:
  template <typename S1, typename S2>
  static decltype(static_cast<const Factory<S1, S2>&>(std::declval<T>()),
                  std::true_type{}) test(const Factory<S1, S2>&);
  static std::false_type test(...);

 public:
  static constexpr bool value =
      decltype(is_derived_from_factory::test(std::declval<T>()))::value;
};

/**
 * @brief Auto register helper template for Command
 *
 * @tparam T the command need to mark as auto regiter
 */
template <typename Product, typename ProductFactory,
          typename =
              std::enable_if_t<is_derived_from_factory<ProductFactory>::value>>
class AutoRegProduct : private ProductFactory::HelperType {
 public:
  template <typename... Args>
  explicit AutoRegProduct(Args&&... args)
      : ProductFactory::ProductType(std::forward<Args>(args)...) {
    (void)AutoRegProduct<Product, ProductFactory>::_reg_status;
  };

  static const constexpr uint_fast16_t PRIORITY = 0;
  static const constexpr typename ProductFactory::IdentifierType IDENTIFIER;

 private:
  inline static const bool _reg_status =
      AutoRegProduct<Product, ProductFactory>::reg_product(
          Product::PRIORITY,
          [](auto a) { return std::make_unique<Product>(a); },
          Product::IDENTIFIER);
  ;
};
}  // namespace Mycc::Utils

#define REG_PRODUCT(class, identifier, factory)              \
 public:                                                     \
  explicit class() : AutoRegProduct<class, factory>(){};     \
  inline static const constexpr uint_fast16_t PRIORITY = -1; \
  inline static const constexpr decltype(identifier) IDENTIFIER = identifier;

#define REG_PRODUCT_PRIORITY(class, identifier, factory, priority) \
 public:                                                           \
  explicit class() : AutoRegProduct<class, factory>(){};           \
  inline static const constexpr uint_fast16_t PRIORITY = priority; \
  inline static const constexpr decltype(identifier) IDENTIFIER = identifier;

#endif  // MYCC_UTILS_AUTO_FACTORY_H