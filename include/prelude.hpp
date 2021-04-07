#ifndef PRELUDE_H
#define PRELUDE_H

#pragma once
#include <memory>
#include <optional>
#include <stdint.h>
#include <string>
#include <variant>

/** type synonyms */
// using isize = intmax_t;
using i8    = int8_t;
using i16   = int16_t;
using i32   = int32_t;
using i64   = int64_t;
using usize = size_t;
using u8    = uint8_t;
using u16   = uint16_t;
using u32   = uint32_t;
using u64   = uint64_t;

using std::make_optional;
using std::make_shared;
using std::make_unique;
using std::optional;
using std::shared_ptr;
using std::string;
using std::unique_ptr;
using std::variant;

template <typename T>
using option = std::optional<T>;
template <typename T>
using Rc = std::shared_ptr<T>; // Arc would be better?

/** overload */
template <class... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};
// required in sepples17
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

/** variant trait */

#endif
