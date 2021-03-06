// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: im_redirect.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "im_redirect.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
// @@protoc_insertion_point(includes)

namespace YouMeIMProtocol {

void protobuf_ShutdownFile_im_5fredirect_2eproto() {
  delete YouMeIM_Command_Redirect::default_instance_;
  delete YouMeIM_Command_RedirectResponse::default_instance_;
}

#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
void protobuf_AddDesc_im_5fredirect_2eproto_impl() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

#else
void protobuf_AddDesc_im_5fredirect_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

#endif
  ::YouMeIMProtocol::protobuf_AddDesc_im_5fcommon_2eproto();
  YouMeIM_Command_Redirect::default_instance_ = new YouMeIM_Command_Redirect();
  YouMeIM_Command_RedirectResponse::default_instance_ = new YouMeIM_Command_RedirectResponse();
  YouMeIM_Command_Redirect::default_instance_->InitAsDefaultInstance();
  YouMeIM_Command_RedirectResponse::default_instance_->InitAsDefaultInstance();
  ::youmecommon::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_im_5fredirect_2eproto);
}

#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AddDesc_im_5fredirect_2eproto_once_);
void protobuf_AddDesc_im_5fredirect_2eproto() {
  ::youmecommon::protobuf::GoogleOnceInit(&protobuf_AddDesc_im_5fredirect_2eproto_once_,
                 &protobuf_AddDesc_im_5fredirect_2eproto_impl);
}
#else
// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_im_5fredirect_2eproto {
  StaticDescriptorInitializer_im_5fredirect_2eproto() {
    protobuf_AddDesc_im_5fredirect_2eproto();
  }
} static_descriptor_initializer_im_5fredirect_2eproto_;
#endif

namespace {

static void MergeFromFail(int line) GOOGLE_ATTRIBUTE_COLD;
static void MergeFromFail(int line) {
  GOOGLE_CHECK(false) << __FILE__ << ":" << line;
}

}  // namespace


// ===================================================================

#ifndef _MSC_VER
const int YouMeIM_Command_Redirect::kMsgHeadFieldNumber;
#endif  // !_MSC_VER

YouMeIM_Command_Redirect::YouMeIM_Command_Redirect()
  : ::youmecommon::protobuf::MessageLite(), _arena_ptr_(NULL) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:YouMeProtocol.YouMeIM_Command_Redirect)
}

void YouMeIM_Command_Redirect::InitAsDefaultInstance() {
#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  msghead_ = const_cast< ::YouMeIMProtocol::PacketHead*>(
      ::YouMeIMProtocol::PacketHead::internal_default_instance());
#else
  msghead_ = const_cast< ::YouMeIMProtocol::PacketHead*>(&::YouMeIMProtocol::PacketHead::default_instance());
#endif
}

YouMeIM_Command_Redirect::YouMeIM_Command_Redirect(const YouMeIM_Command_Redirect& from)
  : ::youmecommon::protobuf::MessageLite(),
    _arena_ptr_(NULL) {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:YouMeProtocol.YouMeIM_Command_Redirect)
}

void YouMeIM_Command_Redirect::SharedCtor() {
  ::youmecommon::protobuf::internal::GetEmptyString();
  _cached_size_ = 0;
  _unknown_fields_.UnsafeSetDefault(
      &::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
  msghead_ = NULL;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

YouMeIM_Command_Redirect::~YouMeIM_Command_Redirect() {
  // @@protoc_insertion_point(destructor:YouMeProtocol.YouMeIM_Command_Redirect)
  SharedDtor();
}

void YouMeIM_Command_Redirect::SharedDtor() {
  _unknown_fields_.DestroyNoArena(
      &::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  if (this != &default_instance()) {
  #else
  if (this != default_instance_) {
  #endif
    delete msghead_;
  }
}

void YouMeIM_Command_Redirect::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const YouMeIM_Command_Redirect& YouMeIM_Command_Redirect::default_instance() {
#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  protobuf_AddDesc_im_5fredirect_2eproto();
#else
  if (default_instance_ == NULL) protobuf_AddDesc_im_5fredirect_2eproto();
#endif
  return *default_instance_;
}

YouMeIM_Command_Redirect* YouMeIM_Command_Redirect::default_instance_ = NULL;

YouMeIM_Command_Redirect* YouMeIM_Command_Redirect::New(::youmecommon::protobuf::Arena* arena) const {
  YouMeIM_Command_Redirect* n = new YouMeIM_Command_Redirect;
  if (arena != NULL) {
    arena->Own(n);
  }
  return n;
}

void YouMeIM_Command_Redirect::Clear() {
  if (has_msghead()) {
    if (msghead_ != NULL) msghead_->::YouMeIMProtocol::PacketHead::Clear();
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  _unknown_fields_.ClearToEmptyNoArena(
      &::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
}

bool YouMeIM_Command_Redirect::MergePartialFromCodedStream(
    ::youmecommon::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) goto failure
  ::youmecommon::protobuf::uint32 tag;
  ::youmecommon::protobuf::io::StringOutputStream unknown_fields_string(
      mutable_unknown_fields());
  ::youmecommon::protobuf::io::CodedOutputStream unknown_fields_stream(
      &unknown_fields_string);
  // @@protoc_insertion_point(parse_start:YouMeProtocol.YouMeIM_Command_Redirect)
  for (;;) {
    ::std::pair< ::youmecommon::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::youmecommon::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required .YouMeProtocol.PacketHead msgHead = 1;
      case 1: {
        if (tag == 10) {
          DO_(::youmecommon::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
               input, mutable_msghead()));
        } else {
          goto handle_unusual;
        }
        if (input->ExpectAtEnd()) goto success;
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::youmecommon::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::youmecommon::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::youmecommon::protobuf::internal::WireFormatLite::SkipField(
            input, tag, &unknown_fields_stream));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:YouMeProtocol.YouMeIM_Command_Redirect)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:YouMeProtocol.YouMeIM_Command_Redirect)
  return false;
#undef DO_
}

void YouMeIM_Command_Redirect::SerializeWithCachedSizes(
    ::youmecommon::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:YouMeProtocol.YouMeIM_Command_Redirect)
  // required .YouMeProtocol.PacketHead msgHead = 1;
  if (has_msghead()) {
    ::youmecommon::protobuf::internal::WireFormatLite::WriteMessage(
      1, *this->msghead_, output);
  }

  output->WriteRaw(unknown_fields().data(),
                   unknown_fields().size());
  // @@protoc_insertion_point(serialize_end:YouMeProtocol.YouMeIM_Command_Redirect)
}

int YouMeIM_Command_Redirect::ByteSize() const {
  int total_size = 0;

  // required .YouMeProtocol.PacketHead msgHead = 1;
  if (has_msghead()) {
    total_size += 1 +
      ::youmecommon::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
        *this->msghead_);
  }
  total_size += unknown_fields().size();

  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void YouMeIM_Command_Redirect::CheckTypeAndMergeFrom(
    const ::youmecommon::protobuf::MessageLite& from) {
  MergeFrom(*::youmecommon::protobuf::down_cast<const YouMeIM_Command_Redirect*>(&from));
}

void YouMeIM_Command_Redirect::MergeFrom(const YouMeIM_Command_Redirect& from) {
  if (GOOGLE_PREDICT_FALSE(&from == this)) MergeFromFail(__LINE__);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_msghead()) {
      mutable_msghead()->::YouMeIMProtocol::PacketHead::MergeFrom(from.msghead());
    }
  }
  mutable_unknown_fields()->append(from.unknown_fields());
}

void YouMeIM_Command_Redirect::CopyFrom(const YouMeIM_Command_Redirect& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool YouMeIM_Command_Redirect::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000001) != 0x00000001) return false;

  if (has_msghead()) {
    if (!this->msghead_->IsInitialized()) return false;
  }
  return true;
}

void YouMeIM_Command_Redirect::Swap(YouMeIM_Command_Redirect* other) {
  if (other == this) return;
  InternalSwap(other);
}
void YouMeIM_Command_Redirect::InternalSwap(YouMeIM_Command_Redirect* other) {
  std::swap(msghead_, other->msghead_);
  std::swap(_has_bits_[0], other->_has_bits_[0]);
  _unknown_fields_.Swap(&other->_unknown_fields_);
  std::swap(_cached_size_, other->_cached_size_);
}

::std::string YouMeIM_Command_Redirect::GetTypeName() const {
  return "YouMeProtocol.YouMeIM_Command_Redirect";
}

#if PROTOBUF_INLINE_NOT_IN_HEADERS
// YouMeIM_Command_Redirect

// required .YouMeProtocol.PacketHead msgHead = 1;
bool YouMeIM_Command_Redirect::has_msghead() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
void YouMeIM_Command_Redirect::set_has_msghead() {
  _has_bits_[0] |= 0x00000001u;
}
void YouMeIM_Command_Redirect::clear_has_msghead() {
  _has_bits_[0] &= ~0x00000001u;
}
void YouMeIM_Command_Redirect::clear_msghead() {
  if (msghead_ != NULL) msghead_->::YouMeIMProtocol::PacketHead::Clear();
  clear_has_msghead();
}
const ::YouMeIMProtocol::PacketHead& YouMeIM_Command_Redirect::msghead() const {
  // @@protoc_insertion_point(field_get:YouMeProtocol.YouMeIM_Command_Redirect.msgHead)
#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  return msghead_ != NULL ? *msghead_ : *default_instance().msghead_;
#else
  return msghead_ != NULL ? *msghead_ : *default_instance_->msghead_;
#endif
}
::YouMeIMProtocol::PacketHead* YouMeIM_Command_Redirect::mutable_msghead() {
  set_has_msghead();
  if (msghead_ == NULL) {
    msghead_ = new ::YouMeIMProtocol::PacketHead;
  }
  // @@protoc_insertion_point(field_mutable:YouMeProtocol.YouMeIM_Command_Redirect.msgHead)
  return msghead_;
}
::YouMeIMProtocol::PacketHead* YouMeIM_Command_Redirect::release_msghead() {
  clear_has_msghead();
  ::YouMeIMProtocol::PacketHead* temp = msghead_;
  msghead_ = NULL;
  return temp;
}
void YouMeIM_Command_Redirect::set_allocated_msghead(::YouMeIMProtocol::PacketHead* msghead) {
  delete msghead_;
  msghead_ = msghead;
  if (msghead) {
    set_has_msghead();
  } else {
    clear_has_msghead();
  }
  // @@protoc_insertion_point(field_set_allocated:YouMeProtocol.YouMeIM_Command_Redirect.msgHead)
}

#endif  // PROTOBUF_INLINE_NOT_IN_HEADERS

// ===================================================================

#ifndef _MSC_VER
const int YouMeIM_Command_RedirectResponse::kMsgHeadFieldNumber;
const int YouMeIM_Command_RedirectResponse::kIpFieldNumber;
const int YouMeIM_Command_RedirectResponse::kPortFieldNumber;
const int YouMeIM_Command_RedirectResponse::kTimeoutFieldNumber;
#endif  // !_MSC_VER

YouMeIM_Command_RedirectResponse::YouMeIM_Command_RedirectResponse()
  : ::youmecommon::protobuf::MessageLite(), _arena_ptr_(NULL) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:YouMeProtocol.YouMeIM_Command_RedirectResponse)
}

void YouMeIM_Command_RedirectResponse::InitAsDefaultInstance() {
#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  msghead_ = const_cast< ::YouMeIMProtocol::ServerPacketHead*>(
      ::YouMeIMProtocol::ServerPacketHead::internal_default_instance());
#else
  msghead_ = const_cast< ::YouMeIMProtocol::ServerPacketHead*>(&::YouMeIMProtocol::ServerPacketHead::default_instance());
#endif
}

YouMeIM_Command_RedirectResponse::YouMeIM_Command_RedirectResponse(const YouMeIM_Command_RedirectResponse& from)
  : ::youmecommon::protobuf::MessageLite(),
    _arena_ptr_(NULL) {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:YouMeProtocol.YouMeIM_Command_RedirectResponse)
}

void YouMeIM_Command_RedirectResponse::SharedCtor() {
  ::youmecommon::protobuf::internal::GetEmptyString();
  _cached_size_ = 0;
  _unknown_fields_.UnsafeSetDefault(
      &::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
  msghead_ = NULL;
  ip_.UnsafeSetDefault(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
  port_ = 0;
  timeout_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

YouMeIM_Command_RedirectResponse::~YouMeIM_Command_RedirectResponse() {
  // @@protoc_insertion_point(destructor:YouMeProtocol.YouMeIM_Command_RedirectResponse)
  SharedDtor();
}

void YouMeIM_Command_RedirectResponse::SharedDtor() {
  _unknown_fields_.DestroyNoArena(
      &::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
  ip_.DestroyNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  if (this != &default_instance()) {
  #else
  if (this != default_instance_) {
  #endif
    delete msghead_;
  }
}

void YouMeIM_Command_RedirectResponse::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const YouMeIM_Command_RedirectResponse& YouMeIM_Command_RedirectResponse::default_instance() {
#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  protobuf_AddDesc_im_5fredirect_2eproto();
#else
  if (default_instance_ == NULL) protobuf_AddDesc_im_5fredirect_2eproto();
#endif
  return *default_instance_;
}

YouMeIM_Command_RedirectResponse* YouMeIM_Command_RedirectResponse::default_instance_ = NULL;

YouMeIM_Command_RedirectResponse* YouMeIM_Command_RedirectResponse::New(::youmecommon::protobuf::Arena* arena) const {
  YouMeIM_Command_RedirectResponse* n = new YouMeIM_Command_RedirectResponse;
  if (arena != NULL) {
    arena->Own(n);
  }
  return n;
}

void YouMeIM_Command_RedirectResponse::Clear() {
#define ZR_HELPER_(f) reinterpret_cast<char*>(\
  &reinterpret_cast<YouMeIM_Command_RedirectResponse*>(16)->f)

#define ZR_(first, last) do {\
  ::memset(&first, 0,\
           ZR_HELPER_(last) - ZR_HELPER_(first) + sizeof(last));\
} while (0)

  if (_has_bits_[0 / 32] & 15u) {
    ZR_(port_, timeout_);
    if (has_msghead()) {
      if (msghead_ != NULL) msghead_->::YouMeIMProtocol::ServerPacketHead::Clear();
    }
    if (has_ip()) {
      ip_.ClearToEmptyNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
    }
  }

#undef ZR_HELPER_
#undef ZR_

  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  _unknown_fields_.ClearToEmptyNoArena(
      &::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
}

bool YouMeIM_Command_RedirectResponse::MergePartialFromCodedStream(
    ::youmecommon::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) goto failure
  ::youmecommon::protobuf::uint32 tag;
  ::youmecommon::protobuf::io::StringOutputStream unknown_fields_string(
      mutable_unknown_fields());
  ::youmecommon::protobuf::io::CodedOutputStream unknown_fields_stream(
      &unknown_fields_string);
  // @@protoc_insertion_point(parse_start:YouMeProtocol.YouMeIM_Command_RedirectResponse)
  for (;;) {
    ::std::pair< ::youmecommon::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::youmecommon::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required .YouMeProtocol.ServerPacketHead msgHead = 1;
      case 1: {
        if (tag == 10) {
          DO_(::youmecommon::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
               input, mutable_msghead()));
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(18)) goto parse_ip;
        break;
      }

      // required string ip = 2;
      case 2: {
        if (tag == 18) {
         parse_ip:
          DO_(::youmecommon::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_ip()));
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(24)) goto parse_port;
        break;
      }

      // required int32 port = 3;
      case 3: {
        if (tag == 24) {
         parse_port:
          DO_((::youmecommon::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::youmecommon::protobuf::int32, ::youmecommon::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &port_)));
          set_has_port();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(32)) goto parse_timeout;
        break;
      }

      // optional int32 timeout = 4;
      case 4: {
        if (tag == 32) {
         parse_timeout:
          DO_((::youmecommon::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::youmecommon::protobuf::int32, ::youmecommon::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &timeout_)));
          set_has_timeout();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectAtEnd()) goto success;
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::youmecommon::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::youmecommon::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::youmecommon::protobuf::internal::WireFormatLite::SkipField(
            input, tag, &unknown_fields_stream));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:YouMeProtocol.YouMeIM_Command_RedirectResponse)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:YouMeProtocol.YouMeIM_Command_RedirectResponse)
  return false;
#undef DO_
}

void YouMeIM_Command_RedirectResponse::SerializeWithCachedSizes(
    ::youmecommon::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:YouMeProtocol.YouMeIM_Command_RedirectResponse)
  // required .YouMeProtocol.ServerPacketHead msgHead = 1;
  if (has_msghead()) {
    ::youmecommon::protobuf::internal::WireFormatLite::WriteMessage(
      1, *this->msghead_, output);
  }

  // required string ip = 2;
  if (has_ip()) {
    ::youmecommon::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      2, this->ip(), output);
  }

  // required int32 port = 3;
  if (has_port()) {
    ::youmecommon::protobuf::internal::WireFormatLite::WriteInt32(3, this->port(), output);
  }

  // optional int32 timeout = 4;
  if (has_timeout()) {
    ::youmecommon::protobuf::internal::WireFormatLite::WriteInt32(4, this->timeout(), output);
  }

  output->WriteRaw(unknown_fields().data(),
                   unknown_fields().size());
  // @@protoc_insertion_point(serialize_end:YouMeProtocol.YouMeIM_Command_RedirectResponse)
}

int YouMeIM_Command_RedirectResponse::RequiredFieldsByteSizeFallback() const {
  int total_size = 0;

  if (has_msghead()) {
    // required .YouMeProtocol.ServerPacketHead msgHead = 1;
    total_size += 1 +
      ::youmecommon::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
        *this->msghead_);
  }

  if (has_ip()) {
    // required string ip = 2;
    total_size += 1 +
      ::youmecommon::protobuf::internal::WireFormatLite::StringSize(
        this->ip());
  }

  if (has_port()) {
    // required int32 port = 3;
    total_size += 1 +
      ::youmecommon::protobuf::internal::WireFormatLite::Int32Size(
        this->port());
  }

  return total_size;
}
int YouMeIM_Command_RedirectResponse::ByteSize() const {
  int total_size = 0;

  if (((_has_bits_[0] & 0x00000007) ^ 0x00000007) == 0) {  // All required fields are present.
    // required .YouMeProtocol.ServerPacketHead msgHead = 1;
    total_size += 1 +
      ::youmecommon::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
        *this->msghead_);

    // required string ip = 2;
    total_size += 1 +
      ::youmecommon::protobuf::internal::WireFormatLite::StringSize(
        this->ip());

    // required int32 port = 3;
    total_size += 1 +
      ::youmecommon::protobuf::internal::WireFormatLite::Int32Size(
        this->port());

  } else {
    total_size += RequiredFieldsByteSizeFallback();
  }
  // optional int32 timeout = 4;
  if (has_timeout()) {
    total_size += 1 +
      ::youmecommon::protobuf::internal::WireFormatLite::Int32Size(
        this->timeout());
  }

  total_size += unknown_fields().size();

  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void YouMeIM_Command_RedirectResponse::CheckTypeAndMergeFrom(
    const ::youmecommon::protobuf::MessageLite& from) {
  MergeFrom(*::youmecommon::protobuf::down_cast<const YouMeIM_Command_RedirectResponse*>(&from));
}

void YouMeIM_Command_RedirectResponse::MergeFrom(const YouMeIM_Command_RedirectResponse& from) {
  if (GOOGLE_PREDICT_FALSE(&from == this)) MergeFromFail(__LINE__);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_msghead()) {
      mutable_msghead()->::YouMeIMProtocol::ServerPacketHead::MergeFrom(from.msghead());
    }
    if (from.has_ip()) {
      set_has_ip();
      ip_.AssignWithDefault(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited(), from.ip_);
    }
    if (from.has_port()) {
      set_port(from.port());
    }
    if (from.has_timeout()) {
      set_timeout(from.timeout());
    }
  }
  mutable_unknown_fields()->append(from.unknown_fields());
}

void YouMeIM_Command_RedirectResponse::CopyFrom(const YouMeIM_Command_RedirectResponse& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool YouMeIM_Command_RedirectResponse::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000007) != 0x00000007) return false;

  if (has_msghead()) {
    if (!this->msghead_->IsInitialized()) return false;
  }
  return true;
}

void YouMeIM_Command_RedirectResponse::Swap(YouMeIM_Command_RedirectResponse* other) {
  if (other == this) return;
  InternalSwap(other);
}
void YouMeIM_Command_RedirectResponse::InternalSwap(YouMeIM_Command_RedirectResponse* other) {
  std::swap(msghead_, other->msghead_);
  ip_.Swap(&other->ip_);
  std::swap(port_, other->port_);
  std::swap(timeout_, other->timeout_);
  std::swap(_has_bits_[0], other->_has_bits_[0]);
  _unknown_fields_.Swap(&other->_unknown_fields_);
  std::swap(_cached_size_, other->_cached_size_);
}

::std::string YouMeIM_Command_RedirectResponse::GetTypeName() const {
  return "YouMeProtocol.YouMeIM_Command_RedirectResponse";
}

#if PROTOBUF_INLINE_NOT_IN_HEADERS
// YouMeIM_Command_RedirectResponse

// required .YouMeProtocol.ServerPacketHead msgHead = 1;
bool YouMeIM_Command_RedirectResponse::has_msghead() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
void YouMeIM_Command_RedirectResponse::set_has_msghead() {
  _has_bits_[0] |= 0x00000001u;
}
void YouMeIM_Command_RedirectResponse::clear_has_msghead() {
  _has_bits_[0] &= ~0x00000001u;
}
void YouMeIM_Command_RedirectResponse::clear_msghead() {
  if (msghead_ != NULL) msghead_->::YouMeIMProtocol::ServerPacketHead::Clear();
  clear_has_msghead();
}
const ::YouMeIMProtocol::ServerPacketHead& YouMeIM_Command_RedirectResponse::msghead() const {
  // @@protoc_insertion_point(field_get:YouMeProtocol.YouMeIM_Command_RedirectResponse.msgHead)
#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  return msghead_ != NULL ? *msghead_ : *default_instance().msghead_;
#else
  return msghead_ != NULL ? *msghead_ : *default_instance_->msghead_;
#endif
}
::YouMeIMProtocol::ServerPacketHead* YouMeIM_Command_RedirectResponse::mutable_msghead() {
  set_has_msghead();
  if (msghead_ == NULL) {
    msghead_ = new ::YouMeIMProtocol::ServerPacketHead;
  }
  // @@protoc_insertion_point(field_mutable:YouMeProtocol.YouMeIM_Command_RedirectResponse.msgHead)
  return msghead_;
}
::YouMeIMProtocol::ServerPacketHead* YouMeIM_Command_RedirectResponse::release_msghead() {
  clear_has_msghead();
  ::YouMeIMProtocol::ServerPacketHead* temp = msghead_;
  msghead_ = NULL;
  return temp;
}
void YouMeIM_Command_RedirectResponse::set_allocated_msghead(::YouMeIMProtocol::ServerPacketHead* msghead) {
  delete msghead_;
  msghead_ = msghead;
  if (msghead) {
    set_has_msghead();
  } else {
    clear_has_msghead();
  }
  // @@protoc_insertion_point(field_set_allocated:YouMeProtocol.YouMeIM_Command_RedirectResponse.msgHead)
}

// required string ip = 2;
bool YouMeIM_Command_RedirectResponse::has_ip() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
void YouMeIM_Command_RedirectResponse::set_has_ip() {
  _has_bits_[0] |= 0x00000002u;
}
void YouMeIM_Command_RedirectResponse::clear_has_ip() {
  _has_bits_[0] &= ~0x00000002u;
}
void YouMeIM_Command_RedirectResponse::clear_ip() {
  ip_.ClearToEmptyNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
  clear_has_ip();
}
 const ::std::string& YouMeIM_Command_RedirectResponse::ip() const {
  // @@protoc_insertion_point(field_get:YouMeProtocol.YouMeIM_Command_RedirectResponse.ip)
  return ip_.GetNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
}
 void YouMeIM_Command_RedirectResponse::set_ip(const ::std::string& value) {
  set_has_ip();
  ip_.SetNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:YouMeProtocol.YouMeIM_Command_RedirectResponse.ip)
}
 void YouMeIM_Command_RedirectResponse::set_ip(const char* value) {
  set_has_ip();
  ip_.SetNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:YouMeProtocol.YouMeIM_Command_RedirectResponse.ip)
}
 void YouMeIM_Command_RedirectResponse::set_ip(const char* value, size_t size) {
  set_has_ip();
  ip_.SetNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:YouMeProtocol.YouMeIM_Command_RedirectResponse.ip)
}
 ::std::string* YouMeIM_Command_RedirectResponse::mutable_ip() {
  set_has_ip();
  // @@protoc_insertion_point(field_mutable:YouMeProtocol.YouMeIM_Command_RedirectResponse.ip)
  return ip_.MutableNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
}
 ::std::string* YouMeIM_Command_RedirectResponse::release_ip() {
  clear_has_ip();
  return ip_.ReleaseNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
}
 void YouMeIM_Command_RedirectResponse::set_allocated_ip(::std::string* ip) {
  if (ip != NULL) {
    set_has_ip();
  } else {
    clear_has_ip();
  }
  ip_.SetAllocatedNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited(), ip);
  // @@protoc_insertion_point(field_set_allocated:YouMeProtocol.YouMeIM_Command_RedirectResponse.ip)
}

// required int32 port = 3;
bool YouMeIM_Command_RedirectResponse::has_port() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
void YouMeIM_Command_RedirectResponse::set_has_port() {
  _has_bits_[0] |= 0x00000004u;
}
void YouMeIM_Command_RedirectResponse::clear_has_port() {
  _has_bits_[0] &= ~0x00000004u;
}
void YouMeIM_Command_RedirectResponse::clear_port() {
  port_ = 0;
  clear_has_port();
}
 ::youmecommon::protobuf::int32 YouMeIM_Command_RedirectResponse::port() const {
  // @@protoc_insertion_point(field_get:YouMeProtocol.YouMeIM_Command_RedirectResponse.port)
  return port_;
}
 void YouMeIM_Command_RedirectResponse::set_port(::youmecommon::protobuf::int32 value) {
  set_has_port();
  port_ = value;
  // @@protoc_insertion_point(field_set:YouMeProtocol.YouMeIM_Command_RedirectResponse.port)
}

// optional int32 timeout = 4;
bool YouMeIM_Command_RedirectResponse::has_timeout() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
void YouMeIM_Command_RedirectResponse::set_has_timeout() {
  _has_bits_[0] |= 0x00000008u;
}
void YouMeIM_Command_RedirectResponse::clear_has_timeout() {
  _has_bits_[0] &= ~0x00000008u;
}
void YouMeIM_Command_RedirectResponse::clear_timeout() {
  timeout_ = 0;
  clear_has_timeout();
}
 ::youmecommon::protobuf::int32 YouMeIM_Command_RedirectResponse::timeout() const {
  // @@protoc_insertion_point(field_get:YouMeProtocol.YouMeIM_Command_RedirectResponse.timeout)
  return timeout_;
}
 void YouMeIM_Command_RedirectResponse::set_timeout(::youmecommon::protobuf::int32 value) {
  set_has_timeout();
  timeout_ = value;
  // @@protoc_insertion_point(field_set:YouMeProtocol.YouMeIM_Command_RedirectResponse.timeout)
}

#endif  // PROTOBUF_INLINE_NOT_IN_HEADERS

// @@protoc_insertion_point(namespace_scope)

}  // namespace YouMeProtocol

// @@protoc_insertion_point(global_scope)
