#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>
#include <type_traits>
#include <vector>

namespace sol
{
    template<typename T>
    class ObjectRefSetting
    {
    public:
        ObjectRefSetting() = default;

        ObjectRefSetting(const ObjectRefSetting&) = default;

        ObjectRefSetting(ObjectRefSetting&&) = default;

        ObjectRefSetting(T& obj) : object(&obj) {}

        ObjectRefSetting(T* obj) : object(obj) {}

        ~ObjectRefSetting() noexcept = default;

        ObjectRefSetting& operator=(const ObjectRefSetting&) = default;

        ObjectRefSetting& operator=(ObjectRefSetting&&) noexcept = default;

        /**
         * \brief Assign by reference.
         * \param obj Object.
         * \return *this.
         */
        ObjectRefSetting& operator=(T& obj)
        {
            object = &obj;
            return *this;
        }

        /**
         * \brief Assign by pointer.
         * \param obj Object.
         * \return *this.
         */
        ObjectRefSetting& operator=(T* obj)
        {
            object = obj;
            return *this;
        }

        /**
         * \brief Assign by unique_ptr.
         * \param obj Object.
         * \return *this.
         */
        ObjectRefSetting& operator=(std::unique_ptr<T>& obj)
        {
            object = obj.get();
            return *this;
        }

        /**
         * \brief Assign by shared_ptr.
         * \param obj Object.
         * \return *this.
         */
        ObjectRefSetting& operator=(std::shared_ptr<T>& obj)
        {
            object = obj.get();
            return *this;
        }

        /**
         * \brief Get object.
         * \return Object.
         */
        T& operator()() const { return *object; }

        /**
         * \brief Implicit conversion to handle.
         */
        operator auto() const { return object ? object->get() : VK_NULL_HANDLE; }

    private:
        T* object = nullptr;
    };

    template<typename T>
    class ObjectRefListSetting
    {
        // using handle_t = std::decay_t<decltype(std::declval<T>().get())>;

    public:
        ObjectRefListSetting() = default;

        ObjectRefListSetting(const ObjectRefListSetting&) = default;

        ObjectRefListSetting(ObjectRefListSetting&&) = default;

        ~ObjectRefListSetting() noexcept = default;

        ObjectRefListSetting& operator=(const ObjectRefListSetting&) = default;

        ObjectRefListSetting& operator=(ObjectRefListSetting&&) noexcept = default;

        /**
         * \brief Add by reference.
         * \param obj Object.
         * \return *this.
         */
        ObjectRefListSetting& operator+=(T& obj)
        {
            objects.emplace_back(&obj);
            return *this;
        }

        /**
         * \brief Add by pointer.
         * \param obj Object.
         * \return *this.
         */
        ObjectRefListSetting& operator+=(T* obj)
        {
            if (obj) objects.emplace_back(obj);
            return *this;
        }

        /**
         * \brief Assign from list.
         * \param obj List of objects.
         * \return *this.
         */
        ObjectRefListSetting& operator=(const std::vector<std::unique_ptr<T>>& obj)
        {
            for (const auto& o : obj) *this += o.get();
            return *this;
        }

        /**
         * \brief Conversion to handles.
         */
        [[nodiscard]] auto get() const
        {
            using handle_t = std::decay_t<decltype(std::declval<T>().get())>;
            std::vector<handle_t> handles;
            for (const auto* o : objects) handles.emplace_back(o->get());
            return handles;
        }

    private:
        std::vector<T*> objects;
    };
}  // namespace sol