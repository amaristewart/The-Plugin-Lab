#pragma once
#include <JuceHeader.h>

/**
 * Template class to add weak reference support to any object
 * Use this when you need to safely reference objects that might be deleted
 */
template <class ObjectType>
class WeakReferenceHolder : public juce::ReferenceCountedObject
{
public:
    using Ptr = juce::ReferenceCountedObjectPtr<WeakReferenceHolder>;
    
    explicit WeakReferenceHolder(ObjectType* object) : target(object) {}
    
    ObjectType* get() const noexcept
    {
        return target;
    }
    
    void clear() noexcept
    {
        target = nullptr;
    }
    
private:
    ObjectType* target;
};

/**
 * A safe weak reference to an object that might be deleted
 * Use this when you need to store references to objects that might be deleted by other code
 */
template <class ObjectType>
class SafePointer
{
public:
    SafePointer() = default;
    
    SafePointer(ObjectType* object)
    {
        if (object != nullptr)
        {
            if (!object->safePointerSourceInternal)
                object->safePointerSourceInternal = new WeakReferenceHolder<ObjectType>(object);
                
            holder = object->safePointerSourceInternal;
        }
    }
    
    SafePointer(const SafePointer& other) : holder(other.holder) {}
    
    SafePointer& operator= (const SafePointer& other)
    {
        holder = other.holder;
        return *this;
    }
    
    SafePointer& operator= (ObjectType* newObject)
    {
        *this = SafePointer(newObject);
        return *this;
    }
    
    ObjectType* get() const noexcept
    {
        return holder != nullptr ? holder->get() : nullptr;
    }
    
    void clear() { holder = nullptr; }
    
    ObjectType* operator->() const noexcept            { return get(); }
    operator ObjectType*() const noexcept              { return get(); }
    bool operator== (ObjectType* object) const noexcept { return object == get(); }
    bool operator!= (ObjectType* object) const noexcept { return object != get(); }
    
    // Helper to attach to an object that supports this pattern
    static void attachTo(ObjectType& object, SafePointer& pointer)
    {
        pointer.holder = object.safePointerSourceInternal;
        
        if (pointer.holder == nullptr)
        {
            object.safePointerSourceInternal = new WeakReferenceHolder<ObjectType>(&object);
            pointer.holder = object.safePointerSourceInternal;
        }
    }
    
private:
    typename WeakReferenceHolder<ObjectType>::Ptr holder;
};

/**
 * Mixin base class that adds weak reference support to a class
 */
template <class DerivedClass>
class WeakReferenceable
{
public:
    WeakReferenceable() = default;
    
    WeakReferenceable(const WeakReferenceable&) : safePointerSourceInternal(nullptr) {}
    WeakReferenceable& operator= (const WeakReferenceable&) { return *this; }
    
    ~WeakReferenceable()
    {
        if (safePointerSourceInternal != nullptr)
            safePointerSourceInternal->clear();
    }
    
    // The internal reference holder
    typename WeakReferenceHolder<DerivedClass>::Ptr safePointerSourceInternal;
};
