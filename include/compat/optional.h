#ifndef CUSTOMOS_COMPAT_OPTIONAL_H
#define CUSTOMOS_COMPAT_OPTIONAL_H

// Compatibility layer for std::optional
// Provides fallback for compilers without C++17 support

#if __cplusplus >= 201703L && defined(__has_include)
  #if __has_include(<optional>)
    #include <optional>
    namespace customos {
      template<typename T>
      using optional = std::optional<T>;
      constexpr auto nullopt = std::nullopt;
    }
    #define HAVE_STD_OPTIONAL 1
  #endif
#endif

#ifndef HAVE_STD_OPTIONAL
  // Fallback implementation for older compilers
  #include <stdexcept>
  #include <utility>
  
  namespace customos {
    
    struct nullopt_t {
      explicit constexpr nullopt_t(int) {}
    };
    constexpr nullopt_t nullopt{0};
    
    template<typename T>
    class optional {
    private:
      bool has_value_;
      alignas(T) unsigned char storage_[sizeof(T)];
      
      T* ptr() { return reinterpret_cast<T*>(storage_); }
      const T* ptr() const { return reinterpret_cast<const T*>(storage_); }
      
    public:
      optional() : has_value_(false) {}
      
      optional(nullopt_t) : has_value_(false) {}
      
      optional(const T& value) : has_value_(true) {
        new (storage_) T(value);
      }
      
      optional(T&& value) : has_value_(true) {
        new (storage_) T(std::move(value));
      }
      
      optional(const optional& other) : has_value_(other.has_value_) {
        if (has_value_) {
          new (storage_) T(*other.ptr());
        }
      }
      
      optional(optional&& other) : has_value_(other.has_value_) {
        if (has_value_) {
          new (storage_) T(std::move(*other.ptr()));
          other.reset();
        }
      }
      
      ~optional() {
        reset();
      }
      
      optional& operator=(nullopt_t) {
        reset();
        return *this;
      }
      
      optional& operator=(const T& value) {
        if (has_value_) {
          *ptr() = value;
        } else {
          new (storage_) T(value);
          has_value_ = true;
        }
        return *this;
      }
      
      optional& operator=(T&& value) {
        if (has_value_) {
          *ptr() = std::move(value);
        } else {
          new (storage_) T(std::move(value));
          has_value_ = true;
        }
        return *this;
      }
      
      optional& operator=(const optional& other) {
        if (this != &other) {
          if (other.has_value_) {
            *this = *other.ptr();
          } else {
            reset();
          }
        }
        return *this;
      }
      
      optional& operator=(optional&& other) {
        if (this != &other) {
          if (other.has_value_) {
            *this = std::move(*other.ptr());
            other.reset();
          } else {
            reset();
          }
        }
        return *this;
      }
      
      const T* operator->() const { return ptr(); }
      T* operator->() { return ptr(); }
      
      const T& operator*() const & { return *ptr(); }
      T& operator*() & { return *ptr(); }
      T&& operator*() && { return std::move(*ptr()); }
      const T&& operator*() const && { return std::move(*ptr()); }
      
      explicit operator bool() const { return has_value_; }
      bool has_value() const { return has_value_; }
      
      T& value() & {
        if (!has_value_) throw std::runtime_error("bad optional access");
        return *ptr();
      }
      
      const T& value() const & {
        if (!has_value_) throw std::runtime_error("bad optional access");
        return *ptr();
      }
      
      T&& value() && {
        if (!has_value_) throw std::runtime_error("bad optional access");
        return std::move(*ptr());
      }
      
      const T&& value() const && {
        if (!has_value_) throw std::runtime_error("bad optional access");
        return std::move(*ptr());
      }
      
      template<typename U>
      T value_or(U&& default_value) const & {
        return has_value_ ? *ptr() : static_cast<T>(std::forward<U>(default_value));
      }
      
      template<typename U>
      T value_or(U&& default_value) && {
        return has_value_ ? std::move(*ptr()) : static_cast<T>(std::forward<U>(default_value));
      }
      
      void reset() {
        if (has_value_) {
          ptr()->~T();
          has_value_ = false;
        }
      }
      
      template<typename... Args>
      T& emplace(Args&&... args) {
        reset();
        new (storage_) T(std::forward<Args>(args)...);
        has_value_ = true;
        return *ptr();
      }
    };
    
  } // namespace customos
  
  #define HAVE_STD_OPTIONAL 0
#endif

#endif // CUSTOMOS_COMPAT_OPTIONAL_H
