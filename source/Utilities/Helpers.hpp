#pragma once

template<class T>
using Ref = std::reference_wrapper<T>;

template<class T>
using SharedPtr = std::shared_ptr<T>;

template<class T>
using UniquePtr = std::unique_ptr<T>;

template<class T>
using WeakPtr = std::weak_ptr<T>;

template <typename T>
struct DefaultedSharedPtr : std::shared_ptr<T>
{
    using std::shared_ptr<T>::shared_ptr;
    DefaultedSharedPtr() : std::shared_ptr<T>(std::make_shared<T>()) {}
};

template<typename... Types>
struct TypeList {};

template<typename Type, typename... Ts>
constexpr inline bool TypelistContainsType(TypeList<Ts...>&&)
{ 
    return (std::is_same<Type, Ts>::value || ...);
}

template<typename ... Args>
std::string StringFormat(char const* const format, Args ... args)
{
    int32_t const size = std::snprintf(nullptr, 0, format, args ...) + 1; // Extra space for '\0'
    if (size <= 0)
    {
        return std::string();
    }

    std::unique_ptr<char[]> buffer = std::make_unique<char[]>(size);
    std::snprintf(buffer.get(), size, format, args ...);

    return std::string(buffer.get(), buffer.get() + size - 1); // We don't want the '\0' inside
}

template<typename ... Args>
void ThrowError(char const* const format, Args ... args)
{
    std::string const& message = StringFormat(format, std::forward<Args>(args)...);
    std::cerr << "[Error] " << message << std::endl;
    throw std::runtime_error(message.c_str());
}

template<typename ... Args>
void Warn(char const* const format, Args ... args)
{
    std::string const& message = StringFormat(format, std::forward<Args>(args)...);
    std::cerr << "[Warning] " << message << std::endl;
}

template<typename ... Args>
void Log(char const* const format, Args ... args)
{
    std::string const& message = StringFormat(format, std::forward<Args>(args)...);
    std::cout << message << std::endl;
}

template<typename ... Args>
inline void Assert(bool condition, char const* const format, Args ... args)
{
    if (!condition)
    {
        ThrowError(format, std::forward<Args>(args)...);
    }
}

template<class C, class T>
inline auto Contains(const C& v, const T& x)
-> decltype(end(v), true)
{
    return end(v) != std::find(begin(v), end(v), x);
}

template<class C, class T>
inline void EraseFirstMatch(C& c, const T& v)
{
    auto foundIt = std::find(c.begin(), c.end(), v);
    if (foundIt != c.end())
        c.erase(foundIt);
}

inline void SetEnvVariable(char const* const name, char const* const value)
{
#ifdef __unix
    setenv(name, value, true);
#elif _WIN32
    _putenv_s(name, value);
#endif
}

std::string ReadShaderFile(std::string const& shaderName);

std::string ReadFile(std::string const& fileName);

// Reverse iterator
template<typename Iterator>
class Range
{
public:
    Range(Iterator begin, Iterator end) : m_begin(begin), m_end(end) { }
    inline Iterator begin() const { return m_begin; }
    inline Iterator end() const { return m_end; }

private:
    Iterator m_begin, m_end;
};

template<typename TRange, typename TIterator = decltype(std::begin(std::declval<TRange>())), typename Iterator = std::reverse_iterator<TIterator>>
inline Range<Iterator> Reverse(TRange&& originalRange) {
    return Range<Iterator>(Iterator(std::end(originalRange)), Iterator(std::begin(originalRange)));
}

// Vulkan
template<typename MainT, typename NewT>
inline void PNextChainPushFront(MainT* mainStruct, NewT* newStruct)
{
    newStruct->pNext = mainStruct->pNext;
    mainStruct->pNext = newStruct;
}

template<typename MainT, typename NewT>
inline void PNextChainPushBack(MainT* mainStruct, NewT* newStruct)
{
    struct VkAnyStruct
    {
        VkStructureType sType;
        void* pNext;
    };

    VkAnyStruct* lastStruct = (VkAnyStruct*)mainStruct;
    while(lastStruct->pNext != nullptr)
    {
        lastStruct = (VkAnyStruct*)lastStruct->pNext;
    }
    
    newStruct->pNext = nullptr;
    lastStruct->pNext = newStruct;
}

inline bool operator<(VkDescriptorSetLayoutBinding const& lhs, VkDescriptorSetLayoutBinding const& rhs)
{
    return std::tie(lhs.binding, lhs.descriptorType, lhs.descriptorCount, lhs.stageFlags, lhs.pImmutableSamplers) 
        < std::tie(rhs.binding, rhs.descriptorType, rhs.descriptorCount, rhs.stageFlags, rhs.pImmutableSamplers);
}


// Enums
#define DefineEnumFlagOperators(EnumType) \
    inline EnumType operator~ (EnumType v1) { return (EnumType)~(std::underlying_type_t<EnumType>)v1; } \
    inline EnumType operator| (EnumType v1, EnumType v2) { return (EnumType)((std::underlying_type_t<EnumType>)v1 | (std::underlying_type_t<EnumType>)v2); } \
    inline EnumType operator& (EnumType v1, EnumType v2) { return (EnumType)((std::underlying_type_t<EnumType>)v1 & (std::underlying_type_t<EnumType>)v2); } \
    inline EnumType operator^ (EnumType v1, EnumType v2) { return (EnumType)((std::underlying_type_t<EnumType>)v1 ^ (std::underlying_type_t<EnumType>)v2); } \
    inline EnumType& operator|= (EnumType& v1, EnumType v2) { return (EnumType&)((std::underlying_type_t<EnumType>&)v1 |= (std::underlying_type_t<EnumType>)v2); } \
    inline EnumType& operator&= (EnumType& v1, EnumType v2) { return (EnumType&)((std::underlying_type_t<EnumType>&)v1 &= (std::underlying_type_t<EnumType>)v2); } \
    inline EnumType& operator^= (EnumType& v1, EnumType v2) { return (EnumType&)((std::underlying_type_t<EnumType>&)v1 ^= (std::underlying_type_t<EnumType>)v2); } \
