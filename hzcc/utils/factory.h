//
// Created by chen_ on 2022/11/2.
//
#include <glog/logging.h>

#include <any>
#include <functional>
#include <iostream>
#include <memory>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>
#include <string>
#include <unordered_map>

#include "status/status.h"
#include "status/statusor.h"
#ifndef HZCC_FACTORY_H
#define HZCC_FACTORY_H
namespace hzcc::utils {
/**
 * @brief The factory class for creating the product
 * @tparam Product the type of the product
 * @tparam ProductID the type of the product id
 */
template <typename Product,    // NOLINT
          typename ProductID>  // NOLINT
class AutoRegFactory {
  public:
    /// @brief Construct a factory handle; registrations are shared statically.
    AutoRegFactory() = default;
    /// @brief Destroy this handle without removing shared registrations.
    ~AutoRegFactory() = default;
    /// @brief Factory handles cannot be copied.
    AutoRegFactory(AutoRegFactory&) = delete;
    /// @brief Factory handles cannot be moved.
    AutoRegFactory(AutoRegFactory&&) = delete;
    /// @brief Disable copy assignment of factory handles.
    AutoRegFactory& operator=(AutoRegFactory&) = delete;
    /// @brief Disable move assignment of factory handles.
    AutoRegFactory& operator=(AutoRegFactory&&) = delete;

    using ProductType = Product;
    using ProductIDType = ProductID;

    /**
     * @brief Protected registration bridge for derived registration helpers.
     */
    class RegisterHelper {
      protected:
        /** @brief Forward a creator to the factory's shared registry.
         * @param creator Callable producing a product instance.
         * @param product_id Unique registry key.
         * @param product_desc Human-readable description used in diagnostics.
         * @note Duplicate keys terminate the process.
         */
        template <class ProductCreator>
        static void reg_product(ProductCreator creator,       // NOLINT
                                const ProductID& product_id,  // NOLINT
                                const std::string& product_desc) noexcept {
            internal_reg_product(creator, product_id, product_desc);
        };
    };

  protected:
    /// @brief Internal sample aggregate with a non-trivial string constructor.
    struct Ex1 {
        std::string str;  // member has a non-trivial default ctor
    };

    /** @brief Invoke the registered creator with the requested arguments.
     * @param id Identifier supplied during registration.
     * @param args Constructor arguments forwarded to the stored callable.
     * @return Owned product, or nullptr for an unknown key or callable mismatch.
     * @note The stored std::function signature must match Args exactly.
     */
    template <class... Args>
    static std::unique_ptr<Product> create(const ProductID& id,
                                           Args&&... args) noexcept {
        // Check if the product is registered
        auto it = registry().find(id);
        if (it == registry().end()) {
            LOG(ERROR) << "Product is not registered: " << id;
            return nullptr;
        }

        // acquire the product creator
        using Creator = std::function<std::unique_ptr<Product>(Args...)>;
        auto* creator = std::any_cast<Creator>(&it->second.first);
        if (!creator) {
            LOG(ERROR) << "Registered product " << id << " cannot be created with "
                       << typeid(Creator).name();
            return nullptr;
        }

        // create the product
        return (*creator)(std::forward<Args>(args)...);
    }

    /** @brief Visit registered identifiers in the registry's unspecified order.
     * @param func Callable accepting an identifier and returning Status.
     * @param args Additional arguments forwarded on every invocation.
     * @return One status per registered identifier, in visitation order.
     * @note Repeated forwarding can consume rvalue arguments more than once.
     */
    template <class Func, class... Args>
    static auto for_each_product(Func&& func, Args&&... args) noexcept
        -> std::vector<Status> {
        std::vector<Status> ret;
        for (const auto& [id, entry] : registry()) {
            ret.emplace_back(func(id, std::forward<Args>(args)...));
        }
        return ret;
    }

  private:
    using ProductMeta = std::pair<std::any, std::string>;

    /** @brief Access registrations, including during cross-file static initialization. */
    static auto& registry() {
        static std::unordered_map<ProductID, ProductMeta> products;
        return products;
    }

    /** @brief Store a creator and its metadata in the shared registry.
     * @param creator Product-creation callable stored with its concrete type.
     * @param id Unique identifier; duplicates terminate the process.
     * @param description Description printed in registration diagnostics.
     */
    template <typename Creator>
    static void internal_reg_product(Creator creator,       // NOLINT
                                     const ProductID& id,  // NOLINT
                                     const std::string& description) {
        const bool inserted =
            registry().emplace(id, ProductMeta{std::move(creator), description}).second;
        if (!inserted) {
            for (const auto& [name, entry] : registry()) {
                LOG(ERROR) << "Registered product " << name << ": " << entry.second;
            }
            LOG(FATAL) << "Product already registered: " << id;
        }
    }
};

namespace internal {
/**
 * @brief Auto register class for the product
 * @tparam Product the type of the product
 * @tparam Factory the type of the factory which the product belongs to
 */
template <class Product,  // NOLINT
          class Factory,  // NOLINT
          typename = std::enable_if<
              std::is_base_of_v<typename Factory::ProductType, Product>>>
class Register : private Factory::RegisterHelper {
  public:
    /** @brief Register a product creator when this helper is constructed.
     * @param name Registry key for the product.
     * @param desc Human-readable product description.
     * @tparam PArgs Argument types accepted by the registered creator.
     */
    template <class... PArgs,    // NOLINT
              std::enable_if_t<  // NOLINT
                  std::is_constructible_v<Product, PArgs&&...>, int> = 0>
    Register(typename Factory::ProductIDType name, std::string desc)
        : PASS_DESC(std::move(desc)), PASS_NAME(std::move(name)) {
        using Base = typename Factory::ProductType;
        using Creator = std::function<std::unique_ptr<Base>(PArgs...)>;
        this->reg_product(
            Creator{[](PArgs... args) -> std::unique_ptr<Base> {
                return std::make_unique<Product>(
                    std::forward<PArgs>(args)...);
            }},
            PASS_NAME, PASS_DESC);
    };

    /// @brief Destroy the helper; its static factory registration remains.
    virtual ~Register() = default;

  private:
    const std::string PASS_DESC;
    const typename Factory::ProductIDType PASS_NAME;
};
}  // namespace internal

#define UNIQUE_NAME(name, line) name##line##__COUNTER__
#define HZCC_REGI_PRODUCT_INTERNAL(Product, Factory, name, desc)            \
    static ::hzcc::utils::internal::Register<Product, Factory> UNIQUE_NAME( \
        Product, __LINE__)(name, desc);

}  // namespace hzcc::utils
#endif  // HZCC_FACTORY_H
