// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: protocol.proto

#include "protocol.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/port.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// This is a temporary google only hack
#ifdef GOOGLE_PROTOBUF_ENFORCE_UNIQUENESS
#include "third_party/protobuf/version.h"
#endif
// @@protoc_insertion_point(includes)
namespace test_protobuf {
class inner_testDefaultTypeInternal {
 public:
  ::google::protobuf::internal::ExplicitlyConstructed<inner_test>
      _instance;
} _inner_test_default_instance_;
class testDefaultTypeInternal {
 public:
  ::google::protobuf::internal::ExplicitlyConstructed<test>
      _instance;
} _test_default_instance_;
}  // namespace test_protobuf
namespace protobuf_protocol_2eproto {
void InitDefaultsinner_testImpl() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

#ifdef GOOGLE_PROTOBUF_ENFORCE_UNIQUENESS
  ::google::protobuf::internal::InitProtobufDefaultsForceUnique();
#else
  ::google::protobuf::internal::InitProtobufDefaults();
#endif  // GOOGLE_PROTOBUF_ENFORCE_UNIQUENESS
  {
    void* ptr = &::test_protobuf::_inner_test_default_instance_;
    new (ptr) ::test_protobuf::inner_test();
    ::google::protobuf::internal::OnShutdownDestroyMessage(ptr);
  }
  ::test_protobuf::inner_test::InitAsDefaultInstance();
}

void InitDefaultsinner_test() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &InitDefaultsinner_testImpl);
}

void InitDefaultstestImpl() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

#ifdef GOOGLE_PROTOBUF_ENFORCE_UNIQUENESS
  ::google::protobuf::internal::InitProtobufDefaultsForceUnique();
#else
  ::google::protobuf::internal::InitProtobufDefaults();
#endif  // GOOGLE_PROTOBUF_ENFORCE_UNIQUENESS
  protobuf_protocol_2eproto::InitDefaultsinner_test();
  {
    void* ptr = &::test_protobuf::_test_default_instance_;
    new (ptr) ::test_protobuf::test();
    ::google::protobuf::internal::OnShutdownDestroyMessage(ptr);
  }
  ::test_protobuf::test::InitAsDefaultInstance();
}

void InitDefaultstest() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &InitDefaultstestImpl);
}

::google::protobuf::Metadata file_level_metadata[2];
const ::google::protobuf::EnumDescriptor* file_level_enum_descriptors[1];

const ::google::protobuf::uint32 TableStruct::offsets[] GOOGLE_PROTOBUF_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::test_protobuf::inner_test, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::test_protobuf::inner_test, ninnertest_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::test_protobuf::inner_test, strinnertest_),
  ~0u,  // no _has_bits_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::test_protobuf::test, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::test_protobuf::test, ntest_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::test_protobuf::test, sntest_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::test_protobuf::test, strtest_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::test_protobuf::test, innertest_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::test_protobuf::test, ntestarray_),
};
static const ::google::protobuf::internal::MigrationSchema schemas[] GOOGLE_PROTOBUF_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, sizeof(::test_protobuf::inner_test)},
  { 7, -1, sizeof(::test_protobuf::test)},
};

static ::google::protobuf::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::google::protobuf::Message*>(&::test_protobuf::_inner_test_default_instance_),
  reinterpret_cast<const ::google::protobuf::Message*>(&::test_protobuf::_test_default_instance_),
};

void protobuf_AssignDescriptors() {
  AddDescriptors();
  ::google::protobuf::MessageFactory* factory = NULL;
  AssignDescriptors(
      "protocol.proto", schemas, file_default_instances, TableStruct::offsets, factory,
      file_level_metadata, file_level_enum_descriptors, NULL);
}

void protobuf_AssignDescriptorsOnce() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &protobuf_AssignDescriptors);
}

void protobuf_RegisterTypes(const ::std::string&) GOOGLE_PROTOBUF_ATTRIBUTE_COLD;
void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::internal::RegisterAllTypes(file_level_metadata, 2);
}

void AddDescriptorsImpl() {
  InitDefaults();
  static const char descriptor[] GOOGLE_PROTOBUF_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
      "\n\016protocol.proto\022\rtest_protobuf\"6\n\ninner"
      "_test\022\022\n\nnInnerTest\030\001 \001(\005\022\024\n\014strInnerTes"
      "t\030\002 \001(\t\"\231\001\n\004test\022\r\n\005nTest\030\001 \001(\005\022\016\n\006snTes"
      "t\030\002 \001(\021\022\017\n\007strTest\030\003 \001(\t\022,\n\tinnerTest\030\004 "
      "\001(\0132\031.test_protobuf.inner_test\022\022\n\nnTestA"
      "rray\030\005 \003(\005\"\037\n\006enTest\022\013\n\007DEFAULT\020\000\022\010\n\004TES"
      "T\020\001b\006proto3"
  };
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
      descriptor, 251);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "protocol.proto", &protobuf_RegisterTypes);
}

void AddDescriptors() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &AddDescriptorsImpl);
}
// Force AddDescriptors() to be called at dynamic initialization time.
struct StaticDescriptorInitializer {
  StaticDescriptorInitializer() {
    AddDescriptors();
  }
} static_descriptor_initializer;
}  // namespace protobuf_protocol_2eproto
namespace test_protobuf {
const ::google::protobuf::EnumDescriptor* test_enTest_descriptor() {
  protobuf_protocol_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_protocol_2eproto::file_level_enum_descriptors[0];
}
bool test_enTest_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
      return true;
    default:
      return false;
  }
}

#if !defined(_MSC_VER) || _MSC_VER >= 1900
const test_enTest test::DEFAULT;
const test_enTest test::TEST;
const test_enTest test::enTest_MIN;
const test_enTest test::enTest_MAX;
const int test::enTest_ARRAYSIZE;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

// ===================================================================

void inner_test::InitAsDefaultInstance() {
}
#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int inner_test::kNInnerTestFieldNumber;
const int inner_test::kStrInnerTestFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

inner_test::inner_test()
  : ::google::protobuf::Message(), _internal_metadata_(NULL) {
  if (GOOGLE_PREDICT_TRUE(this != internal_default_instance())) {
    ::protobuf_protocol_2eproto::InitDefaultsinner_test();
  }
  SharedCtor();
  // @@protoc_insertion_point(constructor:test_protobuf.inner_test)
}
inner_test::inner_test(const inner_test& from)
  : ::google::protobuf::Message(),
      _internal_metadata_(NULL),
      _cached_size_(0) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  strinnertest_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (from.strinnertest().size() > 0) {
    strinnertest_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.strinnertest_);
  }
  ninnertest_ = from.ninnertest_;
  // @@protoc_insertion_point(copy_constructor:test_protobuf.inner_test)
}

void inner_test::SharedCtor() {
  strinnertest_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  ninnertest_ = 0;
  _cached_size_ = 0;
}

inner_test::~inner_test() {
  // @@protoc_insertion_point(destructor:test_protobuf.inner_test)
  SharedDtor();
}

void inner_test::SharedDtor() {
  strinnertest_.DestroyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}

void inner_test::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* inner_test::descriptor() {
  ::protobuf_protocol_2eproto::protobuf_AssignDescriptorsOnce();
  return ::protobuf_protocol_2eproto::file_level_metadata[kIndexInFileMessages].descriptor;
}

const inner_test& inner_test::default_instance() {
  ::protobuf_protocol_2eproto::InitDefaultsinner_test();
  return *internal_default_instance();
}

inner_test* inner_test::New(::google::protobuf::Arena* arena) const {
  inner_test* n = new inner_test;
  if (arena != NULL) {
    arena->Own(n);
  }
  return n;
}

void inner_test::Clear() {
// @@protoc_insertion_point(message_clear_start:test_protobuf.inner_test)
  ::google::protobuf::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  strinnertest_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  ninnertest_ = 0;
  _internal_metadata_.Clear();
}

bool inner_test::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!GOOGLE_PREDICT_TRUE(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:test_protobuf.inner_test)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(127u);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // int32 nInnerTest = 1;
      case 1: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(8u /* 8 & 0xFF */)) {

          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &ninnertest_)));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // string strInnerTest = 2;
      case 2: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(18u /* 18 & 0xFF */)) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_strinnertest()));
          DO_(::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
            this->strinnertest().data(), static_cast<int>(this->strinnertest().length()),
            ::google::protobuf::internal::WireFormatLite::PARSE,
            "test_protobuf.inner_test.strInnerTest"));
        } else {
          goto handle_unusual;
        }
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, _internal_metadata_.mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:test_protobuf.inner_test)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:test_protobuf.inner_test)
  return false;
#undef DO_
}

void inner_test::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:test_protobuf.inner_test)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // int32 nInnerTest = 1;
  if (this->ninnertest() != 0) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(1, this->ninnertest(), output);
  }

  // string strInnerTest = 2;
  if (this->strinnertest().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->strinnertest().data(), static_cast<int>(this->strinnertest().length()),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "test_protobuf.inner_test.strInnerTest");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      2, this->strinnertest(), output);
  }

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()), output);
  }
  // @@protoc_insertion_point(serialize_end:test_protobuf.inner_test)
}

::google::protobuf::uint8* inner_test::InternalSerializeWithCachedSizesToArray(
    bool deterministic, ::google::protobuf::uint8* target) const {
  (void)deterministic; // Unused
  // @@protoc_insertion_point(serialize_to_array_start:test_protobuf.inner_test)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // int32 nInnerTest = 1;
  if (this->ninnertest() != 0) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(1, this->ninnertest(), target);
  }

  // string strInnerTest = 2;
  if (this->strinnertest().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->strinnertest().data(), static_cast<int>(this->strinnertest().length()),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "test_protobuf.inner_test.strInnerTest");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        2, this->strinnertest(), target);
  }

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:test_protobuf.inner_test)
  return target;
}

size_t inner_test::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:test_protobuf.inner_test)
  size_t total_size = 0;

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()));
  }
  // string strInnerTest = 2;
  if (this->strinnertest().size() > 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::StringSize(
        this->strinnertest());
  }

  // int32 nInnerTest = 1;
  if (this->ninnertest() != 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::Int32Size(
        this->ninnertest());
  }

  int cached_size = ::google::protobuf::internal::ToCachedSize(total_size);
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = cached_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void inner_test::MergeFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:test_protobuf.inner_test)
  GOOGLE_DCHECK_NE(&from, this);
  const inner_test* source =
      ::google::protobuf::internal::DynamicCastToGenerated<const inner_test>(
          &from);
  if (source == NULL) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:test_protobuf.inner_test)
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:test_protobuf.inner_test)
    MergeFrom(*source);
  }
}

void inner_test::MergeFrom(const inner_test& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:test_protobuf.inner_test)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.strinnertest().size() > 0) {

    strinnertest_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.strinnertest_);
  }
  if (from.ninnertest() != 0) {
    set_ninnertest(from.ninnertest());
  }
}

void inner_test::CopyFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:test_protobuf.inner_test)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void inner_test::CopyFrom(const inner_test& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:test_protobuf.inner_test)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool inner_test::IsInitialized() const {
  return true;
}

void inner_test::Swap(inner_test* other) {
  if (other == this) return;
  InternalSwap(other);
}
void inner_test::InternalSwap(inner_test* other) {
  using std::swap;
  strinnertest_.Swap(&other->strinnertest_);
  swap(ninnertest_, other->ninnertest_);
  _internal_metadata_.Swap(&other->_internal_metadata_);
  swap(_cached_size_, other->_cached_size_);
}

::google::protobuf::Metadata inner_test::GetMetadata() const {
  protobuf_protocol_2eproto::protobuf_AssignDescriptorsOnce();
  return ::protobuf_protocol_2eproto::file_level_metadata[kIndexInFileMessages];
}


// ===================================================================

void test::InitAsDefaultInstance() {
  ::test_protobuf::_test_default_instance_._instance.get_mutable()->innertest_ = const_cast< ::test_protobuf::inner_test*>(
      ::test_protobuf::inner_test::internal_default_instance());
}
#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int test::kNTestFieldNumber;
const int test::kSnTestFieldNumber;
const int test::kStrTestFieldNumber;
const int test::kInnerTestFieldNumber;
const int test::kNTestArrayFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

test::test()
  : ::google::protobuf::Message(), _internal_metadata_(NULL) {
  if (GOOGLE_PREDICT_TRUE(this != internal_default_instance())) {
    ::protobuf_protocol_2eproto::InitDefaultstest();
  }
  SharedCtor();
  // @@protoc_insertion_point(constructor:test_protobuf.test)
}
test::test(const test& from)
  : ::google::protobuf::Message(),
      _internal_metadata_(NULL),
      ntestarray_(from.ntestarray_),
      _cached_size_(0) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  strtest_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (from.strtest().size() > 0) {
    strtest_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.strtest_);
  }
  if (from.has_innertest()) {
    innertest_ = new ::test_protobuf::inner_test(*from.innertest_);
  } else {
    innertest_ = NULL;
  }
  ::memcpy(&ntest_, &from.ntest_,
    static_cast<size_t>(reinterpret_cast<char*>(&sntest_) -
    reinterpret_cast<char*>(&ntest_)) + sizeof(sntest_));
  // @@protoc_insertion_point(copy_constructor:test_protobuf.test)
}

void test::SharedCtor() {
  strtest_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  ::memset(&innertest_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&sntest_) -
      reinterpret_cast<char*>(&innertest_)) + sizeof(sntest_));
  _cached_size_ = 0;
}

test::~test() {
  // @@protoc_insertion_point(destructor:test_protobuf.test)
  SharedDtor();
}

void test::SharedDtor() {
  strtest_.DestroyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (this != internal_default_instance()) delete innertest_;
}

void test::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* test::descriptor() {
  ::protobuf_protocol_2eproto::protobuf_AssignDescriptorsOnce();
  return ::protobuf_protocol_2eproto::file_level_metadata[kIndexInFileMessages].descriptor;
}

const test& test::default_instance() {
  ::protobuf_protocol_2eproto::InitDefaultstest();
  return *internal_default_instance();
}

test* test::New(::google::protobuf::Arena* arena) const {
  test* n = new test;
  if (arena != NULL) {
    arena->Own(n);
  }
  return n;
}

void test::Clear() {
// @@protoc_insertion_point(message_clear_start:test_protobuf.test)
  ::google::protobuf::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  ntestarray_.Clear();
  strtest_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (GetArenaNoVirtual() == NULL && innertest_ != NULL) {
    delete innertest_;
  }
  innertest_ = NULL;
  ::memset(&ntest_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&sntest_) -
      reinterpret_cast<char*>(&ntest_)) + sizeof(sntest_));
  _internal_metadata_.Clear();
}

bool test::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!GOOGLE_PREDICT_TRUE(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:test_protobuf.test)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(127u);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // int32 nTest = 1;
      case 1: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(8u /* 8 & 0xFF */)) {

          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &ntest_)));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // sint32 snTest = 2;
      case 2: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(16u /* 16 & 0xFF */)) {

          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_SINT32>(
                 input, &sntest_)));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // string strTest = 3;
      case 3: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(26u /* 26 & 0xFF */)) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_strtest()));
          DO_(::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
            this->strtest().data(), static_cast<int>(this->strtest().length()),
            ::google::protobuf::internal::WireFormatLite::PARSE,
            "test_protobuf.test.strTest"));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // .test_protobuf.inner_test innerTest = 4;
      case 4: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(34u /* 34 & 0xFF */)) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessage(
               input, mutable_innertest()));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // repeated int32 nTestArray = 5;
      case 5: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(42u /* 42 & 0xFF */)) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPackedPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, this->mutable_ntestarray())));
        } else if (
            static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(40u /* 40 & 0xFF */)) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadRepeatedPrimitiveNoInline<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 1, 42u, input, this->mutable_ntestarray())));
        } else {
          goto handle_unusual;
        }
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, _internal_metadata_.mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:test_protobuf.test)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:test_protobuf.test)
  return false;
#undef DO_
}

void test::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:test_protobuf.test)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // int32 nTest = 1;
  if (this->ntest() != 0) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(1, this->ntest(), output);
  }

  // sint32 snTest = 2;
  if (this->sntest() != 0) {
    ::google::protobuf::internal::WireFormatLite::WriteSInt32(2, this->sntest(), output);
  }

  // string strTest = 3;
  if (this->strtest().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->strtest().data(), static_cast<int>(this->strtest().length()),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "test_protobuf.test.strTest");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      3, this->strtest(), output);
  }

  // .test_protobuf.inner_test innerTest = 4;
  if (this->has_innertest()) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      4, *this->innertest_, output);
  }

  // repeated int32 nTestArray = 5;
  if (this->ntestarray_size() > 0) {
    ::google::protobuf::internal::WireFormatLite::WriteTag(5, ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED, output);
    output->WriteVarint32(static_cast< ::google::protobuf::uint32>(
        _ntestarray_cached_byte_size_));
  }
  for (int i = 0, n = this->ntestarray_size(); i < n; i++) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32NoTag(
      this->ntestarray(i), output);
  }

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()), output);
  }
  // @@protoc_insertion_point(serialize_end:test_protobuf.test)
}

::google::protobuf::uint8* test::InternalSerializeWithCachedSizesToArray(
    bool deterministic, ::google::protobuf::uint8* target) const {
  (void)deterministic; // Unused
  // @@protoc_insertion_point(serialize_to_array_start:test_protobuf.test)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // int32 nTest = 1;
  if (this->ntest() != 0) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(1, this->ntest(), target);
  }

  // sint32 snTest = 2;
  if (this->sntest() != 0) {
    target = ::google::protobuf::internal::WireFormatLite::WriteSInt32ToArray(2, this->sntest(), target);
  }

  // string strTest = 3;
  if (this->strtest().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->strtest().data(), static_cast<int>(this->strtest().length()),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "test_protobuf.test.strTest");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        3, this->strtest(), target);
  }

  // .test_protobuf.inner_test innerTest = 4;
  if (this->has_innertest()) {
    target = ::google::protobuf::internal::WireFormatLite::
      InternalWriteMessageToArray(
        4, *this->innertest_, deterministic, target);
  }

  // repeated int32 nTestArray = 5;
  if (this->ntestarray_size() > 0) {
    target = ::google::protobuf::internal::WireFormatLite::WriteTagToArray(
      5,
      ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED,
      target);
    target = ::google::protobuf::io::CodedOutputStream::WriteVarint32ToArray(
        static_cast< ::google::protobuf::int32>(
            _ntestarray_cached_byte_size_), target);
    target = ::google::protobuf::internal::WireFormatLite::
      WriteInt32NoTagToArray(this->ntestarray_, target);
  }

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:test_protobuf.test)
  return target;
}

size_t test::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:test_protobuf.test)
  size_t total_size = 0;

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()));
  }
  // repeated int32 nTestArray = 5;
  {
    size_t data_size = ::google::protobuf::internal::WireFormatLite::
      Int32Size(this->ntestarray_);
    if (data_size > 0) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
            static_cast< ::google::protobuf::int32>(data_size));
    }
    int cached_size = ::google::protobuf::internal::ToCachedSize(data_size);
    GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
    _ntestarray_cached_byte_size_ = cached_size;
    GOOGLE_SAFE_CONCURRENT_WRITES_END();
    total_size += data_size;
  }

  // string strTest = 3;
  if (this->strtest().size() > 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::StringSize(
        this->strtest());
  }

  // .test_protobuf.inner_test innerTest = 4;
  if (this->has_innertest()) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::MessageSize(
        *this->innertest_);
  }

  // int32 nTest = 1;
  if (this->ntest() != 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::Int32Size(
        this->ntest());
  }

  // sint32 snTest = 2;
  if (this->sntest() != 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::SInt32Size(
        this->sntest());
  }

  int cached_size = ::google::protobuf::internal::ToCachedSize(total_size);
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = cached_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void test::MergeFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:test_protobuf.test)
  GOOGLE_DCHECK_NE(&from, this);
  const test* source =
      ::google::protobuf::internal::DynamicCastToGenerated<const test>(
          &from);
  if (source == NULL) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:test_protobuf.test)
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:test_protobuf.test)
    MergeFrom(*source);
  }
}

void test::MergeFrom(const test& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:test_protobuf.test)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  ntestarray_.MergeFrom(from.ntestarray_);
  if (from.strtest().size() > 0) {

    strtest_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.strtest_);
  }
  if (from.has_innertest()) {
    mutable_innertest()->::test_protobuf::inner_test::MergeFrom(from.innertest());
  }
  if (from.ntest() != 0) {
    set_ntest(from.ntest());
  }
  if (from.sntest() != 0) {
    set_sntest(from.sntest());
  }
}

void test::CopyFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:test_protobuf.test)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void test::CopyFrom(const test& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:test_protobuf.test)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool test::IsInitialized() const {
  return true;
}

void test::Swap(test* other) {
  if (other == this) return;
  InternalSwap(other);
}
void test::InternalSwap(test* other) {
  using std::swap;
  ntestarray_.InternalSwap(&other->ntestarray_);
  strtest_.Swap(&other->strtest_);
  swap(innertest_, other->innertest_);
  swap(ntest_, other->ntest_);
  swap(sntest_, other->sntest_);
  _internal_metadata_.Swap(&other->_internal_metadata_);
  swap(_cached_size_, other->_cached_size_);
}

::google::protobuf::Metadata test::GetMetadata() const {
  protobuf_protocol_2eproto::protobuf_AssignDescriptorsOnce();
  return ::protobuf_protocol_2eproto::file_level_metadata[kIndexInFileMessages];
}


// @@protoc_insertion_point(namespace_scope)
}  // namespace test_protobuf

// @@protoc_insertion_point(global_scope)