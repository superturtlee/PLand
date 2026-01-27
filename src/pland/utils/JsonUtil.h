#pragma once
#include "ll/api/reflection/Deserialization.h"
#include "ll/api/reflection/Serialization.h"
#include "nlohmann/json.hpp"
#include "nlohmann/json_fwd.hpp"
#include <algorithm>
#include <iostream>
#include <string_view>


namespace land {

namespace json_util {


template <class T>
concept HasVersion =
    ll::reflection::Reflectable<T> && std::integral<std::remove_cvref_t<decltype((std::declval<T>().version))>>;

template <typename T>
[[nodiscard]] inline nlohmann::ordered_json struct2json(T& obj) {
    return ll::reflection::serialize<nlohmann::ordered_json>(obj).value();
}

template <typename T, typename J = nlohmann::ordered_json>
inline void json2struct(J& j, T& obj) {
    ll::reflection::deserialize(obj, j).value();
}

template <typename T, typename J = nlohmann::ordered_json>
inline void json2structWithDiffPatch(J& j, T& obj) {
    auto patch = ll::reflection::serialize<nlohmann::ordered_json>(obj).value();
    patch.merge_patch(j);
    json2struct(patch, obj);
}


template <HasVersion T, class J>
inline bool _VersionPatcher(T& obj, J& data) {
    data.erase("version");
    auto patch = ll::reflection::serialize<J>(obj);
    patch.value().merge_patch(data);
    data = *std::move(patch);
    return true;
}

template <typename T, typename J = nlohmann::ordered_json>
inline void json2structWithVersionPatch(J& j, T& obj) {
    bool noNeedMerge = true;
    if (!j.contains("version") || (int64)(j["version"]) != obj.version) {
        noNeedMerge = false;
    }
    if (noNeedMerge || _VersionPatcher(obj, j)) {
        ll::reflection::deserialize(obj, j).value();
    }
}


} // namespace json_util

} // namespace land
