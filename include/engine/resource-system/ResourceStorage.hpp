#ifndef RESOURCE_STORAGE_HPP
#define RESOURCE_STORAGE_HPP

#include <string>
#include <unordered_map>

namespace engine::resourcesystem {
    namespace __detail {
        template<typename Key, typename T>
        T& typeMap() {
            static T value;
            return value;
        }

        template<typename TResource>
        using ResourceDataStorage = std::unordered_map<std::string, TResource>;

        template<typename TResource>
        ResourceDataStorage<TResource>& resourceData() {
            return typeMap<TResource, ResourceDataStorage<TResource>>();
        }
    }

    /**
     * \brief Stores arbitrary resource data, which can be retrieved by
     * their identifiers.
     */
    class ResourceStorage {
    public:
        /**
         * \brief Stores data, assigning it a given identifier.
         */
        template<typename T>
        void store(const std::string& identifier, T&& data);

        /**
         * \brief Retrieves previously stored data. Throws if the identifier
         * is invalid.
         */
        template<typename T>
        T& get(const std::string& identifier) const;

        /**
         * \brief Removes data assigned to the given identifier.
         */
        template<typename T>
        void remove(const std::string& identifier);
    };

    template<typename T>
    void ResourceStorage::store(const std::string& identifier, T&& data) {
        __detail::resourceData<std::decay_t<T>>().insert({
            identifier,
            std::forward<T>(data)
        });
    }

    template<typename T>
    T& ResourceStorage::get(const std::string& identifier) const {
        return __detail::resourceData<T>().at(identifier);
    }

    template<typename T>
    void ResourceStorage::remove(const std::string& identifier) {
        __detail::resourceData<T>().erase(identifier);
    }
}

#endif
