#include "client.h"

namespace Svn
{
using v8::AccessControl;
using v8::Context;
using v8::Integer;
using v8::Local;
using v8::MaybeLocal;
using v8::Name;
using v8::Object;
using v8::PropertyAttribute;
using v8::PropertyCallbackInfo;
using v8::String;
using v8::NewStringType;
using v8::Value;

void Version(Local<Name> property, const PropertyCallbackInfo<Value> &args)
{
	auto isolate = args.GetIsolate();

	auto version = svn_client_version();
	auto oVersion = Object::New(isolate);
	oVersion->Set(String::NewFromUtf8(isolate, "major"), Integer::New(isolate, version->major));
	oVersion->Set(String::NewFromUtf8(isolate, "minor"), Integer::New(isolate, version->minor));
	oVersion->Set(String::NewFromUtf8(isolate, "patch"), Integer::New(isolate, version->patch));
	args.GetReturnValue().Set(oVersion);
}

void Init(Local<Object> exports)
{
	auto isolate = exports->GetIsolate();
	auto context = Context::New(isolate);

	exports->SetAccessor(context,																			// context
						 String::NewFromUtf8(isolate, "version", NewStringType::kNormal).ToLocalChecked(),  // name
						 Version,																			// getter
						 nullptr,																			// setter
						 MaybeLocal<Value>(),																// data
						 AccessControl::ALL_CAN_READ,														// settings
						 (PropertyAttribute)(PropertyAttribute::ReadOnly | PropertyAttribute::DontDelete)); // attribute

	Client::Init(exports, isolate, context);
}

NODE_MODULE(svn, Init)
}
