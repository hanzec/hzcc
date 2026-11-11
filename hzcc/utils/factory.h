//
// Created by chen_ on 2022/11/2.
//
#include <any>
#include <functional>
#include <iostream>
#include <memory>
#include <queue>
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
template <typename Product,                                          // NOLINT
          typename ProductID,                                        // NOLINT
          std::enable_if_t<                                          // NOLINT
              std::negation_v<std::is_abstract<Product>>, int> = 0>  // NOLINT
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
    template <class... Args,           // NOLINT
              std::enable_if_t<        // NOLINT
                  std::disjunction_v<  // NOLINT
                      std::is_default_constructible<ProductType>,
                      std::is_constructible<ProductType, Args&&...>>,
                  int> = 0>
    static std::unique_ptr<Product> create(const ProductID& id,
                                           Args&&... args) noexcept {
        // Check if the product is registered
        auto it = _registered_map.find(id);
        if (it == _registered_map.end()) {
            LOG(ERROR) << "The product id %s is not registered" << id.c_str();
            return nullptr;
        }

        // acquire the product creator
        try {
            if constexpr (sizeof...(Args) == 0) {
                // Assert the case that the product is default constructive if
                // no additional arguments are provided
                static_assert(std::is_default_constructible_v<Product>,
                              "The product is not default constructive");

                // create the product
                auto creator =
                    std::any_cast<std::function<std::unique_ptr<Product>()>>(
                        _creator_collection[it->second.first]);
                return creator();
            } else {
                // assert the product is constructive with the given arguments
                static_assert(std::is_constructible_v<ProductType, Args&&...>,
                              "The product is not constructible with the given "
                              "arguments");

                // create the product
                auto& creator = std::any_cast<
                    std::function<std::unique_ptr<Product>(Args...)>>(
                    _creator_collection[it->second.first]);

                return creator(std::forward<Args>(args)...);
            }
        } catch (const std::bad_any_cast& e) {
            LOG(ERROR) << "Failed to cast product[" << id.c_str()
                       << "] to the expected type"
                       << typeid(
                              std::function<std::unique_ptr<Product>(Args...)>)
                              .name();
            return nullptr;
        }
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
        for (const auto& [id, desc] : _registered_map) {
            ret.emplace_back(func(id, std::forward<Args>(args)...));
        }
        return ret;
    }

  private:
    using ProductMeta = std::pair<uint32_t, std::string>;

    inline static std::vector<std::any> _creator_collection{};
    inline static std::unordered_map<ProductID, ProductMeta> _registered_map{};

    /** @brief Store a creator and its metadata in the shared registry.
     * @param creator Product-creation callable stored with its concrete type.
     * @param product_id Unique identifier; duplicates terminate the process.
     * @param product_desc Description printed in registration diagnostics.
     */
    template <
        class... PArgs,
        typename ProductCreator =
            std::function<std::unique_ptr<Product>(PArgs&...)>,
        std::enable_if_t<std::is_constructible_v<Product, PArgs&&...>, int> = 0>
    static void internal_reg_product(ProductCreator creator,       // NOLINT
                                     const ProductID& product_id,  // NOLINT
                                     const std::string& product_desc) noexcept {
        try {
            if (_registered_map.find(product_id) != _registered_map.end()) {
                std::cerr << "Product " << product_id
                          << " already registered:" << std::endl;
                std::cerr << "*********** Registered product list ***********"
                          << std::endl
                          << ": " << std::endl;
                for (const auto& [product, meta] : _registered_map) {
                    std::cerr << "\t" << product << " (id:" << meta.first
                              << ") : " << meta.second << std::endl;
                }
                std::cerr << "***********************************************"
                          << std::endl;
                exit(1);
            } else {
                _registered_map[product_id] =
                    std::make_pair(_creator_collection.size(), product_desc);
                _creator_collection.emplace_back(
                    std::make_any<ProductCreator>(creator));
            }
        } catch (const std::exception& e) {
            LOG(FATAL) << "Exception in factory: " << e.what() << std::endl;
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
        : PASS_NAME(std::move(name)), PASS_DESC(std::move(desc)) {
        this->reg_product(
            [](PArgs&&... args) {
                return std::unique_ptr<Product>(
                    std::forward<decltype(args)>(args)...);
            },
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
