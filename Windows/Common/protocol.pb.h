// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: protocol.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_protocol_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_protocol_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3012000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3012003 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/inlined_string_field.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_protocol_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_protocol_2eproto {
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTableField entries[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::AuxillaryParseTableField aux[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTable schema[1]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::FieldMetadata field_metadata[];
  static const ::PROTOBUF_NAMESPACE_ID::internal::SerializationTable serialization_table[];
  static const ::PROTOBUF_NAMESPACE_ID::uint32 offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_protocol_2eproto;
class test01;
class test01DefaultTypeInternal;
extern test01DefaultTypeInternal _test01_default_instance_;
PROTOBUF_NAMESPACE_OPEN
template<> ::test01* Arena::CreateMaybeMessage<::test01>(Arena*);
PROTOBUF_NAMESPACE_CLOSE

// ===================================================================

class test01 PROTOBUF_FINAL :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:test01) */ {
 public:
  inline test01() : test01(nullptr) {};
  virtual ~test01();

  test01(const test01& from);
  test01(test01&& from) noexcept
    : test01() {
    *this = ::std::move(from);
  }

  inline test01& operator=(const test01& from) {
    CopyFrom(from);
    return *this;
  }
  inline test01& operator=(test01&& from) noexcept {
    if (GetArena() == from.GetArena()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return GetMetadataStatic().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return GetMetadataStatic().reflection;
  }
  static const test01& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const test01* internal_default_instance() {
    return reinterpret_cast<const test01*>(
               &_test01_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(test01& a, test01& b) {
    a.Swap(&b);
  }
  inline void Swap(test01* other) {
    if (other == this) return;
    if (GetArena() == other->GetArena()) {
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(test01* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetArena() == other->GetArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline test01* New() const final {
    return CreateMaybeMessage<test01>(nullptr);
  }

  test01* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<test01>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const test01& from);
  void MergeFrom(const test01& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  ::PROTOBUF_NAMESPACE_ID::uint8* _InternalSerialize(
      ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(test01* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "test01";
  }
  protected:
  explicit test01(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  private:
  static void ArenaDtor(void* object);
  inline void RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_protocol_2eproto);
    return ::descriptor_table_protocol_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kArrInt32FieldNumber = 2,
    kArrInt64FieldNumber = 4,
    kArrStrsFieldNumber = 6,
    kArrBytesFieldNumber = 9,
    kStrStringFieldNumber = 5,
    kNInt64FieldNumber = 3,
    kNInt32FieldNumber = 1,
    kFFloatFieldNumber = 8,
    kDDoubleFieldNumber = 7,
  };
  // repeated int32 arrInt32 = 2 [packed = true];
  int arrint32_size() const;
  private:
  int _internal_arrint32_size() const;
  public:
  void clear_arrint32();
  private:
  ::PROTOBUF_NAMESPACE_ID::int32 _internal_arrint32(int index) const;
  const ::PROTOBUF_NAMESPACE_ID::RepeatedField< ::PROTOBUF_NAMESPACE_ID::int32 >&
      _internal_arrint32() const;
  void _internal_add_arrint32(::PROTOBUF_NAMESPACE_ID::int32 value);
  ::PROTOBUF_NAMESPACE_ID::RepeatedField< ::PROTOBUF_NAMESPACE_ID::int32 >*
      _internal_mutable_arrint32();
  public:
  ::PROTOBUF_NAMESPACE_ID::int32 arrint32(int index) const;
  void set_arrint32(int index, ::PROTOBUF_NAMESPACE_ID::int32 value);
  void add_arrint32(::PROTOBUF_NAMESPACE_ID::int32 value);
  const ::PROTOBUF_NAMESPACE_ID::RepeatedField< ::PROTOBUF_NAMESPACE_ID::int32 >&
      arrint32() const;
  ::PROTOBUF_NAMESPACE_ID::RepeatedField< ::PROTOBUF_NAMESPACE_ID::int32 >*
      mutable_arrint32();

  // repeated int64 arrInt64 = 4 [packed = true];
  int arrint64_size() const;
  private:
  int _internal_arrint64_size() const;
  public:
  void clear_arrint64();
  private:
  ::PROTOBUF_NAMESPACE_ID::int64 _internal_arrint64(int index) const;
  const ::PROTOBUF_NAMESPACE_ID::RepeatedField< ::PROTOBUF_NAMESPACE_ID::int64 >&
      _internal_arrint64() const;
  void _internal_add_arrint64(::PROTOBUF_NAMESPACE_ID::int64 value);
  ::PROTOBUF_NAMESPACE_ID::RepeatedField< ::PROTOBUF_NAMESPACE_ID::int64 >*
      _internal_mutable_arrint64();
  public:
  ::PROTOBUF_NAMESPACE_ID::int64 arrint64(int index) const;
  void set_arrint64(int index, ::PROTOBUF_NAMESPACE_ID::int64 value);
  void add_arrint64(::PROTOBUF_NAMESPACE_ID::int64 value);
  const ::PROTOBUF_NAMESPACE_ID::RepeatedField< ::PROTOBUF_NAMESPACE_ID::int64 >&
      arrint64() const;
  ::PROTOBUF_NAMESPACE_ID::RepeatedField< ::PROTOBUF_NAMESPACE_ID::int64 >*
      mutable_arrint64();

  // repeated string arrStrs = 6;
  int arrstrs_size() const;
  private:
  int _internal_arrstrs_size() const;
  public:
  void clear_arrstrs();
  const std::string& arrstrs(int index) const;
  std::string* mutable_arrstrs(int index);
  void set_arrstrs(int index, const std::string& value);
  void set_arrstrs(int index, std::string&& value);
  void set_arrstrs(int index, const char* value);
  void set_arrstrs(int index, const char* value, size_t size);
  std::string* add_arrstrs();
  void add_arrstrs(const std::string& value);
  void add_arrstrs(std::string&& value);
  void add_arrstrs(const char* value);
  void add_arrstrs(const char* value, size_t size);
  const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField<std::string>& arrstrs() const;
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField<std::string>* mutable_arrstrs();
  private:
  const std::string& _internal_arrstrs(int index) const;
  std::string* _internal_add_arrstrs();
  public:

  // repeated bytes arrBytes = 9;
  int arrbytes_size() const;
  private:
  int _internal_arrbytes_size() const;
  public:
  void clear_arrbytes();
  const std::string& arrbytes(int index) const;
  std::string* mutable_arrbytes(int index);
  void set_arrbytes(int index, const std::string& value);
  void set_arrbytes(int index, std::string&& value);
  void set_arrbytes(int index, const char* value);
  void set_arrbytes(int index, const void* value, size_t size);
  std::string* add_arrbytes();
  void add_arrbytes(const std::string& value);
  void add_arrbytes(std::string&& value);
  void add_arrbytes(const char* value);
  void add_arrbytes(const void* value, size_t size);
  const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField<std::string>& arrbytes() const;
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField<std::string>* mutable_arrbytes();
  private:
  const std::string& _internal_arrbytes(int index) const;
  std::string* _internal_add_arrbytes();
  public:

  // string strString = 5;
  void clear_strstring();
  const std::string& strstring() const;
  void set_strstring(const std::string& value);
  void set_strstring(std::string&& value);
  void set_strstring(const char* value);
  void set_strstring(const char* value, size_t size);
  std::string* mutable_strstring();
  std::string* release_strstring();
  void set_allocated_strstring(std::string* strstring);
  GOOGLE_PROTOBUF_RUNTIME_DEPRECATED("The unsafe_arena_ accessors for"
  "    string fields are deprecated and will be removed in a"
  "    future release.")
  std::string* unsafe_arena_release_strstring();
  GOOGLE_PROTOBUF_RUNTIME_DEPRECATED("The unsafe_arena_ accessors for"
  "    string fields are deprecated and will be removed in a"
  "    future release.")
  void unsafe_arena_set_allocated_strstring(
      std::string* strstring);
  private:
  const std::string& _internal_strstring() const;
  void _internal_set_strstring(const std::string& value);
  std::string* _internal_mutable_strstring();
  public:

  // int64 nInt64 = 3;
  void clear_nint64();
  ::PROTOBUF_NAMESPACE_ID::int64 nint64() const;
  void set_nint64(::PROTOBUF_NAMESPACE_ID::int64 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::int64 _internal_nint64() const;
  void _internal_set_nint64(::PROTOBUF_NAMESPACE_ID::int64 value);
  public:

  // int32 nInt32 = 1;
  void clear_nint32();
  ::PROTOBUF_NAMESPACE_ID::int32 nint32() const;
  void set_nint32(::PROTOBUF_NAMESPACE_ID::int32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::int32 _internal_nint32() const;
  void _internal_set_nint32(::PROTOBUF_NAMESPACE_ID::int32 value);
  public:

  // float fFloat = 8;
  void clear_ffloat();
  float ffloat() const;
  void set_ffloat(float value);
  private:
  float _internal_ffloat() const;
  void _internal_set_ffloat(float value);
  public:

  // double dDouble = 7;
  void clear_ddouble();
  double ddouble() const;
  void set_ddouble(double value);
  private:
  double _internal_ddouble() const;
  void _internal_set_ddouble(double value);
  public:

  // @@protoc_insertion_point(class_scope:test01)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  ::PROTOBUF_NAMESPACE_ID::RepeatedField< ::PROTOBUF_NAMESPACE_ID::int32 > arrint32_;
  mutable std::atomic<int> _arrint32_cached_byte_size_;
  ::PROTOBUF_NAMESPACE_ID::RepeatedField< ::PROTOBUF_NAMESPACE_ID::int64 > arrint64_;
  mutable std::atomic<int> _arrint64_cached_byte_size_;
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField<std::string> arrstrs_;
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField<std::string> arrbytes_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr strstring_;
  ::PROTOBUF_NAMESPACE_ID::int64 nint64_;
  ::PROTOBUF_NAMESPACE_ID::int32 nint32_;
  float ffloat_;
  double ddouble_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_protocol_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// test01

// int32 nInt32 = 1;
inline void test01::clear_nint32() {
  nint32_ = 0;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 test01::_internal_nint32() const {
  return nint32_;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 test01::nint32() const {
  // @@protoc_insertion_point(field_get:test01.nInt32)
  return _internal_nint32();
}
inline void test01::_internal_set_nint32(::PROTOBUF_NAMESPACE_ID::int32 value) {
  
  nint32_ = value;
}
inline void test01::set_nint32(::PROTOBUF_NAMESPACE_ID::int32 value) {
  _internal_set_nint32(value);
  // @@protoc_insertion_point(field_set:test01.nInt32)
}

// repeated int32 arrInt32 = 2 [packed = true];
inline int test01::_internal_arrint32_size() const {
  return arrint32_.size();
}
inline int test01::arrint32_size() const {
  return _internal_arrint32_size();
}
inline void test01::clear_arrint32() {
  arrint32_.Clear();
}
inline ::PROTOBUF_NAMESPACE_ID::int32 test01::_internal_arrint32(int index) const {
  return arrint32_.Get(index);
}
inline ::PROTOBUF_NAMESPACE_ID::int32 test01::arrint32(int index) const {
  // @@protoc_insertion_point(field_get:test01.arrInt32)
  return _internal_arrint32(index);
}
inline void test01::set_arrint32(int index, ::PROTOBUF_NAMESPACE_ID::int32 value) {
  arrint32_.Set(index, value);
  // @@protoc_insertion_point(field_set:test01.arrInt32)
}
inline void test01::_internal_add_arrint32(::PROTOBUF_NAMESPACE_ID::int32 value) {
  arrint32_.Add(value);
}
inline void test01::add_arrint32(::PROTOBUF_NAMESPACE_ID::int32 value) {
  _internal_add_arrint32(value);
  // @@protoc_insertion_point(field_add:test01.arrInt32)
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedField< ::PROTOBUF_NAMESPACE_ID::int32 >&
test01::_internal_arrint32() const {
  return arrint32_;
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedField< ::PROTOBUF_NAMESPACE_ID::int32 >&
test01::arrint32() const {
  // @@protoc_insertion_point(field_list:test01.arrInt32)
  return _internal_arrint32();
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedField< ::PROTOBUF_NAMESPACE_ID::int32 >*
test01::_internal_mutable_arrint32() {
  return &arrint32_;
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedField< ::PROTOBUF_NAMESPACE_ID::int32 >*
test01::mutable_arrint32() {
  // @@protoc_insertion_point(field_mutable_list:test01.arrInt32)
  return _internal_mutable_arrint32();
}

// int64 nInt64 = 3;
inline void test01::clear_nint64() {
  nint64_ = PROTOBUF_LONGLONG(0);
}
inline ::PROTOBUF_NAMESPACE_ID::int64 test01::_internal_nint64() const {
  return nint64_;
}
inline ::PROTOBUF_NAMESPACE_ID::int64 test01::nint64() const {
  // @@protoc_insertion_point(field_get:test01.nInt64)
  return _internal_nint64();
}
inline void test01::_internal_set_nint64(::PROTOBUF_NAMESPACE_ID::int64 value) {
  
  nint64_ = value;
}
inline void test01::set_nint64(::PROTOBUF_NAMESPACE_ID::int64 value) {
  _internal_set_nint64(value);
  // @@protoc_insertion_point(field_set:test01.nInt64)
}

// repeated int64 arrInt64 = 4 [packed = true];
inline int test01::_internal_arrint64_size() const {
  return arrint64_.size();
}
inline int test01::arrint64_size() const {
  return _internal_arrint64_size();
}
inline void test01::clear_arrint64() {
  arrint64_.Clear();
}
inline ::PROTOBUF_NAMESPACE_ID::int64 test01::_internal_arrint64(int index) const {
  return arrint64_.Get(index);
}
inline ::PROTOBUF_NAMESPACE_ID::int64 test01::arrint64(int index) const {
  // @@protoc_insertion_point(field_get:test01.arrInt64)
  return _internal_arrint64(index);
}
inline void test01::set_arrint64(int index, ::PROTOBUF_NAMESPACE_ID::int64 value) {
  arrint64_.Set(index, value);
  // @@protoc_insertion_point(field_set:test01.arrInt64)
}
inline void test01::_internal_add_arrint64(::PROTOBUF_NAMESPACE_ID::int64 value) {
  arrint64_.Add(value);
}
inline void test01::add_arrint64(::PROTOBUF_NAMESPACE_ID::int64 value) {
  _internal_add_arrint64(value);
  // @@protoc_insertion_point(field_add:test01.arrInt64)
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedField< ::PROTOBUF_NAMESPACE_ID::int64 >&
test01::_internal_arrint64() const {
  return arrint64_;
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedField< ::PROTOBUF_NAMESPACE_ID::int64 >&
test01::arrint64() const {
  // @@protoc_insertion_point(field_list:test01.arrInt64)
  return _internal_arrint64();
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedField< ::PROTOBUF_NAMESPACE_ID::int64 >*
test01::_internal_mutable_arrint64() {
  return &arrint64_;
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedField< ::PROTOBUF_NAMESPACE_ID::int64 >*
test01::mutable_arrint64() {
  // @@protoc_insertion_point(field_mutable_list:test01.arrInt64)
  return _internal_mutable_arrint64();
}

// string strString = 5;
inline void test01::clear_strstring() {
  strstring_.ClearToEmpty(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline const std::string& test01::strstring() const {
  // @@protoc_insertion_point(field_get:test01.strString)
  return _internal_strstring();
}
inline void test01::set_strstring(const std::string& value) {
  _internal_set_strstring(value);
  // @@protoc_insertion_point(field_set:test01.strString)
}
inline std::string* test01::mutable_strstring() {
  // @@protoc_insertion_point(field_mutable:test01.strString)
  return _internal_mutable_strstring();
}
inline const std::string& test01::_internal_strstring() const {
  return strstring_.Get();
}
inline void test01::_internal_set_strstring(const std::string& value) {
  
  strstring_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), value, GetArena());
}
inline void test01::set_strstring(std::string&& value) {
  
  strstring_.Set(
    &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::move(value), GetArena());
  // @@protoc_insertion_point(field_set_rvalue:test01.strString)
}
inline void test01::set_strstring(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  
  strstring_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::string(value),
              GetArena());
  // @@protoc_insertion_point(field_set_char:test01.strString)
}
inline void test01::set_strstring(const char* value,
    size_t size) {
  
  strstring_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::string(
      reinterpret_cast<const char*>(value), size), GetArena());
  // @@protoc_insertion_point(field_set_pointer:test01.strString)
}
inline std::string* test01::_internal_mutable_strstring() {
  
  return strstring_.Mutable(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline std::string* test01::release_strstring() {
  // @@protoc_insertion_point(field_release:test01.strString)
  return strstring_.Release(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline void test01::set_allocated_strstring(std::string* strstring) {
  if (strstring != nullptr) {
    
  } else {
    
  }
  strstring_.SetAllocated(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), strstring,
      GetArena());
  // @@protoc_insertion_point(field_set_allocated:test01.strString)
}
inline std::string* test01::unsafe_arena_release_strstring() {
  // @@protoc_insertion_point(field_unsafe_arena_release:test01.strString)
  GOOGLE_DCHECK(GetArena() != nullptr);
  
  return strstring_.UnsafeArenaRelease(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
      GetArena());
}
inline void test01::unsafe_arena_set_allocated_strstring(
    std::string* strstring) {
  GOOGLE_DCHECK(GetArena() != nullptr);
  if (strstring != nullptr) {
    
  } else {
    
  }
  strstring_.UnsafeArenaSetAllocated(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
      strstring, GetArena());
  // @@protoc_insertion_point(field_unsafe_arena_set_allocated:test01.strString)
}

// repeated string arrStrs = 6;
inline int test01::_internal_arrstrs_size() const {
  return arrstrs_.size();
}
inline int test01::arrstrs_size() const {
  return _internal_arrstrs_size();
}
inline void test01::clear_arrstrs() {
  arrstrs_.Clear();
}
inline std::string* test01::add_arrstrs() {
  // @@protoc_insertion_point(field_add_mutable:test01.arrStrs)
  return _internal_add_arrstrs();
}
inline const std::string& test01::_internal_arrstrs(int index) const {
  return arrstrs_.Get(index);
}
inline const std::string& test01::arrstrs(int index) const {
  // @@protoc_insertion_point(field_get:test01.arrStrs)
  return _internal_arrstrs(index);
}
inline std::string* test01::mutable_arrstrs(int index) {
  // @@protoc_insertion_point(field_mutable:test01.arrStrs)
  return arrstrs_.Mutable(index);
}
inline void test01::set_arrstrs(int index, const std::string& value) {
  // @@protoc_insertion_point(field_set:test01.arrStrs)
  arrstrs_.Mutable(index)->assign(value);
}
inline void test01::set_arrstrs(int index, std::string&& value) {
  // @@protoc_insertion_point(field_set:test01.arrStrs)
  arrstrs_.Mutable(index)->assign(std::move(value));
}
inline void test01::set_arrstrs(int index, const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  arrstrs_.Mutable(index)->assign(value);
  // @@protoc_insertion_point(field_set_char:test01.arrStrs)
}
inline void test01::set_arrstrs(int index, const char* value, size_t size) {
  arrstrs_.Mutable(index)->assign(
    reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:test01.arrStrs)
}
inline std::string* test01::_internal_add_arrstrs() {
  return arrstrs_.Add();
}
inline void test01::add_arrstrs(const std::string& value) {
  arrstrs_.Add()->assign(value);
  // @@protoc_insertion_point(field_add:test01.arrStrs)
}
inline void test01::add_arrstrs(std::string&& value) {
  arrstrs_.Add(std::move(value));
  // @@protoc_insertion_point(field_add:test01.arrStrs)
}
inline void test01::add_arrstrs(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  arrstrs_.Add()->assign(value);
  // @@protoc_insertion_point(field_add_char:test01.arrStrs)
}
inline void test01::add_arrstrs(const char* value, size_t size) {
  arrstrs_.Add()->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_add_pointer:test01.arrStrs)
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField<std::string>&
test01::arrstrs() const {
  // @@protoc_insertion_point(field_list:test01.arrStrs)
  return arrstrs_;
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField<std::string>*
test01::mutable_arrstrs() {
  // @@protoc_insertion_point(field_mutable_list:test01.arrStrs)
  return &arrstrs_;
}

// double dDouble = 7;
inline void test01::clear_ddouble() {
  ddouble_ = 0;
}
inline double test01::_internal_ddouble() const {
  return ddouble_;
}
inline double test01::ddouble() const {
  // @@protoc_insertion_point(field_get:test01.dDouble)
  return _internal_ddouble();
}
inline void test01::_internal_set_ddouble(double value) {
  
  ddouble_ = value;
}
inline void test01::set_ddouble(double value) {
  _internal_set_ddouble(value);
  // @@protoc_insertion_point(field_set:test01.dDouble)
}

// float fFloat = 8;
inline void test01::clear_ffloat() {
  ffloat_ = 0;
}
inline float test01::_internal_ffloat() const {
  return ffloat_;
}
inline float test01::ffloat() const {
  // @@protoc_insertion_point(field_get:test01.fFloat)
  return _internal_ffloat();
}
inline void test01::_internal_set_ffloat(float value) {
  
  ffloat_ = value;
}
inline void test01::set_ffloat(float value) {
  _internal_set_ffloat(value);
  // @@protoc_insertion_point(field_set:test01.fFloat)
}

// repeated bytes arrBytes = 9;
inline int test01::_internal_arrbytes_size() const {
  return arrbytes_.size();
}
inline int test01::arrbytes_size() const {
  return _internal_arrbytes_size();
}
inline void test01::clear_arrbytes() {
  arrbytes_.Clear();
}
inline std::string* test01::add_arrbytes() {
  // @@protoc_insertion_point(field_add_mutable:test01.arrBytes)
  return _internal_add_arrbytes();
}
inline const std::string& test01::_internal_arrbytes(int index) const {
  return arrbytes_.Get(index);
}
inline const std::string& test01::arrbytes(int index) const {
  // @@protoc_insertion_point(field_get:test01.arrBytes)
  return _internal_arrbytes(index);
}
inline std::string* test01::mutable_arrbytes(int index) {
  // @@protoc_insertion_point(field_mutable:test01.arrBytes)
  return arrbytes_.Mutable(index);
}
inline void test01::set_arrbytes(int index, const std::string& value) {
  // @@protoc_insertion_point(field_set:test01.arrBytes)
  arrbytes_.Mutable(index)->assign(value);
}
inline void test01::set_arrbytes(int index, std::string&& value) {
  // @@protoc_insertion_point(field_set:test01.arrBytes)
  arrbytes_.Mutable(index)->assign(std::move(value));
}
inline void test01::set_arrbytes(int index, const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  arrbytes_.Mutable(index)->assign(value);
  // @@protoc_insertion_point(field_set_char:test01.arrBytes)
}
inline void test01::set_arrbytes(int index, const void* value, size_t size) {
  arrbytes_.Mutable(index)->assign(
    reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:test01.arrBytes)
}
inline std::string* test01::_internal_add_arrbytes() {
  return arrbytes_.Add();
}
inline void test01::add_arrbytes(const std::string& value) {
  arrbytes_.Add()->assign(value);
  // @@protoc_insertion_point(field_add:test01.arrBytes)
}
inline void test01::add_arrbytes(std::string&& value) {
  arrbytes_.Add(std::move(value));
  // @@protoc_insertion_point(field_add:test01.arrBytes)
}
inline void test01::add_arrbytes(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  arrbytes_.Add()->assign(value);
  // @@protoc_insertion_point(field_add_char:test01.arrBytes)
}
inline void test01::add_arrbytes(const void* value, size_t size) {
  arrbytes_.Add()->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_add_pointer:test01.arrBytes)
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField<std::string>&
test01::arrbytes() const {
  // @@protoc_insertion_point(field_list:test01.arrBytes)
  return arrbytes_;
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField<std::string>*
test01::mutable_arrbytes() {
  // @@protoc_insertion_point(field_mutable_list:test01.arrBytes)
  return &arrbytes_;
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)


// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_protocol_2eproto