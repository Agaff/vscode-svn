#include "svn_error.hpp"
#include "utils.hpp"

namespace Svn
{
namespace SvnError
{
Persistent<Function> _captureStackTrace;
Persistent<Function> _svn_error;

#define DefineReadOnlyValue(object, name, value)   \
    (object)->DefineOwnProperty(context,           \
                                Util_String(name), \
                                (value),           \
                                ReadOnlyDontDelete)

#define DefineNonEnumValue(object, name, value)    \
    (object)->DefineOwnProperty(context,           \
                                Util_String(name), \
                                (value),           \
                                PropertyAttribute::DontEnum)

#define GetProperty(object, name) (object)->Get(context, Util_String(name)).ToLocalChecked()

void Constructor(const FunctionCallbackInfo<Value> &args)
{
    auto isolate = args.GetIsolate();
    auto context = isolate->GetCurrentContext();

    Util_ThrowIf(!args.IsConstructCall(), Util_Error(TypeError, "Class constructor SvnError cannot be invoked without 'new'"));

    Util_ThrowIf(args.Length() == 0, Util_Error(TypeError, "Argument \"code\" must be a number"));
    Util_ThrowIf(!args[0]->IsNumber(), Util_Error(TypeError, "Argument \"code\" must be a number"));

    Util_ThrowIf(args.Length() == 1, Util_Error(TypeError, "Argument \"message\" must be a string"));
    Util_ThrowIf(!args[1]->IsString(), Util_Error(TypeError, "Argument \"message\" must be a string"));

    auto _this = args.This();

    DefineNonEnumValue(_this, "code", args[0]);
    DefineNonEnumValue(_this, "message", args[1]);

    const auto argc = 1;
    Local<Value> argv[argc] = {_this};
    _captureStackTrace.Get(isolate)->CallAsFunction(Undefined(isolate), argc, argv);

    DefineNonEnumValue(_this, "child", args[2]);
}

void Init(Local<Object> exports, Isolate *isolate, Local<Context> context)
{
    auto template_ = Util_FunctionTemplate(Constructor, 2);
    template_->SetClassName(String::NewFromUtf8(isolate, "SvnError"));
    template_->InstanceTemplate()->SetInternalFieldCount(1);
    auto function = template_->GetFunction();

    auto global = context->Global();
    auto error = GetProperty(global, "Error").As<Function>();
    auto error_prototype = GetProperty(error, "prototype");

    _captureStackTrace.Reset(isolate, GetProperty(error, "captureStackTrace").As<Function>());

    auto svn_error_prototype = GetProperty(function, "prototype").As<Object>();
    svn_error_prototype->SetPrototype(context, error_prototype);
    DefineReadOnlyValue(svn_error_prototype, "name", Util_String("SvnError"));

    _svn_error.Reset(isolate, function);
    DefineReadOnlyValue(exports, "SvnError", function);
}

Local<Value> New(Isolate *isolate, Local<Context> context, int code, const char *message, Local<Value> &child = Local<Value>())
{
    const auto argc = 3;
    Local<Value> argv[argc] = {
        Integer::New(isolate, code),
        String::NewFromUtf8(isolate, message, NewStringType::kNormal).ToLocalChecked(),
        child};
    return _svn_error.Get(isolate)->CallAsConstructor(isolate->GetCurrentContext(), argc, argv).ToLocalChecked();
}

Local<Value> New(Isolate *isolate, Local<Context> context, svn_error_t *error)
{
    const auto argc = 3;
    Local<Value> argv[argc] = {
        Integer::New(isolate, error->apr_err),
        String::NewFromUtf8(isolate, error->message, NewStringType::kNormal).ToLocalChecked(),
        error->child != nullptr ? New(isolate, context, error->child) : Undefined(isolate)};
    return _svn_error.Get(isolate)->CallAsConstructor(isolate->GetCurrentContext(), argc, argv).ToLocalChecked();
}
}
}
