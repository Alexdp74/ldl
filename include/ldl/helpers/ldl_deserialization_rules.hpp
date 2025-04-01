#include <type_traits>
#include <tuple>


namespace little_deserialization_library::deserialization_rules
{
    template<typename T> struct rule
    {
        static_assert(!std::is_same_v<T, T>, "Missing deserialization rule for type T");
    };

    template<typename T> using rule_t = rule<T>::type;
}
